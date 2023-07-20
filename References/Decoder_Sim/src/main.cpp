#include <Arduino.h>
#include <time.h>

// the number of the LED pin
const byte pin_clock_v = 2;
const byte pin_clock_i = 4;
const byte pin_trigger_source = 5;

// setting PWM properties
const uint32_t freq_clock_v = 83333; // in Hz
const byte channel_clock_v = 0;
const byte res_clock_v = 8;

const uint32_t freq_clock_i = 83333; // in Hz
const byte channel_clock_i = 1;
const byte res_clock_i = 8;

void setup(){
  // configure PWM functionalities
  ledcSetup(channel_clock_v, freq_clock_v, res_clock_v);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(pin_clock_v, channel_clock_v);

  // configure PWM functionalities
  ledcSetup(channel_clock_i, freq_clock_i, res_clock_i);
  // attach the channel to the GPIO to be controlled
  ledcAttachPin(pin_clock_i, channel_clock_i);

  // random seed with current time
  srand((unsigned)time(NULL));

  pinMode(pin_trigger_source, OUTPUT);
  Serial.begin(115200);
}

uint8_t duty_cycle_i = 128;
bool random_duty_cycle = false;
bool flop_duty_cycle = false, flop_state = true;
uint8_t flop_counter = 0;

void loop(){
  // set the pwm of clock V channel
  ledcWrite(channel_clock_v, 128);

  // set the pwm of clock I channel
  //ledcWrite(channel_clock_i, random_duty);
  //ledcWrite(channel_clock_i, 52);
  //ledcWrite(channel_clock_i, 191);
  
  if(random_duty_cycle)
  {
    // generate random duty cycle every 6 clocks
    static uint8_t clk_counter = 0;
    static uint8_t random_duty = 0;
    if((clk_counter++) >= 6)
    {
      // random duty cycle from 20% to 80%
      random_duty = 51 + rand() % 153;
      clk_counter = 0;
    }
    ledcWrite(channel_clock_i, random_duty);
  }
  else if(flop_duty_cycle)
  {
    ledcWrite(channel_clock_i, flop_state ? 64 : 191);
    flop_state = !flop_state;
    flop_counter++;
    if(flop_counter == 2)
      flop_state = !flop_state;
    else if(flop_counter == 3)
      flop_counter = 0;
  }
  else
    ledcWrite(channel_clock_i, duty_cycle_i);

  if(Serial.available())
  {
    String buffer = Serial.readStringUntil('\r');
    String command = "", parameter = "";
    int split_index = buffer.indexOf(" ");
    if(split_index < 0)
      command = buffer;
    else
    {
      command = buffer.substring(0, split_index);
      parameter = buffer.substring(split_index + 1, buffer.length());
    }

    if(command == "duty")
    {
      if(parameter == "random")
      {
        random_duty_cycle = true;
        flop_duty_cycle = false;
      }
      else if(parameter == "flop")
      {
        random_duty_cycle = false;
        flop_duty_cycle = true;
      }
      else
      {
        random_duty_cycle = false;
        flop_duty_cycle = false;
        duty_cycle_i = parameter.toInt();
      }
      Serial.print("Duty Changed\r");
    }
    else
    {
      ledcWrite(channel_clock_i, 52); // send high clock bits

      digitalWrite(pin_trigger_source, HIGH);
      delayMicroseconds(12);
      digitalWrite(pin_trigger_source, LOW);
      Serial.print("Triggerring Signal Sent\r");
    }
  }
}