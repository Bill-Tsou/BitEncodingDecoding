#include <Arduino.h>

#define SERIAL_TERMINATOR '\r'
#define MAX_SEND_CHAR_SIZE  8
#define MAX_DATA_SIZE (8 * MAX_SEND_CHAR_SIZE + 2)  // including start bit and end bit

const byte pin_clock_v = 4;
const byte pin_encode_output = 22;

volatile bool new_data_arrived = false;
volatile bool sending_new_data = false;
volatile bool send_data[MAX_DATA_SIZE];  // transfer 8 characters with start bit and end bit
volatile uint8_t clock_v_counter = 0;
volatile uint8_t sending_data_bit_index = 0;

void IRAM_ATTR isr_Callback();

void setup()
{
  pinMode(pin_encode_output, OUTPUT);
  pinMode(pin_clock_v, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(pin_clock_v), isr_Callback, RISING);

  Serial.begin(115200);
}

//volatile unsigned long micro1 = 0, micro2 = 0;

void loop()
{
  if(Serial.available())
  {
    String cmd_buf = Serial.readStringUntil(SERIAL_TERMINATOR);
    int splitter_index = cmd_buf.indexOf(' ');
    String command = "", parameter = "";
    if(splitter_index < 0)
      command = cmd_buf;
    else
    {
      command = cmd_buf.substring(0, splitter_index);
      parameter = cmd_buf.substring(splitter_index + 1, cmd_buf.length());
    }
    if(command == "encode" && !sending_new_data)
    {
      const char *data = parameter.c_str();
      volatile bool *ptr_bit = send_data;
      *ptr_bit++ = true;  // start bit
      for(uint8_t i = 0; i < MAX_SEND_CHAR_SIZE; i++)
        for(uint8_t j = 0; j < 8; j++)  // 8 bits for each character
          *ptr_bit++ = (data[i] >> (7 - j)) & 1;
      *ptr_bit++ = true;  // end bit
      new_data_arrived = true;

// for(int i = 0; i < MAX_SEND_CHAR_SIZE; i++)
//   for(int j = 0; j < MAX_DATA_SIZE; j++)
//     Serial.printf("send_data[%d][%d] = %d\r", i, j, send_data[i].bit[j]);
// Serial.println();
// do
// {
//   isr_Callback();
//   delayMicroseconds(1);
// }while(sending_new_data);
    }
  }
  else
  {
    // for(uint8_t i = 0; i < 10; i++)
    //   Serial.printf("0x%X ", send_data[i]);
    // Serial.println();
  }
}

void IRAM_ATTR isr_Callback()
{
  // if(micro1 < micro2)
  // {
  //   micro1 = micros();
  //   micro2 = 0;
  // }
  // else if(micro2 == 0)
  //   micro2 = micros();

  static bool wait_for_transmission = false, end_transmit = false;
  //static bool pause_bit = false;
  //static uint8_t pause_clock_counter = 0;
  if(new_data_arrived)
  {
    clock_v_counter = 0;
    sending_data_bit_index = 0;
    new_data_arrived = false;
    sending_new_data = true;
  }
  else if(wait_for_transmission)
  {
    if((++clock_v_counter) % 6 == 0)
      wait_for_transmission = false;
  }
  // else if(pause_bit)
  // { // wait for previous bit transmit completed
  //   digitalWrite(pin_encode_output, LOW);
  //   // pause for 2 clock cycles
  //   if(++pause_clock_counter >= 2)
  //   {
  //     pause_clock_counter = 0;
  //     pause_bit = false;
  //   }
  // }
  else if(end_transmit)
  { // recover until the final bit transmitted
    digitalWrite(pin_encode_output, LOW);
    end_transmit = false;
    sending_new_data = false;
  }
  // else if(start_bit | end_bit)
  // {
  //   clock_v_counter++;
  //   if(clock_v_counter >= 6)
  //     start_bit = end_bit = false;
  // }
  else if(sending_new_data)
  {
    if(clock_v_counter == 0)
    {
      // output the first bit from data
      bool output_bit = send_data[sending_data_bit_index];
      digitalWrite(pin_encode_output, output_bit);
//Serial.println("data_index = " + String(sending_data_index) + ", data_bit_index = " + String(sending_data_bit_index) + ", output = " + output_bit);
      //gpio_set_level()
      // move on to the next byte needs to be send
      sending_data_bit_index++;
      if(sending_data_bit_index >= MAX_DATA_SIZE)
      {
        sending_data_bit_index = 0;
        //  pause_bit = true;

        // complete data sending
        wait_for_transmission = true;
        end_transmit = true;
      }
    }
    clock_v_counter = (++clock_v_counter) % 6;
  }
}