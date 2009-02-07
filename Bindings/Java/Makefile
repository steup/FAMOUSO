include ../../make/Makefile
INC=-I../include
FILE=famouso.i
GEN=generated

all:
	mkdir -p $(GEN)
	swig -java -c++ -outdir $(GEN) $(INC) $(FILE)
	$(CXX) -fpic -c ../client/famouso.cc -o $(GEN)/famouso.o -I/usr/lib/jvm/java-1.5.0-gcj/include -I/usr/lib/jvm/java-1.5.0-gcj/include/linux $(CXXFLAGS) $(INC)
	$(CXX) -fpic -c famouso_wrap.cxx -o $(GEN)/famouso_wrap.o -I/usr/lib/jvm/java-1.5.0-gcj/include -I/usr/lib/jvm/java-1.5.0-gcj/include/linux $(CXXFLAGS) $(INC)
	$(CXX) -shared $(GEN)/famouso.o $(GEN)/famouso_wrap.o -o libfamouso.so $(LIB) $(ADDITIONAL_LIBS)
	javac -cp ./$(GEN) $(GEN)/famouso.java
	javac -cp ./$(GEN) Publisher.java
	javac -cp ./$(GEN)  Subscriber.java

clean:
	rm -fr *.o *~ $(GEN) famouso_wrap.cxx libfamouso* *.class
