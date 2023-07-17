#include "MainFunctions.h"

void setup()
{
  Initialize();
}

void loop()
{
  if(IsDecodeTriggered())
  { // do nothing when triggered, but record data to raw_cycle_result array
    delay(200);
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

    ProcessUART_Msg(command, parameter);
  }
  else if(GetOpMode() == OpMode_t::Decoding)
  { // process received data when in decoding mode
    if(IsDecodeRawDataReceived())
    {
      SetDecodeRawDataReceived(false);
      ProcessDecodeRawData();
    }
  }
}

