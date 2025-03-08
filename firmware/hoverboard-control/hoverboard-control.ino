
#define HoverSerial Serial1  

#include "hoverboardcomm.h"



void setup() {
  Serial.begin(SERIAL_BAUD);      
  Serial.println("Hoverboard Serial v1.1 - Manual Control");

  HoverSerial.begin(HOVER_SERIAL_BAUD);
  pinMode(LED_BUILTIN, OUTPUT);
}


void loop() { 

  hoverboard_Receive(); 
  Serial.print(feedback_speedR);
  hoverboard_Set_speed(100);
  
  delay(200);  
  hoverboard_Set_speed(0);
  delay(200); 
  hoverboard_Set_speed(-100);
  delay(200); 
}
