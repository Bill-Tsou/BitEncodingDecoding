#ifndef BIT_ENCODER_H
#define BIT_ENCODER_H

#include "Params.h"

void BitEncoderSetup();
void BitEncoderEnd();

volatile bool* GetEncodeSendDataPtr();
void EncodeSendData();
bool IsEncoderSendingNewData();

void IRAM_ATTR isr_Callback();

#endif