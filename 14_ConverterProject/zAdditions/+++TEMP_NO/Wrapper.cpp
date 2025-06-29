
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
////////
// #include "TextAdapter.h"
// #include "RemoveSubstrings.h"

// Function to validate command-line arguments and extract the file path
bool ValidateArguments(int argc, char* argv[], std::string& filepath)
{
    if (argc != 3 || std::string(argv[1]) != "-in")
    {
        std::cerr << "Usage: program -in <filepath>\n";
        return false;
    }
    filepath = argv[2];
    return true;
}

// Function to read the entire content of the file
bool ReadFileContent(const std::string& filepath, std::string& content)
{
    std::ifstream file(filepath, std::ios::in);
    if (!file)
    {
        std::cerr << "Error: Cannot open file for reading: " << filepath << '\n';
        return false;
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    content = buffer.str();
    file.close();
    return true;
}

// Function to overwrite the file with the new content
bool OverwriteFileContent(const std::string& filepath, const std::string& content)
{
    std::ofstream file(filepath, std::ios::out | std::ios::trunc);
    if (!file)
    {
        std::cerr << "Error: Cannot open file for writing: " << filepath << '\n';
        return false;
    }

    file << content;
    file.close();
    return true;
}

int main(int argc, char* argv[])
{
    std::string filepath;
    assert(ValidateArguments(argc, argv, filepath));

    std::string text;
    assert(ReadFileContent(filepath, text));

    // Processing pipeline
    
    // TextAdapter TextAdapterObj;
    // TextAdapterObj.AdaptText(text, false);

    // BlueprintLogicExtractor Extractor;
    // Extractor.RefineCoreBlueprintLogic(text);

    assert(OverwriteFileContent(filepath, text));

    std::cout << "File processed and overwritten successfully.\n";
    return 0;
}
