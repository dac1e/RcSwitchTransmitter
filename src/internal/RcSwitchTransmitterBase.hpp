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

#pragma once

template<typename T, typename ...R> struct TxProtocolTable;
#include "TxProtocolTimingSpec.hpp"

#ifndef RCSWITCH_TRANSMITTER_INTERNAL_RCSWITCHTRANSMITTERBASE_HPP_
#define RCSWITCH_TRANSMITTER_INTERNAL_RCSWITCHTRANSMITTERBASE_HPP_

namespace RcSwitchTx {

class RcSwitchTransmitterBase {
  RcSwitchTx::TxTimingSpecTable mTxTimingSpecTable;
  size_t mRepeatCount;

  void transmitBit(const int ioPin,
      const RcSwitchTx::TxTimingSpec &timingSpec,
      const RcSwitchTx::TxPulsePairTime &pulsePairTime);

protected:
  RcSwitchTransmitterBase() : mTxTimingSpecTable{nullptr,0}, mRepeatCount(3) {}

  void begin( const RcSwitchTx::TxTimingSpecTable& txTimingSpecTable) {
     mTxTimingSpecTable = txTimingSpecTable;
  }

  inline void setRepeatCount(const size_t repeatCount) {
    mRepeatCount = repeatCount;
  }

  bool send(const int ioPin, const size_t protocolIndex, const uint32_t code,
      const size_t bitCount);
};

} // namespace RcSwitchTx
#endif /* RCSWITCH_TRANSMITTER_INTERNAL_RCSWITCHTRANSMITTERBASE_HPP_ */
