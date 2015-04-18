#ifndef _INTU_HPP
#define _INTU_HPP
#define NO_REGS 6


#include <list>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <vector>

extern std::ofstream codeFile;
std::string reg_name(int);
void backpatch(std::vector<int*>, int);
void reset_regs();
int find_reg();
extern bool avail_regs[NO_REGS];
extern int glabel;
extern int level;
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


	bool operator==(DataType);

	void print(std::ostream& a);
	/********************************************************************************
	INPUT: This is a member function. 
	RETURN VALUE: Returns the size occupied by the data type on which this function is called. Size occupied by Int, Float is 4 and that of Void is 0. Size occupied by an array is calculated depending on the length and type of array.
	********************************************************************************/
	int size();
	int len(); 
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

	/********************************************************************************
	INPUT: This is a member function
	FUNCTION: Prints the type, scope, size, offset fields of the object
	OUTPUT: none (void).
	********************************************************************************/
	void print();

};	


extern std::map<std::string, SymbolTableEntry*> *currentTable;
extern std::map<std::string, SymbolTableEntry*> *globalTable;


/* THIS IS THE CODE FROM 3RD ASSIGNMENT. THIS WAS USED TO CREATE ASTS FOR THE NON-DECLARATIVE PART */
//class basic_types{
//};

class symbolTable{};

class abstractAST{
	public:
		virtual void print(std::string format = "") = 0;
		virtual std::string generate_code() = 0;
		virtual bool checkTypeofAST() = 0;
		virtual float getVal(){};
		virtual void generate_label(){};
		virtual std::string actual_code(){};
		int label;
		int reg;
		int* truelabel;
		int* falselabel;
		int* nextlabel;
		std::vector<int* > nextlist;
		DataType astType;
		virtual std::string get_name(){};
	protected:
		virtual void setType(DataType) = 0;
	private:
//		typeExp astnode_type;
};


class castAST : public abstractAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code(){
			generate_label();
			actual_code();
			return "";
		};
		virtual std::string actual_code(){
			first->actual_code();
			reg = first->reg;
			if(castType == "TO_FLOAT"){
				codeFile << "\tintTofloat(" << reg_name(reg) << ");\n";
			}
			else{
				codeFile << "\tfloatToint(" << reg_name(reg) << ");\n";
			}
			return "";
		} 
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		virtual float getVal(){};
		DataType astType;
		castAST(std::string a, abstractAST *pointer){
			castType = a;
			first = pointer;
		}
		virtual void generate_label(){
			first->generate_label();
			label = first->label ;
			std::cout << "the label of cast is " << label << std::endl;
		}
	protected:
		virtual void setType(DataType) {};
	private:
//		typeExp astnode_type;
		std::string castType;
		abstractAST *first;
};


class stmtAST : public abstractAST{
	public:
		virtual void print(std::string format = "") {};
		virtual std::string generate_code() {};
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		bool hasReturn;
	protected:
		virtual void setType(DataType) {};
	private:
};


class expAST : public abstractAST{
	public:
		virtual void print(std::string format = "") {};
		virtual std::string generate_code() {
			generate_label();
			return "";
		};
//		virtual void generate_label(){};
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		virtual float getVal(){};
//		int label;
	protected:
		virtual void setType(DataType) {};
	private:
};

class funcStmtAST : public stmtAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code() {};
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		funcStmtAST(std::string a, std::list<abstractAST*> b){
			name = a;
			first = b;
		}

	protected:
		virtual void setType(DataType) {};
	private:
		std::string name;
		std::list <abstractAST*> first;
};


class blockAST : public stmtAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code() {
			for(std::list<abstractAST*>::iterator it = first.begin(); it != first.end(); it++){
				std::cout << "generated\n";
				std::cout.flush();
				(*it)->generate_code();
				reset_regs();
			}
			return "";
		};
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		blockAST(std::list<abstractAST*> a){
			first = a;
		}
	protected:
		virtual void setType(DataType) {};
	private:
		std::list<abstractAST*> first;
};


