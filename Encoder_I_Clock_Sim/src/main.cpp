#include <Arduino.h>

#define CLK_CYCLE_NUM       6

// data format: [1 start-bit (1)] [1 sync-bit (0)] [1 id bit] [8x8 data bits] [1 end-bit (1)] with total 68 bits
#define DUMMY_BITS          1
#define ID_BIT              1       // 1 id bit for differentiating encoders
#define DATA_CHAR_BITS      8       // 8 bits for a single character
#define MAX_CHAR_NUM        8       // send or receive 8 chars in a row
#define MAX_DATA_BITS       (DUMMY_BITS + 2 + ID_BIT + DATA_CHAR_BITS * MAX_CHAR_NUM + 1) // including start bit, end bit and sync bit

/** parameters of UART */
#define SERIAL_TERMINATOR '\r'
#define SERIAL_RESPONSE(msg) Serial.print(msg + String(SERIAL_TERMINATOR))

// the number of the LED pin
const byte pin_clock_v = 4;   // V clock input for interrupt
const byte pin_trigger_source = 5;
const byte pin_clock_i_sim = 22;  // I clock output for simulation

uint8_t encoder_id = 0;
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
    else if(command == "id")
    {
        encoder_id = parameter.toInt();
        SERIAL_RESPONSE("OK");
    }
    else if(command == "encode")
    {
      if(parameter.length() != MAX_CHAR_NUM)
        SERIAL_RESPONSE("Invalid Data Num, which should be " + String(MAX_CHAR_NUM));
      else
      {

        const char *data = parameter.c_str();
        // encode data to binary ASCII code
        volatile bool *ptr_sim = send_data;
        *ptr_sim++ = true;  // start bit
        for(uint8_t i = 0; i < DUMMY_BITS; i++)
          *ptr_sim++ = true;  // dummy bits
        *ptr_sim++ = false; // sync bit
        *ptr_sim++ = encoder_id & 1;    // id bit
        for(uint8_t i = 0; i < MAX_CHAR_NUM; i++)
            for(uint8_t j = 0; j < DATA_CHAR_BITS; j++)  // 8 bits for each character
                *ptr_sim++ = (data[i] >> (7 - j)) & 1;
        *ptr_sim++ = true;  // end bit

        String encoded_binary = "";
        ptr_sim = send_data;
        uint8_t bit_index = 0;
        encoded_binary += ptr_sim[bit_index++] ? "1" : "0"; // start bit
        encoded_binary += " ";
        encoded_binary += ptr_sim[bit_index++] ? "1" : "0"; // sync bit
        encoded_binary += " ";
        encoded_binary += ptr_sim[bit_index++] ? "1" : "0"; // id bit
        encoded_binary += " ";
        for(; bit_index < MAX_DATA_BITS; bit_index++)       // data bits and end bit
        {
            encoded_binary += (*(ptr_sim + bit_index) ? "1" : "0");
            if((bit_index - 2) % DATA_CHAR_BITS == 0)
                encoded_binary += " ";
        }
        SERIAL_RESPONSE("OK " + encoded_binary);

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
  else
  {
    digitalWrite(pin_clock_i_sim, HIGH);
      delayMicroseconds(5);
    digitalWrite(pin_clock_i_sim, LOW);
  }
}