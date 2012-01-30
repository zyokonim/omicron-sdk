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
#endif

#ifdef OMICRON_OS_WIN
	#include <winsock2.h>
	#include <ws2tcpip.h>
#else
	#include <stdio.h>
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

namespace omicron
{
	//////////////////////////////////////////////////////////////////////////////////////////////////
	class NetService: public Service
	{
	public:
		// Allocator function
		NetService();
		static NetService* New() { return new NetService(); }

	public:
		virtual void setup(Setting& settings);
		virtual void initialize();
		virtual void poll();
		virtual void dispose();
		void setServer(const char*,const char*);
		void setDataport(const char*);
		void setScreenResolution(int,int);
		void setTouchTimeout(float);
	private:
		void initHandshake();
		void parseDGram(int);
	private:
		NetService* mysInstance;

	#ifdef OMICRON_OS_WIN	
		WSADATA wsaData;
	#endif

		SOCKET ConnectSocket;
		SOCKET RecvSocket;
		struct timeval timeout;
		sockaddr_in SenderAddr;

		const char* serverAddress;
		const char* serverPort;
		const char* dataPort;

		#define DEFAULT_BUFLEN 512
		char recvbuf[DEFAULT_BUFLEN];
		int iResult, iSendResult;

		int SenderAddrSize;
		int recvbuflen;
		bool readyToReceive;
	};

};

#endif
