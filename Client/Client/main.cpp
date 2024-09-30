#include <iostream>
#include <WinSock2.h>
#include <wS2tcpip.h>
#include <tchar.h>
#include <stdio.h>
#include <thread>


using namespace std;

#pragma comment(lib,"ws2_32.lib")

bool Initialize()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}
void SendMsg(SOCKET s)
{
	cout << "Enter your chat name: " << endl;
	string name;
	getline(cin, name);
	string message;
	while (1)
	{
		getline(cin, message);
		string msg = name + " : " + message;
		int bytesent = send(s, msg.c_str(), msg.length(), 0);
		if (bytesent == SOCKET_ERROR)
		{
			cout << "error sending message" << endl;
			break;
		}
		if (message == "quit")
		{
			cout << "stopping the application" << endl;
			break;
		}
	}
	closesocket(s);
	WSACleanup();
}

void ReceiveMsg(SOCKET s)
{
	char buffer[4096];
	int recvlength;
	string msg = "";
	while (1)
	{
		recvlength = recv(s, buffer, sizeof(buffer), 0);
		if (recvlength <= 0)
		{
			cout << "disconnected from the server" << endl;
			break;
		}
		else
		{
			msg = string(buffer, recvlength);
			cout << msg << endl;
		}
	}
	closesocket(s);
	WSACleanup();
}

int main()
{
	// initialize winsock
	if (!Initialize())
	{
		cout << "Initialize winsock failed" << endl;

		return 1;
	}


	//create socket
	SOCKET s;
	s = socket(AF_INET, SOCK_STREAM, 0);
	if (s == INVALID_SOCKET)
	{
		cout << "Invalid socket created" << endl;
		return 1;
	}

	int port = 12345;
	string serveraddress = "127.0.0.1";
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port);

	//bind
	inet_pton(AF_INET, serveraddress.c_str(), &(serveraddr.sin_addr));

	//connect socket to the server
	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
	{
		cout << "not able to connect from server" << endl;
		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "Successfully connected to server" << endl;
	thread senderthread(SendMsg, s);
	thread receiverthread(ReceiveMsg, s);

	senderthread.join();
	receiverthread.join();


	return 0;
}