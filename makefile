all: parser test
	./parser < test

parser:  lex.cc parse.cc Scanner.h Scannerbase.h Scanner.ih Parser.h Parserbase.h Parser.ih
	g++ -g --std=c++0x -o parser lex.cc parse.cc main.cc intu.cpp;

lex.cc: lex.l Scanner.ih
	rm -f Scanner.ih;
	flexc++ lex.l; 
	sed -i '/include/a #include "Parserbase.h"' Scanner.ih; 

parse.cc: parse.y Parser.ih Parser.h Parserbase.h
	bisonc++  --construction parse.y; 
	./sedscript 

Parser.ih: parse.y
Parser.h:  parse.y
Parserbase.h: parse.y
Scanner.ih: lex.l
Scanner.h: lex.l
Scannerbase.h: lex.l

clean:
	rm -f junk parser lex.cc parse.cc Parserbase.h Parser* Scanner*
