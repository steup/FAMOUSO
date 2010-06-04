/*******************************************************************************
 *
 * Copyright (c) 2010 Thomas Kiebel <kiebel@ivs.cs.uni-magdeburg.de>
 * All rights reserved.
 *
 *    Redistribution and use in source and binary forms, with or without
 *    modification, are permitted provided that the following conditions
 *    are met:
 *
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in
 *      the documentation and/or other materials provided with the
 *      distribution.
 *
 *    * Neither the name of the copyright holders nor the names of
 *      contributors may be used to endorse or promote products derived
 *      from this software without specific prior written permission.
 *
 *
 *    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
 *    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 *    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 *    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * $Id$
 *
 ******************************************************************************/
#ifndef __BOOST_ASIO_SERIAL_DRIVER_h__
#define __BOOST_ASIO_SERIAL_DRIVER_h__

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/serial_port.hpp>
#include <deque>
#include <iostream>

#include "case/Delegate.h"
#include "devices/nic/serial/SerialFrameDelimiter.h"
#include "util/CommandLineParameterGenerator.h"
#include "util/ios.h"

#include "debug.h"

using namespace std;
namespace device {
    namespace nic {
        namespace serial {

            CLP2(ASIOOptions,
                 "ASIO Serial Driver",
                 "serial,s",
                 "The device that is used and the baudrate in the form of device:baudrate"
                 "Values:\n"
                 "  Device:     \tdevice that has to be used\n"
                 "  Baudrate:   \tlegal values are\n"
                 "                 0 =    300 bps\n"
                 "                 1 =    600 bps\n"
                 "                 2 =   1200 bps\n"
                 "                 3 =   2400 bps\n"
                 "                 4 =   4800 bps\n"
                 "                 5 =   9600 bps\n"
                 "                 6 =  19200 bps\n"
                 "                 7 =  38400 bps\n"
                 "                 8 =  57600 bps\n"
                 "                 9 = 115200 bps\n"
                 "  Packetsize: \tmax size of a packet\n"
                 "(e.g. /dev/ttyS0:6: (default))",
                 std::string, device, "/dev/ttyS0",
                 int, baudrate, 6);

            /*! \class  AsioSerialDriver AsioSerialDriver.h "device/nic/serial/AsioSerialDriver.h"
             *  \brief  %ASIO based %driver for the serial network layer.
             *
             *  \tparam available payload of the %driver (255 byte)
             *  \tparam byte modifier for framing/stuffing
             *  \todo separate hardware management from package format management
             *  \todo use same framing/stuffing implementation as the AtmelDriver
             */
            template <
                        uint8_t PL  = 255,
                        class SFD = struct SerialFrameDelimiter
                     >
            class AsioSerialDriver : boost::noncopyable {
                public:
                    /*! \brief  definition of class type*/
                    typedef AsioSerialDriver< PL, SFD > type;
                    /*! \brief  definition of driver specific information*/
                    struct cfgInfo {
                        enum {
                            payload = PL
                        };
                    };
                    /*! \brief  definition of driver specific message object*/
                    typedef struct __attribute__((packed)) {
                        uint8_t size;
                        uint8_t payload[cfgInfo::payload];
                    } mob_t;

                    AsioSerialDriver() :
                        _io_service(famouso::util::ios::instance()),
                        _serialPort(_io_service) {
                    }
                    ~AsioSerialDriver() {
                        _serialPort.close();
                    }

                    /*! \brief  Closes the connection of the driver.
                     *  \param[in] error cause of the connection close
                     */
                    void close(const boost::system::error_code& error) {
                        // if this call is the result of a timer cancel() ignore it
                        // because the connection cancelled the timer
                        if (error == boost::asio::error::operation_aborted)
                            return;
                        if (error) {
                            ::logging::log::emit< ::logging::Error>()
                                << error.message().c_str()
                                << ::logging::log::endl;
                        }
                        _serialPort.close();
                    }

                    /*! \brief  Triggered if async_read_until finished or failed.
                     *
                     *  \param[in]  error   %error that occured if the operation has failed
                     *  \param[in]  transferred   number of bytes transfered
                     */
                    void doneRead(const boost::system::error_code& error, std::size_t size) {
                        char aChar;

                        if (!error) {
                            std::istream is(&trxBuffer);
                            while (size--) {
                                driverState = ( _rmobj.size > cfgInfo::payload ) ? invalid : driverState;
                                is.get( aChar );
                                switch ( aChar ) {
                                    case (SFD::sofr): // frame start
                                        driverState = ( driverState == invalid ) ? regular : invalid;
                                        _rmobj.size  = 0;
                                        break;
                                    case (SFD::eofr): // frame end
                                        driverState = ( driverState == regular ) ? valid   : invalid;
                                        if ( (driverState == valid) && this->onReceive) this->onReceive();
                                        break;
                                    case (SFD::esc):  // modifier detected
                                        driverState = ( driverState == regular ) ? stuff   : invalid;
                                        break;
                                    default:          // everything else
                                        if ( driverState == regular || driverState == stuff ) {
                                            aChar = ( driverState == stuff && SFD::escmod != 0) ? (aChar ^ SFD::escmod) : aChar;
                                            _rmobj.payload[_rmobj.size++] = (uint8_t)aChar;
                                            driverState  = regular;
                                        } else driverState = invalid;
                                        break;
                                }
                            }
                            boost::asio::async_read_until(
                                _serialPort, trxBuffer, SFD::eofr,
                                boost::bind(&type::doneRead, this,
                                            boost::asio::placeholders::error,
                                            boost::asio::placeholders::bytes_transferred));
                        } else close(error);
                    }

