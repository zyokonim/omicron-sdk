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

#include "omicron/Sound.h"

using namespace omicron;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SoundManager* Sound::manager;
int nextBufferID = 0;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Sound::Sound(char* soundName)
{
	this->soundName = soundName;
	bufferID = nextBufferID;
	nextBufferID++;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Sound::loadFromFile(const char* filePath)
{
	printf( "%s: Not fully implemented\n", __FUNCTION__);
	this->filePath = filePath;

	Message msg("/loadBuffer");
	msg.pushInt32(this->getBufferID());
	msg.pushStr(this->getFilePath());
	manager->sendOSCMessage(msg);
	printf("Loaded buffer ID %d with path %s \n", this->getBufferID(), this->getFilePath());
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Sound::loadFromMemory(const void* buf, size_t length)
{
	printf( "%s: Not yet implemented\n", __FUNCTION__);
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Sound::getDuration()
{
	printf( "%s: Not yet implemented\n", __FUNCTION__);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Sound::setVolumeScale(float volume)
{
	printf( "%s: Not yet implemented\n", __FUNCTION__);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float Sound::getVolumeScale()
{
	printf( "%s: Not yet implemented\n", __FUNCTION__);
	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Sound::setSoundManager(SoundManager* manager)
{
	this->manager = manager;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SoundManager* Sound::getSoundManager()
{
	return manager;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int Sound::getBufferID()
{
	return bufferID;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SoundInstance* Sound::play(){
	SoundInstance* newInstance = new SoundInstance(this);
	newInstance->play();
	return newInstance;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const char* Sound::getFilePath(){
	return filePath;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SoundManager* SoundInstance::soundManager;
int nextInstanceID = 4001;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
SoundInstance::SoundInstance(Sound* sound)
{
	this->sound = sound;
	instanceID = nextInstanceID; // This should be globally incremented on each new instance. Must be 1001 or greater.
	nextInstanceID++;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SoundInstance::setLoop(bool value)
{
	printf( "%s: Not yet implemented\n", __FUNCTION__);
	loop = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SoundInstance::getLoop()
{
	return loop;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SoundInstance::play()
{
	printf( "%s: Playing buffer %d with instanceID: %d\n", __FUNCTION__, sound->getBufferID(), instanceID);
	Message msg("/play");
	msg.pushInt32(instanceID);
	msg.pushInt32(sound->getBufferID());

	msg.pushFloat(volume);

	msg.pushFloat( position[0] );
	msg.pushFloat( position[1] );
	msg.pushFloat( position[2] );
	
	Vector3f audioListener = soundManager->getListenerPosition();
	msg.pushFloat( audioListener[0] );
	msg.pushFloat( audioListener[1] );
	msg.pushFloat( audioListener[2] );

	soundManager->sendOSCMessage(msg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SoundInstance::pause()
{
	printf( "%s: Not yet implemented\n", __FUNCTION__);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SoundInstance::stop()
{
	printf( "%s: Freeing instanceID: %d\n", __FUNCTION__, instanceID);
	Message msg("/freeNode");
	msg.pushInt32(instanceID);
	soundManager->sendOSCMessage(msg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SoundInstance::isPlaying()
{
	printf( "%s: Not yet implemented\n", __FUNCTION__);
	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SoundInstance::setPosition(Vector3f pos)
{
	position = pos;

	Message msg("/setObjectLoc");
	msg.pushInt32(instanceID);

	msg.pushFloat( position[0] );
	msg.pushFloat( position[1] );
	msg.pushFloat( position[2] );

	soundManager->sendOSCMessage(msg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Vector3f SoundInstance::getPosition()
{
	return position;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool SoundInstance::isEnvironmentSound()
{
	return environmentSound;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SoundInstance::setEnvironmentSound(bool value)
{
	environmentSound = value;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SoundInstance::setVolume(float value)
{
	this->volume = value;

	printf( "%s: for instanceID: %d\n", __FUNCTION__, instanceID);
	Message msg("/setVol");
	msg.pushInt32(instanceID);
	msg.pushFloat(this->volume);
	soundManager->sendOSCMessage(msg);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
float SoundInstance::getVolume()
{
	return volume;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void SoundInstance::setSoundManager(SoundManager* manager)
{
	this->soundManager = manager;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int SoundInstance::getID()
{
	return instanceID;
}