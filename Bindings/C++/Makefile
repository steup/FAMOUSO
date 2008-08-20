include ../../make/Makefile
ADDITIONAL_CFLAGS	+= -I./include
all: sendEvent distance

distance: distance.o
	@$(RULEECHO) ; \
	$(CXX) -g -Wall -o distance distance.o $(LIB) $(ADDITIONAL_LIBS)

sendEvent: sendEvent.o
	@$(RULEECHO) ; \
	$(CXX) -g -Wall -o sendEvent sendEvent.o $(LIB) $(ADDITIONAL_LIBS)

clean:
	rm -f *.o *~ distance sendEvent
