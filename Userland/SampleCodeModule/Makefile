include ../Makefile.inc

MODULE=0000-sampleCodeModule.bin
SOURCES=$(wildcard [^_]*.c)
SOURCES_ASM=$(wildcard asm/*.asm)
# Aca llama a la regla que compila los .asm y "guarda todos los .o" en ASM_OBJECTS (es como una lista de los nombres de archivos separados con espacios)
ASM_OBJECTS=$(SOURCES_ASM:.asm=.o)
ALL_OBJECTS=$(ASM_OBJECTS) $(SOURCES)

all: $(MODULE)

$(MODULE): $(ALL_OBJECTS)
# el $^ representa a los prerrequisitos (en este caso ALL_OBJECTS), los concatena en la instruccion
	$(GCC) $(GCCFLAGS) -T sampleCodeModule.ld _loader.c $^ -o ../$(MODULE)

# Compila todos los .asm (de a uno) en un .o (para c/u) con su mismo nombre (el % seria el filename)
%.o: %.asm
# el $< es el filename del primer prerrequisito (nombre del .asm) y el $@ usa el mismo nombre que tenia el $<
	$(ASM) $(ASMFLAGS) $< -o $@

clean:
	rm -rf *.o

.PHONY: all clean print
