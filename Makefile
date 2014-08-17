CC = gcc
CCFLAGS = -Wall -W -w
CFLAGS = -Wall -W -I. -pedantic -std=gnu99
LIBFLAGS =
PROGRAMS = hextool
INDENTFLAGS = -i4 -br -ce -nprs -nbfda -npcs -ncs -sob -brf -nut -bap -bad -npsl -l140

.PHONY: all clean wipe tidy

all: $(PROGRAMS)

hextool: hextool.o
	$(CC) $(CCFLAGS) -o $@ $^ $(LIBFLAGS)

test: hextool
	./hextool 0xf0 < test.hex > test.bin
	hexdump -C -v test.bin
	sha256sum test.bin | grep -q 8f0602e5ec59763263cd04a85e5e925d0dcd0f8fef5f97828d81301a14c2185f && true || false

clean:
	rm -f *.o *~ test.bin $(PROGRAMS)

tidy:
	indent $(INDENTFLAGS) *.c
