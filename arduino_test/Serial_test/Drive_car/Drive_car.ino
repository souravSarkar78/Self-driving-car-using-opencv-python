#include<Servo.h>

#include <Wire.h>                                    


byte  start, data, v_counter, engine = true, acci = false;

int loop_counter, gyro_address, vibration_counter, vibcount_thresh = 60;
int vibration, g;
long acc_x, acc_y, acc_z, acc_total_vector[20], acc_av_vector, vibration_total_result;

int acc_axis[4], gyro_axis[4];

int cal_int;
double gyro_axis_cal[4];

Servo servo;
char v;
int error;
int angle;

int m11=9, m12=6, m21=3, m22=5;
boolean manual = true, dist = false;
float kp = 23;
float ki = 0.08;
float kd = 10;
float p, i, d;
int baseAngle = 1600, maxLim = 1850, minLim = 1250;
int pid, prev_error;
int range = 250;



void setup(){
  Serial.begin(9600);
  servo.attach(7);
  
   pinMode(13, OUTPUT);
  pinMode(8, OUTPUT); 
  pinMode(10, OUTPUT); 
  pinMode(2, OUTPUT);
  digitalWrite(2, 0); 
  pinMode(4, INPUT); 
  Wire.begin();              //Start the wire library as master
  TWBR = 12;                //Set the I2C clock speed to 400kHz.


  gyro_address = 0x68; //Store the gyro address in the variable.

  set_gyro_registers();
  servo.writeMicroseconds(baseAngle);
}

void check_conditions(){
  
  Serial.flush();
  while (Serial.available() ){
    
    
    v = Serial.read();
    
    if (v == 'w') 
    {
      if (acci == false)
      {
      dist = true;
      if (engine == true)forward();
      g = 0;
      //v_counter = true;
      }
    }
    else if ((v == 's') && (manual == true)) 
    {
      dist = false;
      g = 0;
      backward();
    }
    else if (v == 'r') 
    {
      dist = false;
      g = 0;
      engineOff();
    }
    
    else if (v == 'z') 
    {
    
    digitalWrite(10, !digitalRead(10)); 
    
    }
    
    if (v == 'a' && manual == true) 
    {
      //Serial.println("a");
      angle = maxLim;
    }
    if (v == 'd' && manual == true) angle = minLim;
    if (v == 'e' && manual == true) angle = baseAngle;
  
  
    else if (v == 'l') {
      if (manual == false){
        manual = true;
        
      }
      else{
        manual = false;
        
      }
    }
    else if (v == 'm') {
      baseAngle = baseAngle-10;
      maxLim = maxLim - 10;
      minLim = minLim - 10;
    }
    else if (v == 'n') {
      baseAngle = baseAngle+10;
      maxLim = maxLim + 10;
      minLim = minLim + 10;
    }
    else if (v == 'o') 
    {
      engine = true;
      acci = false;
      digitalWrite(8, 0);
    }
    
    else{
      if (manual == false){
        
        error = v;
        error = error - 78;
      }
    } 
  }
}

void loop(){

  check_conditions();
  
  if (manual == true)manual_mode();
  
  else {
    //Serial.println("AutoPilot Enabled    ");
    calculate_pid();
  
    angle = baseAngle - pid;
  }
  if (angle > maxLim)angle = maxLim;
    
  if (angle <  minLim)angle = minLim;
 
  
  
  chk_vibration();
  /*Serial.print(angle);
  Serial.print("   ");*/
  Serial.println(vibration);
  //if (v_counter == true) g_count();
  g_count();
  if (vibration >160 && g == vibcount_thresh) 
  {
    //digitalWrite(13, 1);
    if (engine == true){
      digitalWrite(2, 1);
      delay(500);
    }
    engine = false;
    acci = true;
    dist = false;
    digitalWrite(8, 1);
    Serial.println(vibration);
    
  }
  else 
  {
    digitalWrite(2, 0);
    digitalWrite(13, 0);
  }
  if (engine == false || (digitalRead(4) == 1 && v != 's')) engineOff();
  else if (dist == true)forward();
  
  /*Serial.print(g);
  Serial.print("   ");*/
  //Serial.println(vibration);
  //Serial.println(digitalRead(10));
  
  
  servo.writeMicroseconds(angle);
  
  
 /* Serial.println(p);
  Serial.print("   ");
  Serial.println(i);
  Serial.print("         ");
  Serial.println(angle);
  Serial.println(v);*/
  
  
}
  
//PID calculation for steering control
void calculate_pid(){
  p = error * kp;
  
  if ((error <= 5) && (error >=-5)){
    
  i = i + (ki*error);
  if (i > 100)i = 100;
  if (i< -100)i = -100;
  }
  
  
  d = kd*(error - prev_error);

  
  pid = p + i + d;
  
  if (pid > range)pid = range;
  if (pid < -1*(range)) pid = -1*(range);
  
  if (pid > 0)pid = pid*1.4;
  //Serial.println(pid);
  
  prev_error = error;
}  
  

