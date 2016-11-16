SRC=array2d.c
HEADER=array2d.h

hw2:  ${SRC} ${HEADER}
	gcc -std=c11 -Wall -g -c -Wno-pointer-to-int-cast -I ../jansson/include ${SRC} -L ../jansson/lib -l:libjansson.a -o hw2.o

run: hw2
	./hw2 test.json

clean:
	rm -f *.o *~ hw2 json.out