#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#endif
#include <stdexcept>

class RawSocket {
private:
    int socketDescriptor;
    bool forceOpen;

public:
    RawSocket(bool socketForceOpen = false) : socketDescriptor(-1), forceOpen(socketForceOpen) {
#ifdef _WIN32
        WSADATA data;
        if (WSAStartup(MAKEWORD(2, 2), &data) != 0) {
            throw std::runtime_error("Failed to initialize Winsock");
        }
#endif
        socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
        if (socketDescriptor < 0) {
            throw std::runtime_error("Failed to create socket");
        }
    }

    ~RawSocket() {
        if (!forceOpen)
        {
#ifdef _WIN32
            closesocket(socketDescriptor);
            WSACleanup();
#else
            close(socketDescriptor);
#endif
        }
    }

    // Send raw data
    int send(const void* data, size_t length) {
        return ::send(socketDescriptor, static_cast<const char*>(data), static_cast<int>(length), 0);
    }

    // Receive raw data
    int receive(void* buffer, size_t bufferSize) {
        return ::recv(socketDescriptor, static_cast<char*>(buffer), static_cast<int>(bufferSize), 0);
    }

    // Listen for incoming connections
    void listen(const int port) {
        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddr.sin_port = htons(port);

        if (bind(socketDescriptor, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
            throw std::runtime_error("Failed to bind");
        }

        if (::listen(socketDescriptor, SOMAXCONN) < 0) {
            throw std::runtime_error("Failed to listen");
        }

        return;
    }

    // Accept an incoming connection
    RawSocket accept() {
        sockaddr_in clientAddr{};
        socklen_t clientAddrLen = sizeof(clientAddr);

        int clientSocketDescriptor = ::accept(socketDescriptor, reinterpret_cast<sockaddr*>(&clientAddr), &clientAddrLen);
        if (clientSocketDescriptor < 0) {
            throw std::runtime_error("Failed to accept connection");
        }

        RawSocket clientSocket;
        clientSocket.socketDescriptor = clientSocketDescriptor;
        return clientSocket;
    }

    // Connect to a remote server
    void connect(const std::string host, const int port) {
        sockaddr_in serverAddr{};
        serverAddr.sin_family = AF_INET;
        serverAddr.sin_addr.s_addr = inet_addr(host.c_str());
        serverAddr.sin_port = htons(port);

        if (::connect(socketDescriptor, reinterpret_cast<const sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
            throw std::runtime_error("Failed to connect to server");
        }

        return;
    }
};
