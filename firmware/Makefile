OUTPUT_NAME = fw
OBJECTS = main.o gpio.o

MCU=atxmega32e5

CROSS ?= avr-
CFLAGS := -std=c11 -Wall -Wextra -Werror -O2 -flto -g -mmcu=${MCU} ${CFLAGS}

CC = ${CROSS}gcc
OBJDUMP = ${CROSS}objdump
SIZE = ${CROSS}size

.PHONY: all clean

all: ${OUTPUT_NAME}.elf ${OUTPUT_NAME}.disas

clean:
	${RM} -f ${OUTPUT_NAME}.elf ${OUTPUT_NAME}.disas
	${RM} -f ${OBJECTS}

${OUTPUT_NAME}.elf: ${OBJECTS}
	${CC} ${CFLAGS} -o $@ $^

%.disas: %.elf
	${OBJDUMP} -S $< > $@