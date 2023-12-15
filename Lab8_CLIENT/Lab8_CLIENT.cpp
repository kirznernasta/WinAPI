#include <iostream>
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include <sstream>

#pragma comment(lib, "ws2_32.lib")


WSAData data;

addrinfo hints;
addrinfo* serverInfo = nullptr;

SOCKET connectSocket;

std::thread receive_thread;
void setHints()
{
	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
}

bool validateUsername(const std::string& s)
{
	for (const char c : s) {
		if (!isalpha(c) && !isdigit(c) && c != '_')
			return false;
	}

	return true;
}

void sendMessages() {
	std::string username;
	bool valid_username = false;
	while (!valid_username) {
		std::cout << "Enter your username (valid symbols are letters, numbers and underscore _):";
		std::cin >> username;
		valid_username = validateUsername(username);
		if (!valid_username) {
			std::cout << "\033[1;31mInvalid username!\033[0m\n";
		}
	}
	
	
	send(connectSocket, username.c_str(), username.length(), 0);
	std::string message;
	while (true) {
		std::cin >> message;
		if (send(connectSocket, message.c_str(), message.length(), 0) == SOCKET_ERROR) {
			std::cout << "Error occured when seding message to server.";
		}
	}
}

std::string colors[] = {"\n\033[1;32m", "\n\033[1;33m", "\n\033[1;34m", "\n\033[1;35m", "\n\033[1;36m"};

std::string getColor() {
	int random = rand() % 5;
	return colors[random];
}

void receiveMessageFromServer()
{
	char receivebuffer[256];
	while (true) {
		ZeroMemory(receivebuffer, 256);
		if (recv(connectSocket, receivebuffer, 256, 0) != SOCKET_ERROR)
		{
			std::cout << receivebuffer;
		}
	}
}

void clear(const std::string& message, addrinfo* info, SOCKET* socket = nullptr)
{
	std::cout << message << "\n";
	if (info)
		freeaddrinfo(info);
	if (socket)
		closesocket(*socket);

	WSACleanup();
}

int main() {
	int res = WSAStartup(MAKEWORD(2, 2), &data);
	if (res != 0)
	{
		std::cout << "Startup failed with code " << res << '\n';
		return 1;
	}

	setHints();

	res = getaddrinfo("127.0.0.1", "80", &hints, &serverInfo);
	if (res != 0)
	{
		clear("getaddrinfo error. Code : " + std::to_string(res) + "\n", nullptr);
		return false;
	}

	connectSocket = socket(serverInfo->ai_family, serverInfo->ai_socktype, serverInfo->ai_protocol);
	if (connectSocket == INVALID_SOCKET)
	{
		clear("Connection ConnectSocket failed.\n", serverInfo);
		return 1;
	}

	res = connect(connectSocket, serverInfo->ai_addr, serverInfo->ai_addrlen);
	if (res == SOCKET_ERROR)
	{
		clear("Unable connect to server.\n", serverInfo, &connectSocket);
		return false;
	}

	std::cout << "Connected to server. Write your messages!\n";

	std::thread sendMessagesThread(&sendMessages);
	std::thread receiveMessagesThread(&receiveMessageFromServer);

	sendMessagesThread.join();
	receiveMessagesThread.join();

	clear("", serverInfo, &connectSocket);
	WSACleanup();
}