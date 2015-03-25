%scanner Scanner.h
%scanner-token-function d_scanner.lex()
%token INT VOID FLOAT IF WHILE ELSE FOR ASSIGN_OP INC_OP NOT_OP OR_OP AND_OP EQ_OP NE_OP LE_OP GE_OP INT_CONSTANT
%token RETURN FLOAT_CONSTANT IDENTIFIER STRING_LITERAL
%baseclass-preinclude "intu.hpp"
%polymorphic ast: abstractAST*; listTag : std::list<abstractAST*>; stringTag : std::string;

%type <ast> iteration_statement selection_statement statement expression l_expression primary_expression postfix_expression unary_expression multiplicative_expression additive_expression compound_statement assignment_statement logical_and_expression equality_expression relational_expression constant_expression
%type <stringTag> unary_operator IDENTIFIER INT_CONSTANT FLOAT_CONSTANT STRING_LITERAL INC_OP
%type <listTag> expression_list statement_list

/*assumptions being used in function overloading:
	Arrays are not passed as parameters
*/

%%
translation_unit:
	function_definition
	| translation_unit function_definition
{
//    	printSymbolTable(globalTable);
}
	;

function_definition:
	type_specifier fun_declarator compound_statement
{
	if(*((*globalTable)[functionName + "#" + suffixString]->dataType) == DataType(DataType::Void)){
		if(((stmtAST*) ($3))->hasReturn){
			std::cerr << "Line no " << lineNo << ":\tNon-void return value specified in a void function " << functionName << " \n";
			std::exit(1);
		}
	}
	else{
		if(!((stmtAST*) ($3))->hasReturn){
			std::cerr << "Line no " << lineNo << ":\tNo return value specified in a non-void function " << functionName << " \n";
			std::exit(1);
		}
	}
//	std::cout << "line 40\n";
//	std::cout.flush();
	(*globalTable)[functionName + "#" + suffixString]->pointer = currentTable;
//	std::cout << "line 41\n";
//	std::cout.flush();
	currentTable = new std::map<std::string, SymbolTableEntry*>(); 
	currentOffset = -4;
	returnCount = 0;
	suffixString = "";
	$3->print();
}
	;

type_specifier:
	 VOID
{
	currentType = DataType(DataType::Void);
}
	| INT
{
	currentType = DataType(DataType::Int);
}
	| FLOAT
{
	currentType = DataType(DataType::Float);
}
	;

fun_declarator:
	 IDENTIFIER '(' parameter_list ')'
{
	functionName = $1;
	currentOffset = 0;
	if(globalTable->find(functionName + "#" + suffixString) != globalTable->end()){//this has to be changed.
		std::cerr << "Line no " << lineNo << ":\tFunction " << functionName << " already defined\n";
		std::exit(1);
	}
	SymbolTableEntry* temp = new SymbolTableEntry();
	temp->type = SymbolTableEntry::FUNC;
	temp->scope = SymbolTableEntry::LOCAL;
	temp->dataType = constructDT(currentType,indexList);
	temp->size = temp->dataType->size();
	std::cout << "to put\n";
	(*globalTable)[functionName + "#" + suffixString]=temp; 
	std::cout << "put\n";
//	suffixString = "";
}
	| IDENTIFIER '(' ')'
{
	functionName = $1;
	currentOffset = 0;
	if(globalTable->find(functionName + "#") != globalTable->end()){ //this has to be changed.
		std::cerr << "Line no " << lineNo << ":\tFunction " << functionName << " already defined\n";
		std::exit(1);
	}
	SymbolTableEntry* temp = new SymbolTableEntry();
	temp->type = SymbolTableEntry::FUNC;
	temp->scope = SymbolTableEntry::LOCAL;
	temp->dataType = constructDT(currentType,indexList);
	temp->size = temp->dataType->size();
	(*globalTable)[functionName + "#"]=temp; 
}
	;

parameter_list:
	 parameter_declaration
	| parameter_list ',' parameter_declaration
	;

