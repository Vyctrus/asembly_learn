all: calc

calc: calc.o
	ld -o calc calc.o

calc.o: calc.s
	as --gstabs -o calc.o calc.s
	
clean:
	rm calc calc.o