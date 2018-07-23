CC = mpicc

DEBUG_FLAGS = -g
CFLAGS = -Wall -pedantic

CF_COUNTERS = $(addprefix ./2pcf_plugins/Corrfunc/, theory/DD/libcountpairs.a)
CF_INCL = $(addprefix -I./2pcf_plugins/Corrfunc/, include/ utils/)
CF_VERSION := 2.1.0

UTILS = $(addprefix ./utils/, divide_box.o)
IO = $(addprefix ./io/, io.o ascii.o bins.o)


main: *.c io/*.c utils/*.c
	${CC} ${DEBUG_FLAGS} ${CF_INCL} -c main.c -o main.o
	# Pair computers
	${CC} ${DEBUG_FLAGS} -DVERSION=\"${CF_VERSION}\" ${CF_INCL} -c xi_r.c -o xi_r.o
	# IO
	${CC} ${DEBUG_FLAGS} ${CFLAGS} -c io/io.c -o io/io.o
	${CC} ${DEBUG_FLAGS} ${CFLAGS} -c io/ascii.c -o io/ascii.o
	${CC} ${DEBUG_FLAGS} ${CFLAGS} -c io/bins.c -o io/bins.o
	# Utils
	${CC} ${DEBUG_FLAGS} ${CFLAGS} -c utils/divide_box.c -o utils/divide_box.o
	${CC} ${DEBUG_FLAGS} main.o xi_r.o ${UTILS} ${IO} ${CF_COUNTERS} -lm -fopenmp -o main


CF_IO = $(addprefix ./2pcf_plugins/Corrfunc/, io/io.o io/ftread.o utils/utils.o)
comparison: comparison.c
	${CC} -DVERSION=\"${CF_VERSION}\" ${CF_INCL} -c comparison.c -o comparison.o
	${CC} comparison.o ${CF_IO} ${CF_COUNTERS} -lm -fopenmp -o comparison


.PHONY: run_all_xi_r
run_all_xi_r: main
	# autocorr 0, periodic 0
	mpirun -n 4 main xi_r \
		--filename1 ./inputs/ascii_input.txt --filename2 ./inputs/ascii_input2.txt \
		--format a --binfile ./inputs/bins --boxsize 10 --nthreads 1 \
		--autocorr 0 --periodic 0
	diff ./perf/cmp_periodic_0_autocorr_0 ./perf/xi_r_periodic_0_autocorr_0
	# autocorr 0, periodic 1
	mpirun -n 4 main xi_r \
		--filename1 ./inputs/ascii_input.txt --filename2 ./inputs/ascii_input2.txt \
		--format a --binfile ./inputs/bins --boxsize 10 --nthreads 1 \
		--autocorr 0 --periodic 1
	diff ./perf/cmp_periodic_1_autocorr_0 ./perf/xi_r_periodic_1_autocorr_0
	# autocorr 1, periodic 0
	mpirun -n 4 main xi_r \
		--filename1 ./inputs/ascii_input.txt \
		--format a --binfile ./inputs/bins --boxsize 10 --nthreads 1 \
		--autocorr 1 --periodic 0
	diff ./perf/cmp_periodic_0_autocorr_1 ./perf/xi_r_periodic_0_autocorr_1
	# autocorr 1, periodic 1
	mpirun -n 4 main xi_r \
		--filename1 ./inputs/ascii_input.txt \
		--format a --binfile ./inputs/bins --boxsize 10 --nthreads 1 \
		--autocorr 1 --periodic 1
	diff ./perf/cmp_periodic_1_autocorr_1 ./perf/xi_r_periodic_1_autocorr_1
