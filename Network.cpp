#include "Network.h"

Network::Network(const char* ipAddress, unsigned short serverPort, std::shared_ptr<StepTimer> timer) :
	m_timer(timer),
	m_latency(0),
	m_networkMessageQueue(nullptr),
	m_nextQueueLocation(0),
	m_nextMessageForProcessing(0),
	m_online(false)
{
	// Initialize the network message queue.
	m_networkMessageQueue = new QueueType[MAX_QUEUE_SIZE];

	for (int iii = 0; iii < MAX_QUEUE_SIZE; iii++)
		m_networkMessageQueue[iii].active = false;

	// Initialize winsock for using window's sockets. Will throw on error
	InitializeWinSock();

	// Connect to the server. Will throw on error
	ConnectToServer(ipAddress, serverPort);
		
	// Send a request to the zone server for the list of user and non-user entities currently online as well as their status. Will throw on error
	//RequestEntityList();
}

Network::~Network()
{
	// Send a message to the server letting it know this client is disconnecting.
	SendDisconnectMessage();

	// Set the client to be offline.
	m_online = false;

	// Wait for the network I/O thread to complete.
	while (m_threadActive)
	{
		Sleep(5);
	}

	// Close the socket.
	closesocket(m_clientSocket);

	// Shutdown winsock.
	WSACleanup();

	// Release the network message queue.
	if (m_networkMessageQueue)
	{
		delete[] m_networkMessageQueue;
		m_networkMessageQueue = 0;
	}
}


void Network::Update()
{
	//bool newMessage;


	// Update the network latency.
	ProcessLatency();

	// Read and process the network messages that are in the queue.
	ProcessMessageQueue();

	// Check if there is a chat message that this user wants to send to the server.
	// m_userInterface->CheckForChatMessage(m_uiMessage, newMessage);
	// if (newMessage)
	//{
	//	SendChatMessage(m_uiMessage);
	//}
}

/*
void Network::SetZonePointer(std::shared_ptr<BlackForestClass> blackForest)
{
	m_zone = blackForest;
}


void Network::SetUIPointer(std::shared_ptr<UserInterfaceClass> userInterface)
{
	m_userInterface = userInterface;
}
*/


void Network::InitializeWinSock()
{
	WSADATA wsaData;

	// Get the data to see if it handles version 2.2
	int error = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (error != 0)
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}

	// Check to see if the winsock dll is version 2.2
	if ((LOBYTE(wsaData.wVersion) != 2) || (HIBYTE(wsaData.wVersion) != 2))
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}

	/*
		The code below is used to verify that TCP & UDP are supported by the system.
		For now, just assume that they are supported. In the future, this should be added
		back, however, WSAPROTOCOL_INFOA is deprecated, so implement this in a different
		way. See: https://docs.microsoft.com/en-us/windows/win32/api/winsock2/nf-winsock2-wsaenumprotocolsa

	unsigned long bufferSize = 0;
	//WSAPROTOCOL_INFOW* protocolBuffer;
	WSAPROTOCOL_INFOA* protocolBuffer;
	int protocols[2];



	// Request the buffer size needed for holding the protocols available.
	WSAEnumProtocols(NULL, NULL, &bufferSize);

	// Create a buffer for the protocol information structs.
	//protocolBuffer = new WSAPROTOCOL_INFOW[bufferSize];
	protocolBuffer = new WSAPROTOCOL_INFOA[bufferSize];
	if (!protocolBuffer)
	{
		return false;
	}

	// Create the list of protocols we are looking for which are TCP and UDP.
	protocols[0] = IPPROTO_TCP;
	protocols[1] = IPPROTO_UDP;

	// Retrieve information about available transport protocols, if no socket error then the protocols from the list will work.
	error = WSAEnumProtocols(protocols, protocolBuffer, &bufferSize);
	if (error == SOCKET_ERROR)
	{
		return false;
	}

	// Release the protocol buffer.
	delete[] protocolBuffer;
	protocolBuffer = 0;

	*/
}



