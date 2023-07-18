#ifndef PARAMS_H
#define PARAMS_H

#include <Arduino.h>

// global parameters
#define CLK_CYCLE_NUM       6
#define DATA_CHAR_BITS      8       // 8 bits for a single character
#define MAX_CHAR_NUM        8       // send or receive 8 chars in a row
#define MAX_DATA_BITS       (DATA_CHAR_BITS * MAX_CHAR_NUM + 2) // including start bit and end bit

const byte pin_clock_v = 4;


// encoding pin definitions
const byte pin_encode_output = 22;


// decoding parameters and pin definitions
#define RECV_CLK_RATE       83333   // in Hz
const uint16_t MAX_CYCLE_DATA = MAX_DATA_BITS * CLK_CYCLE_NUM;  // including start bit and end bit

const byte pin_clock_i = 5;
const byte pin_trigger_source = 21; // connects to encode output (pin_encode_output) when decoding


#endif