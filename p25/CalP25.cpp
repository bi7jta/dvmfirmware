/**
* Digital Voice Modem - DSP Firmware
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / DSP Firmware
*
*/
//
// Based on code from the MMDVM project. (https://github.com/g4klx/MMDVM)
// Licensed under the GPLv2 License (https://opensource.org/licenses/GPL-2.0)
//
/*
*   Copyright (C) 2018 by Andy Uribe CA6JAU
*
*   This program is free software; you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation; either version 2 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program; if not, write to the Free Software
*   Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/
#include "Globals.h"
#include "p25/CalP25.h"

using namespace p25;

// ---------------------------------------------------------------------------
//  Constants
// ---------------------------------------------------------------------------

// Recommended 1011 Hz test pattern for P25 Phase 1 (ANSI/TIA-102.CAAA)
// NAC: 0x293, srcID: 1, dstID: TG1
unsigned char LDU1_1K[] = { 0x00,
    0x55, 0x75, 0xF5, 0xFF, 0x77, 0xFF, 0x29, 0x35, 0x54, 0x7B, 0xCB, 0x19, 0x4D, 0x0D, 0xCE, 0x24, 0xA1, 0x24,
    0x0D, 0x43, 0x3C, 0x0B, 0xE1, 0xB9, 0x18, 0x44, 0xFC, 0xC1, 0x62, 0x96, 0x27, 0x60, 0xE4, 0xE2, 0x4A, 0x10,
    0x90, 0xD4, 0x33, 0xC0, 0xBE, 0x1B, 0x91, 0x84, 0x4C, 0xFC, 0x16, 0x29, 0x62, 0x76, 0x0E, 0xC0, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x89, 0x28, 0x49, 0x0D, 0x43, 0x3C, 0x02, 0xF8, 0x6E, 0x46, 0x11, 0x3F, 0xC1, 0x62, 0x94,
    0x89, 0xD8, 0x39, 0x00, 0x00, 0x00, 0x00, 0x1C, 0x38, 0x24, 0xA1, 0x24, 0x35, 0x0C, 0xF0, 0x2F, 0x86, 0xE4,
    0x18, 0x44, 0xFF, 0x05, 0x8A, 0x58, 0x9D, 0x83, 0xB0, 0x00, 0x00, 0x00, 0x00, 0x70, 0xE2, 0x4A, 0x12, 0x40,
    0xD4, 0x33, 0xC0, 0xBE, 0x1B, 0x91, 0x84, 0x4F, 0xF0, 0x16, 0x29, 0x62, 0x76, 0x0E, 0x6D, 0xE5, 0xD5, 0x48,
    0xAD, 0xE3, 0x89, 0x28, 0x49, 0x0D, 0x43, 0x3C, 0x08, 0xF8, 0x6E, 0x46, 0x11, 0x3F, 0xC1, 0x62, 0x96, 0x24,
    0xD8, 0x3B, 0xA1, 0x41, 0xC2, 0xD2, 0xBA, 0x38, 0x90, 0xA1, 0x24, 0x35, 0x0C, 0xF0, 0x2F, 0x86, 0xE4, 0x60,
    0x44, 0xFF, 0x05, 0x8A, 0x58, 0x9D, 0x83, 0x94, 0xC8, 0xFB, 0x02, 0x35, 0xA4, 0xE2, 0x4A, 0x12, 0x43, 0x50,
    0x33, 0xC0, 0xBE, 0x1B, 0x91, 0x84, 0x4F, 0xF0, 0x58, 0x29, 0x62, 0x76, 0x0E, 0xC0, 0x00, 0x00, 0x00, 0x0C,
    0x89, 0x28, 0x49, 0x0D, 0x43, 0x3C, 0x0B, 0xE1, 0xB8, 0x46, 0x11, 0x3F, 0xC1, 0x62, 0x96, 0x27, 0x60, 0xE4 };

unsigned char LDU2_1K[] = { 0x00,
    0x55, 0x75, 0xF5, 0xFF, 0x77, 0xFF, 0x29, 0x3A, 0xB8, 0xA4, 0xEF, 0xB0, 0x9A, 0x8A, 0xCE, 0x24, 0xA1, 0x24,
    0x0D, 0x43, 0x3C, 0x0B, 0xE1, 0xB9, 0x18, 0x44, 0xFC, 0xC1, 0x62, 0x96, 0x27, 0x60, 0xEC, 0xE2, 0x4A, 0x10,
    0x90, 0xD4, 0x33, 0xC0, 0xBE, 0x1B, 0x91, 0x84, 0x4C, 0xFC, 0x16, 0x29, 0x62, 0x76, 0x0E, 0x40, 0x00, 0x00,
    0x00, 0x00, 0x03, 0x89, 0x28, 0x49, 0x0D, 0x43, 0x3C, 0x02, 0xF8, 0x6E, 0x46, 0x11, 0x3F, 0xC1, 0x62, 0x94,
    0x89, 0xD8, 0x3B, 0x00, 0x00, 0x00, 0x00, 0x00, 0x38, 0x24, 0xA1, 0x24, 0x35, 0x0C, 0xF0, 0x2F, 0x86, 0xE4,
    0x18, 0x44, 0xFF, 0x05, 0x8A, 0x58, 0x9D, 0x83, 0x90, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE2, 0x4A, 0x12, 0x40,
    0xD4, 0x33, 0xC0, 0xBE, 0x1B, 0x91, 0x84, 0x4F, 0xF0, 0x16, 0x29, 0x62, 0x76, 0x0E, 0xE0, 0xE0, 0x00, 0x00,
    0x00, 0x03, 0x89, 0x28, 0x49, 0x0D, 0x43, 0x3C, 0x08, 0xF8, 0x6E, 0x46, 0x11, 0x3F, 0xC1, 0x62, 0x96, 0x24,
    0xD8, 0x39, 0xAE, 0x8B, 0x48, 0xB6, 0x49, 0x38, 0x90, 0xA1, 0x24, 0x35, 0x0C, 0xF0, 0x2F, 0x86, 0xE4, 0x60,
    0x44, 0xFF, 0x05, 0x8A, 0x58, 0x9D, 0x83, 0xB9, 0xA8, 0xF4, 0xF1, 0xFD, 0x60, 0xE2, 0x4A, 0x12, 0x43, 0x50,
    0x33, 0xC0, 0xBE, 0x1B, 0x91, 0x84, 0x4F, 0xF0, 0x58, 0x29, 0x62, 0x76, 0x0E, 0x40, 0x00, 0x00, 0x00, 0x0C,
    0x89, 0x28, 0x49, 0x0D, 0x43, 0x3C, 0x0B, 0xE1, 0xB8, 0x46, 0x11, 0x3F, 0xC1, 0x62, 0x96, 0x27, 0x60, 0xEC };

// ---------------------------------------------------------------------------
//  Public Class Members
// ---------------------------------------------------------------------------

/// <summary>
/// Initializes a new instance of the CalP25 class.
/// </summary>
CalP25::CalP25() :
    m_transmit(false),
    m_state(P25CAL1K_IDLE)
{
    /* stub */
}

