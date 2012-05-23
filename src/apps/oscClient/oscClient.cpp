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

#include "omicron/OSCService.h"

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
class OSCClientTest
{
public:
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Checks the type of event. If a valid event, creates an event packet and returns true. Else return false.
	virtual void handleEvent(const Event& evt)
	{
		// If the event has been processed locally (i.e. by a filter event service)
		if(evt.isProcessed()) return;
		
		if( evt.getServiceType() == Service::Controller ){
			//printf("Controller %f %f \n", evt.getExtraDataFloat(0), evt.getExtraDataFloat(1) );

			float pos = evt.getExtraDataFloat(0) / 1000.0;
			float amp = -(evt.getExtraDataFloat(3) - 1000) / 2000.0;
			printf("Pos %f Amp %f \n", pos, amp );
			
			if( evt.isButtonDown(Event::Button1) ){
				Message msg("/s_new");
				msg.pushStr("PlaySound");
				msg.pushInt32(-1);
				msg.pushInt32(0);
				msg.pushInt32(0);
				msg.pushStr("amp");
				msg.pushFloat(amp);

				//osc->sendOSCMessage(msg);
			}
		}
		if( evt.getServiceType() == Service::Mocap ){
			//printf("Controller %f %f \n", evt.getExtraDataFloat(0), evt.getExtraDataFloat(1) );

			float xPos = evt.getPosition(0);
			float yPos = evt.getPosition(1);
			float zPos = evt.getPosition(2);
			float angle = atan( xPos / zPos ) * ( 180 / 3.14159 );
			printf("Angle %f \n", angle );
			osc->updateSoundAngle( 0, angle, true );
		}
	}
	

	void addServiceManager(ServiceManager* sm);

private:
	OSCClient* osc;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCClientTest::addServiceManager( ServiceManager* sm )
{
	osc->setServiceManager(sm);
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main(int argc, char** argv)
{
	OSCClientTest app;

	// Read config file name from command line or use default one.
	const char* cfgName = "oscTest.cfg";
	if(argc == 2) cfgName = argv[1];

	Config* cfg = new Config(cfgName);

	DataManager* dm = DataManager::getInstance();
	// Add a default filesystem data source using current work dir.
	dm->addSource(new FilesystemDataSource("./"));
	dm->addSource(new FilesystemDataSource(OMICRON_DATA_PATH));

	ServiceManager* sm = new ServiceManager();
	sm->setupAndStart(cfg);

	app.addServiceManager(sm);

	float delay = -0.01f; // Seconds to delay sending events (<= 0 disables delay)
#ifdef _DEBUG
	bool printOutput = true;
#else
	bool printOutput = false;
#endif

	while(true)
	{
		sm->poll();

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
			//if( printOutput )
			//	printf("------------------------------------------------------------------------------\n");
		}
	}

	sm->stop();
	delete sm;
	delete cfg;
	delete dm;
}
