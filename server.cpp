// server.cpp
// #include <syncstream>
#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <thread>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>



class TCPServer {
private:
    int port;

public:
    TCPServer(int p) : port(p) {}

    void run() {
        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(port);
        int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

        if (serverSocket < 0) {
            std::cerr << "Error: Could not create socket\n";
            exit(EXIT_FAILURE);
        }

        if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
            std::cerr << "Error: Binding failed\n";
            exit(EXIT_FAILURE);
        }

        if (listen(serverSocket, 5) < 0) {
            std::cerr << "Error: Listen failed\n";
            exit(EXIT_FAILURE);
        }

        while (true) {
            struct sockaddr_in clientAddr;
            socklen_t clientLen = sizeof(clientAddr);
            int clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientLen);
            if (clientSocket < 0) {
                std::cerr << "Error: Accept failed\n";
                exit(EXIT_FAILURE);
            }

            std::thread clientThread(&TCPServer::handleClient, this, clientSocket);
            clientThread.detach(); 
        }
    }

private:
    void handleClient(int clientSocket) {
        char buffer[1024];
        int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        if (bytesRead < 0) {
            std::cerr << "Error: Reading from client failed\n";
            close(clientSocket);
            return;
        }
        buffer[bytesRead] = '\0';
        std::string message(buffer);

        writeToLog(message);
        close(clientSocket);
    }

    void writeToLog(const std::string& message) {
        std::ofstream logFile("log.txt", std::ios::app);
        if (!logFile.is_open()) {
            std::cerr << "Error: Unable to open log file\n";
            return;
        }

        logFile << message;
        logFile.close();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <port>\n";
        return EXIT_FAILURE;
    }

    int port = atoi(argv[1]);

    TCPServer server(port);
    server.run();

    return EXIT_SUCCESS;
}
