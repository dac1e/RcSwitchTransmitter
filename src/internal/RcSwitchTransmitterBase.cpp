/*
  RcSwitchReceiver - Arduino libary for remote control transmitter Copyright (c)
  2024 Wolfgang Schmieder.  All right reserved.

  Contributors:
  - Wolfgang Schmieder

  Project home: https://github.com/dac1e/RcSwitchTransmitter/

  This library is free software; you can redistribute it and/or modify it
  the terms of the GNU Lesser General Public License as under published
  by the Free Software Foundation; either version 3.0 of the License,
  or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301 USA
*/

#include "RcSwitchTransmitterBase.hpp"

#if not RCSWITCH_TRANSMITTER_TIMING_CORRECTION
#if defined (ARDUINO_AVR_UNO)
  #define RCSWITCH_TRANSMITTER_TIMING_CORRECTION (40) // usec
#else
  #define RCSWITCH_TRANSMITTER_TIMING_CORRECTION (0)  // usec
#endif
#endif

namespace RcSwitchTx {

inline void delayMicros(uint32_t) __attribute__((always_inline, unused));
inline void delayMicros(const uint32_t usec) {
  const uint32_t start = micros();
  while(true) {
    const uint32_t delta = micros() - start;
    if(delta >= usec) {
      break;
    }
  }
}

void RcSwitchTransmitterBase::transmitBit(const int ioPin, const RcSwitchTx::TxTimingSpec &timingSpec,
    const RcSwitchTx::TxPulsePairTime &pulsePairTime) {

  const unsigned logicLevelA = (timingSpec.bInverseLevel) ? LOW : HIGH;
  const unsigned logicLevelB = (timingSpec.bInverseLevel) ? HIGH : LOW;
  digitalWrite(ioPin, logicLevelA);
  delayMicros(pulsePairTime.durationA - RCSWITCH_TRANSMITTER_TIMING_CORRECTION);
  digitalWrite(ioPin, logicLevelB);
  delayMicros(pulsePairTime.durationB - RCSWITCH_TRANSMITTER_TIMING_CORRECTION);
}

bool RcSwitchTransmitterBase::send(const int ioPin, const size_t protocolIndex,
    const uint32_t code, const size_t bitCount) {
  if (mTxTimingSpecTable.start != nullptr) {
    if (protocolIndex < mTxTimingSpecTable.size) {
      const RcSwitchTx::TxTimingSpec &timingSpec =
          mTxTimingSpecTable.start[protocolIndex];
      // Send synch at the beginning of the first repetition
      transmitBit(ioPin, timingSpec, timingSpec.synchronizationPulsePair);
      for (size_t repeat = 0; repeat < mRepeatCount; repeat++) {
        for (int i = bitCount - 1; i >= 0; i--) {
          if (code & (1L << i)) {
            transmitBit(ioPin, timingSpec, timingSpec.data1pulsePair);
          } else {
            transmitBit(ioPin, timingSpec, timingSpec.data0pulsePair);
          }
        }
        // Send synch at the end if each repeat
        transmitBit(ioPin, timingSpec, timingSpec.synchronizationPulsePair);
      }
      return true;
    }
  }
  return false;
}

} // namespace RcSwitchTx
