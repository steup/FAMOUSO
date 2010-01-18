################################################################################
##
## Copyright (c) 2008, 2009 Michael Schulze <mschulze@ivs.cs.uni-magdeburg.de>
## All rights reserved.
##
##    Redistribution and use in source and binary forms, with or without
##    modification, are permitted provided that the following conditions
##    are met:
##
##    * Redistributions of source code must retain the above copyright
##      notice, this list of conditions and the following disclaimer.
##
##    * Redistributions in binary form must reproduce the above copyright
##      notice, this list of conditions and the following disclaimer in
##      the documentation and/or other materials provided with the
##      distribution.
##
##    * Neither the name of the copyright holders nor the names of
##      contributors may be used to endorse or promote products derived
##      from this software without specific prior written permission.
##
##
##    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
##    IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
##    TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
##    PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
##    OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
##    SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
##    LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
##    DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
##    THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
##    (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
##    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
##
##
## $Id$
##
################################################################################

# PEAK Driver specific things
PCANINC                  = $(EXTERNALSDIR)/PEAK/Disk/PCAN-Light/Api
PCANDEVICE               = USB

ifeq ($(PCANDEVICE), USB)
ADDITIONAL_CFLAGS       += -DPEAKUSB
PCANLIB                  = $(EXTERNALSDIR)/PEAK/Disk/PCAN-Light/Lib/Visual\ C++/Pcan_usb.lib
else

ifeq ($(PCANDEVICE), PCI)
ADDITIONAL_CFLAGS       += -DPEAKPCI
PCANLIB                  = $(EXTERNALSDIR)/PEAK/Disk/PCAN-Light/Lib/Visual\ C++/Pcan_pci.lib
else
$(error Under Windows only PCANDEVICE=USB or =PCI supported)
endif

endif

PCANLIBRARY="$(shell test -f $(PCANLIB) && echo $(PCANLIB))"

ADDITIONAL_LIBS         += -lwsock32 -lws2_32 $(PCANLIBRARY)
ADDITIONAL_CFLAGS       += -DWIN32 -D_WIN32_WINNT=0x0501 -D__USE_W32_SOCKETS -mno-cygwin -DWIN32_LEAN_AND_MEAN $(PEAKDEVICE) -I$(PCANINC)

SUFFIX=.exe

