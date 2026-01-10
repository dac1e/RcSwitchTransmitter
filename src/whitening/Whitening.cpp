/*
  RcSwitchTransmitter - Arduino libary for remote control transmitter Copyright (c)
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

#include "string.h"
#include "Whitening.hpp"

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

}
