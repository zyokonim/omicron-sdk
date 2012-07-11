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

#include "omicron/SoundManager.h"

using namespace omicron;
using namespace oscpkt;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
UdpSocket SoundManager::serverSocket;
SoundEnvironment* SoundManager::environment;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SoundManager::SoundManager()
{
	environment = new SoundEnvironment(this);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SoundManager::SoundManager(char* serverIP, int serverPort)
{
	environment = new SoundEnvironment(this);
	connectToServer(serverIP, serverPort);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SoundManager::connectToServer(char* serverIP, int serverPort)
{
	// Create socket and connect to OSC server
	serverSocket.connectTo(serverIP, serverPort);
	if (!serverSocket.isOk()) {
		ofmsg( "SoundManager: Error connection to port %1%: %2%", %serverPort %serverSocket.errorMessage() );
	} else {
		ofmsg( "SoundManager: Connected to server, will send messages to %1% on port %2%", %serverIP %serverPort );
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SoundManager::startSoundServer()
{
	Message msg("/startServer");
	sendOSCMessage(msg);

	Message msg2("/loadSynth");
	sendOSCMessage(msg2);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SoundManager::stopSoundServer()
{
	Message msg("/killServer");
	sendOSCMessage(msg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SoundManager::isSoundServerRunning()
{
	printf( "%s: Not yet implemented\n", __FUNCTION__);

	// Message the server and inquire if the sound server is running
	serverSocket.receiveNextPacket(-1); // Paramater is timeout in milliseconds. -1 (default) will wait indefinatly 

	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SoundEnvironment* SoundManager::getSoundEnvironment()
{
	return environment;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SoundManager::setEnvironment(SoundEnvironment* newEnv)
{
	environment = newEnv;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3f SoundManager::getListenerPosition()
{
	printf( "%s: Not yet implemented\n", __FUNCTION__);
	return Vector3f(0,0,0);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SoundManager::setListenerPosition(Vector3f)
{
	printf( "%s: Not yet implemented\n", __FUNCTION__);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SoundManager::sendOSCMessage(Message msg)
{
	PacketWriter pw;
	pw.startBundle().addMessage(msg).endBundle();
	return serverSocket.sendPacket(pw.packetData(), pw.packetSize());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SoundManager* SoundEnvironment::soundManager;
float SoundEnvironment::globalVolume = 0.5;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SoundEnvironment::SoundEnvironment(SoundManager* soundManager)
{
	this->soundManager = soundManager;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Sound* SoundEnvironment::createSound(char* soundName)
{
	printf( "%s: Not fully implemented\n", __FUNCTION__);
	Sound* newSound = new Sound(soundName);
	newSound->setSoundManager(soundManager);

	soundList[newSound->getBufferID()] = newSound;

	return newSound;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SoundInstance* SoundEnvironment::createInstance(Sound* sound)
{
	printf( "%s: Not fully implemented\n", __FUNCTION__);

	SoundInstance* newInstance = new SoundInstance(sound);

	soundInstanceList[newInstance->getID()] = newInstance;

	return newInstance;
}