void Network::ConnectToServer(const char* ipAddress, unsigned short portNumber)
{
	unsigned long setting, inetAddress, threadId;
	int error, bytesSent, bytesRead;
	MSG_GENERIC_DATA connectMessage;
	bool done, gotId;
	char recvBuffer[4096];
	MSG_NEWID_DATA* message;
	HANDLE threadHandle;

	double startTime;


	// Create a UDP socket.
	m_clientSocket = socket(AF_INET, SOCK_DGRAM, 0);
	if (m_clientSocket == INVALID_SOCKET)
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}

	// Set the client socket to non-blocking I/O.
	setting = 1;
	error = ioctlsocket(m_clientSocket, FIONBIO, &setting);
	if (error == SOCKET_ERROR)
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}

	// Save the size of the server address structure.
	m_addressLength = sizeof(m_serverAddress);

	// Convert the string representation of the IP address to a numeric binary representation
	// See: https://docs.microsoft.com/en-us/windows/win32/api/ws2tcpip/nf-ws2tcpip-inet_pton
	int result = inet_pton(AF_INET, ipAddress, &inetAddress);
	if (result == 0)
	{
		// A 0 result means a non-valid string representation was passed in
		throw new ChameleonException(__LINE__, __FILE__);
	}
	else if (result < 0)
	{
		// A -1 value means an internal error occurred and it can be retrieved via WSAGetLastError()
		throw new ChameleonException(__LINE__, __FILE__);
	}

	memset((char*)&m_serverAddress, 0, m_addressLength);
	m_serverAddress.sin_family = AF_INET;
	m_serverAddress.sin_port = htons(portNumber);
	m_serverAddress.sin_addr.s_addr = inetAddress;

	// Setup a connect message to send to the server.
	connectMessage.type = MSG_CONNECT;

	// Send the connect message to the server.
	bytesSent = sendto(m_clientSocket, (char*)&connectMessage, sizeof(MSG_GENERIC_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytesSent < 0)
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}

	// Record the time when the connect packet was sent.
	startTime = m_timer->GetTotalSeconds();

	// Set the boolean loop values.
	done = false;
	gotId = false;

	// Loop for two seconds checking for the ID return message.
	while (!done)
	{
		// Check for a reply message from the server.
		bytesRead = recvfrom(m_clientSocket, recvBuffer, 4096, 0, (struct sockaddr*)&m_serverAddress, &m_addressLength);
		if (bytesRead > 0)
		{
			done = true;
			gotId = true;
		}
		else
		{
			// Check to see if this loop has been running for longer than 2 seconds.
			if (m_timer->GetTotalSeconds() > (startTime + 2))
			{
				done = true;
				gotId = false;
			}
			else
			{
				// We have to make sure the timer continues to update
				m_timer->Tick([&]() {});
			}
		}
	}

	// If it didn't get an ID in 2 seconds then the server was not up.
	if (!gotId)
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}

	// Coerce the network message that was received into a new id message type.
	message = (MSG_NEWID_DATA*)recvBuffer;

	// Ensure it was a new id message.
	if (message->type != MSG_NEWID)
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}

	// Store the ID number for this client for all future communication with the server.
	m_idNumber = message->idNumber;
	m_sessionId = message->sessionId;

	// Set the client to be online now.
	m_online = true;

	// Initialize the thread activity variable.
	m_threadActive = false;

	// Create a thread to listen for network I/O from the server.
	threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)NetworkReadFunction, (void*)this, 0, &threadId);
	if (threadHandle == NULL)
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}

	// Initialize the network latency variables.
	m_pingTime = m_timer->GetTotalSeconds();
}


void NetworkReadFunction(void* ptr)
{
	Network* NetworkPtr;
	struct sockaddr_in serverAddress;
	int addressLength;
	int bytesRead;
	char recvBuffer[4096];


	// Get a pointer to the calling object.
	NetworkPtr = (Network*)ptr;

	// Notify parent object that this thread is now active.
	NetworkPtr->SetThreadActive();

	// Set the size of the address.
	addressLength = sizeof(serverAddress);

	// Loop and read network messages while the client is online.
	while (NetworkPtr->Online())
	{
		// Check if there is a message from the server.
		bytesRead = recvfrom(NetworkPtr->GetClientSocket(), recvBuffer, 4096, 0, (struct sockaddr*)&serverAddress, &addressLength);
		if (bytesRead > 0)
		{
			NetworkPtr->ReadNetworkMessage(recvBuffer, bytesRead, serverAddress);
		}
	}

	// Notify parent object that this thread is now inactive.
	NetworkPtr->SetThreadInactive();

	// Release the pointer.
	NetworkPtr = nullptr;
}



