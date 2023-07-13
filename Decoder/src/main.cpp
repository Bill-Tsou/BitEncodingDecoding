#include <Arduino.h>

#define SERIAL_TERMINATOR '\r'

#define CLK_RATE            83333   // in Hz
#define CYCLE_NUM           6
#define DATA_BIT_NUM        8
#define MAX_DATA_SIZE       8       // send 8 chars in a row
const uint16_t MAX_CYCLE_DATA = (MAX_DATA_SIZE * DATA_BIT_NUM + 2) * CYCLE_NUM;  // including start bit and end bit

const byte pin_clock_v = 4;
const byte pin_clock_i = 5;
const byte pin_trigger_source = 21;
const byte pin_decode_output = 22;

// with default threshold values 1s / 83333 Hz = 12us, so 50% overlap = 6us
volatile uint16_t overlap_micro_th = 6;
volatile bool new_raw_data = false;

volatile bool raw_cycle_result[MAX_CYCLE_DATA];
volatile uint16_t cycle_index = 0;

// volatile uint8_t cycle_index = 0;
// volatile uint8_t bit_index = 0;
// volatile uint8_t data_index = 0;
// volatile bool cycle_result[CYCLE_NUM];
// volatile bool bit_result[DATA_BIT_NUM];
// volatile char decoded_result[MAX_DATA_SIZE];

/**
 * @brief change V & I signal overlapping threshold 
 * @param overlap_percentage between 20 to 80 percent
 */
void ChangeOverlapPercentage(uint8_t overlap_percentage);

void DecodeRawCycleData(const volatile bool *raw_cycle_data, bool *start_bit, bool *end_bit, char *decoded_result);

void IRAM_ATTR switch_trigger_source(bool new_state);
void IRAM_ATTR isr_trig_source();
void IRAM_ATTR isr_v_rising();
void IRAM_ATTR isr_i_falling();

void setup()
{
  pinMode(pin_decode_output, OUTPUT);
  pinMode(pin_trigger_source, INPUT_PULLDOWN);
  pinMode(pin_clock_v, INPUT_PULLDOWN);
  pinMode(pin_clock_i, INPUT_PULLDOWN);

  switch_trigger_source(false);
  //switch_trigger_source(true);

  Serial.begin(115200);

  // set the threshold of V & I overlapping percentage
  ChangeOverlapPercentage(40);
Serial.printf("overlap time = %d\r", overlap_micro_th);
}

volatile unsigned long micro_prev_v = 0, micro_prev_i = 0;
volatile bool is_first_data_bit = false;
// for debug
volatile unsigned long micro_diff = 0;
volatile bool is_triggered = false;

void loop()
{
  if(is_triggered)
  { // do nothing when triggered, but record data to raw_cycle_result array
    delay(100);
  }
  else if(Serial.available())
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

    if(command == "th")
    {
      ChangeOverlapPercentage(parameter.toInt());
      Serial.print("V I Overlapped Percentage Changed" + String(SERIAL_TERMINATOR));
    }

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
  else
  {
    // if(new_decoded_data)
    // {
    //   new_decoded_data = false;
    //   for(uint8_t i = 0; i < MAX_DATA_SIZE; i++)
    //     Serial.printf("%d ", (uint8_t)decoded_result[i]);
    //   Serial.print(SERIAL_TERMINATOR);
    // }
    if(new_raw_data)
    {
      new_raw_data = false;
      uint16_t raw_data_idx = 0;

      // display start bit array
      for(; raw_data_idx < 6; raw_data_idx++)
        Serial.printf("%d ", (uint8_t)raw_cycle_result[raw_data_idx]);

      // display data array
      for(; raw_data_idx < (MAX_DATA_SIZE * DATA_BIT_NUM + 1) * CYCLE_NUM; raw_data_idx++)
      {
        if((raw_data_idx - 6) % 48 == 0)
          Serial.print(SERIAL_TERMINATOR);
        Serial.printf("%d ", (uint8_t)raw_cycle_result[raw_data_idx]);
      }
      Serial.print(SERIAL_TERMINATOR);

      // display end bit array
      for(; raw_data_idx < MAX_CYCLE_DATA; raw_data_idx++)
        Serial.printf("%d ", (uint8_t)raw_cycle_result[raw_data_idx]);
      Serial.print(SERIAL_TERMINATOR);

      // start decoding raw cycle data
      char decode_result[MAX_DATA_SIZE];
      bool start_bit, end_bit;
      DecodeRawCycleData(raw_cycle_result, &start_bit, &end_bit, decode_result);
      Serial.printf("Start Bit = %d, End Bit = %d", (uint8_t)start_bit, (uint8_t)end_bit);
      Serial.print(SERIAL_TERMINATOR);
      Serial.print("Decoded Result: ");
      for(uint8_t i = 0; i < MAX_DATA_SIZE; i++)
        Serial.printf("%d ", (uint8_t)decode_result[i]);
      Serial.print(SERIAL_TERMINATOR);
    }
//Serial.printf("diff=%lu\r", micro_diff);
//delay(100);
  }
}

void ChangeOverlapPercentage(uint8_t overlap_percentage)
{
  if(overlap_percentage < 20 || overlap_percentage > 80)
    return; // invalid parameter
  overlap_micro_th = (uint16_t)(1.0f / CLK_RATE * 1e6 * (overlap_percentage * 0.01));
}

void DecodeRawCycleData(const volatile bool *raw_cycle_data, bool *start_bit, bool *end_bit, char *decoded_result)
{
  uint16_t c_index = 0;
  bool bit_result[DATA_BIT_NUM];
  uint8_t h_counter = 0;  // high level counter

  // extract start bit
  for(uint8_t c = 0; c < CYCLE_NUM; c++)
    if(raw_cycle_data[c_index++] == 1)
      h_counter++;
  *start_bit = (h_counter > 3 ? true : false);

  // extract data bits
  for(uint8_t data_index = 0; data_index < MAX_DATA_SIZE; data_index++)
  {
    for(uint8_t bit_index = 0; bit_index < DATA_BIT_NUM; bit_index++)
    {
      h_counter = 0;
      for(uint8_t c = 0; c < CYCLE_NUM; c++)
        if(raw_cycle_data[c_index++] == 1)
          h_counter++;
      bit_result[bit_index] = (h_counter > 3 ? 1 : 0);
    }
    // compose the character
    decoded_result[data_index] = (char) 0;
    for(uint8_t i = 0; i < DATA_BIT_NUM; i++)
      decoded_result[data_index] |= (char)(bit_result[i] << (7 - i));
  }

  // extract end bit
  h_counter = 0;
  for(uint8_t c = 0; c < CYCLE_NUM; c++)
    if(raw_cycle_data[c_index++] == 1)
      h_counter++;
  *end_bit = (h_counter > 3 ? true : false);
}

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
//       if(data_index >= MAX_DATA_SIZE)
//       {
//         // end of decoding, return to trigger state
//         digitalWrite(pin_decode_output, LOW);
//         switch_trigger_source(false);
//         new_decoded_data = true;
//       }
//     }
//   }
}