class assAST : public stmtAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code() {
			generate_label();
			actual_code();//TODO: This is not it. We don't yet know where the result of second resides.
			return "";
		}
		virtual std::string actual_code(){
			second->actual_code();
			reg = second->reg;
			if(*((*currentTable)[first->get_name()]->dataType) == DataType(DataType::Int)){
				codeFile << "\tstorei(" << reg_name(reg) << ", ind(ebp, " << -(*currentTable)[first->get_name()]->offset << "));\n";
			}
			else if(*((*currentTable)[first->get_name()]->dataType) == DataType(DataType::Float)){
				codeFile << "\tstoref(" << reg_name(reg) << ",ind(ebp, " << -(*currentTable)[first->get_name()]->offset << "));\n";
			}
			else{
				level++;
				first->actual_code();
				level--;
				codeFile << "\taddi(ebp, " << reg_name(first->reg) << ");\n";
				if((first->astType).basetype == 0){
					codeFile << "\tstorei(" << reg_name(reg) << ",ind(" << reg_name(first->reg) << "));\n";
				}
				else if((first->astType).basetype == 1){
					codeFile << "\tstoref(" << reg_name(reg) << ",ind(" << reg_name(first->reg) << "));\n";
				}
				avail_regs[first->reg] = true;
			}
			reset_regs();
			return "";
		}
		virtual void generate_label(){
			second->generate_label();
			label = second->label;
			std::cout << "the label of assignment is " << label << std::endl;
		}
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		assAST(abstractAST *a, abstractAST *b){
			first = a;
			second = b;
/*			nextlabel = new int;
			(*newlabel) = -1;
			nextlist.push_back(nextlabel);
*/		}
		
	protected:
		virtual void setType(DataType) {};
	private:
		abstractAST *first;
		abstractAST *second;
};


class ifAST : public stmtAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code() {
			first->generate_label();
			actual_code();
			return "";
		};
		virtual std::string actual_code(){
			first->actual_code();
			if(first->astType == DataType(DataType::Int)){
				codeFile << "\tcmpi(0," << reg_name(first->reg) << ");\n";
			}
			else if(first->astType == DataType(DataType::Float)){
				codeFile << "\tcmpf(0," << reg_name(first->reg) << ");\n";
			}
			int fl = glabel++;
			codeFile << "\tje(L" << fl << ");\n";
			int ol = glabel++;
			second->generate_code();
			codeFile << "\tj(L" << ol << ");\n";
  			codeFile << "L" << fl << ":\n";
			third->generate_code();
			codeFile << "L" << ol << ":\n";
/*			codeFile << "\tje(L" << *(first->falselabel) << ");\n";
			second->actual_code;
			codeFile << "\tje(" << *(second->nextlabel) << ");\n";
			codeFile << "L" << *(first->falselabel) << ":\n";
			third->actual_code;
			codeFile << "\tje(" << *(third->nextlabel) << ");\n";*/
			reset_regs();
			return "";
		} 
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		ifAST(abstractAST *a, abstractAST *b, abstractAST *c){
			first = a;
			second = b;
			third = c;
//			int m1 = glabel++;
//			int m2 = glabel++;
//			*(first->truelabel) = m1;
//			*(first->falselabel) = m2;
//			
//			merge(second->nextlist,third->nextlist); 
//			nextlist = second->nextlist;	
		}
	protected:
		virtual void setType(DataType) {};
	private:
		abstractAST *first;
		abstractAST *second;
		abstractAST *third;
};


class whileAST : public stmtAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code() {
			actual_code();
			return "";
		};
		virtual std::string actual_code(){
			int openl = glabel++;
			codeFile << "L" << openl << ":\n";
			first->generate_code();
			int exitl = glabel++;
			if(first->astType == DataType(DataType::Int)){
				codeFile << "\tcmpi(0," << reg_name(first->reg) << ");\n";
			}
			else if(first->astType == DataType(DataType::Float)){
				codeFile << "\tcmpf(0," << reg_name(first->reg) << ");\n";
			}
			codeFile << "\tje(L" << exitl << ");\n";
			second->generate_code();
			codeFile << "\tj(L" << openl << ");\n";
			codeFile << "L" << exitl << ":\n";
			reset_regs();
			return "";
		}
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		whileAST(abstractAST *a, abstractAST *b){
			first = a;
			second = b;
		}
	protected:
		virtual void setType(DataType) {};
	private:
		abstractAST *first;
		abstractAST *second;
};




class returnAST : public stmtAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code() {
			return "";
		};
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		returnAST(abstractAST *a){
			first = a;
		}
	protected:
		virtual void setType(DataType) {};
	private:
		abstractAST *first;
};

