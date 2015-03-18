#include "intu.hpp"
std::map<std::string, SymbolTableEntry*> currentTable;
std::map<std::string, SymbolTableEntry*> globalTable;
DataType currentType;
int currentOffset = 0;
std::string name;
std::list<int> indexList;
void blockAST::print(std::string format){
	std::cout << format << "(Block [" << std::endl;
	std::list<abstractAST*>::iterator iter = first.begin();
	for(; iter != --first.end();iter++){
		(*iter)->print(format+"        ");
		std::cout << std::endl;
	}
	(*iter)->print(format+ "        ");
	std::cout << "])";
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
	std::cout << "(" << name << " ";
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
		if(list.size() == 0){ //in this case, a will be a base type for sure.
			DataType *toReturn = new DataType();
			toReturn->tag = DataType::Base;
			toReturn->basetype = a.basetype;
			return toReturn;
		}
		else{
			DataType *toReturn = new DataType();
			toReturn->tag = DataType::Array;
			toReturn->length = *list.begin();
			list.erase(list.begin());
			toReturn->arrayType = constructDT(a,list );
		}
	}


