#include<Servo.h>
Servo servo;
char v;
int error;
int angle;

int m11=9, m12=6, m21=3, m22=5;
boolean manual = false;
float kp = 23;
float ki = 0.08;
float kd = 10;
float p, i, d;
int baseAngle = 1700, maxLim = 1950, minLim = 1350;
int pid, prev_error;
int range = 250;
void setup(){
  Serial.begin(9600);
  servo.attach(7);
  servo.writeMicroseconds(baseAngle);
  delay(1000);
}

void check_conditions(){
  if (Serial.available() > 0){
    v = Serial.read();
    
    if (v == 'w') forward();
    else if (v == 'r') engineOff();
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
  Serial.println(angle);
  Serial.print("   ");
  servo.writeMicroseconds(angle);
  
  
 /* Serial.println(p);
  Serial.print("   ");
  Serial.println(i);
  Serial.print("         ");
  Serial.println(angle);*/
  
}
  
  
void calculate_pid(){
  p = error * kp;
  
  if ((error <= 5) && (error >=-5)){
    
  i = i + (ki*error);
  if (i > 100)i = 100;
  if (i< -100)i = -100;
  }
  
  
  d = kd*(error - prev_error);
  Serial.println(i);
  
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
  //Serial.println("Autopilot Disabled    ");
 // if (v == 'w')forward();
  if (v == 'a') angle = maxLim;
  if (v == 'd') angle = minLim;
  if (v == 's') backward();
  if (v == 'e') angle = baseAngle;
  //if (v == 'r') engineOff();
}
  

  
  
