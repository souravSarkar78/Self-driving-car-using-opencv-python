float speeds= 331.13; //349.09; //meter per second
float pingTime;
float distance;
int t=10,e=11;
int acc_counter=0, count = 0;
byte closed_s = false;
long acc_alarm=0;
bool lightAuto, lightManu;

void setup() {
  pinMode(A0, INPUT);
  pinMode(13, OUTPUT);
  pinMode(9, OUTPUT);
  pinMode(7, OUTPUT);
  pinMode(t,OUTPUT);
  pinMode(e,INPUT);
  pinMode(4,INPUT);
  Serial.begin(9600);

}

void loop() {

  
  checkDarkStreet();
  if (digitalRead(8) == 1) alarm();
  else get_distance();
  //Serial.println(digitalRead(13));
}

/* Automatic Light System*/

void checkDarkStreet(){
  int light = analogRead(A0);
  if (light < 180) lightAuto = true;
  else lightAuto = false;
  Serial.println(light);

  if (digitalRead(4) == 1) lightManu = true;
  else lightManu = false;

  if (lightAuto == true || lightManu == true) analogWrite(9, 110);
  else digitalWrite(9, 0);
}

/*Detecting objects on front of the car via ultrasonic sensor*/

void get_distance(){
  digitalWrite(t,LOW);
  delayMicroseconds(6);
  digitalWrite(t,HIGH);
  delayMicroseconds(2);
  digitalWrite(t,LOW);

  pingTime=pulseIn(e,HIGH);
  pingTime=pingTime/1000000;  //microseconds to seconds

  distance= speeds * pingTime; 
  distance= distance/2;
  distance= distance*100;   //Converts meter to cm

  if (distance < 18) 
  {
    accident_alarm();
    digitalWrite(7, 1);
  }
  else {
    digitalWrite(7, 0);
    acc_counter = 0;
    acc_alarm = 0;
    digitalWrite(13 ,0);
  }
  Serial.print(distance);
  Serial.print("   ");
  /*Serial.print(acc_counter);
  Serial.print("   ");
  Serial.println(acc_alarm);*/
}

void accident_alarm(){
    if (acc_alarm == 5 && acc_counter < 6){
       acc_counter ++;
       //Serial.println(acc_counter);
        digitalWrite(13, !digitalRead(13));
        
        acc_alarm = 0;
      }
      acc_alarm ++;  
      if (acc_alarm > 7)acc_alarm = 0;
      //Serial.println(acc_alarm);
}

void alarm(){

  if (count == 15){
    digitalWrite(13, !digitalRead(13));
    count = 0;
  }
  count ++;
  
}
