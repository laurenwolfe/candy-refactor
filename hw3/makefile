SRC= game_window.c
HEADERS= game_window.h
LIBS=../hw2/libhw2.a
CFLAGS = -Wall -Wpedantic -std=c11 -Wno-pointer-to-int-cast
IFLAGS =  -I../hw2 -I../jansson/include
GTKCFLAGS = `pkg-config --cflags gtk+-3.0`
LDFLAGS =  -L../hw2 -L../jansson/lib -lhw2 -l:libjansson.a
GTKLDFLAGS = `pkg-config --libs gtk+-3.0`

all: hw3

hw3: $(SRC) $(HEADERS) $(LIBS)
	gcc $(CFLAGS) -g -O0 $(IFLAGS) $(GTKCFLAGS) -o hw3 $(SRC) $(LDFLAGS) $(GTKLDFLAGS)


clean:
	rm -f *.o *~ hw3