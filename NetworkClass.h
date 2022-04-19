#pragma once
#include "pch.h"


const int MAX_MESSAGE_SIZE = 512;
const int MAX_QUEUE_SIZE = 200;

#pragma comment(lib, "Ws2_32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <memory>

#include "NetworkMessages.h"
#include "UserInterfaceClass.h"
#include "BlackForestClass.h"
#include "StepTimer.h"


class NetworkClass
{
private:
	struct QueueType
	{
		bool active;
		struct sockaddr_in address;
		int size;
		char message[MAX_MESSAGE_SIZE];
	};

public:
	NetworkClass();
	~NetworkClass();

	bool Initialize(char*, unsigned short, std::shared_ptr<StepTimer> timer);
	void Shutdown();
	void Frame();

	void SetZonePointer(std::shared_ptr<BlackForestClass> blackForest);
	void SetUIPointer(std::shared_ptr<UserInterfaceClass> userInterface);
	int GetLatency();
	void SetThreadActive();
	void SetThreadInactive();
	bool Online();
	SOCKET GetClientSocket();

	void ReadNetworkMessage(char*, int, struct sockaddr_in);

	//bool SendStateChange(char);
	//bool SendPositionUpdate(float, float, float, float, float, float);

private:
	bool InitializeWinSock();
	void ShutdownWinsock();

	bool ConnectToServer(char*, unsigned short);
	void HandlePingMessage();
	void ProcessLatency();
	void SendPing();
	void SendDisconnectMessage();

	void AddMessageToQueue(char*, int, struct sockaddr_in);
	void ProcessMessageQueue();
	//void HandleChatMessage(int);
	//void HandleEntityInfoMessage(int);
	//void HandleNewUserLoginMessage(int);
	//void HandleUserDisconnectMessage(int);
	//void HandleStateChangeMessage(int);
	//void HandlePositionMessage(int);
	//void HandleAIRotateMessage(int);

	//bool SendChatMessage(char*);
	//bool RequestEntityList();

	std::shared_ptr<StepTimer> m_timer;

	double m_pingTime;

	std::shared_ptr<BlackForestClass> m_zone;
	std::shared_ptr<UserInterfaceClass> m_userInterface;

	int m_latency;
	SOCKET m_clientSocket;
	int m_addressLength;
	struct sockaddr_in m_serverAddress;
	unsigned short m_idNumber, m_sessionId;
	bool m_online, m_threadActive;
	
	QueueType* m_networkMessageQueue;
	int m_nextQueueLocation, m_nextMessageForProcessing;
	char m_chatMessage[64];
	char m_uiMessage[50];
};


/////////////////////////
// FUNCTION PROTOTYPES //
/////////////////////////
// void NetworkReadFunction(void*);