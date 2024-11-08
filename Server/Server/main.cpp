#include <iostream>
#include <winsock2.h>
#include <wS2tcpip.h>
#include <tchar.h>
#include <stdio.h>
#include <thread>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <sstream>
#include <limits>
#include <iostream>
#include <string>
#include <cctype>
#include <mutex>

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

bool isIntegerBetween(int message, int min, int max) {
	//int num = message;

		// Check if the integer is within the specified range
		return message >= min && message <= max;
	

	//return false;
}

std::string removeNonPrintable(const std::string& input) {
	std::string result; 
		for (char c : input) { 
		if (isprint(static_cast<unsigned char>(c))) {
			result += c;
		} 
	} 
	return result; 

}

int extractNumberAfterColon(const std::string& input) {
	size_t colonPos = input.find(':');
	if (colonPos == std::string::npos) {
		// Return a default value if colon is not found 
		return -1; 
	} // Extract the substring after the colon and trim any leading whitespace 
	std::string numberStr = input.substr(colonPos + 1);
	numberStr.erase(0, numberStr.find_first_not_of(" \t"));
	try { 
		// Convert the extracted substring to an integer 
		int number = std::stoi(numberStr); 
		return number; 
	} catch (std::invalid_argument& e) {
		// Handle the case where conversion fails
		 return -1; 
	} catch (std::out_of_range& e) {
		// Handle the case where the number is out of integer range
		 return -1; 
	} 
}

void InteractWithClient(SOCKET clientSocket, vector<SOCKET>& clients, int secretNumber)
{
	//Send and receive to interact with multiple clients 
	cout << "Client connected" << endl;
	char buffer[50] = { 0 };

	string message(buffer);
	cout << "The message and buffer: " << message << endl;

		while (1)
		{

			int bytesrecvd = recv(clientSocket, buffer, sizeof(buffer), 0);

			if (bytesrecvd <= 0)
			{
				cout << "Client disconnected" << endl;
				break;
			}

			string message(buffer);
			cout << "The message: " << message << endl;

			int something = stoi(message);
			
			std::string inputMessage = removeNonPrintable(message);

			cout << "The INPUT message: " << inputMessage << endl;

			//if (clients.size() >= 2) {
				string response;
				try {
					int guess = extractNumberAfterColon(inputMessage);
					cout << "The guess: " << guess << endl;
					//cout << "message from client: " << message << endl;

					if (isIntegerBetween(guess, 1, 100)) {
						std::cout << "The received message is an integer between 1 and 100.";
						if (guess < secretNumber) {
							response = "Too low!";

						}
						else if (guess > secretNumber) {
							response = "Too high!";
							//send(clientSocket, response.c_str(), response.length() + 1, 0);
						}
						else {
							response = "Correct!";
							//send(clientSocket, response.c_str(), response.length() + 1, 0);
							for (auto client : clients)
							{
								send(client, response.c_str(), response.length() + 1, 0);
							}
							break; // Exit loop on correct guess 
						}
						send(clientSocket, response.c_str(), response.length() + 1, 0);
					}
					else {
						//std::cout << "The received message is not an integer between 1 and 100.\n";
						response = "The received message is not an integer between 1 and 100.";
						//send(clientSocket, response.c_str(), response.length() + 1, 0);
					}


					//string message(buffer, bytesrecvd);
					//cout << "message from client : " << message << endl;


					for (auto client : clients)
					{
						if (client != clientSocket)
						{

							send(client, inputMessage.c_str(), inputMessage.length(), 0);
							send(client, response.c_str(), response.length() + 1, 0);
						}
					}


				}
				catch (const std::invalid_argument& e) {
					std::cerr << "Invalid input: " << e.what() << std::endl;
					// Handle the invalid input, e.g., send an error message to the client
				}
				catch (const std::out_of_range& e) {
					std::cerr << "Out of range: " << e.what() << std::endl;
					// Handle the overflow/underflow, e.g., send an error message to the client
				}
			//}

			//if (clients.size() == 1) {
				//string response = "Waiting on more players";
			//	send(clientSocket, response.c_str(), response.length() + 1, 0);
			//}
		}

		


	auto it = find(clients.begin(), clients.end(), clientSocket);
	if (it != clients.end())
	{
		clients.erase(it);
	}

	//close the socket
	closesocket(clientSocket);
	
}


int main(int argc, char* argv[])
{
	if (argc != 2)
	{
		cout << "Usage: " << argv[0] << " <port>" << endl;
		return 1;
	}

	int port = atoi(argv[1]);
	if (port <= 0 || port > 65535)
	{
		cout << "Invalid port number. Please specify a port between 1 and 65535." << endl;
		return 1;
	}


	if (!Initialize())
	{
		cout << "Could not initialize Winsock 2" << endl;
		return 1;
	}
	else
	{
		//create the socket
		SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0);
		if (listenSocket == INVALID_SOCKET)
		{
			cout << "socket creation failed" << endl;
			return 1;
		}

		//create address structure
		sockaddr_in serveraddr;
		serveraddr.sin_family = AF_INET;
		serveraddr.sin_port = htons(port); //host to network short

		//convert the ip address AND put it inside the sin_family in binary
		if (InetPton(AF_INET, _T("0.0.0.0"), &serveraddr.sin_addr) != 1)
		{
			cout << "setting address structure failed" << endl;
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		//Bind the ip and port with the socket created
		if (bind(listenSocket, reinterpret_cast<sockaddr*>(&serveraddr), sizeof(serveraddr)) == SOCKET_ERROR)
		{
			cout << "Bind failed" << endl;
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		//listen on the socket
		if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR)
		{
			cout << "Listen failed" << endl;
			closesocket(listenSocket);
			WSACleanup();
			return 1;
		}

		cout << "Server has connected on port: " << port << " ." << endl;
		
		vector<SOCKET> clients;
		// Generate a random number for the game 
		srand(time(0));  // Initialize random number generator 
		int secretNumber = rand() % 100 + 1; // Secret number between 1 and 100
		
		cout << "The Secret Number" << endl;
		cout << secretNumber << endl;

		std::mutex turnMutex;
		std::condition_variable turnCV;
		int currentTurn = 0;

		while (1)
		{
			//accept
			SOCKET clientSocket = accept(listenSocket, nullptr, nullptr);
			if (clientSocket == INVALID_SOCKET)
			{
				cout << "Invalid client socket" << endl;
			}

			clients.push_back(clientSocket);

			if (clients.size() >= 2) {
				// Signal the first client's turn
				turnCV.notify_one();
			}

			thread t1(InteractWithClient, clientSocket, std::ref(clients), secretNumber);
			t1.detach();
		}
		
		closesocket(listenSocket);

		//clean up winsock library
		WSACleanup();

		return 0;
	}
}
