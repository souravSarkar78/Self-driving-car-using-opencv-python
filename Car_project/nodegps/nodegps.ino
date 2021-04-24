#include "TinyGPS++.h"
#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <SoftwareSerial.h>

BlynkTimer timer;


const char* ssid = "YOUR_SSID";
const char* pass = "YOUR_PASS";


char auth[] = "YOUR AUTHENTICATION CODE";

float latitude, longitude;
String mail;
SoftwareSerial serial_connection(4, 5); //RX=pin 10, TX=pin 11
TinyGPSPlus gps;           //This is the GPS object that will pretty much do all the grunt work with the NMEA data
WidgetMap  carMap(V0);


void setup()
{
  
  Serial.begin(115200);
  
  Blynk.begin(auth, ssid, pass); 
  Serial.println("GPS Start");
  serial_connection.begin(9600);
  pinMode(D4, INPUT);

  //timer.setInterval(1000L, prinnt);
}

void loop()

{
  
while (serial_connection.available() > 0) 
    {
      // sketch displays information every time a new sentence is correctly encoded.
      if (gps.encode(serial_connection.read()))
      if (gps.location.isValid() ){
        float latitude = (gps.location.lat());     //Storing the Lat. and longitude. 
        float longitude = (gps.location.lng()); 

    //Blynk.virtualWrite(V1, String(latitude, 6));   
    //Blynk.virtualWrite(V2, String(longitude, 6));  
      mail = " Your Car's Location is   https://www.google.com/maps/place/";

      mail += String(latitude, 6);
      mail += ",";
      mail += String(longitude, 6);
    
    carMap.location(1, String(latitude, 5), String(longitude, 5), "GPS_Location");
    
       Blynk.virtualWrite(V3, gps.speed.kmph());
       
    
       Blynk.virtualWrite(V4, gps.satellites.value());
      }
  }

 //Sending email to phone

  if (digitalRead(D4) == 1){
    Blynk.notify(mail);
    Blynk.notify("Accident");
    Blynk.email("Accident Detected   ", mail);
  }
  Blynk.run();
  timer.run();

  Serial.print(gps.location.lat(), 6);
    Serial.print("   ");
      Serial.println(digitalRead(D4));
  //timer.run();

}
