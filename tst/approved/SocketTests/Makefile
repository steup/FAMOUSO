PREFIX=../../..

include $(PREFIX)/make/Makefile

LIBS = $(LIB) $(ADDITIONAL_LIBS)

all: TCPEchoClient TCPEchoServer UDPEchoClient UDPEchoServer \
BroadcastSender BroadcastReceiver MulticastSender MulticastReceiver

TCPEchoClient: TCPEchoClient.cc
	$(CXX) $(CXXFLAGS) -o TCPEchoClient TCPEchoClient.cc $(LIBS)

TCPEchoServer: TCPEchoServer.cc
	$(CXX) $(CXXFLAGS) -o TCPEchoServer TCPEchoServer.cc $(LIBS)

TCPOutputServer: TCPOutputServer.cc
	$(CXX) $(CXXFLAGS) -o TCPOutputServer TCPOutputServer.cc $(LIBS)

UDPEchoClient: UDPEchoClient.cc
	$(CXX) $(CXXFLAGS) -o UDPEchoClient UDPEchoClient.cc $(LIBS)

UDPEchoServer: UDPEchoServer.cc
	$(CXX) $(CXXFLAGS) -o UDPEchoServer UDPEchoServer.cc $(LIBS)

TCPEchoServer-Thread: TCPEchoServer-Thread.cc
	@echo $@
#	$(CXX) -D_GNU_SOURCE -o TCPEchoServer-Thread TCPEchoServer-Thread.cc $(LIBS) -lpthread

BroadcastSender: BroadcastSender.cc
	$(CXX) $(CXXFLAGS) -o BroadcastSender BroadcastSender.cc $(LIBS)

BroadcastReceiver: BroadcastReceiver.cc
	$(CXX) $(CXXFLAGS) -o BroadcastReceiver BroadcastReceiver.cc $(LIBS)

MulticastSender: MulticastSender.cc
	$(CXX) $(CXXFLAGS) -o MulticastSender MulticastSender.cc $(LIBS)

MulticastReceiver: MulticastReceiver.cc
	$(CXX) $(CXXFLAGS) -o MulticastReceiver MulticastReceiver.cc $(LIBS)

clean:
	$(RM) TCPEchoClient TCPEchoServer UDPEchoClient UDPEchoServer TCPEchoServer-Thread \
        BroadcastSender BroadcastReceiver MulticastSender MulticastReceiver