void Network::ReadNetworkMessage(char* recvBuffer, int bytesRead, struct sockaddr_in serverAddress)
{
	MSG_GENERIC_DATA* message;


	// Check that the address the message came from is the correct IP address from the server and not a hack attempt from someone else.


	// Check for buffer overflow.
	if (bytesRead > MAX_MESSAGE_SIZE)
	{
		return;
	}

	// If it is a ping message then process it immediately for accurate stats.
	message = (MSG_GENERIC_DATA*)recvBuffer;
	if (message->type == MSG_PING)
	{
		HandlePingMessage();
	}
	// Otherwise place the message in the queue to be processed during the frame processing for the network.
	else
	{
		AddMessageToQueue(recvBuffer, bytesRead, serverAddress);
	}
}


void Network::HandlePingMessage()
{
	// Convert time delta to ms then cast to int
	m_latency = static_cast<int>((m_timer->GetTotalSeconds() - m_pingTime) * 1000);
}


void Network::ProcessLatency()
{
	// Ping the server every 5 seconds
	if (m_timer->GetTotalSeconds() >= (m_pingTime + 5))
	{
		m_pingTime = m_timer->GetTotalSeconds();
		SendPing();
	}
}


void Network::SendPing()
{
	MSG_PING_DATA message;
	int bytesSent;


	// Create the ping message.
	message.type = MSG_PING;
	message.idNumber = m_idNumber;
	message.sessionId = m_sessionId;

	// Send the ping message to the server.
	bytesSent = sendto(m_clientSocket, (char*)&message, sizeof(MSG_PING_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytesSent != sizeof(MSG_PING_DATA))
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}
}


void Network::SendDisconnectMessage()
{
	MSG_DISCONNECT_DATA message;
	int bytesSent;


	// Create the disconnect message.
	message.type = MSG_DISCONNECT;
	message.idNumber = m_idNumber;
	message.sessionId = m_sessionId;

	// Send the disconnect message to the server.
	bytesSent = sendto(m_clientSocket, (char*)&message, sizeof(MSG_DISCONNECT_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytesSent != sizeof(MSG_DISCONNECT_DATA))
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}
}


void Network::AddMessageToQueue(char* message, int messageSize, struct sockaddr_in serverAddress)
{
	// Check for buffer overflow.
	if (messageSize > MAX_MESSAGE_SIZE)
	{
	}

	// Otherwise add it to the circular message queue to be processed.
	else
	{
		m_networkMessageQueue[m_nextQueueLocation].address = serverAddress;
		m_networkMessageQueue[m_nextQueueLocation].size = messageSize;
		memcpy(m_networkMessageQueue[m_nextQueueLocation].message, message, messageSize);

		// Set active last so that racing conditions in processing the queue do not exist.
		m_networkMessageQueue[m_nextQueueLocation].active = true;

		// Increment the queue position.
		m_nextQueueLocation++;
		if (m_nextQueueLocation == MAX_QUEUE_SIZE)
		{
			m_nextQueueLocation = 0;
		}
	}
}


void Network::ProcessMessageQueue()
{
	MSG_GENERIC_DATA* message;


	// Loop through all process all the active messages in the queue.
	while (m_networkMessageQueue[m_nextMessageForProcessing].active == true)
	{
		// Coerce the message into a generic format to read the type of message.
		message = (MSG_GENERIC_DATA*)m_networkMessageQueue[m_nextMessageForProcessing].message;

		/*
		switch (message->type)
		{
		case MSG_CHAT:
		{
			HandleChatMessage(m_nextMessageForProcessing);
			break;
		}
		case MSG_ENTITY_INFO:
		{
			HandleEntityInfoMessage(m_nextMessageForProcessing);
			break;
		}
		case MSG_NEW_USER_LOGIN:
		{
			HandleNewUserLoginMessage(m_nextMessageForProcessing);
			break;
		}
		case MSG_USER_DISCONNECT:
		{
			HandleUserDisconnectMessage(m_nextMessageForProcessing);
			break;
		}
		case MSG_STATE_CHANGE:
		{
			HandleStateChangeMessage(m_nextMessageForProcessing);
			break;
		}
		case MSG_POSITION:
		{
			HandlePositionMessage(m_nextMessageForProcessing);
			break;
		}
		case MSG_AI_ROTATE:
		{
			HandleAIRotateMessage(m_nextMessageForProcessing);
			break;
		}
		default:
		{
			break;
		}
		}
		*/
		// Set the message as processed.
		m_networkMessageQueue[m_nextMessageForProcessing].active = false;

		// Increment the queue position.
		m_nextMessageForProcessing++;
		if (m_nextMessageForProcessing == MAX_QUEUE_SIZE)
		{
			m_nextMessageForProcessing = 0;
		}
	}
}

