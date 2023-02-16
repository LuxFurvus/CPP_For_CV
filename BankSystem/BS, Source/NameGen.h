#pragma once
#ifndef _NAMEGEN_H_
#define _NAMEGEN_H_

#include <string>

namespace { // hidden part of Name Generator
	int VowNum = 0;
	int ConsNum = 0;
	bool VorC = static_cast<bool>(rand() % 2);

	char Consonant(bool small = 1) {
		char out = rand() % 26 + 65;
		switch (out) {
		case 'A': 	out += 1;
			break;
		case 'U':	out += 1;
			break;
		case 'E':	out += 1;
			break;
		case 'O':	out += 1;
			break;
		case 'I':	out += 1;
		}
		out = (small) ? out + 32 : out;
		return out;
	}

	char Vowel(bool small = 1) {
		char out;
		int WhichVowel = rand() % 5 + 1;
		switch (WhichVowel) {
		case 1: out = 'A';
			break;
		case 2:	out = 'U';
			break;
		case 3:	out = 'E';
			break;
		case 4:	out = 'O';
			break;
		case 5:	out = 'I';
		}
		out = (small) ? out + 32 : out;
		return out;
	}

	/////Each syllable has 1 vowel and 1 consonant
	char VowOrCons(bool small = 1) {
		char out = (VorC) ?
			Vowel(small) :
			Consonant(small);
		VorC = (VorC) ? 0 : 1;
		out = (out == 'Q') ? 'R' : out;
		out = (out == 'q') ? 'r' : out;
		return out;
	}
}

namespace NameGen {
	typedef unsigned int uint;

	uint RandNum(uint min = 1U, uint max = 101U) {
		return (rand() % (max - min + 1)) + min;
	}


	std::string RandName(uint MinLetters = 3U, uint MaxLetters = 6U) {
		uint LetterNum = RandNum(MinLetters, MaxLetters);
		std::string NewName;
		for (uint i = 0; i < LetterNum; i++)
		{
			NewName = (i == 0) ? NewName += VowOrCons(0) : NewName += VowOrCons();
		}
		VorC = static_cast<bool>(rand() % 2);
		return NewName;
	}
}






#endif