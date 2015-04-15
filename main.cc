#include <iostream>
#include "Scanner.h"
#include "Parser.h"
using namespace std;
int main (int argc, char** arg)
{
//  std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< AST DUMP >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << "\n";
	codeFile.open("./machine/code.asm");	
    Parser parser;
    parser.parse();
	codeFile.close();
//  std::cout << "\n";
//  std::cout << "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< SYMBOL TABLE >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>" << "\n";
//  printSymbolTable(globalTable);

}
