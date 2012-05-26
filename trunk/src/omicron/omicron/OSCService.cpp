/********************************************************************************************************************** 
* THE OMICRON PROJECT
 *---------------------------------------------------------------------------------------------------------------------
 * Copyright 2010								Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Arthur Nishimoto							anishimoto42@gmail.com
 *---------------------------------------------------------------------------------------------------------------------
 * Copyright (c) 2010, Electronic Visualization Laboratory, University of Illinois at Chicago
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are permitted provided that the 
 * following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this list of conditions and the following 
 * disclaimer. Redistributions in binary form must reproduce the above copyright notice, this list of conditions 
 * and the following disclaimer in the documentation and/or other materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, 
 * INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE 
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE  GOODS OR 
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE 
 * USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *********************************************************************************************************************/

#include "omicron/OSCService.h"

using namespace omicron;
using namespace oscpkt;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OSCService* OSCService::mysInstance = NULL;
const char* OSCService::serverIP = "localhost";
int OSCService::serverPort = 9109;
UdpSocket OSCService::sock;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCService::setup(Setting& settings)
{
	
	if( settings.exists("serverIP") ){
		serverIP =  (const char*)settings["serverIP"];
	}
	if( settings.exists("serverPort") ){
		serverPort = settings["serverPort"];
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCService::initialize() 
{
	omsg("OSCService: Initialize\n");
	mysInstance = this;

	connectToOSCServer();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCService::poll() 
{
	mysInstance->lockEvents();
	ServiceManager* sm = mysInstance->getManager();
	int eventListSize = sm->getAvailableEvents();

	for( int i = 0; i < eventListSize; i++ )
	{
		Event* evt = sm->getEvent(i);
		if( evt->getServiceType() == Service::Audio )
		{
			switch( evt->getType() )
			{
				case( Event::Update ):
					float angle = evt->getExtraDataFloat(0);

					Message msg("/OmegaUpdateAngle");
					msg.pushFloat(angle);

					sendOSCMessage(msg);
					break;
			}
		}
	}
	mysInstance->unlockEvents();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCService::dispose() 
{
	mysInstance = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCService::connectToOSCServer(char* serverIP, int serverPort)
{
	// Create socket and connect to OSC server
	sock.connectTo(serverIP, serverPort);
	if (!sock.isOk()) {
		ofmsg( "OSCService: Error connection to port %1%: %2%", %serverPort %sock.errorMessage() );
	} else {
		ofmsg( "OSCService: Client started, will send packets to %1% on port %2%", %serverIP %serverPort );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCService::connectToOSCServer()
{
	// Create socket and connect to OSC server
	sock.connectTo(serverIP, serverPort);
	if (!sock.isOk()) {
		ofmsg( "OSCService: Error connection to port %1%: %2%", %serverPort %sock.errorMessage() );
	} else {
		ofmsg( "OSCService: Client started, will send packets to %1% on port %2%", %serverIP %serverPort );
		
		// Send a test message to server
		//int iping = 1;
		Message msg("/OmegaOSCPing");
		msg.pushStr("InitTest");
		msg.pushInt32(1);
		msg.pushFloat(2.0);
		msg.pushDouble(3.0);
		sendOSCMessage(msg);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool OSCService::sendOSCMessage(Message msg)
{
	PacketWriter pw;
	pw.startBundle().addMessage(msg).endBundle();
	return sock.sendPacket(pw.packetData(), pw.packetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// OSCClient
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OSCClient* OSCClient::mysInstance = NULL;
ServiceManager* OSCClient::serviceManager = NULL;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCClient::setup(Setting& settings)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCClient::initialize() 
{
	mysInstance = this;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCClient::poll() 
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCClient::dispose() 
{
	mysInstance = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCClient::setServiceManager(ServiceManager* sm)
{
	serviceManager = sm;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCClient::updateSoundPosition(int objectID, float xPos, float yPos, float zPos, bool isPlaying)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Update sound via Kinect horizontal angle. i.e. 'angle = atan( xPos / yPos ) * (180/PI)'
void OSCClient::updateSoundAngle(int objectID, float angle, bool isPlaying)
{
	serviceManager->lockEvents();

	Event* evt = serviceManager->writeHead();
	evt->reset( Event::Update, Service::Audio, objectID );

	evt->setExtraDataType(Event::ExtraDataFloatArray);
	evt->setExtraDataFloat(0, angle);

	serviceManager->unlockEvents();
}