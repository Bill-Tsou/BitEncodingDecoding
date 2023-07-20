#include <Arduino.h>

#define CLK_CYCLE_NUM       6
#define DATA_CHAR_BITS      8       // 8 bits for a single character
#define MAX_CHAR_NUM        8       // send or receive 8 chars in a row
#define MAX_DATA_BITS       (DATA_CHAR_BITS * MAX_CHAR_NUM + 2) // including start bit and end bit

/** parameters of UART */
#define SERIAL_TERMINATOR '\r'
#define SERIAL_RESPONSE(msg) Serial.print(msg + String(SERIAL_TERMINATOR))

// the number of the LED pin
const byte pin_clock_v = 4;   // V clock input for interrupt
const byte pin_trigger_source = 5;
const byte pin_clock_i_sim = 22;  // I clock output for simulation

volatile bool sending_new_data = false;
volatile bool send_data[MAX_DATA_BITS];  // transfer 8 characters with start bit and end bit
volatile uint8_t clock_v_counter = 0;
volatile uint8_t sending_data_bit_index = 0;

void IRAM_ATTR isr_Callback();

void setup()
{
  pinMode(pin_clock_v, INPUT_PULLDOWN);
  pinMode(pin_clock_i_sim, OUTPUT);
  pinMode(pin_trigger_source, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(pin_clock_v), isr_Callback, RISING);
  Serial.begin(115200);
}

void loop()
{
  if(Serial.available())
  {
    String buffer = Serial.readStringUntil(SERIAL_TERMINATOR);
    String command = "", parameter = "";
    int split_index = buffer.indexOf(" ");
    if(split_index < 0)
      command = buffer;
    else
    {
      command = buffer.substring(0, split_index);
      parameter = buffer.substring(split_index + 1, buffer.length());
    }

    if(command == "mode")
    { // only for V & I encode mode
      SERIAL_RESPONSE("OK");
    }
    else if(command == "encode")
    {
      if(parameter.length() != MAX_CHAR_NUM)
        SERIAL_RESPONSE("Invalid Data Num, which should be " + String(MAX_CHAR_NUM));
      else
      {
        SERIAL_RESPONSE("OK");

        const char *data = parameter.c_str();
        // encode data to binary ASCII code
        volatile bool *ptr_sim = send_data;
        *ptr_sim++ = true;  // start bit
        for(uint8_t i = 0; i < MAX_CHAR_NUM; i++)
            for(uint8_t j = 0; j < DATA_CHAR_BITS; j++)  // 8 bits for each character
                *ptr_sim++ = (data[i] >> (7 - j)) & 1;
        *ptr_sim++ = true;  // end bit

        // put a trigger source
        digitalWrite(pin_trigger_source, HIGH);
        delayMicroseconds(12);
        digitalWrite(pin_trigger_source, LOW);

        clock_v_counter = 0;
        sending_data_bit_index = 0;
        sending_new_data = true;
      }
    }
  }
}

void IRAM_ATTR isr_Callback()
{
  if(sending_new_data)
  {
    digitalWrite(pin_clock_i_sim, HIGH);
      send_data[sending_data_bit_index] ? delayMicroseconds(1) : delayMicroseconds(5);
    digitalWrite(pin_clock_i_sim, LOW);

    if(clock_v_counter == 0)
    {
      // move on to the next byte needs to be send
      sending_data_bit_index++;
      if(sending_data_bit_index >= MAX_DATA_BITS)
        sending_new_data = false;
    }
    clock_v_counter = (++clock_v_counter) % CLK_CYCLE_NUM;
  }
}