#pragma once
#ifndef _GAW_GAMEBODY_
#define _GAW_GAMEBODY_

#include "GaW_Includes.h"

void GameBody() {

NewWord:

    std::string WordToGuess = RandWord();
    std::cout << "\n+++ Here are " << WordList.size() << " secret words +++\n";

    std::cout << "<To get help,\t\tenter 0>\n"
        << "<To skip the word,\tenter 00.>\n"
        << "<To go to Main Menu,\tenter 000.>\n"
        << "Guess a noun! ->\n";

TryAgain:

    std::cout << WordToGuess.substr(0, HintsNumber);
    for (int i = 0; i < WordToGuess.length() - HintsNumber; i++) {
        std::cout << "*";
    }
    std::cout << '\n';

    std::string MyGuess;
    std::cin >> MyGuess;
    ToLowerCase(MyGuess);

    int MyInput;

    if (MyGuess == WordToGuess) { MyInput = 1; }
    else if (MyGuess == "0") { MyInput = 2; }
    else if (MyGuess == "00") { MyInput = 3; }
    else if (MyGuess == WordToGuess && WordList.size() <= 0) { MyInput = 4; }
    else if (MyGuess == "000") { MyInput = 5; }
    else { MyInput = 0; }

    switch (MyInput) {
    case 1:
        TriesNumber = 1U;
        WordList.erase(std::remove(begin(WordList), end(WordList), WordToGuess), end(WordList));
        Score += WordToGuess.length() - HintsNumber;
        std::cout << "\nThat's right!\nYou get " << WordToGuess.length() - HintsNumber << " points!\n"
            << "Your TOTAL SCORE: " << Score << " (out of 412)!\n";
        HintsNumber = 1U;
        TriesNumber = 1U;
        SaveProfile();
        goto NewWord;

    case 2:
        HintsNumber++;
        if (WordToGuess.length() - HintsNumber != 0) { goto TryAgain; }
    case 3:
        std::cout << "\nIt was \"" << WordToGuess << "\"!\n";
        std::cout << "Oh, dear! Is it that hard!?\nDon't worry, let's try something else...\n\n";
        WordToGuess = RandWord();
        HintsNumber = 1U;
        TriesNumber = 1U;
        goto TryAgain;

    case 4:
        std::cout << "\n\nWOW!!! You've done all words!\n\nYour TOTAL SCORE: " << Score;
        system("pause");
        return;

    case 5:
        return;

    default:
        TriesNumber++;
        std::cout << "\nTry again! It's your [" << TriesNumber << "] time.\n\n";
        goto TryAgain;

    }

}


#endif