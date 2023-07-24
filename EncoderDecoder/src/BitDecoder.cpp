#include "BitDecoder.h"

// with default threshold values 1s / 83333 Hz = 12us, so 50% overlap = 6us
volatile uint16_t overlap_trig_th = 4;
volatile uint16_t overlap_micro_th = 4;
volatile bool new_raw_data = false;

volatile bool trig_moving_array[CLK_CYCLE_NUM] = {false};
volatile bool raw_cycle_result[MAX_CYCLE_DATA];
volatile uint16_t cycle_index = 0;

volatile unsigned long micro_prev_v = 0, micro_prev_i = 0;
volatile bool is_first_data_bit = false;
// for debug
//volatile uint8_t micro_diff_result[MAX_CYCLE_DATA];
volatile unsigned long micro_diff = 0;
volatile bool is_triggered = false;
volatile uint8_t pin_i_clock_selection = pin_clock_i1;

void BitDecoderSetup()
{
#ifdef PHYSICAL_SIMULATION
    pinMode(pin_trigger_source1, INPUT_PULLDOWN);
#endif

    pinMode(pin_clock_v, INPUT_PULLDOWN);
    pinMode(pin_clock_i1, INPUT_PULLDOWN);

#ifdef TWO_I_CLOCKS
    pinMode(pin_clock_i2, INPUT_PULLDOWN);
    pinMode(pin_trigger_source2, INPUT_PULLDOWN);
#endif

#ifdef PHYSICAL_SIMULATION
    // wait until trigger source is triggered
    switch_trigger_source(WAIT_FOR_TRIGGER);
#else
    // constantly perspecting whether I is lagged V clock signal
    switch_trigger_source(TRIGGER_SOURCE1);
#endif
}

void BitDecoderEnd()
{
  detachInterrupt(digitalPinToInterrupt(pin_clock_v));
#ifdef PHYSICAL_SIMULATION
  detachInterrupt(digitalPinToInterrupt(pin_trigger_source1));
#endif

#ifdef TWO_I_CLOCKS
  detachInterrupt(digitalPinToInterrupt(pin_trigger_source2));
#endif
}

bool Decode_ChangeVI_Th(uint8_t overlap_percentage)
{
  if(overlap_percentage < 10 || overlap_percentage > 80)
    return false; // invalid parameter
  overlap_micro_th = (uint16_t)(1.0f / RECV_CLK_RATE * 1e6 * (overlap_percentage * 0.01));
  return true;
}

