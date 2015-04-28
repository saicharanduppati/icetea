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
std::ofstream codeFile;
int glabel = 0;
bool avail_regs[NO_REGS];
int reg_type[NO_REGS];
int lineNo = 1;
std::string suffixString = "";
bool loadkaru;

bool  pairComparator(std::pair<int, DataType> a, std::pair<int, DataType> b){
	return a.first < b.first;
}


void backpatch(std::vector<int*> v, int label){
	for(int i= 0 ; i < v.size() ; i++){
		(*v[i]) = label;
	}
}

void merge(std::vector<int* > v1, std::vector<int* > v2){
	v1.insert(v1.end(),v2.begin(),v2.end());
} 
std::string reg_name(int reg){
	switch(reg)
	{
		case(0):
			return "eax";
			break;
		case(1):
			return "ebx";
			break;
		case(2):
			return "ecx";
			break;
		case(3):
			return "edx";
			break;
		case(4):
			return "esi";
			break;
		case(5):
			return "edi";
			break;
	}
}

void reset_regs(){
	for(int i = 0; i< NO_REGS;i++){
		avail_regs[i] = true;
	}
}

int find_reg(){
	for(int i = 0; i < NO_REGS ; i++){
		if(avail_regs[i]){
			avail_regs[i] = false;
			return i;
		}
	}
//	std::cout << "NO REGISTERS AVAILABLE SCREEEEEEEEEEEEECCCCHHHHHH" << std::endl;
	exit(-1);
	return -1;
}
void label_manager(abstractAST* p, abstractAST* c1, abstractAST* c2){
	c1->generate_label();
	c2->generate_label();
//	if(c1->label == 0 && c2->label == 0){
//		c2->label = 1;
//	}
	if(c1->label == c2->label){
		p->label = c1->label + 1;
	}
	else{
		p->label = std::max(c1->label,c2->label);
	}
}
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
//		std::cout << "gone" << std::endl;
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
	std::cout << "])";
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


