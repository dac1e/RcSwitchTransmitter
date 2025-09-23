/*
  RcSwitchReceiver - Arduino libary for remote control transmitter Copyright (c)
  2025 Wolfgang Schmieder.  All right reserved.

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

#ifndef RCSWITCH_TRANSMITTER_API_HPP_
#define RCSWITCH_TRANSMITTER_API_HPP_

#include <Arduino.h>

#include "internal/ISR_ATTR.hpp"
#include <stddef.h>
#include <stdint.h>

/*
 * The remote control protocol is a stream of pulse pairs with different duration and
 * pulse levels. In the context of this documentation, the first pulse will be
 * referred to as "pulse A" and the second one as "pulse B".
 *
 *   Normal level protocols start with a high level:
 *          ___________________
 *     XXXX|                   |____________________|XXXX
 *
 *
 *
 *   Inverse level protocols start with a low level:
 *                              ____________________
 *     XXXX|___________________|                    |XXXX
 *
 *         ^                   ^                    ^
 *         | pulse A duration  | pulse B duration   |
 *
 *
 *  In the synchronization phase there is a short pulse followed by a very long pulse:
 *     Normal level protocols:
 *          ____
 *     XXXX|    |_____________________________________________________________|XXXX
 *
 *
 *
 *     Inverse level protocols:
 *               _____________________________________________________________
 *     XXXX|____|                                                             |XXXX
 *
 *
 *  In the data phase there is
 *   a short pulse followed by a long pulse for a logical 0 data bit:
 *     Normal level protocols:
 *          __
 *     XXXX|  |________|XXXX
 *
 *     Inverse level protocols:
 *             ________
 *     XXXX|__|        |XXXX
 *
 *   a long pulse followed by a short pulse for a logical 1 data bit:
 *     Normal level protocols:
 *          ________
 *     XXXX|        |__|XXXX
 *
 *     Inverse level protocols:
 *                   __
 *     XXXX|________|  |XXXX
 *
 *
 * Pulse durations sent out by a real world transmitter can vary. Hence the
 * timing specification for receiving pulses must have a time range
 * for a pulse to be recognized as a valid synchronization pulse respectively
 * data pulse.
 *
 * Synch. pulses and data pulses are defined as a multiple of a protocol
 * specific clock cycle.
 *
 * There is a decision to be made, when the received number of data bits constitute
 * a completed message packet so that further reception of data bits must be
 * stopped. It is assumed, that the transmitter transmits the same message
 * packets multiple times in a row. The completion of a message packet is
 * determined, upon receiving new synch pulses from a subsequent transmission.
 */

/**
 * makeTxTimingSpec
 *
 * Calculates the pulse timings specification from a given protocol specification
 * at compile time. Compile time calculation keeps the interrupt handler quick.
 * makeTxTimingSpec is to be used in combination with TxProtocolTable below.
 */
template<
  /** A protocol specification is given by the following parameters: */
  unsigned int usecClock,                /* The clock rate in microseconds.  */
  unsigned int synchA,  unsigned int synchB,   /* Number of clocks for the synchronization pulse pair. */
  unsigned int data0_A, unsigned int data0_B,  /* Number of clocks for a logical 0 bit data pulse pair. */
  unsigned int data1_A, unsigned int data1_B,  /* Number of clocks for a logical 1 bit data pulse pair. */
  bool inverseLevel>               /* Flag whether pulse levels are normal or inverse. */
struct makeTxTimingSpec;

/**
 * RxProtocolTable
 *
 * Provides an array of timing specifications from given protocol specifications.
 * The array gets sorted at compile time. Sort criteria are the inverseLevel
 * flag and the lowerBound of the synch A pulse.
 * Sorting the table at compile time provides an opportunity to speed up the
 * interrupt handler.
 *
 * static const TxProtocolTable <
 * //                  clk, syA,  syB,  d0A,d0B,  d1A,d1B, inverseLevel
 *    makeTxTimingSpec<350,   1,   31,    1,  3,    3,  1, false>, // (PT2262)
 *    makeTxTimingSpec<650,   1,   10,    1,  3,    3,  1, false>, // ()
 *    makeTxTimingSpec<100,  30,   71,    4, 11,    9,  6, false>, // ()
 *    makeTxTimingSpec<380,   1,    6,    1,  3,    3,  1, false>, // ()
 *    makeTxTimingSpec<500,   6,   14,    1,  2,    2,  1, false>, // ()
 *    makeTxTimingSpec<450,   1,   23,    1,  2,    2,  1, true>,  // (HT6P20B)
 *    makeTxTimingSpec<150,   2,   62,    1,  6,    6,  1, false>, // (HS2303-PT)
 *    makeTxTimingSpec<200,   3,  130,    7, 16,    3, 16, false>, // (Conrad RS-200)
 *    makeTxTimingSpec<365,   1,   18,    3,  1,    1,  3, true>,  // (1ByOne Doorbell)
 *    makeTxTimingSpec<270,   1,   36,    1,  2,    2,  1, true>,  // (HT12E)
 *    makeTxTimingSpec<320,   1,   36,    1,  2,    2,  1, true>   // (SM5212)
 *  > txProtocolTable;
 *
 *  The resulting array of timing specifications can be dumped for debug purpose:
 *  ...
 *  txProtocolTable.dumpTimingSpec(serial);
 *  ...
 *
 */
template<typename T, typename ...R> struct TxProtocolTable;
#include "internal/TxProtocolTimingSpec.hpp"
#include "internal/RcSwitchTransmitterBase.hpp"
/**
 * This is the library API class for transmitting data to a remote control receiver.
 * The IO pin to be used is defined at compile time by the template
 * parameter IOPIN.
 * Multiple RcSwitchTransmitter can be instantiated for different IO pins.
 * E.g. if you have a 433Mhz transmitter hardware connected to pin 5 and a
 * 315Mhz transmitter hardware to pin 6 you can create 2 RcSwitchTransmitter
 * instances as follows:
 *
 * RcSwitchTransmitter<5> rcSwitchTransmitter433;
 * RcSwitchTransmitter<6> rcSwitchTransmitter315;
 */

using RcSwitchTx::TxTimingSpecTable;

template<int IOPIN> class RcSwitchTransmitter : protected RcSwitchTx::RcSwitchTransmitterBase {
  typedef RcSwitchTx::RcSwitchTransmitterBase base_t;
public:
  /**
   * Sets the protocol timing specification table to be used for transmitting data.
   * Sets up pin mode.
   */
  void begin(const TxTimingSpecTable& txTimingSpecTable) {
    base_t::begin(txTimingSpecTable);
    pinMode(IOPIN, OUTPUT);
  }

  inline bool send(const size_t protocolIndex, const uint32_t code, const size_t bitCount) {
    return base_t::send(IOPIN, protocolIndex, code, bitCount);
  }

  inline void setRepeatCount(const size_t repeatCount) {
    base_t::setRepeatCount(repeatCount);
  }

};

#endif /* RCSWITCH_TRANSMITTER_API_HPP_ */
