CC := i686-elf-gcc
CFLAGS := -T linker.ld -o ocmx86 -ffreestanding -O2 -nostdlib entry.o kernOC.o -lgcc

all:
	$(CC) $(CFLAGS)

clean:
	rm -rf *.o ocmx86
