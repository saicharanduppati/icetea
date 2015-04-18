all: a.out

a.out: machine.cc code.asm
	g++ -g3 -O0 -m32 machine.cc


.PHONY: clean
	
clean:
	rm a.out
