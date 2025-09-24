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

#pragma once

#ifndef RCSWITCH_TRANSMITTER_INTERNAL_PROTOCOL_TIMING_SPEC_HPP_
#define RCSWITCH_TRANSMITTER_INTERNAL_PROTOCOL_TIMING_SPEC_HPP_

#include <Arduino.h>
#include <stddef.h>
#include <stdint.h>

#include "TxTimingSpecTable.hpp"


namespace RcSwitchTx {

struct TxPulsePairTime {
  unsigned int durationA;
  unsigned int durationB;
};

struct TxTimingSpec {
  bool bInverseLevel;
  TxPulsePairTime  synchronizationPulsePair;
  TxPulsePairTime  data0pulsePair;
  TxPulsePairTime  data1pulsePair;
};

struct TxPulsePairTiming {
  unsigned int durationA;
  unsigned int durationB;
};

namespace Debug {
  typedef typeof(Serial) serial_t;
  void dumpTxTimingSpecTable(serial_t &serial, const TxTimingSpecTable &txtimingSpecTable);
}

} // namespace RcSwitch

/**
 * makeTxTimingSpec
 */
template<
  unsigned int usecClock,
  unsigned int synchA,  unsigned int synchB,
  unsigned int data0_A, unsigned int data0_B,
  unsigned int data1_A, unsigned int data1_B,
  bool inverseLevel>

struct makeTxTimingSpec { // Calculate the timing specification from the protocol definition.
  static constexpr bool INVERSE_LEVEL = inverseLevel;

  static constexpr unsigned int uSecSynchA = usecClock * synchA;
  static constexpr unsigned int uSecSynchB = usecClock * synchB;

  static constexpr unsigned int uSecData0_A = usecClock * data0_A;
  static constexpr unsigned int uSecData0_B = usecClock * data0_B;

  static constexpr unsigned int uSecData1_A = usecClock * data1_A;
  static constexpr unsigned int uSecData1_B = usecClock * data1_B;

  typedef RcSwitchTx::TxTimingSpec tx_spec_t;
  static constexpr tx_spec_t TX = {INVERSE_LEVEL,
    { /* synch pulses */
        uSecSynchA, uSecSynchB
    },
    {   /* LOGICAL_0 data bit pulses */
      uSecData0_A, uSecData0_B
    },
    {
      /* LOGICAL_1 data bit pulses */
      uSecData1_A, uSecData1_B
    },
  };
};

/**
 * TxProtocolTable
 */

template<typename T, typename ...R> struct
TxProtocolTable {
private:
  const RcSwitchTx::TxTimingSpec* toArray() const {return &m;}
public:
  static constexpr size_t ROW_COUNT = sizeof(TxProtocolTable) / sizeof(RcSwitchTx::TxTimingSpec);
  RcSwitchTx::TxTimingSpec m = T::TX;
  TxProtocolTable<R...> r;

  /* Convert to txTimingSpecTable */
  inline RcSwitchTx::TxTimingSpecTable toTimingSpecTable() const {
    constexpr size_t rowCount = ROW_COUNT;
    return RcSwitchTx::TxTimingSpecTable{toArray(), rowCount};
  }
  inline void dumpTimingSpec(RcSwitchTx::Debug::serial_t &serial) const {
    RcSwitchTx::Debug::dumpTxTimingSpecTable(serial, toTimingSpecTable());
  }
};

/**
 * TxProtocolTable specialization for a table with just 1 row.
 */
template<typename T> struct
TxProtocolTable<T> {
private:
  const RcSwitchTx::TxTimingSpec* toArray() const {return &m;}
public:
  static constexpr size_t ROW_COUNT = sizeof(TxProtocolTable) / sizeof(RcSwitchTx::TxTimingSpec);
  RcSwitchTx::TxTimingSpec m = T::TX;

  /* Convert to txTimingSpecTable */
  inline RcSwitchTx::TxTimingSpecTable toTimingSpecTable() const {
    constexpr size_t rowCount = ROW_COUNT;
    return RcSwitchTx::TxTimingSpecTable{toArray(), rowCount};
  }
  inline void dumpTimingSpec(RcSwitchTx::Debug::serial_t &serial) const {
    RcSwitchTx::Debug::dumpTxTimingSpecTable(serial, toTimingSpecTable());
  }
};

#endif // RCSWITCH_TRANSMITTER_INTERNAL_PROTOCOL_TIMING_SPEC_HPP_