class forAST : public stmtAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code() {
			actual_code();
			return "";
		};
		virtual std::string actual_code(){
			first->generate_code();
			int entryl = glabel++;
			codeFile << "L" << entryl << ":\n";
			second->generate_code();
			if(second->astType == DataType(DataType::Int)){
				 codeFile << "\tcmpi(0," << reg_name(second->reg) << ");\n";
			}
			else if(second->astType == DataType(DataType::Float)){
				 codeFile << "\tcmpf(0," << reg_name(second->reg) << ");\n";
			}
			int exitl = glabel++;
			codeFile << "\tje(L" << exitl << ");\n";
			fourth->generate_code();
			third->generate_code();
			codeFile << "\tj(L" << entryl << ");\n";
			codeFile << "L" << exitl << ":\n";
			reset_regs();
			return "";
		}
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		forAST(abstractAST *a, abstractAST *b, abstractAST *c, abstractAST *d){
			first = a;
			second = b;
			third = c;
			fourth = d;
		}
	protected:
		virtual void setType(DataType) {};
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
		virtual std::string generate_code() {
			generate_label();
			std::cout << "labels ready mein Fuhrer\n";
			std::cout.flush();
			actual_code();
			return "";
		};
		virtual std::string actual_code(){
			std::cout << "Im into this thing\n";
			std::cout.flush();
			if(first->label > second->label){
				first->actual_code();
				if(first->label >= NO_REGS){
					if(first->astType == DataType(DataType::Float)){
						codeFile << "\tpushf(1);\n\tstoref(" << reg_name(first->reg) << ", ind(esp));\n";
					}	
					if(first->astType == DataType(DataType::Int)){
						codeFile << "\tpushi(1);\n\tstorei(" << reg_name(first->reg) << ", ind(esp));\n";
					}
					avail_regs[first->reg] = true;
				}
				second->actual_code();	
				if(first->label >= NO_REGS){
					first->reg = find_reg();
					if(first->astType == DataType(DataType::Float)){
//						codeFile << "\tpushf(1);\n\tstoref(" << reg_name(first->reg) << ", ind(esp));\n";
						codeFile << "\tloadf(ind(esp), " << reg_name(first->reg) << ");\n\tpopf(1);\n";
					}	
					if(first->astType == DataType(DataType::Int)){
//						codeFile << "\tpushi(1);\n\tstorei(" << reg_name(first->reg) << ", ind(esp));\n";
						codeFile << "\tloadi(ind(esp), " << reg_name(first->reg) << ");\n\tpopi(1);\n";
					}
//					avail_regs[first->reg] = true;
				}
			}
			else{
				second->actual_code();
			std::cout << "Nothing is second\n";
			std::cout.flush();
				if(second->label >= NO_REGS){
					if(second->astType == DataType(DataType::Float)){
						codeFile << "\tpushf(1);\n\tstoref(" << reg_name(second->reg) << ", ind(esp));\n";
					}	
					if(second->astType == DataType(DataType::Int)){
						codeFile << "\tpushi(1);\n\tstorei(" << reg_name(second->reg) << ", ind(esp));\n";
					}
					avail_regs[second->reg] = true;
				}
				first->actual_code();	
				std::cout << "std is bad\n";
				std::cout.flush();
				if(second->label >= NO_REGS){
					second->reg = find_reg();
					if(second->astType == DataType(DataType::Float)){
						codeFile << "\tloadf(ind(esp), " << reg_name(second->reg) << ");\n\tpopf(1);\n";
					}	
					if(second->astType == DataType(DataType::Int)){
						codeFile << "\tloadi(ind(esp), " << reg_name(second->reg) << ");\n\tpopi(1);\n";
					}
				}
			}
			if(op == "PLUS"){
				codeFile << "\taddi(" << reg_name(second->reg) << ", " << reg_name(first->reg) << ");\n";
			}
			else if(op == "PLUS_FLOAT"){
				codeFile << "\taddf(" << reg_name(second->reg) << ", " << reg_name(first->reg) << ");\n";
			}
			else if(op == "MINUS"){
				codeFile << "\tmuli(-1," << reg_name(second->reg) << ");\n";
				codeFile << "\taddi(" << reg_name(second->reg) << ", " << reg_name(first->reg) << ");\n";
			}	
			else if(op == "MINUS_FLOAT"){
				codeFile << "\tmulf(-1," << reg_name(second->reg) << ");\n";
				codeFile << "\taddf(" << reg_name(second->reg) << ", " << reg_name(first->reg) << ");\n";
			}	
			else if(op == "MULT"){
				codeFile << "\tmuli(" << reg_name(second->reg) << ", " << reg_name(first->reg) << ");\n";
			}	
			else if(op == "MULT_FLOAT"){
				codeFile << "\tmulf(" << reg_name(second->reg) << ", " << reg_name(first->reg) << ");\n";
			}	
			else if(op == "DIVIDE"){
				codeFile << "\tdivi(" << reg_name(second->reg) << ", " << reg_name(first->reg) << ");\n";
			}	
			else if(op == "DIVIDE_FLOAT"){
				codeFile << "\tdivf(" << reg_name(second->reg) << ", " << reg_name(first->reg) << ");\n";
			}	
			else{
				std::cout << "how is this case possible?\n" ;
			}
			reg = first->reg;
			avail_regs[second->reg] = true;
			return "";
		}

				
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		bopAST(std::string a, abstractAST *b, abstractAST *c){
			op = a; //a may be "PLUS", "PLUSfloat", "MINUS", "MINUSfloat"
			first = b;
			second = c;
			label = 1;
		}
		virtual void generate_label();
