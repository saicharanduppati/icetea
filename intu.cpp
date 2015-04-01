#include "intu.hpp"
std::map<std::string, SymbolTableEntry*> *globalTable = new std::map<std::string, SymbolTableEntry*>(); //this holds the pointer to the global symbol table. GST contains only functions in our language.
std::map<std::string, SymbolTableEntry*> *currentTable = new std::map<std::string, SymbolTableEntry*>(); //this holds the pointer to the global symbol table. GST contains only functions in our language.
DataType currentType; //this is the most recently seen DataType.
DataType currentFuncType;
DataType pastType;
int currentOffset = -4; //offset of the next to-be-seen variable.
std::string name; //name of the last seen variable.
std::string functionName; //name of the last seen function.
std::list<int> indexList; //list that contains the int indices seen in an array declaration. If the declaration is int a[4][5][8], list contains [4,5,8].
int returnCount = 0;

int lineNo = 1;
std::string suffixString = "";


int stringCost(std::string a, std::string b){// a and b have to be of same size.
	int toReturn = 0;
	for(int i = 0; i < a.size(); i++){
		toReturn += ((a[i] == b[i]) ? 0 : 1);
	}
	return toReturn;
}


std::string findBestFunction(std::string name, std::string suffix){
	int count = 0, minCost = 1000000, cost;
	std::string toReturn = "2"; //this signifies that there is no matching fucntion.
	for(std::map<std::string, SymbolTableEntry*>::iterator iter = globalTable->begin(); iter != globalTable->end(); iter++){
		if(iter->first.substr(0, iter->first.find_first_of("#")) != name){
			continue;
		}
		std::string stored = iter->first.substr(iter->first.find_first_of("#") + 1);
		if(stored.size() != suffix.size()){
			continue;
		}
		if(stringCost(stored, suffix) == minCost){
			count++;
		}
		if(stringCost(stored, suffix) < minCost){
			minCost = stringCost(stored, suffix);
			toReturn = stored;
			count = 1;
		}
	}
	if(count > 1){
		return "1";//this signifies that there are conflicting definitions
	}
	return toReturn;
}





bool hasReturnInList(std::list<abstractAST*> l){
	for(std::list<abstractAST*>::iterator it = l.begin(); it != l.end(); it++){
		if(((stmtAST*) (*it))->hasReturn) return true;
	}
	return false;
}




/*bool offsetCompare(SymbolTableEntry* first, SymbolTableEntry *second){
	return first->offset > second->offset;

//	return other.offset < offset;
}*/
void printSymbolTable(std::map<std::string, SymbolTableEntry*> *argument){
//	std::cout << "size is " << argument->size() << "\n";
	for(std::map<std::string, SymbolTableEntry*>::iterator iter = argument->begin(); iter != argument->end(); iter++){
		std::cout.width(10);
		std::cout << std::left << iter->first.substr(0,iter->first.find_first_of("#"));
		iter->second->print();
		if(iter->second->type == SymbolTableEntry::FUNC){
			std::cout << "SYMBOL TABLE OF " << iter->first.substr(0,iter->first.find_first_of("#")) << " ------------------\n";
			printSymbolTable(iter->second->pointer);
			std::cout << "----------------------------------\n\n";
		}
	}
}

//bool assignmentCompatible(DataType left, DataType right){
//	return false;
//}


void blockAST::print(std::string format){
	std::cout << format << "(Block [" << std::endl;
	std::list<abstractAST*>::iterator iter = first.begin();
	for(; iter != --first.end();iter++){
		(*iter)->print(format+"        ");
		std::cout << std::endl;
	}
	(*iter)->print(format+ "        ");
	std::cout << "])\n";
}

void funcStmtAST::print(std::string format){
	std::cout << format;
	std::cout << "(" << name.substr(0,name.find_first_of("#")) << " ";
	for(std::list<abstractAST*>::iterator iter = first.begin();iter!=first.end();iter++){
		(*iter)->print();
	}
	std::cout << ")";
}

void assAST::print(std::string format){
	std::cout << format << "(Assign_exp ";
    first->print();
	std::cout << " ";
	second->print();
	std::cout << ")";
}	

