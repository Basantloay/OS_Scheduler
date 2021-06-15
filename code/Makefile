build:
	gcc headers.h
	gcc process_generator.c -o process_generator.out
	gcc clk.c -o clk.out
	gcc scheduler.c -lm -o scheduler.out
	gcc process.c -o process.out
	gcc test_generator.c -o test_generator.out
	gcc testq.c -o testq.out

	
clean:
	rm -f *.out  test.log scheduler.log scheduler.perf

all: clean build run

run:
	./process_generator.out processes.txt -sch 5 -q 3
