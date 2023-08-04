#pragma once
#ifndef _GAW_MAINMENU_
#define _GAW_MAINMENU_
#include "GaW_Includes.h"


bool CreateProfile() {
    std::cout << "Enter your profile name:\n"
        << "<To go to Main Menu,\tenter 000.>\n";
    std::string name;
    std::cin >> name;

    if (name == "000")    {
        return false;
    }    

    std::filesystem::create_directory("Profiles");

    std::string file_name = "Profiles/" + name + ".bin";
    std::ofstream file(file_name, std::ios::binary | std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Failed to create: " << file_name << '\n';
        return false;
    }

    PlayerProfile = file_name;
    WordList = ConstWordList;
    Score = 0;
    return true;
}

////////////////////////////////

bool SelectProfileToLoad() {

    std::vector<std::string> files;

    std::filesystem::path p("Profiles");
    std::filesystem::directory_iterator end_itr;

    if (std::filesystem::exists(p) && std::filesystem::is_directory(p)) {
        // Iterate over the directory and display the names of the files
        for (const auto& entry : std::filesystem::directory_iterator("Profiles")) {
            std::string file_name = entry.path().filename().string();
            if (file_name.length() >= 4 && file_name.substr(file_name.length() - 4) == ".bin") {
                files.push_back(file_name);
            }
        }
    }
    else {
        std::cout << "The directory " << p << " not found.\n";
        return false;
    }


    if (files.empty()) {
        std::cerr << "No profiles found!\n";
        return false;
    }

    std::cout << "Select a profile to load:\n";
    for (int i = 0; i < files.size(); ++i) {
        std::cout << i + 1 << ": " << files[i] << '\n';
    }

    int file_index;
    std::string FileOrQuit;
    while (true) {
        std::cout << "\nEnter a number of a profile:\n< To return to Main Menu, enter 000 >\n";
        std::cin >> FileOrQuit;
        if (FileOrQuit == "000") {
            return false;
        }
        file_index = StrToInt(FileOrQuit);
        if (file_index > 0 && file_index <= files.size()) {
            PlayerProfile = files[file_index - 1];
            return true;
        }
        else std::cout << "Invalid input. Try again.\n";
    }
}


//////////////////////////////

void MainMenu() {

BackToMain:
    std::cout << "\n+++WELCOME TO < GUESS A WORD > game! +++\n\n";
MMTryAgain:
    std::cout << "MAIN MENU\n\n"
        << "1. New game\n"
        << "2. Load game\n"
        << "3. Quit\n";

    std::cout << "\nEnter your choice:\n";

    std::string choice;
    std::cin >> choice;

    if (choice == "1" || choice == "0") {
        bool CanIPlay = CreateProfile();
        if (CanIPlay) GameBody();
        goto MMTryAgain;
    }
    else if (choice == "2" || choice == "00") {
        bool IsProfileSelected = SelectProfileToLoad();
        bool CanIPlay = LoadProfile();
        
        if (IsProfileSelected && CanIPlay) {
            GameBody();
            goto BackToMain;
        }
        goto MMTryAgain;
    }
    else if (choice == "3" || choice == "000") { return; }

    else {
        std::cout << "Invalid choice. Try again.\n";
        goto MMTryAgain;
    };
}


#endif