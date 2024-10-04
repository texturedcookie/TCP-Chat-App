#include <iostream>
#include <WinSock2.h>
#include <wS2tcpip.h>
#include <tchar.h>
#include <stdio.h>


using namespace std;

#pragma comment(lib,"ws2_32.lib")

bool Initialize()
{
	WSADATA data;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
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
	inet_pton(s, serveraddress.c_str(), &(serveraddr.sin_addr));

	//connect socket to the server
	if (connect(s, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
	{
		cout << "not able to connect from server" << endl;
		closesocket(s);
		WSACleanup();
		return 1;
	}

	cout << "Successfully connected to server" << endl;

	//send and recieve
	string message = "hello world";
	int bytesent = send(s, message.c_str(), message.length(), 0);
	if (bytesent == SOCKET_ERROR)
	{
		cout << "Send failed" << endl;
	}

	// close the socket
	closesocket(s);

	cout << "Inside client, program has started" << endl;

	WSACleanup();
	return 0;
}