/// <summary>
/// Process local state and transmit on the air interface.
/// </summary>
void CalP25::process()
{
    if (m_modemState == STATE_P25_CAL) {
        m_state = P25CAL1K_IDLE; 

        if (m_transmit) {
            p25TX.setCal(true);
            p25TX.process();
        }
        else {
            p25TX.setCal(false);
        }
    }
    else {
        p25TX.process();

        uint16_t space = p25TX.getSpace();
        if (space < 1U)
            return;

        switch (m_state) {
            case P25CAL1K_LDU1:
                p25TX.writeData(LDU1_1K, P25_LDU_FRAME_LENGTH_BYTES + 1U);
                m_state = P25CAL1K_LDU2;
                break;
            case P25CAL1K_LDU2:
                p25TX.writeData(LDU2_1K, P25_LDU_FRAME_LENGTH_BYTES + 1U);
                if (!m_transmit) {
                    m_state = P25CAL1K_IDLE;
                }
                else
                    m_state = P25CAL1K_LDU1;
                break;
            default:
                m_state = P25CAL1K_IDLE;
                break;
        }
    }
}

/// <summary>
/// Write P25 calibration data to the local buffer.
/// </summary>
/// <param name="data"></param>
/// <param name="length"></param>
/// <returns></returns>
uint8_t CalP25::write(const uint8_t* data, uint8_t length)
{
    if (length != 1U)
        return RSN_ILLEGAL_LENGTH;

    m_transmit = data[0U] == 1U;

    if (m_transmit && m_state == P25CAL1K_IDLE)
        m_state = P25CAL1K_LDU1;

    return RSN_OK;
}
