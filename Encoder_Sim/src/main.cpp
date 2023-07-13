#include <Arduino.h>

// the number of the LED pin
const byte pin_clock_v = 2;

// setting PWM properties
const int freq_clock_v = 83333; // in Hz
const int channel_clock_v = 0;
const int res_clock_v = 8;
 
void setup(){
  // configure LED PWM functionalitites
  ledcSetup(channel_clock_v, freq_clock_v, res_clock_v);
  
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(pin_clock_v, channel_clock_v);

  // set the pwm of clock V channel
  ledcWrite(channel_clock_v, 128);
}

void loop(){
  delay(10);
}