parameter_declaration:
	 type_specifier declarator
{
//	std::cout << "param\n";
	if(currentType.basetype == DataType::Void){
		std::cerr << "Line no " << lineNo << ":\tParameter " << name << " declared as void\n";
		std::exit(1);
	}
	if(currentTable->find(name) != currentTable->end()){
		std::cerr << "Line no " << lineNo << ":\tVariable " << name << "already defined\n";
		std::exit(1);
	}
	SymbolTableEntry* temp = new SymbolTableEntry();
	temp->type = SymbolTableEntry::VAR;
	temp->scope = SymbolTableEntry::PARAM;
	temp->dataType = constructDT(currentType, indexList);
	temp->size = temp->dataType->size();
	temp->offset = currentOffset;
	currentOffset -= temp->size;
	indexList.clear();
	(*currentTable)[name]=temp; 
	if(currentType == DataType(DataType::Int)){
		suffixString += "i";
	}
	if(currentType == DataType(DataType::Float)){
		suffixString += "f";
	}
}	
	;

declarator:
	 IDENTIFIER
{
//	std::cout << "name is seen\n";
	name = $1;
//	std::cout << "put in list\n";
}
	| declarator '[' constant_expression ']'
{
  //std::cout << (($3)->astType.tag == DataType::Base) << std::endl;
  	if((($3)->astType.tag != DataType::Base) || ((($3)->astType).basetype != DataType::Int)){
		std::cerr << "Line no " << lineNo << ":\tIndex of array " << name << " is not an integer\n";
		std::exit(1);
	}
//	if($3->getVal() < 0){
//		std::cerr << "Line no " << lineNo << ":\tSize of array " << name << " is negative\n";
//		std::exit(1);
//	}
	indexList.push_back($3->getVal()); //it has to be ensured that this constant_expression is an int.
}
	;

constant_expression:
	 INT_CONSTANT
{
	$$ = new intAST($1);
	($$)->astType = DataType(DataType::Int);
}
        | FLOAT_CONSTANT
{
	$$ = new floatAST($1);
	($$)->astType = DataType(DataType::Float);
}
        ;

compound_statement:
	 '{' '}'
{
	$$ = new blockAST(std::list<abstractAST*>());
	((stmtAST*) ($$))->hasReturn = false;
}
	| '{' statement_list '}'
{
	$$ = new blockAST($2);
	((stmtAST*) ($$))->hasReturn = hasReturnInList($2);
}
    | '{' declaration_list statement_list '}'
{
	$$ = new blockAST($3);
	((stmtAST*) ($$))->hasReturn = hasReturnInList($3);
}
	;

statement_list:
	 statement
{
	$$ = std::list<abstractAST*>(1, $1);
}
	| statement_list statement
{
	($1).push_back($2);
	$$ = $1;
}
	;

statement:
	'{' statement_list '}'		 
{
	$$ = new blockAST($2);
	((stmtAST*) ($$))->hasReturn = hasReturnInList($2);
}
	| selection_statement
{
	$$ = $1;
	((stmtAST*) ($$))->hasReturn = ((stmtAST*) ($1))->hasReturn;
}
	| iteration_statement
{
	$$ = $1;
	((stmtAST*) ($$))->hasReturn = ((stmtAST*) ($1))->hasReturn;
}
	| assignment_statement
{
	$$ = $1;
	((stmtAST*) ($$))->hasReturn = ((stmtAST*) ($1))->hasReturn;
}
    | RETURN expression ';'
{
	if((*((*globalTable)[functionName + "#" + suffixString]->dataType) == DataType(DataType::Int)) && (($2)->astType == DataType(DataType::Float))){
		abstractAST *temp = new castAST("TO_INT", $2);
		$$ = new returnAST(temp);
	}
	else if((*((*globalTable)[functionName + "#" + suffixString]->dataType) == DataType(DataType::Float)) && (($2)->astType == DataType(DataType::Int))){
		abstractAST *temp = new castAST("TO_FLOAT", $2);
		$$ = new returnAST(temp);
	}
	else if(!(*((*globalTable)[functionName + "#" + suffixString]->dataType) == ($2)->astType)){
		std::cerr << "Line no " << lineNo << ":\tReturn type mismatch in function " << functionName << "\n";
		std::exit(1);
	}
	else{
		$$ = new returnAST($2);
	}
	returnCount++;
	((stmtAST*) ($$))->hasReturn = true;
}
	;

