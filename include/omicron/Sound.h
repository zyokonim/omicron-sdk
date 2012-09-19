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
#ifndef __SOUND_H__
#define __SOUND_H__

#include "omicron/osystem.h"
#include "omicron/StringUtils.h"


#include "omicron/SoundManager.h"

namespace omicron
{

class SoundManager;
class SoundInstance;

class OMICRON_API Sound
{
public:
	Sound(char*);
	bool loadFromFile(const char*);
	bool loadFromMemory(const void*,size_t);
	float getDuration();
	void setVolumeScale(float);
	float getVolumeScale();
	
	int getBufferID();
	const char* getFilePath();

	// Temp?
	void setSoundManager(SoundManager*);
	SoundManager* getSoundManager();

	SoundInstance* play();
private:

public:
private:
	char* soundName;
	const char* filePath;
	int bufferID;
	float duration;
	float volumeScale;

	// Temp?
	static SoundManager* manager;
};// Sound

class OMICRON_API SoundInstance
{
public:
	SoundInstance(Sound*);

	void setLoop(bool);
	bool getLoop();
	void play();
	void pause();
	void stop();
	bool isPlaying();
	
	void setPosition(Vector3f);
	Vector3f getPosition();
	bool isEnvironmentSound();
	void setEnvironmentSound(bool);
	void setVolume(float);
	float getVolume();
	
	int getID();

	// Temp?
	void setSoundManager(SoundManager*);
private:
	
public:
private:
	Sound* sound;
	int instanceID;

	enum State {playing, paused, stopped};
	State playState;

	bool loop;
	bool environmentSound;
	float volume;
	float pitch;
	Vector3f position;

	// Temp?
	static SoundManager* soundManager;
};// Sound

}; // namespace omicron

#endif

