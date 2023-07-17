#include "BitDecoder.h"

// with default threshold values 1s / 83333 Hz = 12us, so 50% overlap = 6us
volatile uint16_t overlap_micro_th = 6;
volatile bool new_raw_data = false;

volatile bool raw_cycle_result[MAX_CYCLE_DATA];
volatile uint16_t cycle_index = 0;

volatile unsigned long micro_prev_v = 0, micro_prev_i = 0;
volatile bool is_first_data_bit = false;
// for debug
volatile unsigned long micro_diff = 0;
volatile bool is_triggered = false;

void BitDecoderSetup()
{
    //pinMode(pin_decode_output, OUTPUT);
    pinMode(pin_trigger_source, INPUT_PULLDOWN);
    pinMode(pin_clock_v, INPUT_PULLDOWN);
    pinMode(pin_clock_i, INPUT_PULLDOWN);

    // wait until trigger source is triggered
    switch_trigger_source(false);
}

void BitDecoderEnd()
{
  detachInterrupt(digitalPinToInterrupt(pin_clock_v));
  detachInterrupt(digitalPinToInterrupt(pin_clock_i));
  detachInterrupt(digitalPinToInterrupt(pin_trigger_source));
}

bool Decode_ChangeVI_Th(uint8_t overlap_percentage)
{
  if(overlap_percentage < 20 || overlap_percentage > 80)
    return false; // invalid parameter
  overlap_micro_th = (uint16_t)(1.0f / RECV_CLK_RATE * 1e6 * (overlap_percentage * 0.01));
  return true;
}

void DecodeRawCycleData(const volatile bool *raw_cycle_data, bool *start_bit, bool *end_bit, char *decoded_result)
{
  uint16_t c_index = 0;
  bool bit_result[DATA_CHAR_BITS];
  uint8_t h_counter = 0;  // high level counter

  // extract start bit
  for(uint8_t c = 0; c < CLK_CYCLE_NUM; c++)
    if(raw_cycle_data[c_index++] == 1)
      h_counter++;
  *start_bit = (h_counter > 3 ? true : false);

  // extract data bits
  for(uint8_t data_index = 0; data_index < MAX_CHAR_NUM; data_index++)
  {
    for(uint8_t bit_index = 0; bit_index < DATA_CHAR_BITS; bit_index++)
    {
      h_counter = 0;
      for(uint8_t c = 0; c < CLK_CYCLE_NUM; c++)
        if(raw_cycle_data[c_index++] == 1)
          h_counter++;
      bit_result[bit_index] = (h_counter > 3 ? 1 : 0);
    }
    // compose the character
    decoded_result[data_index] = (char) 0;
    for(uint8_t i = 0; i < DATA_CHAR_BITS; i++)
      decoded_result[data_index] |= (char)(bit_result[i] << (7 - i));
  }

  // extract end bit
  h_counter = 0;
  for(uint8_t c = 0; c < CLK_CYCLE_NUM; c++)
    if(raw_cycle_data[c_index++] == 1)
      h_counter++;
  *end_bit = (h_counter > 3 ? true : false);
}

bool IsDecodeTriggered()
{ return is_triggered; }

bool IsDecodeRawDataReceived()
{ return new_raw_data; }

void SetDecodeRawDataReceived(bool new_state)
{ new_raw_data = new_state; }

volatile bool* GetDecodeRawCycleResults()
{ return raw_cycle_result; }

void IRAM_ATTR switch_trigger_source(bool new_state)
{
  if(new_state)
  {
    detachInterrupt(digitalPinToInterrupt(pin_trigger_source));
    
    // initialize variables
    is_triggered = true;
    is_first_data_bit = true;
    cycle_index = 0;
    for(uint8_t i = 0; i < 6; i++)
      raw_cycle_result[i] = 0;
    
    // start decoding from V and I
    attachInterrupt(digitalPinToInterrupt(pin_clock_v), isr_v_rising, RISING);
    attachInterrupt(digitalPinToInterrupt(pin_clock_i), isr_i_falling, FALLING);
  }
  else
  { // return to triggering state
    detachInterrupt(digitalPinToInterrupt(pin_clock_v));
    detachInterrupt(digitalPinToInterrupt(pin_clock_i));
    attachInterrupt(digitalPinToInterrupt(pin_trigger_source), isr_trig_source, RISING);
    is_triggered = false;
  }
}

void IRAM_ATTR isr_trig_source()
{
  //cycle_index = bit_index = data_index = 0;
  switch_trigger_source(true);
}

void IRAM_ATTR isr_v_rising()
{
  micro_prev_v = micros();
}

void IRAM_ATTR isr_i_falling()
{
  micro_prev_i = micros();
micro_diff = micro_prev_i - micro_prev_v;
  if(micro_diff == 0)
   return;
  bool cycle_res = (micro_diff > overlap_micro_th) ? 0 : 1;

//digitalWrite(pin_decode_output, cycle_res);
  
  // search for the first data bit
  if(is_first_data_bit && (cycle_res == 0))
  {
    is_first_data_bit = false;
    // skip the start bit when found the first data bit as 0
    //while(cycle_index < 6)
    //  raw_cycle_result[cycle_index++] = 0;
    cycle_index = 6;
  }
  else if(cycle_index >= MAX_CYCLE_DATA)
  {
    switch_trigger_source(false);
    new_raw_data = true;
    return;
  }

  raw_cycle_result[cycle_index++] = cycle_res;

//   cycle_result[cycle_index++] = (micro_diff > overlap_micro_th) ? 0 : 1;
//   if(cycle_index >= CYCLE_NUM)
//   {
//     cycle_index = 0;
// new_decoded_data = true;

//     volatile uint8_t h_counter = 0;  // high cycle bit level counter
//     //volatile bool *ptr_cycle = cycle_result;
//     for(uint8_t i = 0; i < CYCLE_NUM; i++)
//       if(cycle_result[i] == 1)
//         h_counter++;
//     bit_result[bit_index++] = (h_counter > 3) ? 1 : 0;

//     if(bit_index >= DATA_BIT_NUM)
//     {
//       bit_index = 0;

//       //decoded_result[data_index] = (char)0;
//       uint8_t byte_res = 0;
//       for(uint8_t i = 0; i < 8; i++)
//         byte_res |= (bit_result[i] << (7 - i));
//         //decoded_result[data_index] |= (char) ((1 & bit_result[i]) << (7 - i));
//       //data_index++;
//       decoded_result[data_index++] = (char)byte_res;
//       if(data_index >= MAX_CHAR_NUM)
//       {
//         // end of decoding, return to trigger state
//         digitalWrite(pin_decode_output, LOW);
//         switch_trigger_source(false);
//         new_decoded_data = true;
//       }
//     }
//   }
}