assignment_statement:
	 ';'
{
	$$ = new stmtAST();
	((stmtAST*) ($$))->hasReturn = false;
}
	|  l_expression ASSIGN_OP expression ';'
{		
  //	std::cout << "yeah reached here\n";
	//	std::cout << "actual beware now\n";
  //	($1)->astType.print(std::cout);
  //	($3)->astType.print(std::cout);
	if((($1)->astType == DataType(DataType::Int)) && ($3->astType == DataType(DataType::Int))){
	  //		std::cout << "yeah reached here first\n" << std::endl;		
		$$ = new assAST($1, $3);
		($$)->astType = DataType(DataType::Int);
		((stmtAST*) ($$))->hasReturn = false;
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Float))){
	  //	std::cout << "yeah reached here second\n" << std::endl;
		$$ = new assAST($1, $3);
		($$)->astType = DataType(DataType::Float);
		((stmtAST*) ($$))->hasReturn = false;
	}
	else if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Float))){
	  //	std::cout << "third\n" << std::endl;
		abstractAST *temp = new castAST("TO_INT", $3);
		$$ = new assAST($1, temp);
		($$)->astType = DataType(DataType::Int);
		((stmtAST*) ($$))->hasReturn = false;
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Int))){
	  //std::cout << "fourth\n" << std::endl;	
		abstractAST *temp = new castAST("TO_FLOAT", $3);
		$$ = new assAST($1, temp);
		($$)->astType = DataType(DataType::Float);
		((stmtAST*) ($$))->hasReturn = false;

	}
	else{
		std::cerr << "Line no " << lineNo << ":\tInvalid types for = operator\n";// << $1 << " undefined\n";
		std::exit(1);
	}
}
	;

expression:
	 logical_and_expression
{
	$$ = $1;
}
	| expression OR_OP logical_and_expression
{
	$$ = new bopAST("OR", $1, $3);
	($$)->astType = DataType(DataType::Int);
}
	;

logical_and_expression:
	 equality_expression
{
	$$ = $1;
}
	| logical_and_expression AND_OP equality_expression
{
	$$ = new bopAST("AND", $1, $3);
	($$)->astType = DataType(DataType::Int);
}
	;

equality_expression:
	 relational_expression
{
	$$ = $1;
}
	| equality_expression EQ_OP	 relational_expression
{
	if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Int))){
		$$ = new bopAST("EQ_OP", $1, $3);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Float))){
		$$ = new bopAST("EQ_OP_FLOAT", $1, $3);
	}
	else if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Float))){
		abstractAST *temp = new castAST("TO_FLOAT", $1);
		$$ = new bopAST("EQ_OP_FLOAT", temp, $3);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Int))){
		abstractAST *temp = new castAST("TO_FLOAT", $3);
		$$ = new bopAST("EQ_OP_FLOAT", $1, temp);
	}
	else{
		std::cerr << "Line no " << lineNo << ":\tInvalid types for equality operator\n";// << $1 << " undefined\n";
		std::exit(1);
	}
	($$)->astType = DataType(DataType::Int);
}
	| equality_expression NE_OP relational_expression
{
	if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Int))){
		$$ = new bopAST("NE_OP", $1, $3);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Float))){
		$$ = new bopAST("NE_OP_FLOAT", $1, $3);
	}
	else if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Float))){
		abstractAST *temp = new castAST("TO_FLOAT", $1);
		$$ = new bopAST("NE_OP_FLOAT", temp, $3);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Int))){
		abstractAST *temp = new castAST("TO_FLOAT", $3);
		$$ = new bopAST("NE_OP_FLOAT", $1, temp);
	}
	else{
		std::cerr << "Line no " << lineNo << ":\tInvalid types for equality operator\n";// << $1 << " undefined\n";
		std::exit(1);
	}
	($$)->astType = DataType(DataType::Int);
}
	;
