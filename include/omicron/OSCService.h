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
#include "omicron/StringUtils.h"
#include "osc/oscpkt.h"
#include "osc/udp.h"

class OscManager;

using namespace oscpkt;

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
	void connectToOSCServer(char*, int);
	bool sendOSCMessage(Message);
private:
	static OSCService* mysInstance;
	static const char* serverIP;
	static int serverPort;

	static UdpSocket sock;
};

class OMICRON_API OSCClient: public Service
{
public:
	// Allocator function
	static OSCClient* New() { return new OSCClient(); }

public:
	void setup(Setting& settings);
	virtual void initialize();
	virtual void poll();
	virtual void dispose();

	void setServiceManager(ServiceManager* sm);
	void updateSoundPosition(int, float, float, float, bool);
	void updateSoundAngle(int, float, bool);
private:
	static OSCClient* mysInstance;
	static ServiceManager* serviceManager;
};

}; // namespace omicron

#endif

