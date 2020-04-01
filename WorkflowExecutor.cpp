#include"workflow.h"

using namespace std;
int main() {

	setlocale(LC_ALL, "rus");
	std::map<std::string, IWorker*> inst; // Workers storage

	inst["read"] = new Read;
	inst["write"] = new Write;
	inst["grep"] = new Grep;
	inst["sort"] = new Sort;
	inst["replace"] = new Replace;
	inst["dump"] = new Dump;
	IWorker* worker = nullptr;
	size_t start_time = clock();
	try {
		size_t start_time = clock();
		map<size_t, pair<IWorker*, vector<string>>> programm; // num-> Worker, agrs
		Parser parser("instructions.txt"); 
		char endOfBlock = '\0';
		std::map<size_t, std::string> toValid;

		while (!parser.endOfInstruction()) // TRUE if END OF INSTRUCTION
		{
			std::string C = parser.getNextComand();
			if (C[0] != endOfBlock) {
				size_t num = parser.getComandNum(C);
				std::string com = parser.getComandName(C);
				std::vector<std::string> arg = parser.getArgs(C);
				worker = nullptr;
				worker = inst.at(com);
				try {
					programm.at(num);
					MyError Equalnums;	//exeption of equals num
					throw Equalnums;	//out of range
				}
				catch (MyError const& e) {
					throw e;
				}
				catch (std::out_of_range const&) {
					programm[num] = pair<IWorker*, vector<string>>(worker, arg);
				}
				toValid[num] = com;		//check that the first is read, end is write
			}
		}

		Validator check(toValid, parser);
		check.correctBlocks();

		size_t sizeOfNumsOfProg = parser.getsize();

		for (size_t i = 0; i < sizeOfNumsOfProg; i++) {
			size_t com_num = parser.getNum(i);
			pair<IWorker*, vector<string>>& s = programm[com_num];
			s.first->toDo(s.second);
		}
		size_t end_time = clock();
		std::cout << "Successful. it takes " << end_time - start_time << " ms\n";
	}
	catch (std::exception const& Er) {
		std::cout << Er.what() << "\n";
	}
	catch (...) {
		std::cout << "unknown error\n";
	}
	delete inst["read"];
	delete inst["write"];
	delete inst["grep"];
	delete inst["sort"];
	delete inst["replace"];
	delete inst["dump"];

	return 0;
}

