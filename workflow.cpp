#include "workflow.h"

Parser::Parser(const std::string FileName) {
	workFile.open(FileName, std::ios_base::in);	//open file for reading
	if (!workFile.is_open()) {
		throw std::invalid_argument("File \"" + FileName + "\" can not be open.");
	}
	numbers = nullptr;
}

bool Parser::endOfInstruction() {
	return workFile.eof();
}

std::string Parser::getNextComand() {
	std::string s;

	workFile >> s;

	if (s == "csed") {
		std::string numberString = this->getNextComand();
		size_t j = 0;
		size_t NotANumber = 0;
		size_t i = 0;
		this->sizeOfNumbers = (numberString.size() / 2) + 1;
		numbers = new size_t[this->sizeOfNumbers];

		while (!numberString.empty()) {
			while (!isdigit(numberString[i])) {
				i++;
			}
			if (i != 0) {
				numberString.erase(0, i);
				i = 0;
			}

			if (isdigit(numberString[0])) {
				this->numbers[j] = static_cast<size_t>(stoi(numberString, &NotANumber));
				numberString.erase(0, NotANumber);
				j++;
			}

		}

		size_t* tmp = new size_t[j];
		for (size_t g = 0; g < j; g++)
			tmp[g] = this->numbers[g];
		this->sizeOfNumbers = j;
		delete[]numbers;
		numbers = tmp;
		s = "\0";
	}
	else if (s == "desc") {
		s = this->getNextComand();
	}
	else {
		char tempChar = 0;

		workFile.get(tempChar);
		for (int i = 0; (tempChar != '\n') && !workFile.eof(); i++) {
			s += tempChar;
			workFile.get(tempChar);
		}
	}
	return s;
}

size_t Parser::getComandNum(std::string& str) {
	size_t num = 0;
	size_t tmp = static_cast<size_t>(stoi(str, &num));
	std::string tmpStr = str.substr(num);				//substr returned substring 
	tmpStr.swap(str);

	size_t i = 0;
	while (str[i] != '=') {
		if (str[i] != ' ') {
			throw std::invalid_argument("Uncorrect symbols after a command's number.");
		}
		i++;
	}
	i++;
	tmpStr = str.substr(i);				//substr returned substring 
	tmpStr.swap(str);
	return tmp;
}

std::string Parser::getComandName(std::string& str) {
	size_t i = 0;
	while (str[i] == ' ') {
		i++;
	}
	std::string tmp = str.substr(i);
	size_t j = 0;
	for (j = 0; (tmp[j] != ' ') && (j < tmp.size()); j++) {}
	tmp.erase(j);
	str = str.substr(i + j);
	j = 0;
	while (str[j] == ' ') {
		if (j == str.size()) {
			str.clear();
			j = 0;
			break;
		}
		j++;
	}

	str = str.substr(j);
	return tmp;
}

std::vector<std::string> Parser::getArgs(std::string& str) {
	std::string tmp;				//doesnt work if between args 2 or more spaces
	std::vector<std::string> Args;
	size_t j = 0;
	std::string::size_type q = 0;

	while (j < 2) {
		q = str.find(' ');
		tmp = str.substr(0, q);
		Args.push_back(tmp);
		j++;
		if (q == std::string::npos) {
			str.clear();
			if (j == 0) {
				Args.clear();
			}
			break;
		}
		str = str.substr(q + 1);
	}
	return Args;
}

Parser::~Parser() {
	if (numbers != nullptr)
		delete[] numbers;
	workFile.close();
}

const size_t Parser::getNum(size_t ind) const {
	return numbers[ind];
}
const size_t Parser::getsize() const {
	return sizeOfNumbers;
}

std::string* IWorker::resorce = nullptr;
void IWorker::initData()
{
	delete resorce;
	resorce = new std::string;
}
Read::~Read() {
	delete[]this->resorce;
}
Read::Read() {
	initData();
}

