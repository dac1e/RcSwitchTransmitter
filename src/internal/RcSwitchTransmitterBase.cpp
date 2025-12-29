/*
  RcSwitchTransmitter - Arduino libary for remote control transmitter Copyright (c)
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

#if defined(ARDUINO_ARCH_SAM)
// use own microseconds delay on ARDUINO_ARCH_SAM, because global delayMicroseconds() is causing problems.
#define RCSWITCH_TRANSMITTER_USE_LOCAL_DELAY_MICROS true
#else
#define RCSWITCH_TRANSMITTER_USE_LOCAL_DELAY_MICROS false
#endif

#if not RCSWITCH_TRANSMITTER_USE_LOCAL_DELAY_MICROS
#include <Arduino.h>
#undef min
#undef max
#endif

#if not RCSWITCH_TRANSMITTER_TIMING_CORRECTION
#if defined (ARDUINO_AVR_UNO)
  // We need to shorten the delay on UNO, because of its delayMicroseconds() function shifts the pulses
  // length beyond tolerance.
  #define RCSWITCH_TRANSMITTER_TIMING_CORRECTION (40) // usec
#else
  #define RCSWITCH_TRANSMITTER_TIMING_CORRECTION (0)  // usec
#endif
#endif

namespace RcSwitchTx {

void computeWhitening(uint8_t* inOut, const size_t bitCount) {
  const size_t remainingBits = bitCount % (8 * sizeof(*inOut));
  uint8_t WhiteningKeyMSB = 0x01;
  uint8_t WhiteningKeyLSB = 0xFF;
  uint8_t WhiteningKeyMSBPrevious = 0;

  const size_t byteCount = (bitCount + 8 * sizeof(*inOut) - 1) / (8 * sizeof(*inOut));
  for(size_t j = 0; j < byteCount; j++) {
    inOut[j] ^= WhiteningKeyLSB;
    const size_t bitCountOfCurrentByte = (j+1 < byteCount) || not remainingBits ? 8 * sizeof(*inOut) : remainingBits;
    for( uint8_t i = 0; i < bitCountOfCurrentByte; i++ ) {
      WhiteningKeyMSBPrevious = WhiteningKeyMSB;
      WhiteningKeyMSB = (WhiteningKeyLSB & 0x01) ^ ((WhiteningKeyLSB >> 5) & 1);
      WhiteningKeyLSB = ((WhiteningKeyLSB >> 1) & 0xFF) | ((WhiteningKeyMSBPrevious << 7) & 0x80);
    }
  }
}

void computeWhitening(uint8_t* out, const uint8_t* in, const size_t bitCount) {
  const size_t byteCount = (bitCount + 8 * sizeof(*in) - 1) / (8 * sizeof(*in));
  memcpy(out, in, byteCount);
  computeWhitening(out, bitCount);
}

inline void delayMicros(uint32_t) __attribute__((always_inline, unused));

#if RCSWITCH_TRANSMITTER_USE_LOCAL_DELAY_MICROS

inline void delayMicros(const uint32_t usec) {
  const uint32_t start = micros();
  while(true) {
    const uint32_t delta = micros() - start;
    if(delta >= usec) {
      break;
    }
  }
}

#else
  inline void delayMicros(const uint32_t usec) {
    ::delayMicroseconds(usec);
  }
#endif

void RcSwitchTransmitterBase::transmitBit(const int ioPin, const RcSwitchTx::TxTimingSpec &timingSpec,
    const RcSwitchTx::TxPulsePairTime &pulsePairTime) {

  const unsigned logicLevelA = (timingSpec.bInverseLevel) ? LOW : HIGH;
  const unsigned logicLevelB = (timingSpec.bInverseLevel) ? HIGH : LOW;
  digitalWrite(ioPin, logicLevelA);
  delayMicros(pulsePairTime.durationA - RCSWITCH_TRANSMITTER_TIMING_CORRECTION);
  digitalWrite(ioPin, logicLevelB);
  delayMicros(pulsePairTime.durationB - RCSWITCH_TRANSMITTER_TIMING_CORRECTION);
}

RESULT RcSwitchTransmitterBase::send(const int ioPin, const size_t protocolIndex,
    const uint32_t* const dwords, const size_t bitCount) {
  if (mTxTimingSpecTable.start != nullptr) {
    if (protocolIndex < mTxTimingSpecTable.size) {
      const RcSwitchTx::TxTimingSpec &timingSpec =
          mTxTimingSpecTable.start[protocolIndex];

      const size_t remainingBits = bitCount % (8 * sizeof(*dwords));

      // Send synch at the beginning of the first repetition
      transmitBit(ioPin, timingSpec, timingSpec.synchronizationPulsePair);

      for (size_t repeat = 0; repeat < mRepeatCount; repeat++) {
        const size_t dwordCount = (bitCount + 8 * sizeof(*dwords) - 1) / (8 * sizeof(*dwords));
        for(size_t c = 0; c < dwordCount; c++) {
          const size_t bitCountOfCurrentDword = (c+1 < dwordCount) || not remainingBits ? 8 * sizeof(*dwords) : remainingBits;
          for (int b = bitCountOfCurrentDword - 1; b >= 0; b--) {
            if (dwords[c] & (1L << b)) {
              transmitBit(ioPin, timingSpec, timingSpec.data1pulsePair);
            } else {
              transmitBit(ioPin, timingSpec, timingSpec.data0pulsePair);
            }
          }
        }

        // Send synch at the end of each repetition
        transmitBit(ioPin, timingSpec, timingSpec.synchronizationPulsePair);
      }
      return OK;
    }
  }
  return INIT_ERR;
}

} // namespace RcSwitchTx
