#ifndef MAIN_FUNCTION_H
#define MAIN_FUNCTION_H

#include <Arduino.h>
#include "BitEncoder.h"
#include "BitDecoder.h"

/** parameters of UART */
#define SERIAL_TERMINATOR '\r'
#define SERIAL_RESPONSE(msg) Serial.print(msg + String(SERIAL_TERMINATOR))

enum OpMode_t
{
    Encoding,
    Decoding
};

void Initialize();

void SwitchOpMode(OpMode_t new_mode);
OpMode_t GetOpMode();

void ProcessUART_Msg(String command, String parameter);
void ProcessDecodeRawData();

#endif