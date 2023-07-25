#ifndef BIT_DECODER_H
#define BIT_DECODER_H

#include "Params.h"

enum Trig_State_t
{
    WAIT_FOR_TRIGGER,
    TRIGGER_SOURCE1,
#ifdef TWO_I_CLOCKS
    TRIGGER_SOURCE2
#endif
};

enum Decode_Type_t
{
    DECODE_TRIGGER,
    DECODE_DATA
};

void BitDecoderSetup();
void BitDecoderEnd();

/**
 * @brief change V & I signal overlapping threshold 
 * @param overlap_percentage between 10 to 80 percent
 * @return true if overlap_percentage is valid, false if it is invalid
 */
bool Decode_ChangeVI_Th(uint8_t overlap_percentage, Decode_Type_t decode_type);

void DecodeRawCycleData(const volatile bool *raw_cycle_data, bool *start_bit, bool *sync_bit, bool *end_bit, uint8_t *decoded_id, char *decoded_result);

bool IsDecodeTriggered();

bool IsDecodeRawDataReceived();
void SetDecodeRawDataReceived(bool new_state);

volatile bool* GetDecodeRawCycleResults();
//volatile uint8_t* GetDecodeMicroDiffResults();

/** internal functions used in Decoder */
void IRAM_ATTR switch_trigger_source(Trig_State_t new_state);
void IRAM_ATTR isr_trig_source1();
#ifdef TWO_I_CLOCKS
void IRAM_ATTR isr_trig_source2();
#endif
void IRAM_ATTR isr_trig_rising();
void IRAM_ATTR isr_decode_rising();
//void IRAM_ATTR isr_i_falling();

#endif