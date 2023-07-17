#ifndef BIT_DECODER_H
#define BIT_DECODER_H

#include "Params.h"

void BitDecoderSetup();
void BitDecoderEnd();

/**
 * @brief change V & I signal overlapping threshold 
 * @param overlap_percentage between 20 to 80 percent
 * @return true if overlap_percentage is valid, false if it is invalid
 */
bool Decode_ChangeVI_Th(uint8_t overlap_percentage);

void DecodeRawCycleData(const volatile bool *raw_cycle_data, bool *start_bit, bool *end_bit, char *decoded_result);

bool IsDecodeTriggered();

bool IsDecodeRawDataReceived();
void SetDecodeRawDataReceived(bool new_state);

volatile bool* GetDecodeRawCycleResults();

/** internal functions used in Decoder */
void IRAM_ATTR switch_trigger_source(bool new_state);
void IRAM_ATTR isr_trig_source();
void IRAM_ATTR isr_v_rising();
void IRAM_ATTR isr_i_falling();

#endif