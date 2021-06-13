build:
	gcc headers.h
	gcc process_generator.c -o process_generator.out
	gcc clk.c -o clk.out
	gcc scheduler.c -o scheduler.out
	gcc process.c -o process.out
	gcc test_generator.c -o test_generator.out
	gcc testq.c -o testq.out

	
clean:
	rm -f *.out  scheduler.log test.log

all: clean build run

run:
	./process_generator.out
