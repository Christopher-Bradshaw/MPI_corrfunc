CC = mpicc

DEBUG_FLAGS = -g
CFLAGS = -Wall -pedantic

CF_COUNTERS = $(addprefix ./2pcf_plugins/Corrfunc/, theory/DD/libcountpairs.a)
CF_INCL = $(addprefix -I./2pcf_plugins/Corrfunc/, include/ utils/)
CF_VERSION := 2.1.0

UTILS = $(addprefix ./utils/, divide_box.o)
IO = $(addprefix ./io/, io.o ascii.o)


main: main.c io/*.c utils/*.c
	-echo $^
	${CC} ${DEBUG_FLAGS} -DVERSION=\"${CF_VERSION}\" ${CF_INCL} -c main.c -o main.o
	${CC} ${DEBUG_FLAGS} ${CFLAGS} -c io/io.c -o io/io.o
	${CC} ${DEBUG_FLAGS} ${CFLAGS} -c io/ascii.c -o io/ascii.o
	${CC} ${DEBUG_FLAGS} ${CFLAGS} -c utils/divide_box.c -o utils/divide_box.o
	${CC} ${DEBUG_FLAGS} main.o ${UTILS} ${IO} ${CF_COUNTERS} -lm -fopenmp -o main


CF_IO = $(addprefix ./2pcf_plugins/Corrfunc/, io/io.o io/ftread.o utils/utils.o)
comparison: comparison.c
	${CC} -DVERSION=\"${CF_VERSION}\" ${CF_INCL} -c comparison.c -o comparison.o
	${CC} comparison.o ${CF_IO} ${CF_COUNTERS} -lm -fopenmp -o comparison