relational_expression:
	 additive_expression
{
	$$ = $1;
}
	| relational_expression '<' additive_expression
{
	if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Int))){
		$$ = new bopAST("LT", $1, $3);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Float))){
		$$ = new bopAST("LT_FLOAT", $1, $3);
	}
	else if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Float))){
		abstractAST *temp = new castAST("TO_FLOAT", $1);
		$$ = new bopAST("LT_FLOAT", temp, $3);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Int))){
		abstractAST *temp = new castAST("TO_FLOAT", $3);
		$$ = new bopAST("LT_FLOAT", $1, temp);
	}
	else{
		std::cerr << "Line no " << lineNo << ":\tInvalid types for < operator\n";// << $1 << " undefined\n";
		std::exit(1);
	}
	($$)->astType = DataType(DataType::Int);
	
}
	| relational_expression '>' additive_expression
{
	if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Int))){
		$$ = new bopAST("GT", $1, $3);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Float))){
		$$ = new bopAST("GT_FLOAT", $1, $3);
	}
	else if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Float))){
		abstractAST *temp = new castAST("TO_FLOAT", $1);
		$$ = new bopAST("GT_FLOAT", temp, $3);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Int))){
		abstractAST *temp = new castAST("TO_FLOAT", $3);
		$$ = new bopAST("GT_FLOAT", $1, temp);
	}
	else{
		std::cerr << "Line no " << lineNo << ":\tInvalid types for > operator\n";// << $1 << " undefined\n";
		std::exit(1);
	}
	($$)->astType = DataType(DataType::Int);
}
	| relational_expression LE_OP additive_expression
{
	if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Int))){
		$$ = new bopAST("LE_OP", $1, $3);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Float))){
		$$ = new bopAST("LE_OP_FLOAT", $1, $3);
	}
	else if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Float))){
		abstractAST *temp = new castAST("TO_FLOAT", $1);
		$$ = new bopAST("LE_OP_FLOAT", temp, $3);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Int))){
		abstractAST *temp = new castAST("TO_FLOAT", $3);
		$$ = new bopAST("LE_OP_FLOAT", $1, temp);
	}
	else{
		std::cerr << "Line no " << lineNo << ":\tInvalid types for <= operator\n";// << $1 << " undefined\n";
		std::exit(1);
	}
	($$)->astType = DataType(DataType::Int);
}
	| relational_expression GE_OP additive_expression
{
	if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Int))){
		$$ = new bopAST("GE_OP", $1, $3);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Float))){
		$$ = new bopAST("GE_OP_FLOAT", $1, $3);
	}
	else if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Float))){
		abstractAST *temp = new castAST("TO_FLOAT", $1);
		$$ = new bopAST("GE_OP_FLOAT", temp, $3);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Int))){
		abstractAST *temp = new castAST("TO_FLOAT", $3);
		$$ = new bopAST("GE_OP_FLOAT", $1, temp);
	}
	else{
		std::cerr << "Line no " << lineNo << ":\tInvalid types for >= operator\n";// << $1 << " undefined\n";
		std::exit(1);
	}
	($$)->astType = DataType(DataType::Int);
}
	;

additive_expression:
	 multiplicative_expression
{
	$$ = $1;
}
	| additive_expression '+' multiplicative_expression
{
	if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Int))){
		$$ = new bopAST("PLUS", $1, $3);
		($$)->astType = DataType(DataType::Int);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Float))){
		$$ = new bopAST("PLUS_FLOAT", $1, $3);
		($$)->astType = DataType(DataType::Float);
	}
	else if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Float))){
		abstractAST *temp = new castAST("TO_FLOAT", $1);
		$$ = new bopAST("PLUS_FLOAT", temp, $3);
		($$)->astType = DataType(DataType::Float);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Int))){
		abstractAST *temp = new castAST("TO_FLOAT", $3);
		$$ = new bopAST("PLUS_FLOAT", $1, temp);
		($$)->astType = DataType(DataType::Float);
	}
	else{
		std::cerr << "Line no " << lineNo << ":\tInvalid types for + operator\n";// << $1 << " undefined\n";
		std::exit(1);
	}
}
	| additive_expression '-' multiplicative_expression
{
	if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Int))){
		$$ = new bopAST("MINUS", $1, $3);
		($$)->astType = DataType(DataType::Int);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Float))){
		$$ = new bopAST("MINUS_FLOAT", $1, $3);
		($$)->astType = DataType(DataType::Float);
	}
	else if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Float))){
		abstractAST *temp = new castAST("TO_FLOAT", $1);
		$$ = new bopAST("MINUS_FLOAT", temp, $3);
		($$)->astType = DataType(DataType::Float);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Int))){
		abstractAST *temp = new castAST("TO_FLOAT", $3);
		$$ = new bopAST("MINUS_FLOAT", $1, temp);
		($$)->astType = DataType(DataType::Float);
	}
	else{
		std::cerr << "Line no " << lineNo << ":\tInvalid types for - operator\n";// << $1 << " undefined\n";
		std::exit(1);
	}
}
	;

