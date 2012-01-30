/**************************************************************************************************
* THE OMICRON PROJECT
 *-------------------------------------------------------------------------------------------------
 * Copyright 2010-2012		Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
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
#include <omicron.h>
#include <vector>

#include <time.h>
using namespace omicron;

#ifdef WIN32
#include <ws2tcpip.h>
#include <winsock2.h>
#define itoa _itoa
#endif


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Based on Winsock UDP Server Example:
// http://msdn.microsoft.com/en-us/library/ms740148
class NetClient
{
private:
	WSADATA wsaData;
	SOCKET SendSocket;
	sockaddr_in RecvAddr;
	int Port;
	//char SendBuf[1024];
	int BufLen;

public:
	NetClient::NetClient( const char* address, int port )
	{
		// Create a socket for sending data
		SendSocket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);

		// Set up the RecvAddr structure with the IP address of
		// the receiver
		RecvAddr.sin_family = AF_INET;
		RecvAddr.sin_port = htons(port);
		RecvAddr.sin_addr.s_addr = inet_addr(address);
		printf("NetClient %s:%i created...\n", address, port);
	}// CTOR

	void NetClient::sendEvent(char* eventPacket, int length)
	{
		// Send a datagram to the receiver
		sendto(SendSocket, 
			eventPacket, 
			length, 
			0, 
			(SOCKADDR*) &RecvAddr, 
			sizeof(RecvAddr));
	}// SendEvent
};

#define OI_WRITEBUF(type, buf, offset, val) *((type*)&buf[offset]) = val; offset += sizeof(type);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class OInputServer
{
public:
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Checks the type of event. If a valid event, creates an event packet and returns true. Else return false.
	virtual void handleEvent(const Event& evt)
	{
		int offset = 0;
		OI_WRITEBUF(bool, eventPacket, offset, evt.isProcessed()); 
		OI_WRITEBUF(unsigned int, eventPacket, offset, evt.getTimestamp()); 
		OI_WRITEBUF(unsigned int, eventPacket, offset, evt.getSourceId()); 
		OI_WRITEBUF(int, eventPacket, offset, evt.getServiceId()); 
		OI_WRITEBUF(unsigned int, eventPacket, offset, evt.getServiceType()); 
		OI_WRITEBUF(unsigned int, eventPacket, offset, evt.getType()); 
		OI_WRITEBUF(unsigned int, eventPacket, offset, evt.getFlags()); 
		OI_WRITEBUF(float, eventPacket, offset, evt.getPosition(0)); 
		OI_WRITEBUF(float, eventPacket, offset, evt.getPosition(1)); 
		OI_WRITEBUF(float, eventPacket, offset, evt.getPosition(2)); 
		OI_WRITEBUF(float, eventPacket, offset, evt.getOrientation().x()); 
		OI_WRITEBUF(float, eventPacket, offset, evt.getOrientation().y()); 
		OI_WRITEBUF(float, eventPacket, offset, evt.getOrientation().z()); 
		OI_WRITEBUF(float, eventPacket, offset, evt.getOrientation().w()); 
		
		OI_WRITEBUF(unsigned int, eventPacket, offset, evt.getExtraDataType()); 
		OI_WRITEBUF(unsigned int, eventPacket, offset, evt.getExtraDataLength()); 
		if(evt.getExtraDataType() != Event::ExtraDataNull)
		{
			memcpy(&eventPacket[offset], evt.getExtraDataBuffer(), evt.getExtraDataSize());
		}
		offset += evt.getExtraDataLength();
		std::map<char*,NetClient*>::iterator itr = netClients.begin();
		while( itr != netClients.end() )
		{
			NetClient* client = itr->second;
			client->sendEvent(eventPacket, offset);
			itr++;
		}
	}
	
	void startConnection();
	SOCKET startListening();

private:
	void createClient(const char*,int);

	WSADATA wsaData;
	const char* serverPort;
	SOCKET ListenSocket;
	
	#define DEFAULT_BUFLEN 512
	char eventPacket[DEFAULT_BUFLEN];

	char recvbuf[DEFAULT_BUFLEN];
	int iResult, iSendResult;
	int recvbuflen;
	
	// Collection of unique clients (IP/port combinations)
	std::map<char*,NetClient*> netClients;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OInputServer::startConnection()
{
	serverPort = "27000";
	ListenSocket = INVALID_SOCKET;
	recvbuflen = DEFAULT_BUFLEN;
	int iResult;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
	if (iResult != 0) 
	{
		oferror("OInputServer: WSAStartup failed: %1%", %iResult);
		return;
	} 
	else 
	{
		oerror("OInputServer: Winsock initialized");
	}

	struct addrinfo *result = NULL, *ptr = NULL, hints;

	ZeroMemory(&hints, sizeof (hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the local address and port to be used by the server
	iResult = getaddrinfo(NULL, serverPort, &hints, &result);
	if (iResult != 0) 
	{
		ofmsg("OInputServer: getaddrinfo failed: %1%", %iResult);
		WSACleanup();
	} 
	else 
	{
		ofmsg("OInputServer: Server set to listen on port %1%", %serverPort);
	}

	// Create a SOCKET for the server to listen for client connections
	ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

	// If iMode != 0, non-blocking mode is enabled.
	u_long iMode = 1;
	ioctlsocket(ListenSocket,FIONBIO,&iMode);

	if (ListenSocket == INVALID_SOCKET) 
	{
		printf("OInputServer: Error at socket(): %ld\n", WSAGetLastError());
		freeaddrinfo(result);
		WSACleanup();
		return;
	} 
	else 
	{
		printf("OInputServer: Listening socket created.\n");
	}

	// Setup the TCP listening socket
	iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
	if (iResult == SOCKET_ERROR) 
	{
		printf("OInputServer: bind failed: %d\n", WSAGetLastError());
		freeaddrinfo(result);
		closesocket(ListenSocket);
		WSACleanup();
		return;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SOCKET OInputServer::startListening()
{
	SOCKET ClientSocket;

	// Listen on socket
	if ( listen( ListenSocket, SOMAXCONN ) == SOCKET_ERROR )
	{
		printf( "OInputServer: Error at bind(): %ld\n", WSAGetLastError() );
		closesocket(ListenSocket);
		WSACleanup();
		return NULL;
	} 
	else
	{
		//printf("NetService: Listening on socket.\n");
	}

	ClientSocket = INVALID_SOCKET;
	sockaddr_in clientInfo;
	int ret = sizeof(struct sockaddr);
	const char* clientAddress;

	// Accept a client socket
	ClientSocket = accept(ListenSocket, (SOCKADDR*)&clientInfo, &ret);

	if (ClientSocket == INVALID_SOCKET) 
	{
		//printf("NetService: accept failed: %d\n", WSAGetLastError());
		// Commented out: We do not want to close the listen socket
		// since we are using a non-blocking socket until we are done listening for clients.
		//closesocket(ListenSocket);
		//WSACleanup();
		return NULL;
	} 
	else 
	{
		// Gets the clientInfo and extracts the IP address
		clientAddress = inet_ntoa(clientInfo.sin_addr);
		printf("OInputServer: Client '%s' Accepted.\n", clientAddress);
	}
	
	// Wait for client handshake
	// Here we constantly loop until data is received.
	// Because we're using a non-blocking socket, it is possible to attempt to receive before data is
	// sent, resulting in the 'recv failed' error that is commented out.
	bool gotData = false;
	float timer = 0.0f;
	float timeout = 500.0f; // milliseconds
	time_t startTime = time (NULL);

	printf("OInputServer: Waiting for client handshake\n");
	do 
	{
		iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
		if (iResult > 0)
		{
			//printf("Service: Bytes received: %d\n", iResult);
			char* inMessage;
			char* portCStr;
			inMessage = new char[iResult];
			portCStr = new char[iResult];

			// Iterate through message string and
			// separate 'data_on,' from the port number
			int portIndex = -1;
			for( int i = 0; i < iResult; i++ )
			{
				if( recvbuf[i] == ',' )
				{
					portIndex = i + 1;
				} 
				else if( i < portIndex )
				{
					inMessage[i] = recvbuf[i];
				} 
				else 
				{
					portCStr[i-portIndex] = recvbuf[i];
				}
			}

			// Make sure handshake is correct
			String handshake = "data_on";
			int dataPort = 7000; // default port
			if( handshake.find(inMessage) )
			{
				// Get data port number
				dataPort = atoi(portCStr);
				printf("OInputServer: '%s' requests data to be sent on port '%d'\n", clientAddress, dataPort);
				createClient( clientAddress, dataPort );
			}
			gotData = true;
			delete inMessage;
			delete portCStr;
		} 
		else if (iResult == 0)
		{
			printf("OInputServer: Connection closing...\n");
		}
		else 
		{
			timer = time (NULL);
			if( timer > startTime + timeout )
			{
				printf("OInputServer: Handshake timed out\n");
				break;
			}
		}
	} while (!gotData);
	

	return ClientSocket;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OInputServer::createClient(const char* clientAddress, int dataPort)
{
	// Generate a unique name for client "address:port"
	char* addr = new char[128];
	strcpy( addr, clientAddress );
	char buf[32];
	strcat( addr, ":" );
	strcat( addr, itoa(dataPort,buf,10) );
	
	// Iterate through client map. If client name already exists,
	// do not add to list.
	std::map<char*, NetClient*>::iterator p;
	for(p = netClients.begin(); p != netClients.end(); p++) 
	{
		printf( "%s \n", p->first );
		if( strcmp(p->first, addr) == 0 )
		{
			printf("OInputServer: NetClient already exists: %s \n", addr );
			return;
		}
	}

	netClients[addr] = new NetClient( clientAddress, dataPort );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main(int argc, char** argv)
{
	OInputServer app;

	// Read config file name from command line or use default one.
	const char* cfgName = "oinputserver.cfg";
	if(argc == 2) cfgName = argv[1];

	Config* cfg = new Config(cfgName);

	DataManager* dm = DataManager::getInstance();
	// Add a default filesystem data source using current work dir.
	dm->addSource(new FilesystemDataSource("./"));
	dm->addSource(new FilesystemDataSource(OMICRON_DATA_PATH));

	ServiceManager* sm = new ServiceManager();
	sm->setupAndStart(cfg);

	app.startConnection();
	
	float delay = -0.01f; // Seconds to delay sending events (<= 0 disables delay)
	bool printOutput = true;

	omsg("OInputServer: Starting to listen for clients...");
	while(true)
	{
		// TODO: Use StopWatch
		if( delay > 0.0 )
			Sleep(1000.0*delay); // Delay sending of data out

		sm->poll();

		// Start listening for clients (non-blocking)
		app.startListening();

		// Get events
		int av = sm->getAvailableEvents();
		if(av != 0)
		{
			// TODO: Instead of copying the event list, we can lock the main one.
			Event evts[OMICRON_MAX_EVENTS];
			sm->getEvents(evts, OMICRON_MAX_EVENTS);
			for( int evtNum = 0; evtNum < av; evtNum++)
			{
				app.handleEvent(evts[evtNum]);
			}
			if( printOutput )
				printf("------------------------------------------------------------------------------\n");
		}
	}

	sm->stop();
	delete sm;
	delete cfg;
	delete dm;
}
