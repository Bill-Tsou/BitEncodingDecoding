#include "MainFunctions.h"

TaskHandle_t task;

// annouce variable to store current operation mode
OpMode_t mode;
uint8_t encoder_id = 0;

void Initialize()
{
    // initialize UART data transmission rate
    Serial.begin(115200);

    // initialize current operation mode
    mode = Encoding;
    BitEncoderSetup();

    // set the initial threshold of V & I overlapping percentage when decoding
    Decode_ChangeVI_Th(40, Decode_Type_t::DECODE_TRIGGER);
    Decode_ChangeVI_Th(40, Decode_Type_t::DECODE_DATA);

    //xTaskCreatePinnedToCore(MainLoop, "CommLoop", 10240, NULL, 0, &task, 0);
}

void MainLoop(void *param)
{
    while(true)
    {
        if(IsDecodeTriggered())
        {   // do nothing when triggered, but record data to raw_cycle_result array
            delay(200);
        }
        else if(Serial.available())
        {
            // pause for dealing with decoding interrupts while UART data transmission
            if(GetOpMode() == OpMode_t::Decoding)
            {
                BitDecoderEnd();
                delayMicroseconds(100);
            }

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

            ProcessUART_Msg(command, parameter);

            // update the mode to make RTOS back on interrupts procedure
            SwitchOpMode(GetOpMode());
        }
        else if(GetOpMode() == OpMode_t::Decoding)
        { // process received data when in decoding mode
            if(IsDecodeRawDataReceived())
            {
                if(GetOpMode() == OpMode_t::Decoding)
                    BitDecoderEnd();

                SetDecodeRawDataReceived(false);
                ProcessDecodeRawData();

                SwitchOpMode(GetOpMode());
            }
        }
    }
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
            SERIAL_RESPONSE("OK");
            SwitchOpMode(OpMode_t::Encoding);
        }
        else if(parameter == "decode")
        {
            SERIAL_RESPONSE("OK");
            SwitchOpMode(OpMode_t::Decoding);
        }
        else
            SERIAL_RESPONSE("Invalid Mode! [encode / decode]");
    }
    else if(command == "id")
    {
        encoder_id = parameter.toInt();
        SERIAL_RESPONSE("OK");
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
            for(uint8_t i = 0; i < DUMMY_BITS; i++)
                *ptr_bit++ = true;  // dummy bits
            *ptr_bit++ = false; // sync bit
            *ptr_bit++ = encoder_id & 1;    // id bit
            for(uint8_t i = 0; i < MAX_CHAR_NUM; i++)
                for(uint8_t j = 0; j < DATA_CHAR_BITS; j++)  // 8 bits for each character
                    *ptr_bit++ = (data[i] >> (7 - j)) & 1;
            *ptr_bit++ = true;  // end bit

            String encoded_binary = "";
            ptr_bit = GetEncodeSendDataPtr();
            uint8_t bit_index = 0;
            encoded_binary += ptr_bit[bit_index++] ? "1" : "0"; // start bit
            encoded_binary += " ";
            encoded_binary += ptr_bit[bit_index++] ? "1" : "0"; // sync bit
            encoded_binary += " ";
            encoded_binary += ptr_bit[bit_index++] ? "1" : "0"; // id bit
            encoded_binary += " ";
            for(; bit_index < MAX_DATA_BITS; bit_index++)       // data bits and end bit
            {
                encoded_binary += (*(ptr_bit + bit_index) ? "1" : "0");
                if((bit_index - 2) % DATA_CHAR_BITS == 0)
                    encoded_binary += " ";
            }

            SERIAL_RESPONSE("OK " + encoded_binary);
            // send the encoded message after serial response
            EncodeSendData();
        }
        else
            SERIAL_RESPONSE("Current Mode is not in Encoding Mode!");
    }
    /** @remark for triggering decoding and data decoding threshold values */
    else if(command == "vi_th_trig")
    {
        if(Decode_ChangeVI_Th(parameter.toInt(), Decode_Type_t::DECODE_TRIGGER) == false)
            SERIAL_RESPONSE("Invalid Percentage!");
        else
            SERIAL_RESPONSE("OK");
    }
    else if(command == "vi_th_dec")
    {
        if(Decode_ChangeVI_Th(parameter.toInt(), Decode_Type_t::DECODE_DATA) == false)
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
    for(raw_data_idx = 0; raw_data_idx < CLK_CYCLE_NUM; raw_data_idx++)
        Serial.printf("%d ", (uint8_t)raw_cycle_result[raw_data_idx]);
    Serial.print(SERIAL_TERMINATOR);

    // display data array
    for(uint16_t i = 1; raw_data_idx < (MAX_CHAR_NUM * DATA_CHAR_BITS + 1) * CLK_CYCLE_NUM; raw_data_idx++, i++)
    {
        Serial.printf("%d ", (uint8_t)raw_cycle_result[raw_data_idx]);
        if(i % (DATA_CHAR_BITS * CLK_CYCLE_NUM) == 0)
            Serial.print(SERIAL_TERMINATOR);
    }

    // display end bit array
    for(uint16_t i = 1; i < CLK_CYCLE_NUM; raw_data_idx++, i++)
        Serial.printf("%d ", (uint8_t)raw_cycle_result[raw_data_idx]);
    Serial.print(SERIAL_TERMINATOR);
#endif

    // start decoding raw cycle data
    char decode_result[MAX_CHAR_NUM];
    uint8_t decoded_id;
    bool start_bit, end_bit, sync_bit;
    DecodeRawCycleData(raw_cycle_result, &start_bit, &sync_bit, &end_bit, &decoded_id, decode_result);

#ifdef DEBUG_MODE
    Serial.printf("Start Bit = %d, Sync Bit = %d, End Bit = %d", (uint8_t)start_bit, (uint8_t)sync_bit, (uint8_t)end_bit);
    Serial.print(SERIAL_TERMINATOR);
#endif

    // return the decoded ID number and ASCII code in decimal
    Serial.print("Decoded Result: ");
    Serial.printf("%d ", decoded_id);
    for(uint8_t i = 0; i < MAX_CHAR_NUM; i++)
        Serial.printf("%d ", (uint8_t)decode_result[i]);
    Serial.print(SERIAL_TERMINATOR);
}