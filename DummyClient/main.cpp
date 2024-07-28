#include <iostream>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <chrono>
#include <thread>

#pragma comment(lib, "ws2_32.lib")

#define SERVER_IP "127.0.0.1" // 서버 IP 주소
#define SERVER_PORT 7700      // 서버 포트 번호
#define BUFFER_SIZE 1024      // 버퍼 크기

int main() {
    WSADATA wsaData;
    SOCKET sock = INVALID_SOCKET;
    struct sockaddr_in serv_addr;
    char buffer[BUFFER_SIZE] = { 0 };
    std::string message;

    // WinSock 초기화
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed: " << WSAGetLastError() << std::endl;
        return -1;
    }

    // 소켓 생성
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == INVALID_SOCKET) {
        std::cerr << "Socket creation error: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);

    // IP 주소 변환
    if (inet_pton(AF_INET, SERVER_IP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        closesocket(sock);
        WSACleanup();
        return -1;
    }

    // 서버에 연결 시도
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

        // 메시지 전송
        if (send(sock, message.c_str(), message.size(), 0) == SOCKET_ERROR) {
            std::cerr << "Send failed: " << WSAGetLastError() << std::endl;
            break;
        }
        std::cout << "Message sent" << std::endl;

        // 서버로부터 메시지 수신
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

        // 버퍼 초기화
        memset(buffer, 0, BUFFER_SIZE);
    }

    // 소켓 닫기
    closesocket(sock);
    WSACleanup();
    return 0;
}