void forward(){
  analogWrite(m11, 255);
  analogWrite(m12, 0);
  analogWrite(m21, 60);
  analogWrite(m22, 0);
}
void backward(){
  analogWrite(m11, 0);
  analogWrite(m12, 255);
  analogWrite(m21, 0);
  analogWrite(m22, 60);
}

void engineOff(){
  analogWrite(m11, 0);
  analogWrite(m12, 0);
  analogWrite(m21, 0);
  analogWrite(m22, 0);  
  
}

void manual_mode(){
  p = 0;
  i = 0;
  d = 0;

  /*if (v == 'a') 
  {
    //Serial.println("a");
    angle = maxLim;
  }
  if (v == 'd') angle = minLim;
  if (v == 'e') angle = baseAngle;
  //if (v == 'r') engineOff();*/
}
  



void chk_vibration(){
                                                           
        Wire.beginTransmission(gyro_address);                                           
        Wire.write(0x3B);                                                               //Start reading @ register 43h and auto increment with every read.
        Wire.endTransmission();                                                         //End the transmission.
        Wire.requestFrom(gyro_address,6);                                               //Request 6 bytes from the gyro.
        while(Wire.available() < 6);                                                    //Wait until the 6 bytes are received.
        acc_x = Wire.read()<<8|Wire.read();                                             //Add the low and high byte to the acc_x variable.
        acc_y = Wire.read()<<8|Wire.read();                                             //Add the low and high byte to the acc_y variable.
        acc_z = Wire.read()<<8|Wire.read();  

       /* Serial.print(acc_x);
        Serial.print("    ");
        Serial.print(acc_y);
        Serial.print("    ");
        Serial.println(acc_z);*/
     

       acc_total_vector[0] = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));          //Calculate the total accelerometer vector.
       //Serial.println(acc_total_vector[0]);   
        acc_av_vector = acc_total_vector[0];                                            //Copy the total vector to the accelerometer average vector variable.
        //Serial.println(acc_z);
        for(start = 16; start > 0; start--){                                            //Do this loop 16 times to create an array of accelrometer vectors.
          acc_total_vector[start] = acc_total_vector[start - 1];                        //Shift every variable one position up in the array.
          acc_av_vector += acc_total_vector[start];                                     //Add the array value to the acc_av_vector variable.
        }

        acc_av_vector /= 17;                                                            //Divide the acc_av_vector by 17 to get the avarage total accelerometer vector.

        if(vibration_counter < 20){                                                     //If the vibration_counter is less than 20 do this.
          vibration_counter ++;                                                         //Increment the vibration_counter variable.
          vibration_total_result += abs(acc_total_vector[0] - acc_av_vector);           //Add the absolute difference between the avarage vector and current vector to the vibration_total_result variable.
        }
        else{
          vibration_counter = 0;                                                        //If the vibration_counter is equal or larger than 20 do this.
          vibration = (vibration_total_result/50);               
          vibration_total_result = 0;                                                   //Reset the vibration_total_result variable.
        }
        //Serial.println(vibration);
        
}



void set_gyro_registers(){
  //Setup the MPU-6050

    Wire.beginTransmission(gyro_address);                        //Start communication with the address found during search.
    Wire.write(0x6B);                                            //We want to write to the PWR_MGMT_1 register (6B hex)
    Wire.write(0x00);                                            //Set the register bits as 00000000 to activate the gyro
    Wire.endTransmission();                                      //End the transmission with the gyro.

    Wire.beginTransmission(gyro_address);                        //Start communication with the address found during search.
    Wire.write(0x1B);                                            //We want to write to the GYRO_CONFIG register (1B hex)
    Wire.write(0x08);                                            //Set the register bits as 00001000 (500dps full scale)
    Wire.endTransmission();                                      //End the transmission with the gyro

    Wire.beginTransmission(gyro_address);                        //Start communication with the address found during search.
    Wire.write(0x1C);                                            //We want to write to the ACCEL_CONFIG register (1A hex)
    Wire.write(0x10);                                            //Set the register bits as 00010000 (+/- 8g full scale range)
    Wire.endTransmission();                                      //End the transmission with the gyro

    //Let's perform a random register check to see if the values are written correct
    Wire.beginTransmission(gyro_address);                        //Start communication with the address found during search
    Wire.write(0x1B);                                            //Start reading @ register 0x1B
    Wire.endTransmission();                                      //End the transmission
    Wire.requestFrom(gyro_address, 1);                           //Request 1 bytes from the gyro
    while(Wire.available() < 1);                                 //Wait until the 6 bytes are received
    if(Wire.read() != 0x08){                                     //Check if the value is 0x08
      digitalWrite(12,HIGH);                                     //Turn on the warning led
      while(1)delay(10);                                         //Stay in this loop for ever
    }

    Wire.beginTransmission(gyro_address);                        //Start communication with the address found during search
    Wire.write(0x1A);                                            //We want to write to the CONFIG register (1A hex)
    Wire.write(0x03);                                            //Set the register bits as 00000011 (Set Digital Low Pass Filter to ~43Hz)
    Wire.endTransmission();                                      //End the transmission with the gyro    

  
}

void g_count(){
  g=g+1;
  if (g > vibcount_thresh) g = vibcount_thresh;
  //Serial.println(g);
}


  
