#pragma once
#ifndef _GAW_GLOBAL_
#define _GAW_GLOBAL_

#include "GaW_WordList.h"

unsigned int TriesNumber = 1;
unsigned int HintsNumber = 1;

std::string PlayerProfile;
unsigned int Score = 0;


std::string RandWord() {
    srand(time(0));
    int w = rand() % WordList.size();
    std::string oneword = WordList[w];
    return oneword;
}

void ToLowerCase(std::string& s) {
    std::transform(s.begin(), s.end(), s.begin(), ::tolower);
}

int StrToInt(const std::string& str) {
    return std::atoi(str.c_str());
}


#endif