                    /*! \brief  Triggered after async_write finished or failed.
                     *
                     *  \param[in] error  code of error that occured
                     */
                    void doneWrite(const boost::system::error_code& error, std::size_t size) {
                        if ( !error ) {
                            trxBuffer.consume(size);
                            if ( this->onReady ) this->onReady();
                        } else close(error);
                    }

                    /*! \brief  (Re)Initializes the %driver for the serial
                     *          connection.
                     *
                     *  Initializes the connection with default values
                     *  (BaudRade 8N1) and checks if connection could be
                     *  opened.
                     */
                    void init() {
                        TRACE_FUNCTION;

                        CLP::config::ASIOOptions::Parameter param;
                        CLP::config::ASIOOptions::instance().getParameter(param);
                        if ( (param.baudrate < 0) || (param.baudrate > 9) ) {
                            ::logging::log::emit< ::logging::Error>()
                                << "Error: parameter baudrate out of Range"
                                << ::logging::log::endl;
                            exit(0);
                        }

                        famouso::util::impl::start_ios();
                        // open serial port for given port
                        _serialPort.open( param.device.c_str() );
                        if ( !_serialPort.is_open() ) {
                            ::logging::log::emit< ::logging::Error>()
                                << "can't open serial device "
                                << param.device.c_str() << ::logging::log::endl;
                            exit(0);
                        }
                        // set defaults for serial port
                        _serialPort.set_option(
                            boost::asio::serial_port_base::character_size( 8 ) );
                        _serialPort.set_option(
                            boost::asio::serial_port_base::flow_control(
                                boost::asio::serial_port_base::flow_control::none ) );
                        uint16_t baudrate = (0x01 << param.baudrate) * 300;
                        _serialPort.set_option(
                            boost::asio::serial_port_base::baud_rate( baudrate ) );
                        _serialPort.set_option(
                            boost::asio::serial_port_base::parity(
                                boost::asio::serial_port_base::parity::none ) );
                        _serialPort.set_option(
                            boost::asio::serial_port_base::stop_bits(
                                boost::asio::serial_port_base::stop_bits::one) );
                        // start reading data
                        boost::asio::async_read_until(
                            _serialPort, trxBuffer, SFD::eofr,
                            boost::bind(&type::doneRead, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
                    }

                    /*! \brief  Fetches the message received last.
                     *  \param[out] message
                     */
                    void recv(mob_t& msg) {
                        TRACE_FUNCTION;

                        msg.size = 0;
                        while ( driverState != valid ); // message available

                        for (;msg.size < _rmobj.size; msg.size++) {
                            msg.payload[msg.size] = _rmobj.payload[msg.size];
                        }
                        driverState = invalid;
                    }

                    /*! \brief  Transmits a message using the serial port.
                     *  \param[in] mob message %object that is to be send
                     */
                    void send(const mob_t& message) {
                        TRACE_FUNCTION;

                        std::ostream os(&trxBuffer);
                        uint8_t aByte = 0;

                        // transfer data into the send buffer
                        os.put( (uint8_t)SFD::sofr );
                        for (uint16_t index = 0;index < message.size;index++) {
                            aByte = message.payload[index];
                            if (    aByte == SFD::esc ||
                                    aByte == SFD::eofr ||
                                    aByte == SFD::sofr    ) {
                                os.put( (uint8_t)SFD::esc );
                                os.put( ((SFD::escmod != 0) ? aByte ^ SFD::escmod : aByte));
                            } else {
                                os.put( aByte );
                            }
                        }
                        os.put( (uint8_t)SFD::eofr );
                        // start ascynchronous send of message
                        boost::asio::async_write(
                            _serialPort,
                            trxBuffer,
                            boost::bind(&type::doneWrite, this,
                                        boost::asio::placeholders::error,
                                        boost::asio::placeholders::bytes_transferred));
                    }

                    /*! \brief  Triggered if a packet was received.*/
                    famouso::util::Delegate<> onReceive;

                    /*! \brief  Triggered if %driver is able to send
                     *          new/further messages.
                     *  \note   This functionality is not yet implemented.
                     *  \todo   provide functionality for the onReady() interrupt
                     */
                    famouso::util::Delegate<> onReady;
                private:
                    boost::asio::io_service& _io_service;
                    boost::asio::serial_port _serialPort;
                    boost::asio::streambuf trxBuffer;
                    enum { valid, invalid, regular, stuff } driverState;
                    mob_t _rmobj;
            }; // class AsioDriver
        } /* namespace serial */
    } /* namesace nic */
} /* namespace device */

#endif  //__BOOST_ASIO_SERIAL_DRIVER_h__
