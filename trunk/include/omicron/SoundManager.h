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
#ifndef __SOUND__MANAGER_H__
#define __SOUND__MANAGER_H__

#include "omicron/osystem.h"
#include "omicron/StringUtils.h"
#include "osc/oscpkt.h"
#include "osc/udp.h"
#include "Sound.h"

using namespace oscpkt;
using namespace std;

namespace omicron
{

class SoundManager;
class Sound;
class SoundInstance;

class OMICRON_API SoundEnvironment
{
public:
	SoundEnvironment(SoundManager*);
	void dispose();
	void setVolume(float);
	float getVolume();

	Sound* createSound(char*);
	Sound* getSound(char*);
	SoundInstance* createInstance(Sound*);
	SoundInstance* getSoundInstance(int);
private:
	static SoundManager* soundManager;
	static float globalVolume;
	map<int,Sound*> soundList;
	map<char*,int> soundBufferIDList;
	map<int,SoundInstance*> soundInstanceList;
};// SoundEnvironment

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class OMICRON_API SoundManager
{
public:
	SoundManager();
	SoundManager(String, int);
	SoundManager(const char*, int);
	void connectToServer(const char*, int);
	bool isSoundServerRunning();
	SoundEnvironment* getSoundEnvironment();
	void setEnvironment(SoundEnvironment*);
	Vector3f getListenerPosition();
	void setListenerPosition(Vector3f);

	void startSoundServer();
	void stopSoundServer();

	bool sendOSCMessage(Message);
private:
	static SoundEnvironment* environment;
	static UdpSocket serverSocket;
	
	static Vector3f listenerPosition;
};// SoundManager

}; // namespace omicron

#endif

