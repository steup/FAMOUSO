INC=../include
FILE=famouso.i

all:
	swig -java -I../include/ $(FILE)
	gcc -fpic -c ../client/famouso.c famouso_wrap.c -I/usr/lib/jvm/java-1.5.0-gcj/include -I/usr/lib/jvm/java-1.5.0-gcj/include/linux -I../../include -I$(INC)
	gcc -shared famouso.o famouso_wrap.o -o libfamouso.so
	javac -cp . famouso.java
	javac -cp . Publisher.java
	javac -cp . Subscriber.java

clean:
	rm -f *.o *~ libfamouso* famouso* *.class
