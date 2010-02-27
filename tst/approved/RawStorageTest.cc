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

#include <string.h>
#include <stdio.h>

#include "logging/logging.h"

#define RAWSTORAGE_LOG_USAGE
#define RAWSTORAGE_FULL_MEM_INFO
#include "object/RawStorage.h"


void print_help() {
    fprintf(stderr,
            "Command error! Valid commands are:\n"
            "  \"alloc <len>\" | \"a ...\" -> addr\n"
            "  \"free <addr>\" | \"f ...\"\n"
            "  \"show\" | \"s ...\"\n"
            "  <CTRL>-<C> to exit\n"
           );
}

#define BUF_SIZE 100

template <uint64_t size>
void test() {
    object::RawStorage<size> rs;
    char buf [BUF_SIZE];
    uint8_t * memory;

    while (1) {
        printf("> ");
        if (scanf("%s", buf) == 0) {
            print_help();
        } else {
            if (strncmp(buf, "alloc", BUF_SIZE) == 0 || strncmp(buf, "a", BUF_SIZE) == 0) {
                unsigned long len;

                if (!scanf("%lu", &len)) {
                    print_help();
                    continue;
                }

                memory = rs.alloc(len);
                if (memory) {
                    for (unsigned int i = 0; i < len; i++)
                        memory[i] = i;
                }

                printf("-> Address: %p\n", memory);
            } else if (strncmp(buf, "free", BUF_SIZE) == 0 || strncmp(buf, "f", BUF_SIZE) == 0) {
                unsigned long addr;
                if (!scanf("%lx", &addr)) {
                    print_help();
                    continue;
                }

                uint8_t * memory = (uint8_t *) addr;
                int len = rs.get_length(memory);
                for (int i = 0; i < len; i++) {
                    if (memory[i] != i) {
                        printf("Memory test pattern error in %p!\n", memory + i);
                        break;
                    }
                }

                rs.free((uint8_t*)addr);
            } else if (strncmp(buf, "show", BUF_SIZE) == 0 || strncmp(buf, "s", BUF_SIZE) == 0) {
                rs.print_mem_info();
            } else {
                print_help();
            }
        }
    }
}

int main() {
    test<100>();
}


