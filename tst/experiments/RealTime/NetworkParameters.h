/*******************************************************************************
 *
 * Copyright (c) 2010 Philipp Werner <philipp.werner@st.ovgu.de>
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

#ifndef __NETWORKPARAMETERS_H_FA83C39FD78C37__
#define __NETWORKPARAMETERS_H_FA83C39FD78C37__

#include <stdint.h>

//void CAN(bits_per_sec, network_id, clock_granul, slot_granul)
// us_per_slot = clock_granul * slot_granul
void bytes_to_slot_params_test(uint16_t payload_bytes, uint32_t & slot_length_us, uint32_t & slot_last_transm_us) {
    // dummy impl
    slot_length_us = 2*payload_bytes;
    slot_last_transm_us = payload_bytes;
    // packetzugabe (zeitpuffer pro paket)
}


struct NetworkParameters {
    // muss auch header des NL und darunter betrachten
    void (* bytes_to_slot_params)(uint16_t payload_bytes, uint32_t & slot_length_us, uint32_t & slot_last_transm_us);
    uint32_t usec_per_aslot;
    bool need_free_slots;
    uint64_t network_id;

    // TODO: init externally
    NetworkParameters() {
        // atomic slot: 10 milli sec
        usec_per_aslot = 10000;         // vielfaches von Uhrengranularität!
        need_free_slots = false;        // frei-liste publishen und auf ACK warten?
        network_id = 0;
        bytes_to_slot_params = &bytes_to_slot_params_test;
    }
    // Funktionen:
    // bytes_to_slots (inklusive reserve, fragmentierungsoverhead)
    // - bytes -> (slot time, slot last transmission time)

    // CAN:
    // #fragments = f(#bytes)
    // time = 2 * #fragments * wc_bits_per_fragment / bits_per_us)     // Backgroundtraffic kommt dazwischen -> Faktor 2
    // last_trans_time = time(#fragments - 1)
    // !! + zustätzliche Latenz auf Senderechner bedenken (auf uC Latenz durch Codeausführung nicht vernachlässigbar, auf PC untersuchen!)
};



#endif // __NETWORKPARAMETERS_H_FA83C39FD78C37__

