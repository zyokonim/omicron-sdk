/********************************************************************************************************************** 
* THE OMICRON PROJECT
 *---------------------------------------------------------------------------------------------------------------------
 * Copyright 2010-2012							Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Arthur Nishimoto								anishimoto42@gmail.com
 *---------------------------------------------------------------------------------------------------------------------
 * Copyright (c) 2010-2011, Electronic Visualization Laboratory, University of Illinois at Chicago
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
#ifndef __OSC__SERVICE_H__
#define __OSC__SERVICE_H__

#include "omicron/osystem.h"
#include "omicron/ServiceManager.h"
#include "osc/oscpkt.h"
#include "osc/udp.h"

class OscManager;

namespace omicron
{
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class OMICRON_API OSCService: public Service
{
public:
	// Allocator function
	static OSCService* New() { return new OSCService(); }

public:
	void setup(Setting& settings);
	virtual void initialize();
	virtual void poll();
	virtual void dispose();
	
	void connectToOSCServer();
private:
	static OSCService* mysInstance;
	static const char* serverIP;
	static int serverPort;
};

}; // namespace omicron

#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Testing OSC client example - Only here for reference until OSCService is fully implemented
using namespace omicron;
using namespace oscpkt;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//OscManager* OscManager::mysInstance = NULL;
const int PORT_NUM = 9109;

class OscManager
{
public:
	///////////////////////////////////////////////////////////////////////////////////////////////
	OscManager()
	{
	}

	void connectToServer()
	{

	}
	void runClientTest()
	{
		UdpSocket sock;
		sock.connectTo("localhost", PORT_NUM);
		if (!sock.isOk()) {
			printf( "OscManager::Error connection to port %d: %s\n", PORT_NUM, sock.errorMessage() );
		} else {
			printf( "OscManager::Client started, will send packets to port %d\n", PORT_NUM );
			int iping = 1;
			while (sock.isOk()) {
				Message msg("/ping"); msg.pushInt32(iping);
				PacketWriter pw;
				pw.startBundle().startBundle().addMessage(msg).endBundle().endBundle();
				bool ok = sock.sendPacket(pw.packetData(), pw.packetSize());
				printf( "OscManager::Client: sent /ping %d, ok=%d \n" , iping++ , ok );
				// wait for a reply ?
				if (sock.receiveNextPacket(30 /* timeout, in ms */)) {
					PacketReader pr(sock.packetData(), sock.packetSize());
					Message *incoming_msg;
					while (pr.isOk() && (incoming_msg = pr.popMessage()) != 0) {
						printf( "OscManager::Client: received %s\n", *incoming_msg );
					}
				}
			}
			printf( "OscManager::sock error: %s -- is the server running?\n", sock.errorMessage() );
		}
	}
	/*
	void runTestServer() {
	  UdpSocket sock; 
	  sock.bindTo(PORT_NUM);
	  if (!sock.isOk()) {
		  cerr << "Error opening port " << PORT_NUM << ": " << sock.errorMessage() << "\n";
	  } else {
		cout << "Server started, will listen to packets on port " << PORT_NUM << std::endl;
		PacketReader pr;
		PacketWriter pw;
		while (sock.isOk()) {      
		  if (sock.receiveNextPacket(30 )) { // timeout, in ms
			pr.init(sock.packetData(), sock.packetSize());
			oscpkt::Message *msg;
			while (pr.isOk() && (msg = pr.popMessage()) != 0) {
			  int iarg;
			  if (msg->match("/ping").popInt32(iarg).isOkNoMoreArgs()) {
				cout << "Server: received /ping " << iarg << " from " << sock.packetOrigin() << "\n";
				Message repl; repl.init("/pong").pushInt32(iarg+1);
				pw.init().addMessage(repl);
				sock.sendPacketTo(pw.packetData(), pw.packetSize(), sock.packetOrigin());
			  } else if (msg->match("/OmegaOSCPing").popInt32(iarg).isOkNoMoreArgs()) {
				cout << "Server: received /OmegaOSCPing " << iarg << " from " << sock.packetOrigin() << "\n";
				//Message repl; repl.init("/pong").pushInt32(iarg+1);
				//pw.init().addMessage(repl);
				//sock.sendPacketTo(pw.packetData(), pw.packetSize(), sock.packetOrigin());
			  } else {
				cout << "Server: unhandled message: " << *msg << "\n";
			  }
			}
		  }
		}
	  }
	}
	*/
private:

};