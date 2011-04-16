/*******************************************************************************
 *
 * Copyright (c) 2011 Philipp Werner <philipp.werner@st.ovgu.de>
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
 * $Id: $
 *
 ******************************************************************************/

#ifndef __REALTIMELOGGER_H_EBF7F9476CC618__
#define __REALTIMELOGGER_H_EBF7F9476CC618__

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>

#ifndef LOGGING_OUTPUT_FILE
#define LOGGING_OUTPUT_FILE "rt.log"
#endif

namespace logging {

    template <bool use_stdout = false>
    class RealTimeLogger {
            enum { length = 1024 * 1024 };
            char * buffer;
            char * end;

            /// only written by write_char()
            char * curr_write;
            /// only written by flush()
            char * curr_flush;

            /// File descriptor
            int fd;

            /// Thread cancelation flag
            volatile bool done;

            void flush() {
                // Get sate of current write marker (may be incremented during this function)
                // -> pointer behind last byte to flush (set flush_end to curr_write)
                char * flush_end;
                (void)__sync_bool_compare_and_swap(&flush_end, flush_end, curr_write);

                if (flush_end == curr_flush) {
                    // Nothing to flush
                } else if (flush_end < curr_flush) {
                    // Block to write wraps around ring buffer end
                    // -> Write first part (curr_flush till end)
                    write(fd, curr_flush, end - curr_flush);
                    // -> Write second part (buffer till flush_end)
                    write(fd, buffer, flush_end - buffer);
                    // Atomic: set curr_flush to flush_end
                    (void)__sync_bool_compare_and_swap(&curr_flush, curr_flush, flush_end);
                } else if (flush_end > curr_flush) {
                    // Continuous block inside the buffer
                    write(fd, curr_flush, flush_end - curr_flush);
                    // Atomic: set curr_flush to flush_end
                    (void)__sync_bool_compare_and_swap(&curr_flush, curr_flush, flush_end);
                }
            }

            void write_char(const char c) {
                *curr_write = c;
                // Atomic increment
                (void)__sync_fetch_and_add(&curr_write, 1);
                // Atomic: if (curr_write == end) curr_write = buffer;
                (void)__sync_bool_compare_and_swap(&curr_write, end, buffer);
            }


            static void * flush_thread_func(void * arg) {
                RealTimeLogger * rto = reinterpret_cast<RealTimeLogger *>(arg);
                while (!rto->done) {
                    sleep(1);
                    rto->flush();
                }
                return 0;
            }

            pthread_t flush_thread;

        public:
            /// Opens file and starts buffer flushing thread
            RealTimeLogger() {
                mlockall (MCL_CURRENT | MCL_FUTURE);
                buffer = new char [length];
                curr_write = buffer;
                curr_flush = buffer;
                end = buffer + length;
                done = false;
                if (use_stdout) {
                    fd = 1;
                } else {
                    fd = open(LOGGING_OUTPUT_FILE, O_WRONLY | O_CREAT | O_TRUNC, 0644);
                }
                pthread_attr_t thread_attr;
                pthread_attr_init(&thread_attr);
                pthread_attr_setdetachstate(&thread_attr, PTHREAD_CREATE_JOINABLE);
                pthread_attr_setschedpolicy(&thread_attr, SCHED_OTHER);
                pthread_create(&flush_thread, 0, &flush_thread_func, this);
#ifdef __XENOMAI__
                pthread_set_name_np(flush_thread, "RT logging buffer flush thread");
#endif
                pthread_attr_destroy(&thread_attr);
            }

            /// Joins buffer flushing thread and closes file
            ~RealTimeLogger() {
                done = true;
                pthread_join(flush_thread, 0);
                if (!use_stdout) {
                    close(fd);
                }
                delete [] buffer;
            }

            /// operator to output a character
            RealTimeLogger & operator<<(const char c) {
                // Write char to buffer
                write_char(c);
                // If buffer is full (curr_write (just incremented by write_char()) equals curr_flush)...
                if (__sync_bool_compare_and_swap(&curr_write, curr_flush, curr_write)) {
                    // ... write warning
                    const char * warning = "### LOGGING BUFFER OVERFLOW! INCREASE BUFFER SIZE ###";
                    while (*warning) {
                        write_char(*(warning++));
                    }
                }
                return *this;
            }
    };

} /* logging */

#endif // __REALTIMELOGGER_H_EBF7F9476CC618__


