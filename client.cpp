// client.cpp
#include <iostream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <chrono>
#include <iomanip>
#include <sstream>

class TCPClient {
private:
    std::string clientName;
    int port;
    int period;

public:
    TCPClient(const std::string& name, int p, int per) : clientName(name), port(p), period(per) {}

    void run() {
        struct sockaddr_in serverAddr;
        memset(&serverAddr, 0, sizeof(serverAddr));
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_port = htons(port);
        serverAddr.sin_addr.s_addr = htonl(INADDR_LOOPBACK);

        while (true) {
            int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
            if (clientSocket < 0) {
                std::cerr << "Error: Could not create socket\n";
                exit(EXIT_FAILURE);
            }

            if (connect(clientSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
                std::cerr << "Error: Connection failed\n";
                close(clientSocket);
                exit(EXIT_FAILURE);
            }

            std::string message = getCurrentTime() + " " + clientName + "\n";
            send(clientSocket, message.c_str(), message.length(), 0);
            close(clientSocket);
            sleep(period);
        }
    }


private:
    std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto now_t = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&now_t), "[%Y-%m-%d %H:%M:%S") 
            << '.' << std::setfill('0') << std::setw(3) << ms.count() << ']';
        return oss.str();
    }
};

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " <client_name> <port> <period>\n";
        return EXIT_FAILURE;
    }

    std::string clientName = argv[1];
    int port = atoi(argv[2]);
    int period = atoi(argv[3]);

    TCPClient client(clientName, port, period);
    client.run();

    return EXIT_SUCCESS;
}
