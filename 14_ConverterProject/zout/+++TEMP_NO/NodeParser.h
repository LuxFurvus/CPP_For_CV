#include <iostream>
#include <string>

std::string ExtractFieldValue(const std::string &input, const std::string &fieldName) {
    const std::string fieldPattern = fieldName + "=";

    std::size_t fieldPos = input.find(fieldPattern);
    if (fieldPos == std::string::npos) {
        return ""; // Field not found
    }

    // Start position after '='
    std::size_t valueStart = fieldPos + fieldPattern.size();

    // Check if value is quoted
    if (input[valueStart] == '"')
    {
        // Quoted value
        std::size_t quoteEnd = input.find('"', valueStart + 1);
        if (quoteEnd == std::string::npos) {
            return ""; // Malformed
        }

        // Extract value
        std::string value = input.substr(valueStart + 1, quoteEnd - valueStart - 1);

        // Remove any parentheses
        value.erase(std::remove(value.begin(), value.end(), '('), value.end());
        value.erase(std::remove(value.begin(), value.end(), ')'), value.end());

        return value;
    }
    else
    {
        // Unquoted value
        std::size_t valueEnd = input.find_first_of(", \n\r", valueStart);
        if (valueEnd == std::string::npos) {
            valueEnd = input.size();
        }

        std::string value = input.substr(valueStart, valueEnd - valueStart);

        // Remove any parentheses
        value.erase(std::remove(value.begin(), value.end(), '('), value.end());
        value.erase(std::remove(value.begin(), value.end(), ')'), value.end());

        return value;
    }
}

std::string ExtractObjectFragment(const std::string &input) {
    const std::string beginMarker = "Begin Object";
    const std::string endMarker = "End Object";

    std::size_t startPos = input.find(beginMarker);
    if (startPos == std::string::npos) {
        return ""; // Not found
    }

    std::size_t endPos = input.find(endMarker, startPos);
    if (endPos == std::string::npos) {
        return ""; // Not found
    }

    // Include end marker in result
    endPos += endMarker.size();

    return input.substr(startPos, endPos - startPos);
}
