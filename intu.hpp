#ifndef _INTU_HPP
#define _INTU_HPP
#define NO_REGS 4


#include <list>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <cstdlib>
#include <stdio.h>
#include <vector>

extern std::string functionName;
extern std::string suffixString;
extern std::ofstream codeFile;
std::string reg_name(int);
void backpatch(std::vector<int*>, int);
void reset_regs();
int find_reg();
extern bool avail_regs[NO_REGS];
extern int reg_type[NO_REGS];
extern int glabel;
extern bool loadkaru; 
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
		bool hasAssignment;
	protected:
		virtual void setType(DataType) = 0;
	private:
//		typeExp astnode_type;
};

void bopGenCodeHelper(abstractAST* , abstractAST* );

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
				reg_type[reg] = 2;	
			}
			else{
				codeFile << "\tfloatToint(" << reg_name(reg) << ");\n";
				reg_type[reg] = 1;

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
			hasAssignment = first->hasAssignment;
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
		virtual std::string generate_code() {
			generate_label();
			actual_code();
			return "";
		};
		virtual std::string actual_code();
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
			if(first->label > second->label){
				std::cout << "assignment cas1\n";
				std::cout.flush();
				loadkaru = false;
				first->actual_code();
				//reset_regs();
				//avail_regs[first->reg] = false;
				if(second->label >= NO_REGS){
					int tfr = reg_type[first->reg];
					codeFile << "\tpushi(1);\n\tstorei(" << reg_name(first->reg) << ",ind(esp);\n";
					avail_regs[first->reg] = true;
					loadkaru = true;
					second->actual_code();
					first->reg = find_reg();
					reg_type[first->reg] = tfr;
					codeFile << "\tloadi((esp)," << first->reg << ");\n" << "\tpopi(1);\n";	
				}
				else{
					loadkaru = true;
					second->actual_code();
				}
				
			}
			else{
				std::cout << "assignment cas2\n";
				std::cout.flush();
				loadkaru = true;
				second->actual_code();
				std::cout << "returned from second" << std::endl;
				//reset_regs();
		//		avail_regs[second->reg] = false;
//				codeFile << "heyhethethehthethehth\n";
				if(first->label >= NO_REGS){
					int tfr = reg_type[second->reg];
					codeFile << "\tpushi(1);\n\tstorei(" << reg_name(second->reg) << ",ind(esp);\n";
					avail_regs[second->reg] = true;
					loadkaru = false;
					first->actual_code();
					second->reg = find_reg();
					reg_type[second->reg] = tfr;
					codeFile << "\tloadi((esp)," << second->reg << ");\n" << "\tpopi(1);\n";	
				}
				else{
					loadkaru = false;
					first->actual_code();
				}
			}
			reg = second->reg;
			if(*((*currentTable)[first->get_name()]->dataType) == DataType(DataType::Int)){
				codeFile << "\tstorei(" << reg_name(reg) << ", ind(ebp, " << -(*currentTable)[first->get_name()]->offset << "));\n";
			}
			else if(*((*currentTable)[first->get_name()]->dataType) == DataType(DataType::Float)){
				codeFile << "\tstoref(" << reg_name(reg) << ",ind(ebp, " << -(*currentTable)[first->get_name()]->offset << "));\n";
			}
			else{
				codeFile << "\taddi(ebp, " << reg_name(first->reg) << ");\n";
				if((first->astType).basetype == 0){
					codeFile << "\tstorei(" << reg_name(reg) << ",ind(" << reg_name(first->reg) << "));\n";
				}
				else if((first->astType).basetype == 1){
					codeFile << "\tstoref(" << reg_name(reg) << ",ind(" << reg_name(first->reg) << "));\n";
				}
				avail_regs[first->reg] = true;
			}
			reset_regs();                  //------> beware of this
			return "";
		}
		virtual void generate_label();
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		assAST(abstractAST *a, abstractAST *b){
			first = a;
			second = b;
			hasAssignment = true;
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
			reset_regs();
			second->generate_code();
			codeFile << "\tj(L" << ol << ");\n";
  			codeFile << "L" << fl << ":\n";
			reset_regs();
			third->generate_code();
			codeFile << "L" << ol << ":\n";
			reset_regs();
			return "";
		} 
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		ifAST(abstractAST *a, abstractAST *b, abstractAST *c){
			first = a;
			second = b;
			third = c;
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
			reset_regs();
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
			first->generate_code();
			int minOffset = -4, width = 0; 
			for(std::map<std::string, SymbolTableEntry*>::iterator it = currentTable->begin(); it != currentTable->end(); it++){
				if(it->second->offset <= minOffset){
					minOffset = it->second->offset;
					width = it->second->dataType->size();
				}
			}
			if(functionName + "#" + suffixString != "main#"){//return value must not be placed for function main(), for others, it has to be placed.
				if(first->astType == DataType(DataType::Int)){
//					std::cout << "minOffset is " << minOffset << " and width is " << width << std::endl;
					codeFile << "\tstorei(" << reg_name(first->reg) << ", ind(ebp, " << -minOffset + width << "));\n";
				}
				if(first->astType == DataType(DataType::Float)){
					codeFile << "\tstoref(" << reg_name(first->reg) << ", ind(ebp, " << -minOffset + width << "));\n";
				}//this part does the storing of return value into space allocated. We can make all regs available now.
			}
			reset_regs();
			//next part is to restore the ebp value (dynamic link);	TODO: These two parts, namely restoring ebp and making esp point to end of params, are to be done even if there is no return statement. Better thing would be to make a label and make everyone jump to that label. presently everything is done locally.
//			codeFile << "\tmove(ebp, esp);\n\tloadi(ind(ebp), ebp);\n\tpopi(1);\n";
			codeFile << "\tj(L" << functionName + "_" + suffixString << ");\n";
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
			reset_regs();
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
		virtual void print(std::string format = "");
		virtual std::string generate_code() {
			generate_label();
//			std::cout << "labels ready mein Fuhrer\n";
//			std::cout.flush();
			actual_code();
			return "";
		};
		virtual std::string actual_code(){
//			std::cout << "Im into this thing\n";
//			std::cout.flush();
			loadkaru = true;
			if(first->hasAssignment){
				bopGenCodeHelper(first, second);
			}
			else if(second->hasAssignment){
				bopGenCodeHelper(second, first);
			}
			else{
				if(first->label > second->label){
					bopGenCodeHelper(first, second);
				}
				else{
					bopGenCodeHelper(second, first);
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
			else if(op == "EQ_OP"){
				int jlabel = glabel++;
				int exitlabel = glabel++;
				codeFile << "\tcmpi(" << reg_name(first->reg) << ", " << reg_name(second->reg) << ");\n\tje(L" << jlabel << ");\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n\tj(L" << exitlabel << ");\n";
				codeFile << "L" << jlabel << ":\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitlabel << ":\n";
			}
			else if(op == "EQ_OP_FLOAT"){
				int jlabel = glabel++;
				int exitlabel = glabel++;
				codeFile << "\tcmpf(" << reg_name(first->reg) << ", " << reg_name(second->reg) << ");\n\tje(L" << jlabel << ");\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n\tj(L" << exitlabel << ");\n";
				codeFile << "L" << jlabel << ":\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitlabel << ":\n";
			}
			else if(op == "NE_OP"){
				int jlabel = glabel++;
				int exitlabel = glabel++;
				codeFile << "\tcmpi(" << reg_name(first->reg) << ", " << reg_name(second->reg) << ");\n\tjne(L" << jlabel << ");\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n\tj(L" << exitlabel << ");\n";
				codeFile << "L" << jlabel << ":\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitlabel << ":\n";
			}
			else if(op == "NE_OP_FLOAT"){
				int jlabel = glabel++;
				int exitlabel = glabel++;
				codeFile << "\tcmpf(" << reg_name(first->reg) << ", " << reg_name(second->reg) << ");\n\tjne(L" << jlabel << ");\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n\tj(L" << exitlabel << ");\n";
				codeFile << "L" << jlabel << ":\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitlabel << ":\n";
			}
			else if(op == "LT"){
				int jlabel = glabel++;
				int exitlabel = glabel++;
				codeFile << "\tcmpi(" << reg_name(first->reg) << ", " << reg_name(second->reg) << ");\n\tjl(L" << jlabel << ");\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n\tj(L" << exitlabel << ");\n";
				codeFile << "L" << jlabel << ":\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitlabel << ":\n";
			}
			else if(op == "LT_FLOAT"){
				int jlabel = glabel++;
				int exitlabel = glabel++;
				codeFile << "\tcmpf(" << reg_name(first->reg) << ", " << reg_name(second->reg) << ");\n\tjl(L" << jlabel << ");\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n\tj(L" << exitlabel << ");\n";
				codeFile << "L" << jlabel << ":\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitlabel << ":\n";
			}
			else if(op == "GT"){
				int jlabel = glabel++;
				int exitlabel = glabel++;
				codeFile << "\tcmpi(" << reg_name(first->reg) << ", " << reg_name(second->reg) << ");\n\tjg(L" << jlabel << ");\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n\tj(L" << exitlabel << ");\n";
				codeFile << "L" << jlabel << ":\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitlabel << ":\n";
			}
			else if(op == "GT_FLOAT"){
				int jlabel = glabel++;
				int exitlabel = glabel++;
				codeFile << "\tcmpf(" << reg_name(first->reg) << ", " << reg_name(second->reg) << ");\n\tjg(L" << jlabel << ");\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n\tj(L" << exitlabel << ");\n";
				codeFile << "L" << jlabel << ":\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitlabel << ":\n";
			}
			else if(op == "LE_OP"){
				int jlabel = glabel++;
				int exitlabel = glabel++;
				codeFile << "\tcmpi(" << reg_name(first->reg) << ", " << reg_name(second->reg) << ");\n\tjle(L" << jlabel << ");\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n\tj(L" << exitlabel << ");\n";
				codeFile << "L" << jlabel << ":\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitlabel << ":\n";
			}
			else if(op == "LE_OP_FLOAT"){
				int jlabel = glabel++;
				int exitlabel = glabel++;
				codeFile << "\tcmpf(" << reg_name(first->reg) << ", " << reg_name(second->reg) << ");\n\tjle(L" << jlabel << ");\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n\tj(L" << exitlabel << ");\n";
				codeFile << "L" << jlabel << ":\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitlabel << ":\n";
			}
			else if(op == "GE_OP"){
				int jlabel = glabel++;
				int exitlabel = glabel++;
				codeFile << "\tcmpi(" << reg_name(first->reg) << ", " << reg_name(second->reg) << ");\n\tjge(L" << jlabel << ");\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n\tj(L" << exitlabel << ");\n";
				codeFile << "L" << jlabel << ":\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitlabel << ":\n";
			}
			else if(op == "GE_OP_FLOAT"){
				int jlabel = glabel++;
				int exitlabel = glabel++;
				codeFile << "\tcmpf(" << reg_name(first->reg) << ", " << reg_name(second->reg) << ");\n\tjge(L" << jlabel << ");\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n\tj(L" << exitlabel << ");\n";
				codeFile << "L" << jlabel << ":\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitlabel << ":\n";
			}
			else if(op == "AND"){
				int falseLabel = glabel++;
				int exitLabel = glabel++;
				codeFile << "\tcmpi(0, " << reg_name(first->reg) << ");\n\tje(L" << falseLabel << ");\n";
				codeFile << "\tcmpi(0, " << reg_name(second->reg) << ");\n\tje(L" << falseLabel << ");\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "j(L" << exitLabel << ");\n";
				codeFile << "L" << falseLabel << ":\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitLabel << ":\n";
			}
			else if(op == "AND_FLOAT"){
				int falseLabel = glabel++;
				int exitLabel = glabel++;
				codeFile << "\tcmpf(0, " << reg_name(first->reg) << ");\n\tje(L" << falseLabel << ");\n";
				codeFile << "\tcmpf(0, " << reg_name(second->reg) << ");\n\tje(L" << falseLabel << ");\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "j(L" << exitLabel << ");\n";
				codeFile << "L" << falseLabel << ":\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitLabel << ":\n";
			}
			else if(op == "OR"){
				int trueLabel = glabel++;
				int exitLabel = glabel++;
				codeFile << "\tcmpi(1, " << reg_name(first->reg) << ");\n\tje(L" << trueLabel << ");\n";
				codeFile << "\tcmpi(1, " << reg_name(second->reg) << ");\n\tje(L" << trueLabel << ");\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n";
				codeFile << "j(L" << exitLabel << ");\n";
				codeFile << "L" << trueLabel << ":\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitLabel << ":\n";
			}
			else if(op == "OR_FLOAT"){
				int trueLabel = glabel++;
				int exitLabel = glabel++;
				codeFile << "\tcmpf(1, " << reg_name(first->reg) << ");\n\tje(L" << trueLabel << ");\n";
				codeFile << "\tcmpf(1, " << reg_name(second->reg) << ");\n\tje(L" << trueLabel << ");\n";
				codeFile << "\tmove(0, " << reg_name(first->reg) << ");\n";
				codeFile << "j(L" << exitLabel << ");\n";
				codeFile << "L" << trueLabel << ":\n";
				codeFile << "\tmove(1, " << reg_name(first->reg) << ");\n";
				codeFile << "L" << exitLabel << ":\n";
			}

//			else if(op == "EQ_OP_FLOAT")
			else{
//				std::cout << "how is this case possible?\n" ;
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
			hasAssignment = first->hasAssignment || second->hasAssignment;
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
		virtual std::string actual_code();

		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		uopAST(std::string a, abstractAST *b){
			op = a;
			first = b;
			hasAssignment = first->hasAssignment;
			if(op == "PP") hasAssignment = true;
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
			hasAssignment = false;
			for(std::list<abstractAST*>::iterator iter = first.begin(); iter != first.end(); iter++){
				hasAssignment = (*iter)->hasAssignment;
				if(hasAssignment) break;
			}
		}
		virtual void generate_label(){
			this->label = 1;
//			std::cout << "label of func is " << label << std::endl;
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
			reg_type[reg] = 2;
			codeFile << "\tmove(" << getVal() << ", " << reg_name(reg) << ");\n";
			return "";
		}
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		floatAST(std::string a){
			hasAssignment = false;
			first = a;
		}
		virtual float getVal(){
			return std::stof(first);
		}
		virtual void generate_label(){
			label = 1;
//			std::cout << "label of float is " << label << std::endl;
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
			reg_type[reg] = 1;
			codeFile << "\tmove(" << getVal() << ", " << reg_name(reg) << ");\n";
			return "";
		}
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		intAST(std::string a){
			hasAssignment = false;
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
			hasAssignment = false;
			first = a;
		}
		std::string first;
	protected:
		virtual void setType(DataType) {};
	private:
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
				reg_type[reg] = 2;
			}
			else if(*((*currentTable)[first]->dataType) == DataType(DataType::Int)){
				codeFile << "\tloadi(ind(ebp, " << -(*currentTable)[first]->offset << "), " << reg_name(reg) << ");\n";
				reg_type[reg] = 1;
			}
			else{
				codeFile << "\tmove(" << -(*currentTable)[first]->offset << ", " << reg_name(reg) << ");\n";
				reg_type[reg] = 1;
			}
			return "";
		}
		virtual DataType getType() {};
		virtual bool checkTypeofAST() {};
		identifierAST(std::string a){
			hasAssignment = false;
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
			generate_label();
//			std::cout << "labels ready mein Fuhrer for indexAST\n";
//			std::cout.flush();
			actual_code();
			return "";
		};
		virtual std::string actual_code(){
			bool loadkarucopy = loadkaru;
			if(first->label > second->label){
				std::cout << "index cas1\n";
				std::cout.flush();
				loadkaru = false;
				first->actual_code();
		//		reset_regs();
				//avail_regs[first->reg] = false;
				if(second->label >= NO_REGS){
					int tfr = reg_type[first->reg];
					codeFile << "\tpushi(1);\n\tstorei(" << reg_name(first->reg) << ",ind(esp);\n";
					avail_regs[first->reg] = true;
					loadkaru = true;
					second->actual_code();
					first->reg = find_reg();
					reg_type[first->reg] = tfr;
					codeFile << "\tloadi((esp)," << first->reg << ");\n" << "\tpopi(1);\n";	
				}
				else{
					loadkaru = true;
					second->actual_code();
				}
				
			}
			else{
				std::cout << "index cas2\n";
				std::cout.flush();
				loadkaru = true;
				second->actual_code();
		//		reset_regs();
				//avail_regs[second->reg] = false;
				if(first->label >= NO_REGS){
					int tfr = reg_type[second->reg];
					codeFile << "\tpushi(1);\n\tstorei(" << reg_name(second->reg) << ",ind(esp);\n";
					avail_regs[second->reg] = true;
					loadkaru = false;
					first->actual_code();
					second->reg = find_reg();
					reg_type[second->reg] = tfr;
					codeFile << "\tloadi((esp)," << second->reg << ");\n" << "\tpopi(1);\n";	
				}
				else{
					loadkaru = false;
					first->actual_code();
				}
			}
	//}
			reg = first->reg;
			codeFile << "\tmuli(" << -((first->astType).arrayType)->size() << "," << reg_name(second->reg) << ");\n";
			codeFile << "\taddi(" << reg_name(second->reg)  << "," << reg_name(first->reg) << ");\n";
			avail_regs[second->reg] = true;
			if(loadkarucopy){
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
			hasAssignment = first->hasAssignment || second->hasAssignment;
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
extern DataType currentType;
extern DataType pastType;
extern DataType currentFuncType;
extern std::list<int> indexList;
extern int lineNo;
extern int returnCount;




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
