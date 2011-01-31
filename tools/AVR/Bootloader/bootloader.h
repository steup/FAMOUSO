/*******************************************************************************
 *
 * Copyright (c) 2010 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
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

#ifndef __BOOTLOADER_H_7D4D59F02C4527__
#define __BOOTLOADER_H_7D4D59F02C4527__

#include <stdint.h>
#include <avr/io.h>
#include <avr/wdt.h>
#include <avr/boot.h>
#include <avr/pgmspace.h>
#include <util/delay.h>

//uint8_t gBuffer[SPM_PAGESIZE];

static inline void boot_program_page (uint32_t page, uint8_t *buf) {
    uint16_t i;
    uint8_t sreg;
    uint16_t w;
    // Disable interrupts.

    sreg = SREG;
    cli();

    eeprom_busy_wait ();

    boot_page_erase (page);
    boot_spm_busy_wait ();      // Wait until the memory is erased.

    for (i=0; i<SPM_PAGESIZE; i+=2)
    {
        // Set up little-endian word.

//        ::logging::log::emit() << buf[i] << buf[i+1];
//        w = (buf[i]);
//        w|= (buf[i+1]<<8);
        w = (buf[i+1]);
        w<<=8;
        w|= buf[i];
//        GPIOR1=w;
//w=        0x5566;
//        w = *buf++;
//        w += (*buf++) << 8;
        boot_page_fill (page + i, w);
        boot_spm_busy_wait ();      // Wait until the memory is erased.
    }

    boot_page_write (page);     // Store buffer in flash page.
    boot_spm_busy_wait();       // Wait until the memory is written.

    // Reenable RWW-section again. We need this if we want to jump back
    // to the application after bootloading.

    boot_rww_enable ();

    // Re-enable interrupts (if they were ever enabled).

    SREG = sreg;
}


//static inline void eraseFlash(void)
//{
//	// erase only main section (bootloader protection)
//	uint32_t addr = 0;
//	while (Board<BOARD>::maxAppEnd > addr) {
//		boot_page_erase(addr);		// Perform page erase
//		boot_spm_busy_wait();		// Wait until the memory is erased.
//		addr += Board<BOARD>::flashPageSize;
//	}
//	boot_rww_enable();
//}
//
//static inline uint16_t writeFlashPage(uint16_t waddr, pagebuf_t size, uint8_t *tmp = gBuffer) {
//	uint32_t pagestart = (uint32_t)waddr<<1;
//	uint32_t baddr = pagestart;
//	uint16_t data;
//
//	do {
//		data = *tmp++;
//		data |= *tmp++ << 8;
//		boot_page_fill(baddr, data);	// call asm routine.
//
//		baddr += 2;			// Select next word in memory
//		size -= 2;			// Reduce number of bytes to write by two
//	} while (size);				// Loop until all bytes written
//
//	boot_page_write(pagestart);
//	boot_spm_busy_wait();
//	boot_rww_enable();			// Re-enable the RWW section
//
//	return baddr>>1;
//}
//
//static inline uint16_t writeEEpromPage(uint16_t address, pagebuf_t size, uint8_t *tmp = gBuffer) {
//
//	do {
//		EEARL = address;		// Setup EEPROM address
//		EEARH = (address >> 8);
//		EEDR = *tmp++;
//		address++;			// Select next byte
//
//		EECR |= (1<<EEMWE);		// Write data into EEPROM
//		EECR |= (1<<EEWE);
//		eeprom_busy_wait();
//
//		size--;				// Decreas number of bytes to write
//	} while (size);				// Loop until all bytes written
//
//	return address;
//}
//

//int main(void) {
//	uint16_t address = 0;
//	uint8_t device = 0, val;
//
//#ifdef START_POWERSAVE
//	uint8_t OK = 1;
//#endif
//
//
//
//
//
//	BLDDR  &= ~(1<<BLPNUM);		// set as Input
//	BLPORT |= (1<<BLPNUM);		// Enable pullup
//
//	// Set baud rate
//	UART_BAUD_HIGH = (UART_CALC_BAUDRATE(BAUDRATE)>>8) & 0xFF;
//	UART_BAUD_LOW = (UART_CALC_BAUDRATE(BAUDRATE) & 0xFF);
//
//#ifdef UART_DOUBLESPEED
//	UART_STATUS = UART_DOUBLE;
//#endif
//
//	UART_CTRL = UART_CTRL_DATA;
//	UART_CTRL2 = UART_CTRL2_DATA;
//
//#if defined(START_POWERSAVE)
//	/*
//		This is an adoption of the Butterfly Bootloader startup-sequence.
//		It may look a little strange but separating the login-loop from
//		the main parser-loop gives a lot a possibilities (timeout, sleep-modes
//	    etc.).
//	*/
//	for(;OK;) {
//		if ((BLPIN & (1<<BLPNUM))) {
//			// jump to main app if pin is not grounded
//			BLPORT &= ~(1<<BLPNUM);	// set to default
//			jump_to_app();		// Jump to application sector
//
//		} else {
//			val = recvchar();
//			/* ESC */
//			if (val == 0x1B) {
//				// AVRPROG connection
//				// Wait for signon
//				while (val != 'S')
//					val = recvchar();
//
//				send_boot();			// Report signon
//				OK = 0;
//
//			} else {
//				sendchar('?');
//			}
//	        }
//		// Power-Save code here
//	}
//#endif
//
//#if defined(START_SIMPLE)
//
//	if ((BLPIN & (1<<BLPNUM))) {
//		// jump to main app if pin is not grounded
//#if !defined(START_WAIT)
//		BLPORT &= ~(1<<BLPNUM);		// set to default
//		jump_to_app();			// Jump to application sector
//	}
//#endif
//#endif
//#if defined(START_WAIT)
//
//	uint16_t cnt = 0;
//
//	while (1) {
//		if (UART_STATUS & (1<<UART_RXREADY))
//			if (UART_DATA == START_WAIT_UARTCHAR)
//				break;
//
//		if (cnt++ >= WAIT_VALUE) {
//			BLPORT &= ~(1<<BLPNUM);		// set to default
//			jump_to_app();			// Jump to application sector
//		}
//
//		_delay_ms(10);
//	}
//	send_boot();
//#if defined(START_SIMPLE)
//	}
//#endif
//#endif
//#if defined(START_BOOTICE)
//#warning "BOOTICE mode - no startup-condition"
//
//#elif (!defined(START_WAIT) && !defined(START_POWERSAVE) && !defined(START_SIMPLE))
//#error "Select START_ condition for bootloader in main.c"
//#endif