//			label_manager(this, first,second);
	protected:
		virtual void setType(DataType) {};
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
		virtual std::string generate_code() {
			generate_label();
			actual_code();
			return "";
		}
		virtual std::string actual_code(){
			reg = first->reg;
			if(op == "PP"){
				if(first->astType == DataType(DataType::Int)){
					codeFile << "\taddi(1,"	<< reg_name(reg) << ");\n";
					codeFile << "\tstorei(" << reg_name(reg) << ",ind(ebp, " << -(*currentTable)[first->get_name()]->offset << "));\n";
				}
				else{
					codeFile << "\taddf(1," << reg_name(reg) << ");\n";
					codeFile << "\tstorefi(" << reg_name(reg) << ",ind(ebp, " << -(*currentTable)[first->get_name()]->offset << "));\n";
				}
				
			}
			if(op == "UMINUS"){
				if(first->astType == DataType(DataType::Int)){
					codeFile << "\tmuli(-1," << reg_name(reg) << ")\n;";
					codeFile << "\tstorei(" << reg_name(reg) << ",ind(ebp, " << -(*currentTable)[first->get_name()]->offset << "));\n";
				}
				else{
					codeFile << "\tmulf(-1," << reg_name(reg) << ")\n;";
					codeFile << "\tstoref(" << reg_name(reg) << ",ind(ebp, " << -(*currentTable)[first->get_name()]->offset << "));\n";
				}
			}	
			return "";

			//TODO : what if the op is not
		}

		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		uopAST(std::string a, abstractAST *b){
			op = a;
			first = b;
		}
		virtual void generate_label(){
			first->generate_label();
			this->label = first->label;
			std::cout << "label of uop is " << label << std::endl;
		}
	protected:
		virtual void setType(DataType) {};
	private:
//		uoperator op;
		std::string op;
		abstractAST *first;
};

class funcAST : public expAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code() {
			generate_label();
			actual_code();
			return "";
		};
		std::string actual_code();
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		funcAST(std::string a, std::list<abstractAST*> b){
			name = a;
			first = b;
		}
		virtual void generate_label(){
			this->label = 1;
			std::cout << "label of func is " << label << std::endl;
		}
	protected:
		virtual void setType(DataType) {};
	private:
		std::string name;
		std::list <abstractAST*> first;
};


class floatAST : public expAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code() {
			generate_label();
			actual_code();
			return "";
		};
		virtual std::string actual_code(){
			reg = find_reg();
			codeFile << "\tmove(" << getVal() << ", " << reg_name(reg) << ");\n";
			return "";
		}
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		floatAST(std::string a){
			first = a;
		}
		virtual float getVal(){
			return std::stof(first);
		}
		virtual void generate_label(){
			label = 1;
			std::cout << "label of float is " << label << std::endl;
		}

	protected:
		virtual void setType(DataType) {};
	private:
		std::string first;
};


class intAST : public expAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code() {
			generate_label();
			actual_code();
			return "";
		};
		virtual std::string actual_code(){
			reg = find_reg();
			codeFile << "\tmove(" << getVal() << ", " << reg_name(reg) << ");\n";
			return "";
		}
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		intAST(std::string a){
			first = a;
		}
		virtual float getVal(){
			return std::stoi(first);
		}
		virtual void generate_label(){
			label = 1;
			std::cout << "label of int is " << label << std::endl;
		}
	protected:
		virtual void setType(DataType) {};
	private:
		std::string first;
};


class stringAST : public expAST{
	/*TODO : label for string?*/
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code() {};
		virtual DataType getType() {
		};
		virtual bool checkTypeofAST() {};
		stringAST(std::string a){
			first = a;
		}
	protected:
		virtual void setType(DataType) {};
	private:
		std::string first;
};



