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

#include "omicron/SoundManager.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class OSCSoundTest
{
public:
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	OSCSoundTest()
	{
		soundManager = new SoundManager();

		env = soundManager->getSoundEnvironment();
	}

	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Checks the type of event. If a valid event, creates an event packet and returns true. Else return false.
	virtual bool handleEvent(const Event& evt)
	{
		switch(evt.getServiceType())
		{
			case Service::Controller:
				float leftRightAnalog = evt.getExtraDataFloat(0) / 1000.0f;
				float upDownAnalog = evt.getExtraDataFloat(1) / 1000.0f;

				volume = evt.getExtraDataFloat(4) / 1000.0f;
				
				if( evt.getType() == Event::Down ){

					if( evt.getFlags() == Event::Button3){
						soundManager->connectToServer("131.193.77.51",57120);
						soundManager->startSoundServer();
					}
					if( evt.getFlags() == Event::Button2){
						soundManager->stopSoundServer();
					}
					
					if( evt.getFlags() == Event::Button5){
						testSound = env->createSound("test");
						testSound->loadFromFile("/flower_duet_mono.wav");
					}

					if( evt.getFlags() == Event::ButtonRight){
						soundInstance = new SoundInstance(testSound);
						soundInstance->play();						
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
				
				if( volume > 0 )
					printf("Volume: %f \n", volume);

				float zeroTolerence = 0.008f;
				if( (leftRightAnalog > zeroTolerence || leftRightAnalog < -zeroTolerence) &&
					(upDownAnalog > zeroTolerence || upDownAnalog < -zeroTolerence)
					){
					position[0] = leftRightAnalog;
					position[1] = upDownAnalog;
					printf("Pos: %f %f\n", position[0], position[1]);
				}
				return true;
				break;
		}
		return false;
	}
private:
	SoundManager* soundManager;
	Sound* testSound;
	SoundInstance* soundInstance;
	SoundEnvironment* env;
private:
	Vector3f position;
	float volume;
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main(int argc, char** argv)
{
	OSCSoundTest app;

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
