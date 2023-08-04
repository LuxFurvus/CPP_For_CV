# include<iostream>
# include<fstream>
# include<string>
#include <chrono>

const std::string FileName = "IO.txt";

int NearestPoint(const std::string TextToProcess) {//Finds a position of the nearest sentence end
	constexpr int PointVarNum = 4;
	int point[PointVarNum];
	point[0] = TextToProcess.find(". ");
	point[1] = TextToProcess.find("! ");
	point[2] = TextToProcess.find("? ");
	point[3] = TextToProcess.find("… ");

	for (int i = 0; i < PointVarNum; i++) {
		point[i] = (point[i] == -1) ? INT_MAX : point[i];
	}

	int min = INT_MAX;
	for (int i = -1; i <= PointVarNum - 2; i++) {
		if (i == -1) point[i] = INT_MAX;
		int pre_min = (point[i] < point[i + 1]) ? point[i] : point[i + 1];
		min = (pre_min < min) ? pre_min : min;
	}
	return min;
}

std::string NoDoubleSpace(const std::string& TextToProcess) {//Removes double whitespaces in the text
	std::string OldStr = TextToProcess;
	std::string NewStr;

	size_t Begin = 0;
	size_t Length = OldStr.length();

	while (Begin < Length) {
		size_t End = OldStr.find_first_of(' ', Begin) + 1;
		NewStr += OldStr.substr(Begin, End - Begin);
		Begin = OldStr.find_first_not_of(' ', End);
	}
	return NewStr;
}

bool FirstTime = 0;
void AppIsWorking() {
	if (FirstTime == 0) {
		std::cout 
			<< "\n==============\nThe program is working.\n\nPut a PDF-crippled text in " 
			<< FileName 
			<< ", reopen it and retrieve a converted text...\n==============\n";
		FirstTime = 1;
	}
}

int main() {
HandleTheText:

	//Get the text from file or create the file if there is no one
	std::ifstream GetLines(FileName);
	if (!GetLines) {
		std::ofstream CreateFile(FileName, std::ios::app);
		std::cout << "File \"" << FileName << "\" not found, so it was recreated.\nPlease, refill it!\n";
		system("pause");
		return 0;
	}

	//Convert the initial text into a one line std::string
	std::string TextToProcess;
	while (GetLines.good()) {
		std::string tempstr;
		getline(GetLines, tempstr);
		TextToProcess += tempstr + " ";
	}

	//Remove double whitespaces in the text
	TextToProcess = NoDoubleSpace(TextToProcess);

	//Cut the text in sentences-paragraphs
	bool FirstRound = 1;
	while (1) {
		std::string ToPrint = TextToProcess;
		int NearSentenceEnd = NearestPoint(TextToProcess);

		if (NearSentenceEnd == INT_MAX) break;

		ToPrint.erase(NearSentenceEnd + 1, TextToProcess.length());

		TextToProcess = TextToProcess.substr(NearSentenceEnd + 2);

		std::ofstream TxtOut(FileName, std::ios::app);
		if (FirstRound) std::ofstream TxtOut(FileName, std::ios::trunc);
		else std::ofstream TxtOut(FileName, std::ios::app);

		TxtOut << ToPrint << "\n";
		FirstRound = 0;
	}

	GetLines.close();

	// Rerun the app
	AppIsWorking();
	std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	goto HandleTheText;

	return 0;
}