#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <chrono>
#include <thread>
#include <iomanip>
#include <sstream>

class TCPClient {
private:
    std::string client_name;
    struct sockaddr_in serv_addr;
    int sock;
    int port;
    int period;

    std::string get_current_time() {
        auto now = std::chrono::system_clock::now();
        auto now_c = std::chrono::system_clock::to_time_t(now);
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;

        std::stringstream ss;
        ss << std::put_time(std::localtime(&now_c), "%Y-%m-%d %H:%M:%S");
        ss << '.' << std::setfill('0') << std::setw(3) << milliseconds.count();
        return ss.str();
    }

public:
    TCPClient(std::string client_name, int port, int period) : client_name(client_name), port(port), period(period), sock(0) {
        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
            perror("Socket creation error");
            exit(EXIT_FAILURE);
        }

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(port);

        if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
            perror("Invalid address/ Address not supported");
            exit(EXIT_FAILURE);
        }
    }

void run() {
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        exit(EXIT_FAILURE);
    }

    int counter = 1;
    std::stringstream ss;
    while (true) {
        std::string message = "[" + get_current_time() + "] \"" + client_name + "\"";
        send(sock, message.c_str(), message.length(), 0);

        ss.str("");
        ss << "Message N " << counter++ << " sent!";
        std::cout << ss.str() << std::flush;

        std::this_thread::sleep_for(std::chrono::seconds(period));

        std::cout << "\r" << std::string(ss.str().length(), ' ') << "\r" << std::flush;
    }
}

};

int main(int argc, char const *argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: client <name> <port> <period>\n";
        return 1;
    }

    std::string name = argv[1];
    int port = std::stoi(argv[2]);
    int period = std::stoi(argv[3]);

    TCPClient client(name, port, period);
    client.run();

    return 0;
}
