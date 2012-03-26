/**************************************************************************************************
* THE OMICRON PROJECT
 *-------------------------------------------------------------------------------------------------
 * Copyright 2010-2012		Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Arthur Nishimoto		anishimoto42@gmail.com
 *  Alessandro Febretti		febret@gmail.com
 *-------------------------------------------------------------------------------------------------
 * Copyright (c) 2010-2011, Electronic Visualization Laboratory, University of Illinois at Chicago
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are permitted 
 * provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this list of conditions 
 * and the following disclaimer. Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the documentation and/or other 
 * materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF 
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************************************/
#ifndef __OMICRON_CONNECTOR_CLIENT__
#define __OMICRON_CONNECTOR_CLIENT__

#ifdef WIN32
	#define OMICRON_OS_WIN
	#pragma comment(lib, "Ws2_32.lib")
#endif

#include <stdio.h>
#ifdef OMICRON_OS_WIN
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
	#include <stdlib.h>
	#include <string.h>
	#include <netdb.h>
	#include <sys/types.h>
	#include <netinet/in.h>
	#include <sys/socket.h>
	#include <arpa/inet.h>
	#include <errno.h>
	#include <unistd.h> // needed for close()
	#include <string>
#endif

#ifdef OMICRON_OS_WIN     
	#define PRINT_SOCKET_ERROR(msg) printf(msg" - socket error: %d\n", WSAGetLastError());
	#define SOCKET_CLOSE(sock) closesocket(sock);
	#define SOCKET_CLEANUP() WSACleanup();
	#define SOCKET_INIT() \
		int iResult;    \
		iResult = WSAStartup(MAKEWORD(2,2), &wsaData); \
		if (iResult != 0) { \
			printf("NetService: WSAStartup failed: %d\n", iResult); \
			return; \
		} else { \
			printf("NetService: Winsock initialized \n"); \
		}

#else
	#define SOCKET_CLOSE(sock) close(sock);
	#define SOCKET_CLEANUP()
	#define SOCKET_INIT()
	#define SOCKET int
	#define PRINT_SOCKET_ERROR(msg) printf(msg" - socket error: %s\n", strerror(errno));
#endif

#define OI_READBUF(type, buf, offset, val) val = *((type*)&buf[offset]); offset += sizeof(type);

namespace omicronConnector
{
	//////////////////////////////////////////////////////////////////////////////////////////////////
	struct EventData
	{
		unsigned int timestamp;
		unsigned int sourceId;
		int serviceId;
		unsigned int serviceType;
		unsigned int type;
		unsigned int flags;
		float posx;
		float posy;
		float posz;
		float orx;
		float ory;
		float orz;
		float orw;

		static const int ExtraDataSize = 1024;
		unsigned int extraDataType;
		unsigned int extraDataLength;
		unsigned int extraDataMask;
		unsigned char extraData[ExtraDataSize];
	};

	//////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename ListenerType>
	class OmicronConnectorClient
	{
	public:
		void connect(const char* server, int port = 27000, int dataPort = 7000);
		void poll();
		void dispose();
		void setDataport(int);

	private:
		void initHandshake();
		void parseDGram(int);

	private:
		typedef ListenerType Listener;

	#ifdef OMICRON_OS_WIN	
		WSADATA wsaData;
	#endif

		SOCKET ConnectSocket;
		SOCKET RecvSocket;
		struct timeval timeout;
		sockaddr_in SenderAddr;

		const char* serverAddress;
		int serverPort;
		int dataPort;

		#define DEFAULT_BUFLEN 512
		char recvbuf[DEFAULT_BUFLEN];
		int iResult, iSendResult;

