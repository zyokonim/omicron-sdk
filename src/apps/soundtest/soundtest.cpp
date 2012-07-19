/**************************************************************************************************
* THE OMICRON PROJECT
 *-------------------------------------------------------------------------------------------------
 * Copyright 2010-2012		Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Arthur Nishimoto		anishimoto42@gmail.com
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

#include "omicron/SoundManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class SoundTest
{
private:
	Vector3f audioListener;

	SoundManager* soundManager;
	SoundEnvironment* env;

	Sound* testSound;
	Sound* backgroundMusic;
	Sound* instantSound;

	SoundInstance* soundInstance;
	bool instanceCreated;
public:


	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	SoundTest()
	{
		//soundManager = new SoundManager();
		//soundManager->connectToServer("131.193.77.211",57120);
		
		// More concise method of above two lines
		//soundManager = new SoundManager("131.193.77.51",57120);
		soundManager = new SoundManager("131.193.77.141",57120);
		//soundManager = new SoundManager("localhost",57120);
		soundManager->startSoundServer();

		// Get default sound environment
		env = soundManager->getSoundEnvironment();

		// Load sound assets
		if( soundManager->isSoundServerRunning() ){
			backgroundMusic = env->createSound("wind");
			backgroundMusic->loadFromFile("/wind.wav");

			instantSound = env->createSound("gunshot");
			instantSound->loadFromFile("/gun.wav");
		}

		
		instanceCreated = false;
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Checks the type of event. If a valid event, creates an event packet and returns true. Else return false.
	virtual bool handleEvent(const Event& evt)
	{
		float leftRightAnalog;
		float upDownAnalog;
		float zeroTolerence = 0.008f;
		float xPos;
		float yPos;
		float zPos;

		switch(evt.getServiceType())
		{
			case Service::Controller:
				leftRightAnalog = evt.getExtraDataFloat(0) / 1000.0f;
				upDownAnalog = evt.getExtraDataFloat(1) / 1000.0f;

				volume = (1000 + evt.getExtraDataFloat(4)) / 2000.0f;
				
				if( evt.getType() == Event::Down ){

					if( evt.getFlags() == Event::Button3){ // Cross
						backgroundMusic = env->createSound("wind");
						backgroundMusic->loadFromFile("/wind.wav");

						instantSound = env->createSound("gunshot");
						instantSound->loadFromFile("/gun.wav");
					}
					if( evt.getFlags() == Event::Button2){ // Circle
						
					}
					
					if( evt.getFlags() == Event::Button5){ // L1
						SoundInstance* gun = new SoundInstance(instantSound);
						gun->play();		
					}

					if( evt.getFlags() == Event::ButtonRight){
						//soundInstance = new SoundInstance(backgroundMusic);
						//soundInstance->play();
						
						// Short version of above
						soundInstance = backgroundMusic->play();

						instanceCreated = true;
					}
					if( evt.getFlags() == Event::ButtonLeft){
						soundInstance->pause();
					}
					if( evt.getFlags() == Event::ButtonUp){
						soundInstance->stop();
					}
					if( evt.getFlags() == Event::ButtonDown){
						soundInstance->setLoop(true);
					}
					//printf("%d \n", evt.getFlags() );
				}
				
				if( instanceCreated && volume > 0 ){
					printf("Volume: %f \n", volume);
					soundInstance->setVolume(volume);
				}

				
				if( (leftRightAnalog > zeroTolerence || leftRightAnalog < -zeroTolerence) &&
					(upDownAnalog > zeroTolerence || upDownAnalog < -zeroTolerence)
					){
					position[0] = leftRightAnalog;
					position[1] = upDownAnalog;
					printf("Pos: %f %f\n", position[0], position[1]);
				}
				return true;
				break;

			case Service::Mocap:
				if( evt.getSourceId() == 0 )
					soundManager->setListenerPosition( Vector3f( evt.getPosition(0), evt.getPosition(1), evt.getPosition(2)) );
				else if( instanceCreated && evt.getSourceId() == 1 )
					soundInstance->setPosition( Vector3f( evt.getPosition(0), evt.getPosition(1), evt.getPosition(2)) );
				//printf("ID: %d Pos: %f %f %f\n", evt.getSourceId(), evt.getPosition(0), evt.getPosition(1), evt.getPosition(2) );
				break;
		}
		return false;
	}

	
private:
	Vector3f position;
	float volume;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main(int argc, char** argv)
{
	SoundTest app;

	// Read config file name from command line or use default one.
	const char* cfgName = "soundTest.cfg";
	if(argc == 2) cfgName = argv[1];

	Config* cfg = new Config(cfgName);

	DataManager* dm = DataManager::getInstance();
	// Add a default filesystem data source using current work dir.
	dm->addSource(new FilesystemDataSource("./"));
	dm->addSource(new FilesystemDataSource(OMICRON_DATA_PATH));

	ServiceManager* sm = new ServiceManager();
	sm->setupAndStart(cfg);

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
