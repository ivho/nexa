#include "Arduino.h"
static int led = 4;
void setup() {             
  Serial.begin(9600);  
  // initialize the digital pin as an output.
//  pinMode(led, OUTPUT);     
}

// the loop routine runs over and over again forever:
void loop() {
  Serial.print("HELLO\n");
#if 0
  digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
  delay(1000);               // wait for a second
  digitalWrite(led, LOW);    // turn the LED off by making the voltage LOW
#endif
  delay(1000);               // wait for a second
}