VERSION:=2.1.0
CC = gcc

LDFLAGS = -lm


CF_IO = $(addprefix ./2pcf_plugins/Corrfunc/, io/io.o io/ftread.o utils/utils.o)
CF_COUNTERS = $(addprefix ./2pcf_plugins/Corrfunc/, theory/DD/libcountpairs.a)
CF_INCL = $(addprefix -I./2pcf_plugins/Corrfunc/, include/ utils/)

main: main.c
	${CC} -DVERSION=\"${VERSION}\" ${CF_INCL} -c main.c -o main.o
	${CC} main.o ${CF_IO} ${CF_COUNTERS} -lm -fopenmp -o main
