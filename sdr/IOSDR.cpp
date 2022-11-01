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
*   Copyright (C) 2016 by Jim McLaughlin KI6ZUM
*   Copyright (C) 2016,2017,2018 by Andy Uribe CA6JAU
*   Copyright (C) 2017,2018 by Jonathan Naylor G4KLX
*   Copyright (C) 2017-2022 Bryan Biedenkapp N2PLL
*   Copyright (C) 2022 Natalie Moore
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
#include "IO.h"
#include "sdr/Log.h"

#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include <vector>

#include <zmq.hpp>

// ---------------------------------------------------------------------------
//  Constants
// ---------------------------------------------------------------------------

#if defined(NATIVE_SDR) || true

const uint16_t DC_OFFSET = 2048U;

// ---------------------------------------------------------------------------
//  Globals Variables
// ---------------------------------------------------------------------------

static pthread_t m_threadTx;
static pthread_mutex_t m_txLock;
static pthread_t m_threadRx;
static pthread_mutex_t m_rxLock;

static zmq::context_t m_zmqContextTx;
static zmq::socket_t m_zmqSocketTx;
static std::vector<short> m_audioBufTx = std::vector<short>();

static zmq::context_t m_zmqContextRx;
static zmq::socket_t m_zmqSocketRx;
static std::vector<short> m_audioBufRx = std::vector<short>();

static bool m_cosInt = false;

// ---------------------------------------------------------------------------
//  Public Class Members
// ---------------------------------------------------------------------------

/// <summary>
/// Hardware interrupt handler.
/// </summary>
void IO::interrupt()
{
    uint16_t sample = DC_OFFSET;
    uint8_t control = MARK_NONE;

    ::pthread_mutex_lock(&m_txLock);
    while(m_txBuffer.get(sample, control))
    {
        sample *= 5; // amplify by 12dB	

        if (m_audioBufTx.size() >= 720)
        {
            zmq::message_t reply = zmq::message_t(720 * sizeof(short));
            ::memcpy(reply.data(), (unsigned char*)m_audioBufTx.data(), 720 * sizeof(short));

            m_zmqSocketTx.send(reply, zmq::send_flags::dontwait);

            usleep(9600 * 3);
            
            m_audioBufTx.erase(m_audioBufTx.begin(), m_audioBufTx.begin() + 720);
            m_audioBufTx.push_back((short)sample);
        }
        else
            m_audioBufTx.push_back((short)sample);
    }
    ::pthread_mutex_unlock(&m_txLock);
   
    sample = 2048U;
    m_watchdog++;
}

/// <summary>
/// Gets the CPU type the firmware is running on.
/// </summary>
/// <returns></returns>
uint8_t IO::getCPU() const
{
    return CPU_TYPE_NATIVE_SDR;
}

/// <summary>
/// Gets the unique identifier for the air interface.
/// </summary>
/// <returns></returns>
void IO::getUDID(uint8_t* buffer)
{
    /* stub */
}

// ---------------------------------------------------------------------------
//  Private Class Members
// ---------------------------------------------------------------------------

/// <summary>
/// Initializes hardware interrupts.
/// </summary>
void IO::initInt()
{
    m_zmqContextTx = zmq::context_t(1);
    m_zmqSocketTx = zmq::socket_t(m_zmqContextTx, ZMQ_PUSH);

    m_zmqContextRx = zmq::context_t(1);
    m_zmqSocketRx = zmq::socket_t(m_zmqContextRx, ZMQ_PULL);

    m_audioBufTx = std::vector<short>();
    m_audioBufRx = std::vector<short>();
}

/// <summary>
/// Starts hardware interrupts.
/// </summary>
void IO::startInt()
{
    m_zmqSocketTx.bind(m_zmqTx);
    m_zmqSocketRx.connect(m_zmqRx);

    if (::pthread_mutex_init(&m_txLock, NULL) != 0) {
        ::LogError(LOG_DSP, "Tx thread lock failed?");
        ::LogFinalise();
        exit(-1);
    }

    if (::pthread_mutex_init(&m_rxLock, NULL) != 0) {
        ::LogError(LOG_DSP, "Rx thread lock failed?");
        ::LogFinalise();
        exit(-2);
    }

    ::pthread_create(&m_threadTx, NULL, txThreadHelper, this);
    ::pthread_create(&m_threadRx, NULL, rxThreadHelper, this);
}

/// <summary></summary>
/// <returns></returns>
bool IO::getCOSInt()
{
    return m_cosInt;
}

/// <summary></summary>
/// <param name="on"></param>
void IO::setLEDInt(bool on)
{
    /* stub */
}

/// <summary></summary>
/// <param name="on"></param>
void IO::setPTTInt(bool on)
{
    /* stub */
}

/// <summary></summary>
/// <param name="on"></param>
void IO::setCOSInt(bool on)
{
    m_cosInt = on;
}

/// <summary></summary>
/// <param name="on"></param>
void IO::setDMRInt(bool on)
{
    /* stub */
}

/// <summary></summary>
/// <param name="on"></param>
void IO::setP25Int(bool on)
{
    /* stub */
}

/// <summary></summary>
/// <param name="on"></param>
void IO::setNXDNInt(bool on)
{
    /* stub */
}

/// <summary></summary>
/// <param name="dly"></param>
void IO::delayInt(unsigned int dly)
{
    usleep(dly * 1000);
}

/// <summary></summary>
/// <param name="arg"></param>
/// <returns></returns>
void* IO::txThreadHelper(void* arg)
{
    IO* p = (IO*)arg;

    while (true)
    {
        if (p->m_txBuffer.getData() < 1)
            usleep(20);
        p->interrupt();
    }

    return NULL;
}

/// <summary></summary>
void IO::interruptRx()
{
    uint16_t sample = DC_OFFSET;
    uint8_t control = MARK_NONE;
    
    zmq::message_t msg;
    zmq::recv_result_t recv = m_zmqSocketRx.recv(msg, zmq::recv_flags::none);

    int size = msg.size();
    if (size < 1)
        return;

    ::pthread_mutex_lock(&m_rxLock);
    uint16_t space = m_rxBuffer.getSpace();

    for (int i = 0; i < size; i += 2)
    {
        short sample = 0;
        ::memcpy(&sample, (unsigned char*)msg.data() + i, sizeof(short));

        m_rxBuffer.put((uint16_t)sample, control);
        m_rssiBuffer.put(3U);
    }
    ::pthread_mutex_unlock(&m_rxLock);        
}

/// <summary></summary>
/// <param name="arg"></param>
/// <returns></returns>
void* IO::rxThreadHelper(void* arg)
{
    IO* p = (IO*)arg;

    while (true)
        p->interruptRx();

    return NULL;
}

#endif // defined(NATIVE_SDR)