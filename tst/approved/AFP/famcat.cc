/*******************************************************************************
 *
 * Copyright (c) 2009-2010 Philipp Werner <philipp.werner@st.ovgu.de>
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


/*!
 * \file
 * \brief Tool like netcat using AFP and famouso for message/event
 *        transmission. ATM only publishes one event and exits. Default
 *        is to subscribe for one event.
 */


// Redirect logging to stderr
#define LOGGING_DEFINE_EXTENDED_OUTPUT_TYPE
#include "logging/logging.h"
LOGGING_DEFINE_OUTPUT( ::logging::config::StdErrLogType )


#include "famouso_bindings.h"

#include "mw/afp/FragConfig.h"
#include "mw/afp/DefragConfig.h"
#include "mw/afp/AFPPublisherEventChannel.h"
#include "mw/afp/AFPSubscriberEventChannel.h"
#include "mw/afp/shared/hexdump.h"

#include "LargeEvent.h"
using famouso::mw::afp::LargeEvent;

#include <stdio.h>

using namespace famouso::mw;

struct CustomPolicies {
    typedef afp::LargeEventSizeProp SizeProperties;
};

typedef afp::DefragConfig <
    afp::one_subject |
    afp::packet_loss | afp::reordering | afp::duplicates |
    afp::FEC,
    CustomPolicies
> MyAFPDefragConfig;

typedef afp::FragConfig <
    afp::packet_loss | afp::reordering | afp::duplicates |
    afp::FEC,
    CustomPolicies
> MyAFPFragConfig;


const char * const default_channel_subject = "_famcat_";
const unsigned int default_mtu = 32;


volatile unsigned int received_events = 0;
unsigned int subscribe_events_num = 1;

void ReceiveCallback(const afp::LargeEvent & event) {
    if (received_events < subscribe_events_num) {
        fwrite(event.data, event.length, 1, stdout);
        fflush(stdout);
        received_events++;
    }
}



void EchoCallback(famouso::mw::api::SECCallBackData& cbd) {
    fprintf(stderr, "Fragment (%u Bytes):\n", (unsigned int)cbd.length);
    afp::shared::hexdump(cbd.data, cbd.length);
}


int main(int argc, char ** argv) {
    enum { subscribe, publish } operation = subscribe;
    unsigned int channel_mtu = default_mtu;
    const char * channel_subject = default_channel_subject;
    bool echo_hexdump = false;

    bool wrong_args = (argc < 1);
    int arg = 1;
    enum { other, subject, mtu, red, sub_num } next_arg = other;

    while (!wrong_args && arg < argc) {
        if (argv[arg][0] == '-') {
            switch (argv[arg][1]) {
                case 's': operation = subscribe; break;
                case 'p': operation = publish; break;
                case 'c': next_arg = subject; break;
                case 'm': next_arg = mtu;  break;
                case 'r': next_arg = red;  break;
                case 'n': next_arg = sub_num;  break;
                case 'e': echo_hexdump = true;  break;
                default: wrong_args = true; break;
            }
        } else {
            if (next_arg == subject) {
                channel_subject = argv[arg];
                next_arg = other;
            } else if (next_arg == mtu) {
                channel_mtu = atoi(argv[arg]);
                next_arg = other;
            } else if (next_arg == red) {
                int red = atoi(argv[arg]);
                if (red < 0 || red > 255) {
                    fprintf(stderr, "Redundancy value: 0 <= RED <= 255\n");
                    wrong_args = true;
                } else {
                    afp::FragmentationRedundancy::value() = red;
                }
                next_arg = other;
            } else if (next_arg == sub_num) {
                subscribe_events_num = atoi(argv[arg]);
                if (subscribe_events_num <= 0) {
                    fprintf(stderr, "Event number: 0 < NUM\n");
                    wrong_args = true;
                }
                next_arg = other;
            } else {
                wrong_args = true;
            }
        }
        arg++;
    }

    if (wrong_args) {
        fprintf(stderr, "Usage:\n"
                "\tfamcat -s [-c CANNEL] [-m MTU] [-e] [-n NUM]\n"
                "\tfamcat -p [-c CANNEL] [-m MTU] [-e] [-r RED]\n"
                "\n"
                "\t-s         subscribe, wait for an event and output data to stdout (default)\n"
                "\t-p         publish event from stdin\n"
                "\t-c CHANNEL specify subject to use, default: \"%s\"\n"
                "\t-m MTU     specify specific MTU to use, default: %u\n"
                "\t-e         echo a hexdump of fragments to stderr\n"
                "\t-n NUM     subsribe for NUM events\n"
                "\t-r RED     add RED %% forward error correction redundancy\n",
                default_channel_subject, (unsigned int)default_mtu);
        return -1;
    } else {
        famouso::init<famouso::config>();

        famouso::config::SEC echo_sec(channel_subject);
        if (echo_hexdump) {
            echo_sec.callback.bind<EchoCallback>();
            echo_sec.subscribe();
        }

        if (operation == subscribe) {

            afp::AFPSubscriberEventChannel<famouso::config::SEC, MyAFPDefragConfig, LargeEvent> sec(channel_subject, channel_mtu);
            sec.callback.bind<ReceiveCallback>();
            sec.subscribe();

            while (received_events < subscribe_events_num) {
                usleep(100000);
            }

        } else if (operation == publish) {

            // Announce
            afp::AFPPublisherEventChannel<famouso::config::PEC, MyAFPFragConfig, 0, LargeEvent> pec(channel_subject, channel_mtu);
            pec.announce();

            // Read data from stdin
            uint32_t buffered_size = 16 * 1024;
            uint8_t * buffer = reinterpret_cast<uint8_t *>(malloc(buffered_size));
            uint32_t size = 0;

            if (!buffer) {
                perror("Error");
                return -1;
            }

            while (!feof(stdin)) {
                if (buffered_size <= size) {
                    buffered_size = size + 16 * 1024;
                    buffer = reinterpret_cast<uint8_t *>(realloc(buffer, buffered_size));
                    if (!buffer) {
                        perror("Error");
                        return -1;
                    }

                }
                size += fread(buffer + size, 1, buffered_size - size, stdin);
            }

            // Publish
            afp::LargeEvent event (pec.subject());
            event.data = buffer;
            event.length = size;
            pec.publish(event);

            free(buffer);
        }

        if (echo_hexdump) {
            usleep(500000);
        }
    }

    return 0;
}