multiplicative_expression:
	 unary_expression
{
	$$ = $1;
}
	| multiplicative_expression '*' unary_expression
{
	if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Int))){
		$$ = new bopAST("MULT", $1, $3);
		($$)->astType = DataType(DataType::Int);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Float))){
		$$ = new bopAST("MULT_FLOAT", $1, $3);
		($$)->astType = DataType(DataType::Float);
	}
	else if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Float))){
		abstractAST *temp = new castAST("TO_FLOAT", $1);
		$$ = new bopAST("MULT_FLOAT", temp, $3);
		($$)->astType = DataType(DataType::Float);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Int))){
		abstractAST *temp = new castAST("TO_FLOAT", $3);
		$$ = new bopAST("MULT_FLOAT", $1, temp);
		($$)->astType = DataType(DataType::Float);
	}
	else{
		std::cerr << "Line no " << lineNo << ":\tInvalid types for * operator\n";// << $1 << " undefined\n";
		std::exit(1);
	}
}
	| multiplicative_expression '/' unary_expression
{
	if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Int))){
		$$ = new bopAST("DIVIDE", $1, $3);
		($$)->astType = DataType(DataType::Int);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Float))){
		$$ = new bopAST("DIVIDE_FLOAT", $1, $3);
		($$)->astType = DataType(DataType::Float);
	}
	else if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Float))){
		abstractAST *temp = new castAST("TO_FLOAT", $1);
		$$ = new bopAST("DIVIDE_FLOAT", temp, $3);
		($$)->astType = DataType(DataType::Float);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Int))){
		abstractAST *temp = new castAST("TO_FLOAT", $3);
		$$ = new bopAST("DIVIDE_FLOAT", $1, temp);
		($$)->astType = DataType(DataType::Float);
	}
	else{
		std::cerr << "Line no " << lineNo << ":\tInvalid types for / operator\n";// << $1 << " undefined\n";
		std::exit(1);
	}
}
	;
unary_expression:
	 postfix_expression
{
	$$ = $1;
}
	| unary_operator postfix_expression
{
	$$ = new uopAST($1, $2);
	($$)->astType = ($2)->astType;
}
	;

