#include<Servo.h>


#include <Wire.h>                                    


byte  start, data;

int loop_counter, gyro_address, vibration_counter;
int temperature, vibration;
long acc_x, acc_y, acc_z, acc_total_vector[20], acc_av_vector, vibration_total_result;

int acc_axis[4], gyro_axis[4];
double gyro_pitch, gyro_roll, gyro_yaw;
float angle_roll_acc, angle_pitch_acc, angle_pitch, angle_roll;


Servo servo;
char v;
int error;
int angle;

int m11=9, m12=6, m21=3, m22=5;
boolean manual = false;
int kp = 4;
int ki = 0.02;
int kd = 0;
float p, i, d;
int pid, prev_error;

void setup(){
  Serial.begin(9600);
  servo.attach(7);
  servo.write(100);
  Wire.begin();                                                                         //Start the wire library as master
  TWBR = 12;                                                                            //Set the I2C clock speed to 400kHz.


  gyro_address = 0x68;                                                       //Store the gyro address in the variable.

  set_gyro_registers();    
}

void check_conditions(){
  if (Serial.available() > 0){
    v = Serial.read();
    
    if (v == 'w') forward();
    else if (v == 'r') engineOff();
  
      }
    }

void loop(){

  check_conditions();
  check_vibration();
  
  manual_mode();
  
 
  if (angle > 140)angle = 140;
    
  if (angle <  50)angle = 50;
  //Serial.println(angle);
  servo.write(angle);
  
  
  Serial.print(vibration);
  Serial.print("   ");
  Serial.print(v);
  Serial.print("         ");
  Serial.println(angle);
  
}
  

void forward(){
  analogWrite(m11, 255);
  analogWrite(m12, 0);
  analogWrite(m21, 100);
  analogWrite(m22, 0);
}
void backward(){
  analogWrite(m11, 0);
  analogWrite(m12, 255);
  analogWrite(m21, 0);
  analogWrite(m22, 100);
}

void engineOff(){
  analogWrite(m11, 0);
  analogWrite(m12, 0);
  analogWrite(m21, 0);
  analogWrite(m22, 0);  
  
}

void manual_mode(){
  //Serial.println("Autopilot Disabled    ");
 // if (v == 'w')forward();
  if (v == 'a') angle = 140;
  if (v == 'd') angle = 50;
  if (v == 's') backward();
  if (v == 'e') angle = 100;
  //if (v == 'r') engineOff();
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
      digitalWrite(13,HIGH);                                     //Turn on the warning led
      while(1)delay(10);                                         //Stay in this loop for ever
    }

    Wire.beginTransmission(gyro_address);                        //Start communication with the address found during search
    Wire.write(0x1A);                                            //We want to write to the CONFIG register (1A hex)
    Wire.write(0x03);                                            //Set the register bits as 00000011 (Set Digital Low Pass Filter to ~43Hz)
    Wire.endTransmission();                                      //End the transmission with the gyro    

  
}


void check_vibration(){
        //For balancing the propellors it's possible to use the accelerometer to measure the vibrations.
                                                           //The MPU-6050 is installed
        Wire.beginTransmission(gyro_address);                                           //Start communication with the gyro.
        Wire.write(0x3B);                                                               //Start reading @ register 43h and auto increment with every read.
        Wire.endTransmission();                                                         //End the transmission.
        Wire.requestFrom(gyro_address,6);                                               //Request 6 bytes from the gyro.
        while(Wire.available() < 6);                                                    //Wait until the 6 bytes are received.
        acc_x = Wire.read()<<8|Wire.read();                                             //Add the low and high byte to the acc_x variable.
        acc_y = Wire.read()<<8|Wire.read();                                             //Add the low and high byte to the acc_y variable.
        acc_z = Wire.read()<<8|Wire.read();  


       acc_total_vector[0] = sqrt((acc_x*acc_x)+(acc_y*acc_y)+(acc_z*acc_z));          //Calculate the total accelerometer vector.
       //Serial.println(acc_total_vector[0]);   
        acc_av_vector = acc_total_vector[0];                                            //Copy the total vector to the accelerometer average vector variable.
        //Serial.println(acc_z);
        for(start = 5; start > 0; start--){                                            //Do this loop 16 times to create an array of accelrometer vectors.
          acc_total_vector[start] = acc_total_vector[start - 1];                        //Shift every variable one position up in the array.
          acc_av_vector += acc_total_vector[start];                                     //Add the array value to the acc_av_vector variable.
        }

        acc_av_vector /= 6;                                                            //Divide the acc_av_vector by 17 to get the avarage total accelerometer vector.

        if(vibration_counter < 7){                                                     //If the vibration_counter is less than 20 do this.
          vibration_counter ++;                                                         //Increment the vibration_counter variable.
          vibration_total_result += abs(acc_total_vector[0] - acc_av_vector);           //Add the absolute difference between the avarage vector and current vector to the vibration_total_result variable.
        }
        else{
          vibration_counter = 0;                                                        //If the vibration_counter is equal or larger than 20 do this.
          vibration = (vibration_total_result/50);               
          vibration_total_result = 0;                                                   //Reset the vibration_total_result variable.
        }
  
}

  
  