// Patched end

//patch by Karl Fessel
//    {
	//write portc high to poweroff sensors on Robbyboard
	//poweroff odometrie only
	//DDRC |= (1<<7)|(1<<6);
	//PORTC |= (1<<7)|(1<<6);

	//poweroff all sensors
//	DDRC = 0xff;
//	PORTC = 0xff;
//    }
//patch end

//	for(;;) {
//		val = recvchar();
//		// Autoincrement?
//		if (val == 'a') {
//			sendchar('Y');			// Autoincrement is quicker
//
//		//write address
//		} else if (val == 'A') {
//			address = recvchar();		//read address 8 MSB
//			address = (address<<8) | recvchar();
//			sendchar('\r');
//
//		// Buffer load support
//		} else if (val == 'b') {
//			sendchar('Y');					// Report buffer load supported
//			sendchar((sizeof(gBuffer) >> 8) & 0xFF);	// Report buffer size in bytes
//			sendchar(sizeof(gBuffer) & 0xFF);
//
//		// Start buffer load
//		} else if (val == 'B') {
//			pagebuf_t size;
//			size = recvchar() << 8;				// Load high byte of buffersize
//			size |= recvchar();				// Load low byte of buffersize
//			val = recvchar();				// Load memory type ('E' or 'F')
//			recvBuffer(size);
//
//			if (device == DEVTYPE) {
//				if (val == 'F') {
//					address = writeFlashPage(address, size);
//
//				} else if (val == 'E') {
//					address = writeEEpromPage(address, size);
//				}
//				sendchar('\r');
//
//			} else {
//				sendchar(0);
//			}
//
//		// Block read
//		} else if (val == 'g') {
//			pagebuf_t size;
//			size = recvchar() << 8;				// Load high byte of buffersize
//			size |= recvchar();				// Load low byte of buffersize
//			val = recvchar();				// Get memtype
//
//			if (val == 'F') {
//				address = readFlashPage(address, size);
//
//			} else if (val == 'E') {
//				address = readEEpromPage(address, size);
//			}
//
//		// Chip erase
// 		} else if (val == 'e') {
//			if (device == DEVTYPE)
//				eraseFlash();
//
//			sendchar('\r');
//
//		// Exit upgrade
//		} else if (val == 'E') {
//			wdt_enable(WDTO_15MS); // Enable Watchdog Timer to give reset
//			sendchar('\r');
//
//#ifdef WRITELOCKBITS
//#warning "Extension 'WriteLockBits' enabled"
//		// TODO: does not work reliably
//		// write lockbits
//		} else if (val == 'l') {
//			if (device == DEVTYPE) {
//				// write_lock_bits(recvchar());
//				boot_lock_bits_set(recvchar());	// boot.h takes care of mask
//				boot_spm_busy_wait();
//			}
//			sendchar('\r');
//#endif
//		// Enter programming mode
//		} else if (val == 'P') {
//			sendchar('\r');
//
//		// Leave programming mode
//		} else if (val == 'L') {
//			sendchar('\r');
//
//		// return programmer type
//		} else if (val == 'p') {
//			sendchar('S');		// always serial programmer
//
//#ifdef ENABLEREADFUSELOCK
//#warning "Extension 'ReadFuseLock' enabled"
//		// read "low" fuse bits
//		} else if (val == 'F') {
//			sendchar(read_fuse_lock(GET_LOW_FUSE_BITS));
//
//		// read lock bits
//		} else if (val == 'r') {
//			sendchar(read_fuse_lock(GET_LOCK_BITS));
//
//		// read high fuse bits
//		} else if (val == 'N') {
//			sendchar(read_fuse_lock(GET_HIGH_FUSE_BITS));
//
//		// read extended fuse bits
//		} else if (val == 'Q') {
//			sendchar(read_fuse_lock(GET_EXTENDED_FUSE_BITS));
//#endif
//
//		// Return device type
//		} else if (val == 't') {
//			sendchar(DEVTYPE);
//			sendchar(0);
//
//		// clear and set LED ignored
//		} else if ((val == 'x') || (val == 'y')) {
//			recvchar();
//			sendchar('\r');
//
//		// set device
//		} else if (val == 'T') {
//			device = recvchar();
//			sendchar('\r');
//
//		// Return software identifier
//		} else if (val == 'S') {
//			send_boot();
//
//		// Return Software Version
//		} else if (val == 'V') {
//			sendchar(VERSION_HIGH);
//			sendchar(VERSION_LOW);
//
//		// Return Signature Bytes (it seems that
//		// AVRProg expects the "Atmel-byte" 0x1E last
//		// but shows it first in the dialog-window)
//		} else if (val == 's') {
//			sendchar(SIG_BYTE3);
//			sendchar(SIG_BYTE2);
//			sendchar(SIG_BYTE1);
//
//		/* ESC */
//		} else if(val != 0x1b) {
//			sendchar('?');
//		}
//	}
//	return 0;
//}

#endif // __BOOTLOADER_H_7D4D59F02C4527__