postfix_expression:
	 primary_expression
{
	$$ = $1;
}
	| IDENTIFIER '(' ')'
{
	if(globalTable->find($1) == globalTable->end()){ //this has to be changed.
		std::cerr << "Line no " << lineNo << ":\tFunction " << $1 << " undefined\n";
		std::exit(1);
	}
	std::map<std::string, SymbolTableEntry*> *currentMap = globalTable->find($1)->second->pointer;
	std::list<SymbolTableEntry*> paramList;
	for(std::map<std::string, SymbolTableEntry*>::iterator name = currentMap->begin(); name != currentMap->end(); name++){
		if(name->second->scope == SymbolTableEntry::PARAM){
			paramList.push_back(name->second);
		}
	}
	if(paramList.size() != 0){
		std::cerr << "Line no " << lineNo << ":\tMismatch in number of arguments to function '" << $1 << "'\n";
		std::exit(1);
	}
	$$ = new funcAST($1, std::list<abstractAST*>());
	($$)->astType = *(globalTable->find($1)->second->dataType);
}
	| IDENTIFIER '(' expression_list ')'
{
	std::string suffix = ""; //this is expression string
	for(std::list<abstractAST*>::iterator iter = ($3).begin(); iter != ($3).end(); iter++){
		if((*iter)->astType == DataType(DataType::Int)){
			suffix += "i";
		}
		if((*iter)->astType == DataType(DataType::Float)){
			suffix += "f";
		}
	}
	std::string returned = findBestFunction($1, suffix); //this is function name string
	if(returned == "2"){
		std::cerr << "Line no " << lineNo << ":\tFunction " << $1 << " undefined\n";
		std::exit(1);
	}
	if(returned == "1"){
		std::cerr << "Line no " << lineNo << ":\tAmbiguous function call " << $1 << "\n";
		std::exit(1);
	}
//	std::map<std::string, SymbolTableEntry*> *currentMap = globalTable->find($1)->second->pointer;
//	std::list<SymbolTableEntry*> paramList;


//	for(std::map<std::string, SymbolTableEntry*>::iterator name = currentMap->begin(); name != currentMap->end(); name++){
//		if(name->second->scope == SymbolTableEntry::PARAM){
//			paramList.push_back(name->second);
//		}
//	}
//	if(paramList.size() != ($3).size()){
//		std::cerr << "Line no " << lineNo << ":\tMismatch in number of arguments to function '" << $1 << "'\n";
//		std::exit(1);
//	}
//	paramList.sort(offsetCompare);
	std::list<abstractAST*>::iterator absIterator = ($3).begin();
//	for(std::list<SymbolTableEntry*>::iterator pIterator = paramList.begin(); pIterator != paramList.end(); pIterator++){
	for(int i = 0; i < returned.size(); i++){
		if((returned[i] == "i") && (suffix[i] == "f")){
			abstractAST *temp = new castAST("TO_INT", *absIterator);
			*absIterator = temp;
		}
		if((returned[i] == "f") && (suffix[i] == "i")){
			abstractAST *temp = new castAST("TO_FLOAT", *absIterator);
			*absIterator = temp;
		}
//		else if(!(*((*pIterator)->dataType) == (*absIterator)->astType)){
		else if(returned[i] != suffix[i]){
			std::cerr << "Line no " << lineNo << ":\tCannot convert type ";// << $1 << "\n";
			(*absIterator)->astType.print(std::cerr);
			std::cerr << " to type ";
			(*pIterator)->dataType->print(std::cerr);
			std::cerr << " in function call " << $1 << "\n";
			std::exit(1);
		}
		absIterator++;
	}
	$$ = new funcAST($1 + "#" + returned, $3);
	($$)->astType = *(globalTable->find($1 + "#" + returned)->second->dataType);
}
	| l_expression INC_OP
{
	if(($1)->astType == DataType(DataType::Int)){
		$$ = new uopAST($2, $1);
	}
	else if(($1)->astType == DataType(DataType::Float)){
		$$ = new uopAST($2 + "_FLOAT", $1);
	}	
	($$)->astType = ($1)->astType;
}
	;

primary_expression:
	 l_expression
{
	$$ = $1;
}
	| l_expression ASSIGN_OP expression 
{
	if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Int))){
		$$ = new assAST($1, $3);
		($$)->astType = DataType(DataType::Int);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Float))){
		$$ = new assAST($1, $3);
		($$)->astType = DataType(DataType::Float);
	}
	else if((($1)->astType == DataType(DataType::Int)) && (($3->astType) == DataType(DataType::Float))){
		abstractAST *temp = new castAST("TO_INT", $3);
		$$ = new assAST($1, temp);
		($$)->astType = DataType(DataType::Int);
	}
	else if((($1)->astType == DataType(DataType::Float)) && (($3->astType) == DataType(DataType::Int))){
		abstractAST *temp = new castAST("TO_FLOAT", $3);
		$$ = new assAST($1, temp);
		($$)->astType = DataType(DataType::Float);
	}
	else{
		std::cerr << "Line no " << lineNo << ":\tInvalid types for / operator\n";// << $1 << " undefined\n";
		std::exit(1);
	}
}	
	| INT_CONSTANT
{
	$$ = new intAST($1);
	$$->astType = DataType(DataType::Int);
}
	| FLOAT_CONSTANT
{
	$$ = new floatAST($1);
	$$->astType = DataType(DataType::Float);
}
    | STRING_LITERAL
{
	$$ = new stringAST($1);
}
	| '(' expression ')'
{
	$$ = $2;
}
	;

