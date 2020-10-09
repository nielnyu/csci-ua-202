BINS := mykernel

all: compile run

compile:
	@echo "Compiling..."
	@rm -f ${BINS}
	@gcc -Wall -o mykernel -Og sav339.c

run:
	@echo "Running..."
	@mykernel

clean: 
	@rm -f ${BINS}
