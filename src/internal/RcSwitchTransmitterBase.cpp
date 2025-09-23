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
  delayMicros(pulsePairTime.durationA);
  digitalWrite(ioPin, logicLevelB);
  delayMicros(pulsePairTime.durationB);
}

} // namespace RcSwitchTx
