BINS := scheduling

all: compile run

compile:
	@echo "Compiling..."
	@rm -f ${BINS}
	@gcc -Wall -o scheduling -Og -std=c99 scheduling.c

run:
	@echo "Running..."
	./scheduling input.txt 0
	./scheduling 2processes.txt 0
	./scheduling 3processes.txt 0
	./scheduling input.txt 1
	./scheduling 2processes.txt 1
	./scheduling 3processes.txt 1
	./scheduling input.txt 2
	./scheduling 2processes.txt 2
	./scheduling 3processes.txt 2

fcfs:
	@echo "Running First-Come-First-Served..."
	./scheduling input.txt 0
	./scheduling 2processes.txt 0
	./scheduling 3processes.txt 0

rr:
	@echo "Running Round-Robin..."
	./scheduling input.txt 1
	./scheduling 2processes.txt 1
	./scheduling 3processes.txt 1

sjf:
	@echo "Running Shortest-Job-First..."
	./scheduling input.txt 2
	./scheduling 2processes.txt 2
	./scheduling 3processes.txt 2

clean:
	@rm -f ${BINS}
