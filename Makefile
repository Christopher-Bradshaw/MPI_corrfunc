VERSION:=2.1.0
CC = gcc # mpicc

DEBUG_FLAGS = -g
CFLAGS = -Wall -pedantic

CF_IO = $(addprefix ./2pcf_plugins/Corrfunc/, io/io.o io/ftread.o utils/utils.o)
CF_COUNTERS = $(addprefix ./2pcf_plugins/Corrfunc/, theory/DD/libcountpairs.a)
CF_INCL = $(addprefix -I./2pcf_plugins/Corrfunc/, include/ utils/)


main: main.c
	${CC} ${DEBUG_FLAGS} -DVERSION=\"${VERSION}\" ${CF_INCL} -c main.c -o main.o
	${CC} ${DEBUG_FLAGS} ${CFLAGS} -c io/io.c -o io/io.o
	${CC} ${DEBUG_FLAGS} main.o io/io.o ${CF_COUNTERS} -lm -fopenmp -o main


comparison: comparison.c
	${CC} -DVERSION=\"${VERSION}\" ${CF_INCL} -c comparison.c -o comparison.o
	${CC} comparison.o ${CF_IO} ${CF_COUNTERS} -lm -fopenmp -o comparison
