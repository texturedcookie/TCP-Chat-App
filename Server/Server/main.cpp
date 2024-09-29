#include <iostream>
#include <winsock2.h>
#include <wS2tcpip.h>
#include <tchar.h>
#include <stdio.h>



using namespace std;

//link with the library 
#pragma comment(lib,"ws2_32.lib")


//Intitialize winsock library by returning the version requested Winsock2
bool Initialize()
{
	WSADATA data;
	//cout << "Inside INITIALIZE" << endl;
	return WSAStartup(MAKEWORD(2, 2), &data) == 0;
}



int main()
{
	if (!Initialize())
	{
		cout << "Could not initialize Winsock 2" << endl;
		return 1;
	}
	//else if (Initialize() == 0)
	//{
//create the socket
	cout << "server program" << endl;
	SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);

	if (listenSocket == INVALID_SOCKET)
	{
		cout << "socket creation failed" << endl;
		return 1;
	}

	//get ip and port it well run on local host 12345

	//create address structure
	int port = 12345;
	sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_port = htons(port); //host to network short

	//convert the ip address put it inside the sin_family in binary
	if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1)
	{
		cout << "setting address structure failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}


//Bind the ip and port with the socket created
	//bind
	if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
	{
		cout << "Bind failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

//listen on the socket
	//listen
	if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "Listen failed" << endl;
		closesocket(listenSocket);
		WSACleanup();
		return 1;
	}

	cout << "Server has started listening on port: " << port << " ." << endl;

//accept
	SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
	if (clientSocket == INVALID_SOCKET)
	{
		cout << "Invalid client socket" << endl;
	}

//recieve
	char buffer[4096];
	int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);
	
	string message(buffer, bytesrecvd);
	cout << "message from client : " << message << endl;

//send

//close the socket
	closesocket(clientSocket);
	closesocket(listenSocket);

//clean up winsock library
	WSACleanup();
			//}

	return 0;
}
