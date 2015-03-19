%scanner Scanner.h
%scanner-token-function d_scanner.lex()
%token INT VOID FLOAT IF WHILE ELSE FOR ASSIGN_OP INC_OP NOT_OP OR_OP AND_OP EQ_OP NE_OP LE_OP GE_OP INT_CONSTANT 
%token RETURN FLOAT_CONSTANT IDENTIFIER STRING_LITERAL 
%baseclass-preinclude "intu.hpp"
%polymorphic ast: abstractAST*; listTag : std::list<abstractAST*>; stringTag : std::string;

%type <ast> iteration_statement selection_statement statement expression l_expression primary_expression postfix_expression unary_expression multiplicative_expression additive_expression compound_statement assignment_statement logical_and_expression equality_expression relational_expression constant_expression
%type <stringTag> unary_operator IDENTIFIER INT_CONSTANT FLOAT_CONSTANT STRING_LITERAL INC_OP
%type <listTag> expression_list statement_list


%%
translation_unit:
	function_definition
	| translation_unit function_definition
{
	printSymbolTable(globalTable);
}
	;

function_definition:
	type_specifier fun_declarator compound_statement
{
	(*globalTable)[functionName]->pointer = currentTable;
	currentTable = new std::map<std::string, SymbolTableEntry*>(); 
	currentOffset = -4;
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
	if(globalTable->find(functionName) != globalTable->end()){
		std::cerr << "Line no " << lineNo << ":\tFunction " << functionName << " already defined\n";
		std::exit(1);
	}
	SymbolTableEntry* temp = new SymbolTableEntry();
	temp->type = SymbolTableEntry::FUNC;
	temp->scope = SymbolTableEntry::LOCAL;
	temp->dataType = constructDT(currentType,indexList);
	temp->size = temp->dataType->size();
	(*globalTable)[functionName]=temp; 
}
	| IDENTIFIER '(' ')'
{
	functionName = $1;
	currentOffset = 0;
	if(globalTable->find(functionName) != globalTable->end()){
		std::cerr << "Line no " << lineNo << ":\tFunction " << functionName << " already defined\n";
		std::exit(1);
	}
	SymbolTableEntry* temp = new SymbolTableEntry();
	temp->type = SymbolTableEntry::FUNC;
	temp->scope = SymbolTableEntry::LOCAL;
	temp->dataType = constructDT(currentType,indexList);
	temp->size = temp->dataType->size();
	(*globalTable)[functionName]=temp; 
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
	if($3->getType().basetype != DataType::Int){
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
}
	| '{' statement_list '}'
{
	$$ = new blockAST($2);
}
    | '{' declaration_list statement_list '}'
{
	$$ = new blockAST($3);
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
}
	| selection_statement
{
	$$ = $1;
}
	| iteration_statement
{
	$$ = $1;
}
	| assignment_statement
{
	$$ = $1;
}
    | RETURN expression ';'
{
	$$ = new returnAST($2);
}
	;

assignment_statement:
	 ';'
{
	$$ = new bopAST("EMPTY", NULL, NULL);
}
	|  l_expression ASSIGN_OP expression ';'
{
	if(!assignmentCompatible($1->getType(), $3->getType()))
	{
		std::cerr << "Line no " << lineNo << ":\tNo equality operator for types "; 
		($1)->getType().print(std::cerr);
		std::cerr << " and ";
		($3)->getType().print(std::cerr);
		std::cerr << "\n";                      
		std::exit(1);
	}
	$$ = new bopAST("ASSIGN", $1, $3);
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
}
	;

equality_expression:
	 relational_expression
{
	$$ = $1;
}
	| equality_expression EQ_OP relational_expression
{
	$$ = new bopAST("EQ_OP", $1, $3);
}
	| equality_expression NE_OP relational_expression
{
	$$ = new bopAST("NE_OP", $1, $3);
}
	;
relational_expression:
	 additive_expression
{
	$$ = $1;
}
	| relational_expression '<' additive_expression
{
	$$ = new bopAST("LT", $1, $3);
}
	| relational_expression '>' additive_expression
{
	$$ = new bopAST("GT", $1, $3);
}
	| relational_expression LE_OP additive_expression
{
	$$ = new bopAST("LE_OP", $1, $3);
}
	| relational_expression GE_OP additive_expression
{
	$$ = new bopAST("GE_OP", $1, $3);
}
	;

additive_expression:
	 multiplicative_expression
{
	$$ = $1;
}
	| additive_expression '+' multiplicative_expression
{
	$$ = new bopAST(std::string("PLUS"), $1, $3);
}
	| additive_expression '-' multiplicative_expression
{
	$$ = new bopAST(std::string("MINUS"), $1, $3);
}
	;

multiplicative_expression:
	 unary_expression
{
	$$ = $1;
}
	| multiplicative_expression '*' unary_expression
{
	$$ = new bopAST(std::string("MULT"), $1, $3);
}
	| multiplicative_expression '/' unary_expression
{
	$$ = new bopAST(std::string("DIVIDE"), $1, $3);
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
}
	;

postfix_expression:
	 primary_expression
{
	$$ = $1;
}
	| IDENTIFIER '(' ')'
{
	$$ = new funcAST($1, std::list<abstractAST*>());
	if(globalTable->find($1) == globalTable->end()){
		std::cerr << "Line no " << lineNo << ":\tFunction " << $1 << " undefined\n";
		std::exit(1);
	}
}
	| IDENTIFIER '(' expression_list ')'
{
	$$ = new funcAST($1, $3);
	if(globalTable->find($1) == globalTable->end()){
		std::cerr << "Line no " << lineNo << ":\tFunction " << $1 << " undefined\n";
		std::exit(1);
	}
}
	| l_expression INC_OP
{
	$$ = new uopAST($2, $1);
}
	;

primary_expression:
	 l_expression
{
	$$ = $1;
}
	| l_expression ASSIGN_OP expression 
{
	$$ = new assAST($1, $3);
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
//	std::map<std::string,SymbolTableEntry* >::iterator it = currentTable->find($1);
	if(currentTable->find($1) == currentTable->end()){
		std::cerr << "Line no " << lineNo << ":\t" << $1 << " doesn't name a type\n";
		std::exit(1);
	}
	$$ = new identifierAST($1);
	($$)->astType = *(((*currentTable->find($1)).second)->dataType);
}
        | l_expression '[' expression ']' 
{
	$$ = new indexAST($1, $3);
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
	$$ = new ifAST($3, $5, $7);
}
	;

iteration_statement:
	 WHILE '(' expression ')' statement
{
	$$ = new whileAST($3, $5);
}
	| FOR '(' expression ';' expression ';' expression ')' statement
{
	$$ = new forAST($3, $5, $7, $9);
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
