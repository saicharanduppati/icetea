#ifndef _INTU_HPP
#define _INTU_HPP
#include <list>
#include <map>
#include <string>
#include <iostream>
struct DataType{
	enum Kind{
		Base, Array, Error, Ok
	};
	enum BaseType{
		Int, Float, Void
	};
	Kind tag;
	BaseType basetype;
	int length;                    
	DataType *arrayType;              //length and arrayType are to be used only if tag == Array
	DataType(BaseType b){
		/********************************************************************************
		INPUT: A base type like int, float, etc..
		OUTPUT: A DataType object representing the given type.
		  ********************************************************************************/
		tag = Base;
		basetype = b;
	}

//	DataType
	DataType(){} //a default constructor

	int size(){
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

};




struct SymbolTableEntry{
	enum Type {VAR, FUNC};
	enum Scope {PARAM, LOCAL};
	
//	std::string name;
	Type type;
	DataType *dataType;
	int size; //since we can only use integer number of bytes
	int offset; //since offset too is an integer number of bytes
	Scope scope;
	std::map<std::string, SymbolTableEntry*> *pointer;
};	




/* THIS IS THE CODE FROM 3RD ASSIGNMENT. THIS WAS USED TO CREATE ASTS FOR THE NON-DECLARATIVE PART */
class basic_types{
};

class symbolTable{};

class abstractAST{
	public:
		virtual void print(std::string format = "") = 0;
		virtual std::string generate_code(const symbolTable&) = 0;
		virtual basic_types getType() = 0;
		virtual bool checkTypeofAST() = 0;
		virtual float getVal(){};
	protected:
		virtual void setType(basic_types) = 0;
	private:
//		typeExp astnode_type;
};


class stmtAST : public abstractAST{
	public:
		virtual void print(std::string format = "") {};
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
	protected:
		virtual void setType(basic_types) {};
	private:
};


class expAST : public abstractAST{
	public:
		virtual void print(std::string format = "") {};
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
		virtual float getVal(){};
	protected:
		virtual void setType(basic_types) {};
	private:
};


class blockAST : public stmtAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
		blockAST(std::list<abstractAST*> a){
			first = a;
		}
	protected:
		virtual void setType(basic_types) {};
	private:
		std::list<abstractAST*> first;
};


class assAST : public stmtAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
		assAST(abstractAST *a, abstractAST *b){
			first = a;
			second = b;
		}
	protected:
		virtual void setType(basic_types) {};
	private:
		abstractAST *first;
		abstractAST *second;
};


class ifAST : public stmtAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
		ifAST(abstractAST *a, abstractAST *b, abstractAST *c){
			first = a;
			second = b;
			third = c;
		}
	protected:
		virtual void setType(basic_types) {};
	private:
		abstractAST *first;
		abstractAST *second;
		abstractAST *third;
};


class whileAST : public stmtAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
		whileAST(abstractAST *a, abstractAST *b){
			first = a;
			second = b;
		}
	protected:
		virtual void setType(basic_types) {};
	private:
		abstractAST *first;
		abstractAST *second;
};




class returnAST : public stmtAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
		returnAST(abstractAST *a){
			first = a;
		}
	protected:
		virtual void setType(basic_types) {};
	private:
		abstractAST *first;
};

class forAST : public stmtAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
		forAST(abstractAST *a, abstractAST *b, abstractAST *c, abstractAST *d){
			first = a;
			second = b;
			third = c;
			fourth = d;
		}
	protected:
		virtual void setType(basic_types) {};
	private:
		abstractAST *first;
		abstractAST *second;
		abstractAST *third;
		abstractAST *fourth;
};



class bopAST : public expAST{
	public:
//		enum boperator {};
		virtual void print(std::string format = "");
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
		bopAST(std::string a, abstractAST *b, abstractAST *c){
			op = a;
			first = b;
			second = c;
		}
	protected:
		virtual void setType(basic_types) {};
	private:
//		boperator op;
		std::string op;
		abstractAST *first;
		abstractAST *second;
};


class uopAST : public expAST{
	public:
//		enum uoperator {UMINUS, NOT, PP};
		virtual void print(std::string format = "");
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
		uopAST(std::string a, abstractAST *b){
			op = a;
			first = b;
		}
	protected:
		virtual void setType(basic_types) {};
	private:
//		uoperator op;
		std::string op;
		abstractAST *first;
};

class funcAST : public expAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
		funcAST(std::string a, std::list<abstractAST*> b){
			name = a;
			first = b;
		}

	protected:
		virtual void setType(basic_types) {};
	private:
		std::string name;
		std::list <abstractAST*> first;
};


class floatAST : public expAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
		floatAST(std::string a){
			first = a;
		}
		virtual float getVal(){
			return std::stof(first);
		}
	protected:
		virtual void setType(basic_types) {};
	private:
		std::string first;
};


class intAST : public expAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
		intAST(std::string a){
			first = a;
		}
		virtual float getVal(){
			return std::stoi(first);
		}
	protected:
		virtual void setType(basic_types) {};
	private:
		std::string first;
};


class stringAST : public expAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
		stringAST(std::string a){
			first = a;
		}
	protected:
		virtual void setType(basic_types) {};
	private:
		std::string first;
};



class arrayrefAST : public expAST{
	public:
		virtual void print(std::string format = "") {};
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
	protected:
		virtual void setType(basic_types) {};
	private:
};


class identifierAST : public arrayrefAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
		identifierAST(std::string a){
			first = a;
		}
	protected:
		virtual void setType(basic_types) {};
	private:
		std::string first;
};

class indexAST : public arrayrefAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code(const symbolTable&) {};
		virtual basic_types getType() {};
		virtual bool checkTypeofAST() {};
		indexAST(abstractAST *a, abstractAST *b){
			first = a;
			second = b;
		}
	protected:
		virtual void setType(basic_types) {};
	private:
		abstractAST *first;
		abstractAST *second;
};
/**************************************************************************************/
extern int currentOffset;
extern std::string name;
extern DataType currentType;
extern std::map<std::string, SymbolTableEntry*> currentTable;
extern std::map<std::string, SymbolTableEntry*> globalTable;
extern std::list<int> indexList;
	DataType *constructDT(DataType a, std::list<int> list);
#endif
