#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1" // ���� IP �ּ�
#define SERVER_PORT 7700      // ���� ��Ʈ ��ȣ
#define BUFFER_SIZE 1024      // ���� ũ��

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = { 0 };
    std::string message;

    // WinSock �ʱ�ȭ
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return -1;
    }

    // ���� ����
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Socket creation error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    // IP �ּ� ��ȯ
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    // ������ ���� �õ�
    bool connected = false;

    while (!connected) {
        if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) == SOCKET_ERROR) {
            std::cerr << "Connection failed: " << WSAGetLastError() << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        else {
            connected = true;
        }
    }

    std::cout << "Connected to the server." << std::endl;

    while (true) {
        std::cout << "Enter message: ";
        std::getline(std::cin, message);

        if (message == "exit") {
            break;
        }

        // �޽��� ����
        if (send(sock, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            break;
        }
        std::cout << "Message sent" << std::endl;

        // �����κ��� �޽��� ����
        int valread = recv(sock, buffer, BUFFER_SIZE, 0);
        if (valread > 0) {
            std::cout << "Echo from server: " << std::string(buffer, valread) << std::endl;
        }
        else if (valread == 0) {
            std::cerr << "Connection closed" << std::endl;
            break;
        }
        else {
            std::cerr << "Recv failed: " << WSAGetLastError() << std::endl;
            break;
        }

        // ���� �ʱ�ȭ
        memset(buffer, 0, BUFFER_SIZE);
    }

    // ���� �ݱ�
    closesocket(sock);
    WSACleanup();
    return 0;
}