void DecodeRawCycleData(const volatile bool *raw_cycle_data, bool *start_bit, bool *sync_bit, bool *end_bit, uint8_t *decoded_id, char *decoded_result)
{
  uint16_t c_index = 0;
  bool bit_result[DATA_CHAR_BITS];
  uint8_t h_counter = 0;  // high level counter

  // extract start bit
  for(uint8_t c = 0; c < CLK_CYCLE_NUM; c++)
    if(raw_cycle_data[c_index++] == 1)
      h_counter++;
  *start_bit = (h_counter > 3 ? true : false);

  // extract sync bit
  h_counter = 0;
  for(uint8_t c = 0; c < CLK_CYCLE_NUM; c++)
    if(raw_cycle_data[c_index++] == 1)
      h_counter++;
  *sync_bit = (h_counter > 3 ? true : false);

  // extract ID bit
  h_counter = 0;
  for(uint8_t c = 0; c < CLK_CYCLE_NUM; c++)
    if(raw_cycle_data[c_index++] == 1)
      h_counter++;
  *decoded_id = (h_counter > 3 ? 1 : 0);

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

// volatile uint8_t* GetDecodeMicroDiffResults()
// { return micro_diff_result; }

void IRAM_ATTR switch_trigger_source(Trig_State_t new_state)
{
  switch(new_state)
  {
    case TRIGGER_SOURCE1:
#ifdef TWO_I_CLOCKS
    case TRIGGER_SOURCE2:
#endif

#ifdef PHYSICAL_SIMULATION
      detachInterrupt(digitalPinToInterrupt(pin_trigger_source1));
#ifdef TWO_I_CLOCKS
      detachInterrupt(digitalPinToInterrupt(pin_trigger_source2));
      pin_i_clock_selection = (new_state == TRIGGER_SOURCE1) ? pin_clock_i1 : pin_clock_i2;
#endif
      // initialize variables
      is_triggered = true;
      is_first_data_bit = true;
      cycle_index = 0;
      for(uint8_t i = 0; i < 6; i++)
        raw_cycle_result[i] = 0;
#endif

      // start decoding from V and I
      attachInterrupt(digitalPinToInterrupt(pin_clock_v), isr_trig_rising, RISING);

    break;

    default:
#ifdef PHYSICAL_SIMULATION
      // return to triggering state
      detachInterrupt(digitalPinToInterrupt(pin_clock_v));
      attachInterrupt(digitalPinToInterrupt(pin_trigger_source1), isr_trig_source1, RISING);
#endif

#ifdef TWO_I_CLOCKS
      attachInterrupt(digitalPinToInterrupt(pin_trigger_source2), isr_trig_source2, RISING);
#endif
      is_triggered = false;
  }
}

void IRAM_ATTR isr_trig_source1()
{
  switch_trigger_source(TRIGGER_SOURCE1);
}

#ifdef TWO_I_CLOCKS
void IRAM_ATTR isr_trig_source2()
{
  switch_trigger_source(TRIGGER_SOURCE2);
}
#endif

void IRAM_ATTR isr_trig_rising()
{
  micro_prev_v = micros();

  uint8_t diff = 0;
  do
  {
    diff = micros() - micro_prev_v;
  } while (diff < overlap_trig_th);  // consider latency when triggering isr for 1us
  
  uint8_t h_cycle_samples = 0;  // high cycle samples
  const uint8_t sample_cycles = 5;
  for(uint8_t i = 0; i < sample_cycles; i++)
    (digitalRead(pin_i_clock_selection) ? h_cycle_samples++ : h_cycle_samples);
  
  bool cycle_res = !(h_cycle_samples > (sample_cycles / 2));

  uint8_t idx;
  h_cycle_samples = 0;
  for(idx = 0; idx < CLK_CYCLE_NUM - 1; idx++)
  {
    trig_moving_array[idx] = trig_moving_array[idx + 1];
    trig_moving_array[idx] ? h_cycle_samples++ : h_cycle_samples;
  }
  trig_moving_array[idx] = cycle_res;
  trig_moving_array[idx] ? h_cycle_samples++ : h_cycle_samples;

  if(h_cycle_samples <= CLK_CYCLE_NUM / 2 + 1)
    return;

  detachInterrupt(digitalPinToInterrupt(pin_clock_v));

  // enter triggering state
  is_triggered = true;
  is_first_data_bit = true;
  cycle_index = 0;
  for(uint16_t i = 0; i < CLK_CYCLE_NUM; i++)
    raw_cycle_result[i] = trig_moving_array[i];
  
  attachInterrupt(digitalPinToInterrupt(pin_clock_v), isr_decode_rising, RISING);
}

void IRAM_ATTR isr_decode_rising()
{
  micro_prev_v = micros();

  uint8_t diff = 0;
  do
  {
    diff = micros() - micro_prev_v;
  } while (diff < overlap_micro_th);  // consider latency when triggering isr for 1us
  
  uint8_t h_cycle_samples = 0;  // high cycle samples
  const uint8_t sample_cycles = 5;
  for(uint8_t i = 0; i < sample_cycles; i++)
    (digitalRead(pin_i_clock_selection) ? h_cycle_samples++ : h_cycle_samples);
  
  bool cycle_res = !(h_cycle_samples > (sample_cycles / 2));

  // search for the first data bit
  if(is_first_data_bit && (cycle_res == 0))
  {
    is_first_data_bit = false;
    cycle_index = 6;
  }
  else if(cycle_index >= MAX_CYCLE_DATA)
  {
#ifdef PHYSICAL_SIMULATION
    switch_trigger_source(WAIT_FOR_TRIGGER);
#else
    detachInterrupt(digitalPinToInterrupt(pin_clock_v));

    is_triggered = false;
    for(uint8_t i = 0; i < CLK_CYCLE_NUM; i++)
      trig_moving_array[i] = false;

    // determine whether the received data is valid
    h_cycle_samples = 0;
    for(int i = 0; i < MAX_CYCLE_DATA; i++)
      if(raw_cycle_result[i])
        h_cycle_samples++;
    if(h_cycle_samples < MAX_CYCLE_DATA / 6)
    {
      BitDecoderSetup();
      return;
    }

#endif
    new_raw_data = true;
    return;
  }

  //micro_diff_result[cycle_index] = diff;
  raw_cycle_result[cycle_index++] = cycle_res;
}

/*
void IRAM_ATTR isr_i_falling()
{
  micro_prev_i = micros();
micro_diff = micro_prev_i - micro_prev_v;
  //if(micro_diff == 0)
  // return;
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

  micro_diff_result[cycle_index] = micro_diff;
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
}*/