l_expression:
	 IDENTIFIER
{
	if(currentTable->find($1) == currentTable->end()){
		std::cerr << "Line no " << lineNo << ":\t" << $1 << " doesn't name a type\n";
		std::exit(1);
	}
	$$ = new identifierAST($1);
	($$)->astType = *(((*currentTable->find($1)).second)->dataType);
//	std::cout << "Printing type.........beware:\n";
//	($$)->astType.print(std::cout);
}
        | l_expression '[' expression ']'
{
	if((($3)->astType.tag != DataType::Base) || (($3)->astType.basetype != DataType::Int)){
		std::cerr << "Line no " << lineNo << ":\tIndex of array is not an integer\n";
		std::exit(1);
	}
	if(($1)->astType.tag != DataType::Array){
		std::cerr << "Line no " << lineNo << ":\toperator [] not defined\n";// << 
		std::exit(1);
	}
	$$ = new indexAST($1, $3);
	($$)->astType = *(($1)->astType.arrayType);
	
}	
        ;
expression_list:
	 expression
{
	$$ = std::list<abstractAST*>(1, $1);
}
        | expression_list ',' expression
{
	($1).push_back($3);
	$$ = $1;
}
	;

unary_operator:
	 '-'
{
	$$ = "UMINUS";
}
	| NOT_OP 
{
	$$ = "NOT";
}
	;

selection_statement:
	 IF '(' expression ')' statement ELSE statement
{
	abstractAST *temp = new castAST("TO_BOOL", $3);
	$$ = new ifAST(temp, $5, $7);
	((stmtAST*) ($$))->hasReturn = ((stmtAST*) ($5))->hasReturn && ((stmtAST*) ($7))->hasReturn;
}
	;

iteration_statement:
	 WHILE '(' expression ')' statement
{
	abstractAST *temp = new castAST("TO_BOOL", $3);
	$$ = new whileAST(temp, $5);
	((stmtAST*) ($$))->hasReturn = ((stmtAST*) ($5))->hasReturn;
}
	| FOR '(' expression ';' expression ';' expression ')' statement
{
	abstractAST *temp = new castAST("TO_BOOL", $5);
	$$ = new forAST($3, temp, $7, $9);
	((stmtAST*) ($$))->hasReturn = ((stmtAST*) ($9))->hasReturn;
}
	;

declaration_list:
	 declaration
	| declaration_list declaration
	;

declaration:
	 type_specifier declarator_list ';'
	;

declarator_list:
	 declarator
{
//	std::cout << "dec before\n";
//	std::cout.flush();
	if(currentType.basetype == DataType::Void){
		std::cerr << "Line no " << lineNo << ":\tVariable " << name << " declared as void\n";
		std::exit(1);
	}
	if(currentTable->find(name) != currentTable->end()){
		std::cerr << "Line no " << lineNo << ":\tVariable " << name << "already defined\n";
		std::exit(1);
	}
	SymbolTableEntry* temp = new SymbolTableEntry();
	temp->type = SymbolTableEntry::VAR;
	temp->scope = SymbolTableEntry::LOCAL;
	temp->dataType = constructDT(currentType, indexList);
	temp->size = temp->dataType->size();
//	std::cout << "dec after\n";
//	std::cout.flush();
	temp->offset = currentOffset;
	currentOffset+=temp->size;
	indexList.clear();
	(*currentTable)[name]=temp; 
}
| declarator_list ',' declarator
{
	if(currentType.basetype == DataType::Void){
		std::cerr << "Line no " << lineNo << ":\tVariable " << name << " declared as void\n";
		std::exit(1);
	}
	if(currentTable->find(name) != currentTable->end()){
		std::cerr << "Line no " << lineNo << ":\tVariable " << name << "already defined\n";
		std::exit(1);
	}
	SymbolTableEntry* temp = new SymbolTableEntry();
	temp->type = SymbolTableEntry::VAR;
	temp->scope = SymbolTableEntry::LOCAL;
	temp->dataType = constructDT(currentType, indexList);
	temp->size = temp->dataType->size();
	temp->offset = currentOffset;
	currentOffset+=temp->size;
	indexList.clear();
	(*currentTable)[name]=temp; 
}
	;
