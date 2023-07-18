#include "MainFunctions.h"

// annouce variable to store current operation mode
OpMode_t mode;

void Initialize()
{
    // initialize UART data transmission rate
    Serial.begin(115200);

    // initialize current operation mode
    mode = Encoding;
    BitEncoderSetup();

    // set the initial threshold of V & I overlapping percentage when decoding
    Decode_ChangeVI_Th(40);
}

void SwitchOpMode(OpMode_t new_mode)
{   // end with the current operation mode
    switch(mode)
    {
        case OpMode_t::Encoding:
            BitEncoderEnd();
        break;
        case OpMode_t::Decoding:
            BitDecoderEnd();
        break;
    }
    // switch to the new operation mode
    switch(new_mode)
    {
        case OpMode_t::Encoding:
            BitEncoderSetup();
        break;
        case OpMode_t::Decoding:
            BitDecoderSetup();
        break;
    }
    mode = new_mode;
}

OpMode_t GetOpMode()
{ return mode; }

void ProcessUART_Msg(String command, String parameter)
{
    // processing UART commands
    if(command == "mode")
    {
        if(parameter == "encode")
        {
            SwitchOpMode(OpMode_t::Encoding);
            SERIAL_RESPONSE("OK");
        }
        else if(parameter == "decode")
        {
            SwitchOpMode(OpMode_t::Decoding);
            SERIAL_RESPONSE("OK");
        }
        else
            SERIAL_RESPONSE("Invalid Mode! [encode / decode]");
    }
    /** @remark for encoding */
    else if(command == "encode")
    {
        if(IsEncoderSendingNewData())
            SERIAL_RESPONSE("Data is still Sending...");
        else if(mode == OpMode_t::Encoding)
        {
            const char *data = parameter.c_str();
            volatile bool *ptr_bit = GetEncodeSendDataPtr();
            *ptr_bit++ = true;  // start bit
            for(uint8_t i = 0; i < MAX_CHAR_NUM; i++)
                for(uint8_t j = 0; j < DATA_CHAR_BITS; j++)  // 8 bits for each character
                    *ptr_bit++ = (data[i] >> (7 - j)) & 1;
            *ptr_bit++ = true;  // end bit

            String encoded_binary = "";
            ptr_bit = GetEncodeSendDataPtr();
            for(uint8_t i = 0; i < MAX_DATA_BITS; i++)
            {
                encoded_binary += (*(ptr_bit + i) ? "1" : "0");
                if((i == 0) || (i % DATA_CHAR_BITS == 0))
                    encoded_binary += " ";
            }

            SERIAL_RESPONSE("OK " + encoded_binary);
            // send the encoded message after serial response
            EncodeSendData();
        }
        else
            SERIAL_RESPONSE("Current Mode is not in Encoding Mode!");
    }
    /** @remark for decoding */
    else if(command == "vi_th")
    {
        if(Decode_ChangeVI_Th(parameter.toInt()) == false)
            SERIAL_RESPONSE("Invalid Percentage!");
        else
            SERIAL_RESPONSE("OK");
    }
}

void ProcessDecodeRawData()
{
    uint16_t raw_data_idx = 0;
    volatile bool *raw_cycle_result = GetDecodeRawCycleResults();

#ifdef DEBUG_MODE
    // display time difference
    // volatile uint8_t *diff_result = GetDecodeMicroDiffResults();
    // display start bit array
    // for(; raw_data_idx < 6; raw_data_idx++)
    //     Serial.printf("%02d ", diff_result[raw_data_idx]);

    // display data array
    // for(; raw_data_idx < (MAX_CHAR_NUM * DATA_CHAR_BITS + 1) * CLK_CYCLE_NUM; raw_data_idx++)
    // {
    //     if((raw_data_idx - 6) % 48 == 0)
    //         Serial.print(SERIAL_TERMINATOR);
    //     Serial.printf("%02d ", diff_result[raw_data_idx]);
    // }
    // Serial.print(SERIAL_TERMINATOR);

    // display end bit array
    // for(; raw_data_idx < MAX_CYCLE_DATA; raw_data_idx++)
    //     Serial.printf("%02d ", diff_result[raw_data_idx]);
    // Serial.print(SERIAL_TERMINATOR);

    /*-------------------------------------*/
    // display start bit array
    for(raw_data_idx = 0; raw_data_idx < 6; raw_data_idx++)
        Serial.printf("%d ", (uint8_t)raw_cycle_result[raw_data_idx]);

    // display data array
    for(; raw_data_idx < (MAX_CHAR_NUM * DATA_CHAR_BITS + 1) * CLK_CYCLE_NUM; raw_data_idx++)
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
#endif

    // start decoding raw cycle data
    char decode_result[MAX_CHAR_NUM];
    bool start_bit, end_bit;
    DecodeRawCycleData(raw_cycle_result, &start_bit, &end_bit, decode_result);

#ifdef DEBUG_MODE
    Serial.printf("Start Bit = %d, End Bit = %d", (uint8_t)start_bit, (uint8_t)end_bit);
    Serial.print(SERIAL_TERMINATOR);
#endif

    // display the decoded result ASCII code in decimal
    Serial.print("Decoded Result: ");
    for(uint8_t i = 0; i < MAX_CHAR_NUM; i++)
        Serial.printf("%d ", (uint8_t)decode_result[i]);
    Serial.print(SERIAL_TERMINATOR);
}