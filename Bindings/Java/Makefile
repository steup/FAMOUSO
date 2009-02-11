include ../../make/Makefile
INC=-I../include
FILE=famouso.i
GEN=generated
JAVAINC=/usr/lib/jvm/java-1.5.0-gcj/include

all:
	mkdir -p $(GEN)/famouso
	swig -java -c++ -package famouso -outdir $(GEN)/famouso $(INC) $(FILE)
	$(CXX) -fpic -c ../client/famouso.cc -o $(GEN)/famouso/famouso.o -I$(JAVAINC) -I$(JAVAINC)/linux $(CXXFLAGS) $(INC)
	$(CXX) -fpic -c famouso_wrap.cxx -o $(GEN)/famouso/famouso_wrap.o -I$(JAVAINC) -I$(JAVAINC)/linux $(CXXFLAGS) $(INC)
	$(CXX) -shared $(GEN)/famouso/famouso.o $(GEN)/famouso/famouso_wrap.o -o libfamouso.so $(LIB) $(ADDITIONAL_LIBS)

	javac -cp ./$(GEN) $(GEN)/famouso/famouso.java
	javac -cp ./$(GEN) Publisher.java
	javac -cp ./$(GEN)  Subscriber.java

clean:
	rm -fr *.o *~ $(GEN) famouso_wrap.cxx libfamouso* *.class
