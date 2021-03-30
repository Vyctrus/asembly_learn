all: subn

subn: subn.o
	ld -m elf_i386 -o subn subn.o

subn.o: subN.s
	as --32 --gstabs -o subn.o subN.s


clean:
	rm subn subn.o  