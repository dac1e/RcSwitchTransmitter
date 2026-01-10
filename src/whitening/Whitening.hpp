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

#pragma once

#ifndef RCSWITCHTRANSMITTER_WHITENING_HPP_
#define RCSWITCHTRANSMITTER_WHITENING_HPP_

#include <stdint.h>
#include <stdlib.h>

namespace RcSwitchTx {

/**
 * Taken from Freescsale Semiconductor AN5070 Rev. 0, 07/2015, page 7 and modified to cover bit accuracy.
 *
 * Optionally use this function for whitening the data to be transmitted. The data can be de-whitnened
 * on the receiver side by feeding this same function with the received whitened data.
 */
void computeWhitening(uint8_t* out, const uint8_t* in, const size_t bitCount);
void computeWhitening(uint8_t* inOut, const size_t bitCount);

}

#endif /* RCSWITCHTRANSMITTER_WHITENING_HPP_ */