void Read::toDo(std::vector<std::string> V) {
	std::ifstream inputFile(V[0]);
	if (!inputFile.is_open()) {
		throw std::runtime_error("Input file \"" + V[0] + "\" is unavailable.\n");
	}
	std::string some;
	std::string someTmp;
	size_t i = 0;
	inputFile >> some;				//formate strinf, if " " delete them
	char tmpC = inputFile.peek();
	while (!inputFile.eof()) {
		someTmp += (some + tmpC);
		inputFile >> some;
		tmpC = inputFile.peek();
	}
	*resorce = someTmp;
	inputFile.close();
}

void Write::toDo(std::vector<std::string> V) {
	std::ofstream OutFile(V[0]);
	if (OutFile.is_open()) {
		OutFile << *resorce;
		OutFile.close();
		initData();
	}
	else {
		throw std::invalid_argument("\nOutPut File " + V[0] + " Not Found\n");
	}

}

void Dump::toDo(std::vector<std::string> V) {
	std::ofstream OutFile(V[0]);
	if (OutFile.is_open()) {
		OutFile << *resorce;
		OutFile.close();
	}
	else
		throw std::invalid_argument("\nDump-File " + V[0] + " Not Found\n");
}

void Replace::toDo(std::vector<std::string> V) {
	size_t position_in_str = 0;
	size_t position_next_start = 0;

	while (position_next_start < std::string::npos) {
		position_in_str = resorce->find(V[0], position_next_start);
		if (position_in_str == std::string::npos) {
			break;
		}
		position_next_start = position_in_str + V[0].size();
		char tmp = (*resorce)[position_next_start];
		if (tmp == ' ' || tmp == '\n' || tmp == '\0')
			resorce->replace(position_in_str, V[0].size(), V[1]);
	}
}

void Grep::toDo(std::vector<std::string> V) {
	std::vector<std::string> s;
	size_t i = 0;
	std::cout << "grep\n";			// << *resorce << "\n";
	size_t q = 0;
	while (q != std::string::npos) {
		q = resorce->find('\n', i);
		s.push_back(resorce->substr(i, q - i) + "\n");
		if (q == std::string::npos) {
			break;
		}
		++q;
		i = q;
	}
	std::string tmpStr;

	size_t tmp = s.size();
	for (i = 0; i < tmp; i++) {
		size_t position_in_str = 0;
		size_t position_next_start = 0;
		while (position_next_start < std::string::npos) {
			position_in_str = resorce->find(V[0], position_next_start);
			if (position_in_str == std::string::npos) {
				break;
			}
			position_next_start = position_in_str + V[0].size();
			char tmp = (*resorce)[position_next_start];
			if (tmp == ' ' || tmp == '\n' || tmp == '\0') {
			}
		}
	}

}

void Sort::toDo(std::vector<std::string> V) {
	std::vector<std::string> s;
	size_t i = 0;
	size_t q = 0;
	while (q != std::string::npos) {
		q = resorce->find('\n', i);
		s.push_back(resorce->substr(i, q - i) + "\n");
		if (q == std::string::npos) {
			break;
		}
		++q;
		i = q;
	}
	std::string tmpStr;
	size_t tmp = s.size();

	std::sort(s.begin(), s.end());
	for (i = 0; i < tmp; i++) {
		tmpStr += s[i];
	}
	*resorce = tmpStr;
}

Validator::Validator(std::map<size_t, std::string>& other, const Parser& otherP) : check(other), p(otherP) {}
void Validator::correctBlocks() {
	size_t tmpSize = p.getsize();

	if ((check[p.getNum(0)] != "read") || (check[p.getNum(p.getsize() - 1)] != "write")) {			//out of range
		throw std::invalid_argument("Error of read - > write");			//not runtime because we are throwing it before program will do smth
	}
	size_t i = 0;
	while (i < tmpSize) {
		try {
			check.at(p.getNum(i));
		}
		catch (std::out_of_range) {
			throw std::runtime_error("Error: Unknown numbers of commands");
		}
		i++;
	}
}

const char* MyError::what() const {
	return "Equal numers of commands\n";
}