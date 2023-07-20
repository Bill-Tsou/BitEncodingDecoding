#ifndef PARAMS_H
#define PARAMS_H

#include <Arduino.h>

// global parameters
#define CLK_CYCLE_NUM       6
#define DATA_CHAR_BITS      8       // 8 bits for a single character
#define MAX_CHAR_NUM        8       // send or receive 8 chars in a row
#define MAX_DATA_BITS       (DATA_CHAR_BITS * MAX_CHAR_NUM + 2) // including start bit and end bit

/** @brief  define DEBUG_MODE only if you'd like to inspect the decode results with UART,
 *          in normal operations, please comment this definition
 */
#define DEBUG_MODE

/** @brief  define TWO_I_CLOCKS when you'd like to connect two encoders to one decoder physically
 *          at the same time
 */
#define TWO_I_CLOCKS

const byte pin_clock_v = 4;

// encoding pin definitions
const byte pin_encode_output = 22;

// decoding parameters and pin definitions
#define RECV_CLK_RATE       83333   // in Hz
const uint16_t MAX_CYCLE_DATA = MAX_DATA_BITS * CLK_CYCLE_NUM;  // including start bit and end bit

const byte pin_clock_i1 = 5;
const byte pin_trigger_source1 = 21; // connects to encode output (pin_encode_output) when decoding

#ifdef TWO_I_CLOCKS
const byte pin_clock_i2 = 25;
const byte pin_trigger_source2 = 23; // connects to encode output (pin_encode_output) when decoding for physical connection
#endif

#endif