void ifAST::print(std::string format){
	std::cout << format << "(If ";
	first->print();
	std::cout << std::endl;
	second->print(format + "    ");
	std::cout << std::endl;
	third->print(format + "    ");
	std::cout << ")";
}

void castAST::print(std::string format){
  std::cout << format << "(" << castType << ")";
  first->print();
}

void whileAST::print(std::string format){
	std::cout << format << "(While ";
	first->print();
	std::cout << std::endl;
	second->print(format + "       " );
//	std::cout << std::endl;
	std::cout << ")";
}

void forAST::print(std::string format){
	std::cout << format << "(For ";
	first->print();
	std::cout << std::endl;
	second->print(format + "     ");
	std::cout << std::endl;
	third->print(format + "     ");
	std::cout << std::endl;
	fourth->print(format + "     ");
	std::cout << ")";
}

void bopAST::print(std::string format){
	std::cout << format;
	if(op == "EMPTY"){
		std::cout << "(Empty)";
	}
	else{
		std::cout << "(" << op << " ";
		first->print();
		std::cout << " ";
		second->print();
	//	std::cout << std::endl;
		std::cout << ")";
	}
}

void uopAST::print(std::string format){
	std::cout << format;
	std::cout << "(" << op << " ";
	first->print();
	std::cout << ")";
}

void funcAST::print(std::string format){
	std::cout << format;
	std::cout << "(" << name.substr(0,name.find_first_of("#")) << " ";
	for(std::list<abstractAST*>::iterator iter = first.begin();iter!=first.end();iter++){
		(*iter)->print();
	}
	std::cout << ")";
}

void floatAST::print(std::string format){
	std::cout << format;
	std::cout << "(FloatConst " << first << ")" ;
}

void intAST::print(std::string format){
	std::cout << format << "(IntConst " << first << ")";
}

void stringAST::print(std::string format){
	std::cout << format << "(StringConst " << first << ")";
}

void identifierAST::print(std::string format){
	std::cout << format << "(Id “" << first << "”)";
}

void indexAST::print(std::string format){
	std::cout << format << "(ArrayRef ";
	first->print();
	std::cout << "[";
	second->print();
	std::cout << "]";
}

void returnAST::print(std::string format){
	std::cout << format << "(Return ";
	first->print();
	std::cout <<")";
}


DataType *constructDT(DataType a, std::list<int> list){
	DataType *toReturn = new DataType();
	if(list.size() == 0){ //in this case, a will be a base type for sure.
		toReturn->tag = DataType::Base;
		toReturn->basetype = a.basetype;
	}
	else{
		toReturn->tag = DataType::Array;
		toReturn->length = *list.begin();
		list.erase(list.begin());
		toReturn->arrayType = constructDT(a,list );
	}
	return toReturn;
}





int DataType::size(){
	if(tag == Error || tag == Ok){
		std::cout << "wrong usage of size" << std::endl;
		exit(1);
	}
	else if(tag == Base){
		switch(basetype){
			case Int: 
			{
				return 4;
			}
			case Float:
			{
				return 4;
			}
			case Void:
			{
				return 0;
			}
		};
	}
	else{
		return length * arrayType->size();
	}
}





void DataType::print(std::ostream& a){
	if(tag == Base){
		switch(basetype){
			case Int:
				{
					a << "int";
					break;
				}
			case Float:
				{
					a << "float";
					break;
				}
			case Void:
				{
					a << "void";
					break;
				}
			default:{}
		};
		return;
	}
	a << std::left << "Array(" << length << ", ";
	arrayType->print(a);
	a << ")";
	return;
}




bool DataType::operator==(DataType second){
	if(tag != second.tag){
		return false;
	}
	if(tag == Base){
		return (basetype == second.basetype);
	}
	return ((*arrayType) == (*(second.arrayType)));
}





void SymbolTableEntry::print(){
	std::cout.width(10);
	std::cout << std::left << ((type == VAR) ? "VAR" : "FUNC");// << "\t\t";
	std::cout.width(10);
	std::cout << std::left << ((scope == PARAM) ? "PARAM" : "LOCAL");// << "\t\t";
	std::cout.width(10);
	std::cout << std::left << size;// << "\t\t";
	std::cout.width(10);
	std::cout << std::left << offset;// << "\t\t";
	dataType->print(std::cout);
	std::cout << "\n";
}
