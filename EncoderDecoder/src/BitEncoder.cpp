#include "BitEncoder.h"

volatile bool new_data_arrived = false;
volatile bool sending_new_data = false;
volatile bool send_data[MAX_DATA_BITS];  // transfer 8 characters with start bit and end bit
volatile uint8_t clock_v_counter = 0;
volatile uint8_t sending_data_bit_index = 0;

void BitEncoderSetup()
{
    pinMode(pin_encode_output, OUTPUT);
    pinMode(pin_clock_v, INPUT_PULLDOWN);
    attachInterrupt(digitalPinToInterrupt(pin_clock_v), isr_Callback, RISING);
}

void BitEncoderEnd()
{   // detach interrupt from encoding mode
    detachInterrupt(digitalPinToInterrupt(pin_clock_v));
}

bool IsEncoderSendingNewData()
{ return sending_new_data; }

volatile bool* GetEncodeSendDataPtr()
{ return send_data; }

void EncodeSendData()
{ new_data_arrived = true; }

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
    if((++clock_v_counter) % CLK_CYCLE_NUM == 0)
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
  //   if(clock_v_counter >= CLK_CYCLE_NUM)
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

      // move on to the next byte needs to be send
      sending_data_bit_index++;
      if(sending_data_bit_index >= MAX_DATA_BITS)
      {
        sending_data_bit_index = 0;
        //  pause_bit = true;

        // complete data sending
        wait_for_transmission = true;
        end_transmit = true;
      }
    }
    clock_v_counter = (++clock_v_counter) % CLK_CYCLE_NUM;
  }
}