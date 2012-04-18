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
	printf("OSCService: Initialize\n");
	mysInstance = this;

	connectToOSCServer();

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCService::poll() 
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCService::dispose() 
{
	mysInstance = NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void OSCService::connectToOSCServer()
{
	// Create socket and connect to OSC server
	UdpSocket sock;
	sock.connectTo(serverIP, serverPort);
	if (!sock.isOk()) {
		ofmsg( "OSCService: Error connection to port %1%: %2%", %serverPort %sock.errorMessage() );
	} else {
		ofmsg( "OSCService: Client started, will send packets to port %1%", %serverPort );
		
		// Send a test message to server
		int iping = 1;
		Message msg("/OmegaOSCPing"); msg.pushInt32(iping);
		PacketWriter pw;
		pw.startBundle().startBundle().addMessage(msg).endBundle().endBundle();
		bool ok = sock.sendPacket(pw.packetData(), pw.packetSize());
	}
}