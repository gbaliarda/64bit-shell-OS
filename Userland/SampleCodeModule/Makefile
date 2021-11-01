include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c)
TESTS=$(wildcard tests/*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
ASM_OBJECTS=$(SOURCES_ASM:.asm=.o)
ALL_OBJECTS=$(ASM_OBJECTS) $(SOURCES) $(TESTS)

all: $(MODULE)

$(MODULE): $(ALL_OBJECTS)
	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld _loader.c $^ -o ../$(MODULE)

%.o: %.asm
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o
	cd asm; rm -rf *.o;

.PHONY: all clean print
