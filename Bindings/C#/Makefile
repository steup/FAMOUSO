INC=../include
FILE=famouso.i

all:
	#swig -csharp -I../include/ -module Blafasel -namespace FAMOUSO $(FILE)
	swig -csharp -I../include/ $(FILE)
	gcc -fpic -c ../client/famouso.c famouso_wrap.c -I../../include -I$(INC)
	gcc -shared famouso.o famouso_wrap.o -o libfamouso.so
	mcs -out:Subscriber.exe Subscriber.cs famouso_event_t.cs famouso.cs famousoPINVOKE.cs
	mcs -out:Publisher.exe Publisher.cs famouso_event_t.cs famouso.cs famousoPINVOKE.cs
#	javac -cp . famouso.java
#	javac -cp . Publisher.java
#	javac -cp . Subscriber.java

clean:
	rm -f *.o *~ libfamouso* famouso* *.class *.exe