class arrayrefAST : public expAST{
	public:
		virtual void print(std::string format = "") {};
		virtual std::string generate_code() {};
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
	protected:
		virtual void setType(DataType) {};
	private:
};


class identifierAST : public arrayrefAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code() {
			generate_label();
			actual_code();
			 return "";
		};
		virtual std::string actual_code(){
			reg = find_reg();
			if(*((*currentTable)[first]->dataType) == DataType(DataType::Float)){
				codeFile << "\tloadf(ind(ebp, " << -(*currentTable)[first]->offset << "), " << reg_name(reg) << ");\n";
			}
			else if(*((*currentTable)[first]->dataType) == DataType(DataType::Int)){
				codeFile << "\tloadi(ind(ebp, " << -(*currentTable)[first]->offset << "), " << reg_name(reg) << ");\n";
			}
			else{
				codeFile << "\tmove(" << -(*currentTable)[first]->offset << ", " << reg_name(reg) << ");\n";
			}
			return "";
		}
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		identifierAST(std::string a){
			first = a;
//			truelabel = new int;
//			falselabel = new int;
//			(*truelabel) = -1;
//			(*falselabel) = -1;
		}
		virtual void generate_label(){
			label = 1;
			std::cout << "label of identifier is " << label << std::endl;
		}
		virtual std::string get_name(){
			return first;
		}
	protected:
		virtual void setType(DataType) {};
	private:
		std::string first;
};

class indexAST : public arrayrefAST{
	public:
		virtual void print(std::string format = "");
		virtual std::string generate_code() {
			actual_code();	
			return "";
		};
		virtual std::string actual_code(){
			std::cout <<  std::endl;
			level++;
			first->generate_code();
			level--;
			second->generate_code();
			reg = first->reg;
			codeFile << "\tmuli(" << -((first->astType).arrayType)->size() << "," << reg_name(second->reg) << ");\n";
			codeFile << "\taddi(" << reg_name(second->reg)  << "," << reg_name(first->reg) << ");\n";
			avail_regs[second->reg] = true;
			if(level == 0){
				if(astType.basetype == 0){
					codeFile << "\taddi(ebp, " << reg_name(reg) << ");\n";
					codeFile << "\tloadi(ind(" << reg_name(reg) << "), " << reg_name(reg) <<");\n";
				}
				else if(astType.basetype == 1){
					codeFile << "\taddf(ebp, " << reg_name(reg) << ");\n";
					codeFile << "\tloadf(ind(" << reg_name(reg) << "), " << reg_name(reg) <<");\n";
				}
			}
			return "";
		}
		virtual std::string get_name(){
			return first->get_name();
		}
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		indexAST(abstractAST *a, abstractAST *b){
//			std::cout << "inside --> ";
//			a->astType.print(std::cout);
			first = a;
			second = b;
//			std::cout << "later inside --> ";
//			a->astType.print(std::cout);
		}
		virtual void generate_label();
//			label_manager(this, first,second);
//		}
	protected:
		virtual void setType(DataType) {};
	private:
		abstractAST *first;
		abstractAST *second;
};
/**************************************************************************************/
extern int currentOffset;
extern std::string name;
extern std::string functionName;
extern DataType currentType;
extern DataType pastType;
extern DataType currentFuncType;
extern std::list<int> indexList;
extern int lineNo;
extern int returnCount;
extern std::string suffixString;




DataType *constructDT(DataType, std::list<int>);
/********************************************************************************
INPUT: A pointer to symbol table
FUNCTION: Prints each entry of a symbol table using call to print function of symbol table entry and prints a new line after every row.
OUTPUT: none(void).
********************************************************************************/
void printSymbolTable(std::map<std::string, SymbolTableEntry*> *);
/********************************************************************************
INPUT: Two SymbolTableEntry pointers.
FUNCTION: This function is used only to sort two symbol table entries in a list. So this is just a comparison function. Sorting is done based on offsets
OUTPUT: We want sort to be done in reverse order. So we reverse > instead of <
********************************************************************************/
//bool offsetCompare(SymbolTableEntry*, SymbolTableEntry*);

bool hasReturnInList(std::list<abstractAST*> l);
std::string findBestFunction(std::string, std::string);
//extern FILE *codeFile;// = fopen("code.asm", "w+");
void label_manager(expAST* , expAST*, expAST*);
#endif
