/**
* Digital Voice Modem - DSP Firmware
* GPLv2 Open Source. Use is subject to license terms.
* DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER.
*
* @package DVM / DSP Firmware
*
*/
//
// Based on code from the MMDVMHost project. (https://github.com/g4klx/MMDVMHost)
// Licensed under the GPLv2 License (https://opensource.org/licenses/GPL-2.0)
//
/*
*   Copyright (C) 2016,2021 by Jonathan Naylor G4KLX
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
#if !defined(__I_SERIAL_PORT_H__)
#define __I_SERIAL_PORT_H__

#include "Defines.h"

namespace sdr
{
    namespace port
    {
        // ---------------------------------------------------------------------------
        //  Class Declaration
        //      Defines a serial port.
        // ---------------------------------------------------------------------------

        class DSP_FW_API ISerialPort {
        public:
            /// <summary>Finalizes a instance of the ISerialPort class.</summary>
            virtual ~ISerialPort() = 0;

            /// <summary>Opens a connection to the port.</summary>
            virtual bool open() = 0;

            /// <summary>Reads data from the port.</summary>
            virtual int read(uint8_t* buffer, uint32_t length) = 0;
            /// <summary>Writes data to the port.</summary>
            virtual int write(const uint8_t* buffer, uint32_t length) = 0;

            /// <summary>Closes the connection to the port.</summary>
            virtual void close() = 0;
        }; // class DSP_FW_API ISerialPort
    } // namespace port
} // namespace sdr

#endif // __I_SERIAL_PORT_H__
