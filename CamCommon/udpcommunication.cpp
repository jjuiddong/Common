
#include "stdafx.h"
#include "udpcommunication.h"

#pragma comment(lib,"ws2_32.lib") //Winsock Library

#define BUFLEN 512  //Max length of buffer

unsigned WINAPI ThreadFunction(void* arg);




cUDPCommunication::cUDPCommunication()
	: m_threadLoop(true)
{
	InitializeCriticalSectionAndSpinCount(&m_CriticalSection, 0x00000400);
}

cUDPCommunication::~cUDPCommunication()
{
	DeleteCriticalSection(&m_CriticalSection);
}


// UDP 서버를 생성한다.
// 첫 번째 클라이언트가 접속할 때까지 대기한다.
void cUDPCommunication::Init(const int port)
{
	std::cout << "UDP Network Communication" << std::endl;

	struct sockaddr_in server;
	WSADATA wsa;

	//Initialise winsock
	printf("\nInitialising Winsock...");
	if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0)
	{
		printf("Failed. Error Code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	printf("Initialised.\n");

	//Create a socket
	if ((m_socket = socket(AF_INET, SOCK_DGRAM, 0)) == INVALID_SOCKET)
	{
		printf("Could not create socket : %d", WSAGetLastError());
	}
	printf("Socket created.\n");

	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(port);

	//Bind
	if (bind(m_socket, (struct sockaddr *)&server, sizeof(server)) == SOCKET_ERROR)
	{
		printf("Bind failed with error code : %d", WSAGetLastError());
		exit(EXIT_FAILURE);
	}
	puts("Bind done");

	printf("port = %d\n", port);

	printf("Waiting for data...");
	fflush(stdout);


	m_handle = (HANDLE)_beginthreadex(NULL, 0, ThreadFunction, this, 0, (unsigned*)&m_threadId);
}


// 데이타를 전송한다.
void cUDPCommunication::SendData(const sSharedData &data)
{
	EnterCriticalSection(&m_CriticalSection);
	m_sharedData = data;
	LeaveCriticalSection(&m_CriticalSection);
}



unsigned WINAPI ThreadFunction(void* arg)
{
	cUDPCommunication *udp = (cUDPCommunication*)arg;

	int slen;
	int recv_len;
	char buf[BUFLEN];

	while (udp->m_threadLoop)
	{
		slen = sizeof(udp->m_si_other);
		//clear the buffer by filling null, it might have previously received data
		memset(buf, '\0', BUFLEN);

		//try to receive some data, this is a blocking call
		if ((recv_len = recvfrom(udp->m_socket, buf, BUFLEN, 0, (struct sockaddr *) &udp->m_si_other, &slen)) == SOCKET_ERROR)
		{
			printf("recvfrom() failed with error code : %d", WSAGetLastError());
			//exit(EXIT_FAILURE);
		}

		//print details of the client/peer and the data received
		if (g_config.m_printNetworkMessage)
			printf("Received packet from %s:%d\n", inet_ntoa(udp->m_si_other.sin_addr), ntohs(udp->m_si_other.sin_port));
		//printf("Data: %s\n", buf);

		EnterCriticalSection(&udp->m_CriticalSection);
		const sSharedData data = udp->m_sharedData;
		LeaveCriticalSection(&udp->m_CriticalSection);

		if (sendto(udp->m_socket, (char*)&data,
			sizeof(data), 0, (struct sockaddr*) &udp->m_si_other, slen) == SOCKET_ERROR)
		{
			printf("sendto() failed with error code : %d", WSAGetLastError());
			//exit(EXIT_FAILURE);
		}
	}

	return 0;
}
