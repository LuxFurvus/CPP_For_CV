#include <iostream>
#include <exception>
#include <string>

class LoggedException : public std::exception
{
private:
    std::string Message;

public:
    explicit LoggedException(const std::string& InMessage)
        : Message(InMessage)
    {
    }

    ~LoggedException() noexcept override
    {
        // Not printed, if the exception is not caught and terminates the program
        std::cerr << "[LoggedException::~LoggedException] Destructor called.\n";
        std::cerr << "[LoggedException] Message was: " << Message << "\n";
    }

    const char* what() const noexcept override
    {
        return Message.c_str();
    }
};

void ThrowingFunc()
{
    throw LoggedException("Something went wrong in ThrowingFunc()");
}

int main()
{
    ThrowingFunc();
}