/*

void Network::HandleChatMessage(int queuePosition)
{
	MSG_CHAT_DATA* msg;
	unsigned short clientId;


	// Confirm this came from the server and not someone else.
	//VerifyServerMessage();

	// Get the contents of the message.
	msg = (MSG_CHAT_DATA*)m_networkMessageQueue[queuePosition].message;
	clientId = msg->idNumber;

	// Copy text into a string of a specific size.
	strcpy_s(m_chatMessage, 64, msg->text);

	// If there was a new message then send it to the user interface.
	//m_userInterface->AddChatMessageFromServer(m_chatMessage, clientId);
}



void Network::HandleEntityInfoMessage(int queuePosition)
{
	MSG_ENTITY_INFO_DATA* message;
	unsigned short entityId;
	char entityType;
	float positionX, positionY, positionZ;
	float rotationX, rotationY, rotationZ;


	// Confirm this came from the server and not someone else.

	// Get the contents of the message.
	message = (MSG_ENTITY_INFO_DATA*)m_networkMessageQueue[queuePosition].message;

	entityId = message->entityId;
	entityType = message->entityType;
	positionX = message->positionX;
	positionY = message->positionY;
	positionZ = message->positionZ;
	rotationX = message->rotationX;
	rotationY = message->rotationY;
	rotationZ = message->rotationZ;

	// Check that the zone pointer is set.
	//if (m_zone)
	//{
		// Add the entity to the zone.
	//	m_zone->AddEntity(entityId, entityType, positionX, positionY, positionZ, rotationX, rotationY, rotationZ);
	//}
}


void Network::HandleNewUserLoginMessage(int queuePosition)
{
	MSG_ENTITY_INFO_DATA* message;
	unsigned short entityId;
	char entityType;
	float positionX, positionY, positionZ;
	float rotationX, rotationY, rotationZ;


	// Confirm this came from the server and not someone else.

	// Get the contents of the message.
	message = (MSG_ENTITY_INFO_DATA*)m_networkMessageQueue[queuePosition].message;

	entityId = message->entityId;
	entityType = message->entityType;
	positionX = message->positionX;
	positionY = message->positionY;
	positionZ = message->positionZ;
	rotationX = message->rotationX;
	rotationY = message->rotationY;
	rotationZ = message->rotationZ;

	// Check that the zone pointer is set.
	//if (m_zone)
	//{
	//	// Add the new user entity to the zone.
	//	m_zone->AddEntity(entityId, entityType, positionX, positionY, positionZ, rotationX, rotationY, rotationZ);
	//}
}


void Network::HandleUserDisconnectMessage(int queuePosition)
{
	MSG_USER_DISCONNECT_DATA* message;
	unsigned short entityId;


	// Confirm this came from the server and not someone else.

	// Get the contents of the message.
	message = (MSG_USER_DISCONNECT_DATA*)m_networkMessageQueue[queuePosition].message;

	entityId = message->idNumber;

	// Check that the zone pointer is set.
	///if (m_zone)
	//{
	//	// Remove the user entity from the zone.
	//	m_zone->RemoveEntity(entityId);
	//}
}


void Network::HandleStateChangeMessage(int queuePosition)
{
	MSG_STATE_CHANGE_DATA* message;
	unsigned short entityId;
	char state;


	// Confirm this came from the server and not someone else.

	// Get the contents of the message.
	message = (MSG_STATE_CHANGE_DATA*)m_networkMessageQueue[queuePosition].message;

	entityId = message->idNumber;
	state = message->state;

	// Check that the zone pointer is set.
	//if (m_zone)
	//{
	//	// Update the state of the entity.
	//	m_zone->UpdateEntityState(entityId, state);
	//}
}


void Network::HandlePositionMessage(int queuePosition)
{
	MSG_POSITION_DATA* message;
	unsigned short entityId;
	float positionX, positionY, positionZ, rotationX, rotationY, rotationZ;


	// Confirm this came from the server and not someone else.

	// Get the contents of the message.
	message = (MSG_POSITION_DATA*)m_networkMessageQueue[queuePosition].message;

	entityId = message->idNumber;
	positionX = message->positionX;
	positionY = message->positionY;
	positionZ = message->positionZ;
	rotationX = message->rotationX;
	rotationY = message->rotationY;
	rotationZ = message->rotationZ;

	// Check that the zone pointer is set.
	//if (m_zone)
	//{
	//	// Update the position of the entity in the zone.
	//	m_zone->UpdateEntityPosition(entityId, positionX, positionY, positionZ, rotationX, rotationY, rotationZ);
	//}
}


void Network::HandleAIRotateMessage(int queuePosition)
{
	MSG_AI_ROTATE_DATA* message;
	unsigned short entityId;
	bool rotate;


	// Confirm this came from the server and not someone else.

	// Get the contents of the message.
	message = (MSG_AI_ROTATE_DATA*)m_networkMessageQueue[queuePosition].message;
	entityId = message->idNumber;
	rotate = message->rotate;

	// Check that the zone pointer is set.
	///if (m_zone)
	//{
	//	// Update the AI entity rotation.
	//	m_zone->UpdateEntityRotate(entityId, rotate);
	//}
}


void Network::SendChatMessage(char* inputMsg)
{
	MSG_CHAT_DATA message;
	int bytesSent;


	// Create the chat message.
	message.type = MSG_CHAT;
	message.idNumber = m_idNumber;
	message.sessionId = m_sessionId;
	strcpy_s(message.text, 64, inputMsg);

	// Send the message to the server.
	bytesSent = sendto(m_clientSocket, (char*)&message, sizeof(MSG_CHAT_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytesSent != sizeof(MSG_CHAT_DATA))
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}
}


void Network::RequestEntityList()
{
	MSG_SIMPLE_DATA message;
	int bytesSent;


	// Create the entity request message.
	message.type = MSG_ENTITY_REQUEST;
	message.idNumber = m_idNumber;
	message.sessionId = m_sessionId;

	// Send the message to the server.
	bytesSent = sendto(m_clientSocket, (char*)&message, sizeof(MSG_SIMPLE_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytesSent != sizeof(MSG_SIMPLE_DATA))
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}
}


void Network::SendStateChange(char state)
{
	MSG_STATE_CHANGE_DATA message;
	int bytesSent;


	// Create the state change message.
	message.type = MSG_STATE_CHANGE;
	message.idNumber = m_idNumber;
	message.sessionId = m_sessionId;
	message.state = state;

	bytesSent = sendto(m_clientSocket, (char*)&message, sizeof(MSG_STATE_CHANGE_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytesSent != sizeof(MSG_STATE_CHANGE_DATA))
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}
}


void Network::SendPositionUpdate(float positionX, float positionY, float positionZ, float rotationX, float rotationY, float rotationZ)
{
	MSG_POSITION_DATA message;
	int bytesSent;


	// Create the position message.
	message.type = MSG_POSITION;
	message.idNumber = m_idNumber;
	message.sessionId = m_sessionId;
	message.positionX = positionX;
	message.positionY = positionY;
	message.positionZ = positionZ;
	message.rotationX = rotationX;
	message.rotationY = rotationY;
	message.rotationZ = rotationZ;

	// Send the position update message to the server.
	bytesSent = sendto(m_clientSocket, (char*)&message, sizeof(MSG_POSITION_DATA), 0, (struct sockaddr*)&m_serverAddress, m_addressLength);
	if (bytesSent != sizeof(MSG_POSITION_DATA))
	{
		throw new ChameleonException(__LINE__, __FILE__);
	}
}

*/