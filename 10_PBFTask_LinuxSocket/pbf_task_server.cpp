#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <mutex>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

class TCPServer {
private:
    std::mutex log_mutex;
    struct sockaddr_in address;
    int addrlen, server_fd, client_socket, port;

    void handle_client(int client_socket) {
        char buffer[1024] = {0};
        std::ofstream logFile("log.txt", std::ios_base::app);

        while (true) {
            int valread = read(client_socket, buffer, 1024);
            if (valread == 0) break;
            
            std::lock_guard<std::mutex> guard(log_mutex);
            logFile << buffer << std::endl;
        }

        close(client_socket);
    }

public:
    TCPServer(int port) : port(port), addrlen(sizeof(address)) {
        if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
            perror("Socket creation error");
            exit(EXIT_FAILURE);
        }

        address.sin_family = AF_INET;
        address.sin_addr.s_addr = INADDR_ANY;
        address.sin_port = htons(port);

        if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
            perror("Bind Failed");
            exit(EXIT_FAILURE);
        }
        if (listen(server_fd, 3) < 0) {
            perror("Listening Failed");
            exit(EXIT_FAILURE);
        }
    }

    void run() {
        std::cout << "Server is listening...\n";
        while (true) {
            if ((client_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) {
                perror("accept");
                continue;
            }
            std::thread(&TCPServer::handle_client, this, client_socket).detach();
        }
    }
};

int main(int argc, char const *argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: server <port>\n";
        return 1;
    }

    int port = std::stoi(argv[1]);
    TCPServer server(port);
    server.run();

    return 0;
}
