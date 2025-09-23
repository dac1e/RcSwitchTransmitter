/*
 * ProtocolTimingSpec.cpp
 *
 *  Created on: 23.11.2024
 *      Author: Wolfgang
 */

#include <stddef.h>
#include <stdlib.h>
#include "TxFormattedPrint.hpp"
#include "TxProtocolTimingSpec.hpp"

namespace {

size_t sprintTime(char *string, const unsigned int time, const size_t width) {
  size_t i=0;
  RcSwitchTx::sprintNumAsDecimal(&string[i], time, width); // adds a null terminated string
  i = strlen(string);
  string[i] = '\0'; // ensure null termination
  return i; // return string length
}

inline size_t sprintTimeRanges(char* string, const RcSwitchTx::TxPulsePairTime& pulsePair, const size_t widthA, const size_t widthB) {
  size_t i=0;
  string[i++] = '{';
  i += sprintTime(&string[i], pulsePair.durationA, widthA); // adds a null terminated string
  string[i++] = ',';
  i += sprintTime(&string[i], pulsePair.durationB, widthB); // adds a null terminated string
  string[i++] = '}';
  string[i] = '\0'; // ensure null termination
  return i;
}

} // anonymous name space

namespace RcSwitchTx {

namespace Debug {

void dumpTxTimingSpecTable(serial_t &serial, const TxTimingSpecTable &txtimingSpecTable) {

  serial.println(" i,{<----SYNCH-->}{<---DATA 0-->}{<---DATA 1-->}");
  serial.println("    PulseA,PulseB  PulseA,PulseB  PulseA,PulseB");
  char buffer[96];

  for (size_t i = 0; i < txtimingSpecTable.size; i++) {
    const TxTimingSpec &p = txtimingSpecTable.start[i];
    if (p.bInverseLevel) {
      serial.print(",1,");
    } else {
      serial.print(",0,");
    }

    sprintTimeRanges(buffer, p.synchronizationPulsePair, 6, 6);
    serial.print(buffer);
    sprintTimeRanges(buffer, p.data0pulsePair, 6, 6);
    serial.print(buffer);
    sprintTimeRanges(buffer, p.data1pulsePair, 6, 6);
    serial.println(buffer);
  }
}

} // namespace Debug
} // namespace RcSwitch