int DataType::len(){
	if(tag == Error || tag == Ok){
		std::cout << "wrong usage of length" << std::endl;
		exit(1);
	}
	else if(tag == Base){
		return 0;
	}
	else{
		return 1 + arrayType->len();
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






void bopAST::generate_label(){
	label_manager(this, first,second);
//	std::cout << "bop label is " << this->label << std::endl;
}
void indexAST::generate_label(){
	label_manager(this, first,second);
//	std::cout << "index label is " << this->label << std::endl;
}
void assAST::generate_label(){
	label_manager(this,first,second);
//	std::cout << "ass label is " <<  this->label << std::endl;
}


std::string funcAST::actual_code(){
	loadkaru = true;
	if(name == "printf"){
		for(std::list<abstractAST*>::iterator it = first.begin(); it != first.end(); it++){
			stringAST *a = dynamic_cast<stringAST*>(*it);
			if(a == NULL){
				loadkaru = true;
				(*it)->generate_code();
				if((*it)->astType == DataType(DataType::Int)){
					codeFile << "\tprint_int(" << reg_name((*it)->reg) << ");\n";
				}
				if((*it)->astType == DataType(DataType::Float)){
					codeFile << "\tprint_float(" << reg_name((*it)->reg) << ");\n";
				}
			}
			else{
				codeFile << "\tprint_string(" << a->first << ");\n";
			}
		}
		return "";
	}
	bool reg_status_copy[NO_REGS];
	bool reg_type_copy[NO_REGS];
	int count = 0;
	for(int i = 0;i<NO_REGS;i++){
		reg_status_copy[i] = avail_regs[i];
		reg_type_copy[i] = reg_type[i];
		if(!avail_regs[i]){
			count++;
			if(reg_type[i] == 1){
				codeFile << "\tpushi(" << reg_name(i) << ");\n";
			}
			else if(reg_type[i] == 2){
				codeFile << "\tpushf(" << reg_name(i) << ");\n";
			}
		}
	}
	if(*((*globalTable)[name]->dataType) == DataType(DataType::Float)){
		codeFile << "\tpushf(0.0);\n";
	}
	else if(*((*globalTable)[name]->dataType) == DataType(DataType::Int)){
		codeFile << "\tpushi(0);\n";
	}
	int space = 0;
	for(std::list <abstractAST*>::iterator it = first.begin(); it != first.end(); it++){
		space += (*it)->astType.size();
	}
	codeFile << "\taddi(" << -space << ", esp);\n";
	for(std::list <abstractAST*>::iterator it = first.begin();it != first.end();it++){
		intAST* a = dynamic_cast<intAST*>(*it);
		if(a != NULL){
//			codeFile << "\tpushi(" << a->getVal() << ");\n";
			codeFile << "\tstorei(" << a->first << ", ind(esp));\n";
			codeFile << "\taddi(" << a->astType.size() << ", esp);\n";
			continue;
		}	
		floatAST* b = dynamic_cast<floatAST*>(*it);
		if(b != NULL){
			codeFile << "\tstoref(" << b->first << ", ind(esp));\n";
			codeFile << "\taddi(" << b->astType.size() << ", esp);\n";
			//codeFile << "\tpushf(" << b->getVal() << ");\n";
			continue;
		}	
		loadkaru = true;
		(*it)->actual_code();
		if((*it)->astType == DataType(DataType::Float)){
//			codeFile << "\tpushf(" << reg_name((*it)->reg) << ");\n";
			codeFile << "\tstoref(" << reg_name((*it)->reg) << ", ind(esp));\n";
			codeFile << "\taddi(" << (*it)->astType.size() << ", esp);\n";
		}
		else if((*it)->astType == DataType(DataType::Int)){
			//codeFile << "\tpushi(" << reg_name((*it)->reg) << ");\n";
			codeFile << "\tstorei(" << reg_name((*it)->reg) << ", ind(esp));\n";
			codeFile << "\taddi(" << (*it)->astType.size() << ", esp);\n";
		}
		reset_regs();
	}
	codeFile << "\taddi(" << -space << ", esp);\n";
	codeFile << "\t" << name.substr(0,name.find_first_of("#")) << "();\n";
	std::string params = name.substr(name.find_first_of("#"));
	for(int i=params.size()-1;i > 0 ; i--){ //this is just parameter popping.
		codeFile << "\tpop" << params[i] << "(1);\n";
		/*TODO
			grouping of parameter to pop simulataneously*/
	}
	for(int i = NO_REGS-1;i>=0;i--){
		avail_regs[i] = reg_status_copy[i]; 
		reg_type[i] = reg_type_copy[i];
	}	
	reg = find_reg();
	if(*((*globalTable)[name]->dataType) == DataType(DataType::Float)){
		codeFile << "\tloadf(ind(esp), " << reg_name(reg) << ");\n";
		codeFile << "\tpopf(1);\n";
	}
	else if(*((*globalTable)[name]->dataType) == DataType(DataType::Int)){
		codeFile << "\tloadi(ind(esp), " << reg_name(reg) << ");\n";
		codeFile << "\tpopi(1);\n";
	}
	else{
		std::cout << "this case not possible in function AST\n";
		std::cout.flush();
	}
	for(int i = NO_REGS-1;i>=0;i--){ 
		if(!avail_regs[i] && !(reg == i)){
			if(reg_type[i] == 1){
				codeFile << "\tloadi(ind(esp), " << reg_name(i) << ");\n\tpopi(1);\n";
			}
			else if(reg_type[i] == 2){
				codeFile << "\tloadf(ind(esp), " << reg_name(i) << ");\n\tpopf(1);\n";
			}
		}
	}
	return "";
}	





std::string funcStmtAST::actual_code(){
	loadkaru = true;
	if(name == "printf"){
		for(std::list<abstractAST*>::iterator it = first.begin(); it != first.end(); it++){
			stringAST *a = dynamic_cast<stringAST*>(*it);
			if(a == NULL){
				(*it)->generate_code();
				if((*it)->astType == DataType(DataType::Int)){
					codeFile << "\tprint_int(" << reg_name((*it)->reg) << ");\n";
				}
				if((*it)->astType == DataType(DataType::Float)){
					codeFile << "\tprint_float(" << reg_name((*it)->reg) << ");\n";
				}
			}
			else{
				codeFile << "\tprint_string(" << a->first << ");\n";
			}
		}
		return "";
	}
	if(*((*globalTable)[name]->dataType) == DataType(DataType::Float)){
		codeFile << "\tpushf(0.0);\n";
	}
	else if(*((*globalTable)[name]->dataType) == DataType(DataType::Int)){
		codeFile << "\tpushi(0);\n";
	}
	int space = 0;
	for(std::list <abstractAST*>::iterator it = first.begin(); it != first.end(); it++){
		space += (*it)->astType.size();
	}
	codeFile << "\taddi(" << -space << ", esp);\n";
	for(std::list <abstractAST*>::iterator it = first.begin();it != first.end();it++){
		intAST* a = dynamic_cast<intAST*>(*it);
		if(a != NULL){
//			codeFile << "\tpushi(" << a->getVal() << ");\n";
			codeFile << "\tstorei(" << a->first << ", ind(esp));\n";
			codeFile << "\taddi(" << a->astType.size() << ", esp);\n";
			continue;
		}	
		floatAST* b = dynamic_cast<floatAST*>(*it);
		if(b != NULL){
			codeFile << "\tstoref(" << b->first << ", ind(esp));\n";
			codeFile << "\taddi(" << b->astType.size() << ", esp);\n";
			//codeFile << "\tpushf(" << b->getVal() << ");\n";
			continue;
		}	
		loadkaru = true;
		(*it)->generate_code();
		if((*it)->astType == DataType(DataType::Float)){
//			codeFile << "\tpushf(" << reg_name((*it)->reg) << ");\n";
			codeFile << "\tstoref(" << reg_name((*it)->reg) << ", ind(esp));\n";
			codeFile << "\taddi(" << (*it)->astType.size() << ", esp);\n";
		}
		else if((*it)->astType == DataType(DataType::Int)){
			//codeFile << "\tpushi(" << reg_name((*it)->reg) << ");\n";
			codeFile << "\tstorei(" << reg_name((*it)->reg) << ", ind(esp));\n";
			codeFile << "\taddi(" << (*it)->astType.size() << ", esp);\n";
		}
		reset_regs();
	}
	codeFile << "\taddi(" << -space << ", esp);\n";
	codeFile << "\t" << name.substr(0,name.find_first_of("#")) << "();\n";
	std::string params = name.substr(name.find_first_of("#"));
	for(int i=params.size()-1;i > 0 ; i--){ //this is just parameter popping.
		codeFile << "\tpop" << params[i] << "(1);\n";
		/*TODO
			grouping of parameter to pop simulataneously*/
	}
	reg = find_reg();
	if(*((*globalTable)[name]->dataType) == DataType(DataType::Float)){
		codeFile << "\tloadf(ind(esp), " << reg_name(reg) << ");\n";
		codeFile << "\tpopf(1);\n";
	}
	else if(*((*globalTable)[name]->dataType) == DataType(DataType::Int)){
		codeFile << "\tloadi(ind(esp), " << reg_name(reg) << ");\n";
		codeFile << "\tpopi(1);\n";
	}
	else{
		std::cout << "this case not possible in function AST\n";
		std::cout.flush();
	}
	return "";
}	




void bopGenCodeHelper(abstractAST* first, abstractAST* second){
	loadkaru = true;
	first->actual_code();
	int ftr = reg_type[first->reg];
	if((first->label >= NO_REGS) || (second->label >= NO_REGS)){
		if(first->astType == DataType(DataType::Float)){
			codeFile << "\tpushf(1);\n\tstoref(" << reg_name(first->reg) << ", ind(esp));\n";
		}	
		if(first->astType == DataType(DataType::Int)){
			codeFile << "\tpushi(1);\n\tstorei(" << reg_name(first->reg) << ", ind(esp));\n";
		}
		avail_regs[first->reg] = true;
	}
	//reset_regs();
	//	avail_regs[first->reg] = false;
	loadkaru = true;
	second->actual_code();	
	if((first->label >= NO_REGS) || (second->label >= NO_REGS)){
		first->reg = find_reg();
		reg_type[first->reg] = ftr;
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













std::string uopAST::actual_code(){
	loadkaru = true;
	if(op == "PP" || op == "PP_FLOAT"){
		indexAST* temp = dynamic_cast<indexAST*>(first);
		if(temp == NULL){
			loadkaru = true;
			first->actual_code();
			reg = first->reg;
			if(first->astType == DataType(DataType::Int)){
				codeFile << "\taddi(1,"	<< reg_name(reg) << ");\n";
				codeFile << "\tstorei(" << reg_name(reg) << ",ind(ebp, " << -(*currentTable)[first->get_name()]->offset << "));\n";
			}
			else if(first->astType == DataType(DataType::Float)){
				codeFile << "\taddf(1," << reg_name(reg) << ");\n";
				codeFile << "\tstoref(" << reg_name(reg) << ",ind(ebp, " << -(*currentTable)[first->get_name()]->offset << "));\n";
			}
		}
		else{
			loadkaru = false;
			first->actual_code();
			reg = first->reg;
//			codeFile << "\tmuli(-1, " << reg_name(reg) << ");\n";
			codeFile << "\taddi(ebp, " << reg_name(reg) << ");\n";
			if(first->astType == DataType(DataType::Int)){
				int temp_reg = find_reg();
				codeFile << "\tloadi(ind(" << reg_name(reg) << ")," << reg_name(temp_reg) <<");\n";
				codeFile << "\taddi(1, " << reg_name(temp_reg) << ");\n";
				codeFile << "\tstorei(" << reg_name(temp_reg) << ", ind(" << reg_name(reg) << "));\n";
				codeFile << "\tmove(" << reg_name(temp_reg) << ", " << reg_name(reg) << ");\n";
				avail_regs[temp_reg] = true;
			}
			if(first->astType == DataType(DataType::Float)){
				int temp_reg = find_reg();
				codeFile << "\tloadf(ind(" << reg_name(reg) << ")," << reg_name(temp_reg) <<");\n";
				codeFile << "\taddf(1, " << reg_name(temp_reg) << ");\n";
				codeFile << "\tstoref(" << reg_name(temp_reg) << ", ind(" << reg_name(reg) << "));\n";
				codeFile << "\tmove(" << reg_name(temp_reg) << ", " << reg_name(reg) << ");\n";
				avail_regs[temp_reg] = true;
			}
		}
	}

	else if(op == "UMINUS"){
			first->actual_code();
			reg = first->reg;

		if(first->astType == DataType(DataType::Int)){
			codeFile << "\tmuli(-1," << reg_name(reg) << ");\n";
//					codeFile << "\tstorei(" << reg_name(reg) << ",ind(ebp, " << -(*currentTable)[first->get_name()]->offset << "));\n";
		}
		else{
			codeFile << "\tmulf(-1," << reg_name(reg) << ");\n";
//					codeFile << "\tstoref(" << reg_name(reg) << ",ind(ebp, " << -(*currentTable)[first->get_name()]->offset << "));\n";
		}
	}	
	else if(op == "NOT"){
			first->actual_code();
			reg = first->reg;
		if(first->astType == DataType(DataType::Int)){
			codeFile << "\tcmpi(0, " << reg_name(reg) << ");\n";
		}
		else if(first->astType == DataType(DataType::Float)){
			codeFile << "\tcmpf(0, " << reg_name(reg) << ");\n";
		}
		int zeroLabel = glabel++;
		codeFile << "\tje(L" << zeroLabel << ");\n";
		int exitLabel = glabel++;
		codeFile << "\tmove(0, " << reg_name(reg) << ");\n\tj(L" << exitLabel << ");\n";
		codeFile << "L" << zeroLabel << ":\n";
		codeFile << "\tmove(1, " << reg_name(reg) << ");\n";
		codeFile << "L" << exitLabel << ":\n";
	}
	return "";
	//TODO : what if the op is not
}