		int SenderAddrSize;
		int recvbuflen;
		bool readyToReceive;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename ListenerType>
	inline void OmicronConnectorClient<ListenerType>::connect(const char* server, int port, int pdataPort) 
	{
		serverAddress = server;
		serverPort = port;
		dataPort = pdataPort;

		char srvPortChr[256];
		sprintf(srvPortChr, "%d", serverPort);

		SOCKET_INIT();
		printf("NetService: Initializing using linux\n");

		struct addrinfo hints, *res;

		// First, load up address structs with getaddrinfo():
		memset(&hints, 0, sizeof hints);
		hints.ai_family = AF_UNSPEC;
		hints.ai_socktype = SOCK_STREAM;

		// Get the server address
		getaddrinfo(serverAddress, srvPortChr, &hints, &res);

		// Generate the socket
		ConnectSocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

		// Connect to server
		int result = ::connect(ConnectSocket, res->ai_addr, res->ai_addrlen);

		if (result == -1) 
		{
			printf("NetService: Unable to connect to server '%s' on port '%d'", serverAddress, serverPort);
			PRINT_SOCKET_ERROR("");
			SOCKET_CLEANUP();
			return;
		}
		else
		{
			printf("NetService: Connected to server '%s' on port '%d'!\n", serverAddress, serverPort);
		}
		initHandshake();

	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename ListenerType>
	inline void OmicronConnectorClient<ListenerType>::initHandshake() 
	{
		char sendbuf[50];
		sprintf(sendbuf, "data_on,%d", dataPort);
		printf("NetService: Sending handshake: '%s'\n", sendbuf);

		iResult = send(ConnectSocket, sendbuf, (int) strlen(sendbuf), 0);

		if (iResult == -1) 
		{
			PRINT_SOCKET_ERROR("NetService: Send failed");
			SOCKET_CLOSE(ConnectSocket);
			SOCKET_CLEANUP()
			return;
		}

		sockaddr_in RecvAddr;
		SenderAddrSize = sizeof(SenderAddr);
		RecvSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		// Bind the socket to any address and the specified port.
		RecvAddr.sin_family = AF_INET;
		RecvAddr.sin_port = htons(dataPort);
		RecvAddr.sin_addr.s_addr = htonl(INADDR_ANY);
		bind(RecvSocket, (const sockaddr*) &RecvAddr, sizeof(RecvAddr));
		readyToReceive = true;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename ListenerType>
	inline void OmicronConnectorClient<ListenerType>::poll()
	{
		if(readyToReceive)
		{
			int result;

			// Create a set of fd_set to store sockets
			fd_set ReadFDs, WriteFDs, ExceptFDs;

			// Set collections to null
			FD_ZERO(&ReadFDs);
			FD_ZERO(&WriteFDs);
			FD_ZERO(&ExceptFDs);

			FD_SET(RecvSocket, &ReadFDs);
			FD_SET(RecvSocket, &ExceptFDs);

			timeout.tv_sec  = 0;
			timeout.tv_usec = 0;

			do
			{
				// Check if UDP socket has data waiting to be read before socket blocks to attempt to read.
				result = select(RecvSocket+1, &ReadFDs, &WriteFDs, &ExceptFDs, &timeout);
				if( result > 0 ) parseDGram(result);
			} while(result > 0);
		}
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename ListenerType>
	inline void OmicronConnectorClient<ListenerType>::dispose() 
	{
		// Close the socket when finished receiving datagrams
		printf("NetService: Finished receiving. Closing socket.\n");
		iResult = SOCKET_CLOSE(RecvSocket);
		if (iResult == -1) 
		{
			PRINT_SOCKET_ERROR("NetService: Closesocket failed");
			return;
		}
		SOCKET_CLEANUP();
		printf("NetService: Shutting down.");
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	template<typename ListenerType>
	inline void OmicronConnectorClient<ListenerType>::parseDGram(int result)
	{
		result = recvfrom(RecvSocket, 
			recvbuf,
			DEFAULT_BUFLEN-1,
			0,
			(sockaddr *)&SenderAddr, 
			(socklen_t*)&SenderAddrSize);
		if(result > 0)
		{
			int offset = 0;
			int msgLen = result - 1;
			char* eventPacket = recvbuf;

			EventData ed;

			OI_READBUF(unsigned int, eventPacket, offset, ed.timestamp); 
			OI_READBUF(unsigned int, eventPacket, offset, ed.sourceId); 
			OI_READBUF(int, eventPacket, offset, ed.serviceId); 
			OI_READBUF(unsigned int, eventPacket, offset, ed.serviceType); 
			OI_READBUF(unsigned int, eventPacket, offset, ed.type); 
			OI_READBUF(unsigned int, eventPacket, offset, ed.flags); 
			OI_READBUF(float, eventPacket, offset, ed.posx); 
			OI_READBUF(float, eventPacket, offset, ed.posy); 
			OI_READBUF(float, eventPacket, offset, ed.posz); 
			OI_READBUF(float, eventPacket, offset, ed.orw); 
			OI_READBUF(float, eventPacket, offset, ed.orx); 
			OI_READBUF(float, eventPacket, offset, ed.ory); 
			OI_READBUF(float, eventPacket, offset, ed.orz); 
		
			OI_READBUF(unsigned int, eventPacket, offset, ed.extraDataType); 
			OI_READBUF(unsigned int, eventPacket, offset, ed.extraDataLength); 
			OI_READBUF(unsigned int, eventPacket, offset, ed.extraDataMask); 
			memcpy(ed.extraData, &eventPacket[offset], EventData::ExtraDataSize);

			ListenerType::onEvent(ed);
		} 
		else 
		{
			PRINT_SOCKET_ERROR("recvfrom failed");
		}
	}
};

#endif
