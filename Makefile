VERSION:=2.1.0
CC = gcc

LDFLAGS = -lm


IO_DEPS = $(addprefix ./2pcf_plugins/Corrfunc/, io/io.o io/ftread.o utils/utils.o)
DD_DEPS = $(addprefix ./2pcf_plugins/Corrfunc/theory/DD/, \
		countpairs_impl_double.o countpairs_impl_float.o countpairs.o) \
		$(addprefix ./2pcf_plugins/Corrfunc/utils/, \
		progressbar.o gridlink_impl_double.o gridlink_impl_float.o cpu_features.o)

main: main.c
	${CC} -DVERSION=\"${VERSION}\" -I./2pcf_plugins/Corrfunc/utils -c main.c -o main.o
	${CC} main.o ${IO_DEPS} ${DD_DEPS} -lm -fopenmp -o main
