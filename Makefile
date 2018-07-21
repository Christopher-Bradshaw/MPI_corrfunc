VERSION:=2.1.0
CC = mpicc

DEBUG_FLAGS = -g
CFLAGS = -Wall -pedantic

CF_IO = $(addprefix ./2pcf_plugins/Corrfunc/, io/io.o io/ftread.o utils/utils.o)
CF_COUNTERS = $(addprefix ./2pcf_plugins/Corrfunc/, theory/DD/libcountpairs.a)
CF_INCL = $(addprefix -I./2pcf_plugins/Corrfunc/, include/ utils/)

UTILS = $(addprefix ./utils/, io.o divide_box.o)


main: main.c utils
	${CC} ${DEBUG_FLAGS} -DVERSION=\"${VERSION}\" ${CF_INCL} -c main.c -o main.o
	${CC} ${DEBUG_FLAGS} ${CFLAGS} -c utils/io.c -o utils/io.o
	${CC} ${DEBUG_FLAGS} ${CFLAGS} -c utils/divide_box.c -o utils/divide_box.o
	${CC} ${DEBUG_FLAGS} main.o ${UTILS} ${CF_COUNTERS} -lm -fopenmp -o main

comparison: comparison.c
	${CC} -DVERSION=\"${VERSION}\" ${CF_INCL} -c comparison.c -o comparison.o
	${CC} comparison.o ${CF_IO} ${CF_COUNTERS} -lm -fopenmp -o comparison
