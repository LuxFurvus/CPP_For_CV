#pragma once
#ifndef _GAW_SAVELOAD_
#define _GAW_SAVELOAD_
#include "GaW_Includes.h"

void SaveProfile() {
    std::string filename = "Profiles/" + PlayerProfile;
    std::ofstream out(filename, std::ios::binary | std::ios::trunc);
    int numWords = static_cast<int>(WordList.size());
    out.write(reinterpret_cast<char*>(&numWords), sizeof(int));
    for (int i = 0; i < numWords; i++) {
        int len = static_cast<int>(WordList[i].length());
        out.write(reinterpret_cast<char*>(&len), sizeof(int));
        out.write(WordList[i].c_str(), len);
    }
    out.write(reinterpret_cast<char*>(&Score), sizeof(int));
    out.close();
}

////////////////////////////////////////////

bool LoadProfile() {
    std::string filename = "Profiles/" + PlayerProfile;
    std::ifstream in(filename, std::ios::binary);
    int numWords;
    in.read(reinterpret_cast<char*>(&numWords), sizeof(int));
    WordList.resize(numWords);
    for (int i = 0; i < numWords; i++) {
        int len;
        in.read(reinterpret_cast<char*>(&len), sizeof(int));
        WordList[i].resize(len);
        in.read(&WordList[i][0], len);
    }
    in.read(reinterpret_cast<char*>(&Score), sizeof(int));
    in.close();
    return true;
}
////////////////////////////////////////The function 'LoadProfile' must recieve nothing, but use global string variable 'PlayerProfile' as a file name.



#endif


