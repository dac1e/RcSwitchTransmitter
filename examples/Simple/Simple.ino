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

#include <Arduino.h>
#include "RcSwitchTransmitter.hpp"

// You can add own protocols and remove not needed protocols. Note that this changes the protocol
// index, which is a parameter for the send() function.
// However, the number of normal level protocols as well as the number of inverse level
// Protocols should not exceed 7 in this table.
DATA_ISR_ATTR static const TxProtocolTable <
  //               clk,syA,  syB,  d0A,d0B,  d1A,d1B, inverseLevel                protocol index implicitly given by position
  makeTxTimingSpec<350,  1,   31,    1,  3,    3,  1, false>, // (PT2262)         [0]
  makeTxTimingSpec<650,  1,   10,    1,  3,    3,  1, false>, // ()               [1]
  makeTxTimingSpec<100, 30,   71,    4, 11,    9,  6, false>, // ()               [2]
  makeTxTimingSpec<380,  1,    6,    1,  3,    3,  1, false>, // ()               [3]
  makeTxTimingSpec<500,  6,   14,    1,  2,    2,  1, false>, // ()               [4]
  makeTxTimingSpec<450,  1,   23,    1,  2,    2,  1, true>,  // (HT6P20B)        [5]
  makeTxTimingSpec<150,  2,   62,    1,  6,    6,  1, false>, // (HS2303-PT)      [6]
  makeTxTimingSpec<200,  3,  130,    7, 16,    3, 16, false>, // (Conrad RS-200)  [7]
  makeTxTimingSpec<365,  1,   18,    3,  1,    1,  3, true>,  // (1ByOne Doorbell)[8]
  makeTxTimingSpec<270,  1,   36,    1,  2,    2,  1, true>,  // (HT12E)          [9]
  makeTxTimingSpec<320,  1,   36,    1,  2,    2,  1, true>,  // (SM5212)        [10]
  // Note that last row must not end with a comma.
  makeTxTimingSpec<300, 2,   23,    2,  4,    4,  2, false>  // (Sygonix)        [11]
> txProtocolTable;


// For curiosity or debugging print the timing spec table that has been created.
#define DUMP_TIMING_SPEC_TABLE true

#if defined (ARDUINO_AVR_UNO)
constexpr int TX433_DATA_PIN = 13;
#else
constexpr int TX433_DATA_PIN = 7;
#endif

constexpr int TRIGGER_BUTTON = 12;

// Reference to the serial to be used for printing.
typeof(Serial)& output = Serial;

static RcSwitchTransmitter<TX433_DATA_PIN> rcSwitchTransmitter;

void setup()
{
  output.begin(9600);
  output.println("\n>>>>>>>> TransmitData <<<<<<<<\n");

#if DUMP_TIMING_SPEC_TABLE
  output.println();
  txProtocolTable.dumpTimingSpec(output);
  output.println();

  // Allow time to finalize printing the table.
  delay(500);
#endif
  rcSwitchTransmitter.begin(txProtocolTable.toTimingSpecTable());

  pinMode(TRIGGER_BUTTON, INPUT_PULLUP);
}

/* Some typical PT2262 buttons: */
constexpr uint32_t BUTTON_CODE_A = 5592332;
constexpr uint32_t BUTTON_CODE_B = 5592512;
constexpr uint32_t BUTTON_CODE_C = 5592323;
constexpr uint32_t BUTTON_CODE_D = 5592368;

/* An arbitrary code for demo purpose: */
constexpr uint32_t BUTTON_CODE_DEMO = 5393;

void loop()
{
  static constexpr size_t PROTCOL_INDEX_1 =  0; // PT2262
  static constexpr size_t PROTCOL_INDEX_2 = 11; // Sygonix

  // Send code on protocol 2262
  while(digitalRead(TRIGGER_BUTTON) == LOW) {
    rcSwitchTransmitter.send(PROTCOL_INDEX_1, BUTTON_CODE_DEMO, 24);
  }


//  // Send code on protocol Sygonix
//  rcSwitchTransmitter.send(PROTCOL_INDEX_2, BUTTON_CODE_DEMO, 24);
//  delay(1000);
}
