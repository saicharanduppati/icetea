all:    one two main.cc Scanner.h Scannerbase.h Scanner.ih Parser.h Parserbase.h Parser.ih
	./sedscript
	g++   --std=c++11 lex.cc parse.cc main.cc intu.cpp;
	./a.out < test-assembly > junk;
#	sed -e '1s/^/digraph G{ordering=out;\n/' -e '$$s/$$/}/' -i junk
#	sed 's/^\([^-]*\)->\([^;]*\);/\2->\1;/' -i junk
#	dot -Tps junk -o graph1.ps
#	cat junk;
#	egrep 'nextToken|reduce|shift' junk
        

one: lex.l Scanner.ih 
#	./cond_remove_scannerih; 
	rm -f Scanner.ih;
	flexc++ lex.l; 
	sed -i '/include/a #include "Parserbase.h"' Scanner.ih; 

two: parse.y Parser.ih Parser.h Parserbase.h
	bisonc++  --construction parse.y; 
#	sed '5s/^.*$$/int var = 0;/' -i parse.cc
#	bisonc++   --construction -V parse.y; 
#	sed -i '/ifndef/a #include "tree_util.hh"' Parserbase.h;
#	sed -i '/ifndef/a #include "tree.hh"' Parserbase.h;
#	./sedscript
     

Parser.ih: parse.y
Parser.h:  parse.y
Parserbase.h: parse.y
Scanner.ih: lex.l
Scanner.h: lex.l
Scannerbase.h: lex.l

clean:
	rm -f junk a.out lex.cc parse.cc Parserbase.h Parser* Scanner*
