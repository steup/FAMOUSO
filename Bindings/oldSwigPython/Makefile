INC=../include
FILE=famouso.i


all:
	swig -python -I../include/ $(FILE)
	gcc -c ../client/famouso.c famouso_wrap.c -I/usr/include/python2.5 -I../../include -I$(INC)
	gcc -shared famouso.o famouso_wrap.o -o _famouso.so

clean:
	rm -f *.o *~ _famouso.so famouso_wrap.c famouso.py famouso.pyc
