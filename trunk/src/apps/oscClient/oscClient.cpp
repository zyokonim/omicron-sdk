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
class EventViewer
{
public:
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// Checks the type of event. If a valid event, creates an event packet and returns true. Else return false.
	virtual bool handleEvent(const Event& evt)
	{
		char* eventPacket = new char[256];

		itoa(evt.getServiceType(), eventPacket, 10); // Append input type
		strcat( eventPacket, ":" );
		char floatChar[32];

		switch(evt.getServiceType())
		{
			case Service::Controller:
				// Converts id to char, appends to eventPacket
				sprintf(floatChar,"%d",evt.getSourceId());
				strcat( eventPacket, floatChar );
				strcat( eventPacket, "," ); // Spacer

				// See DirectXInputService.cpp for parameter details
				for( int i = 0; i < evt.getExtraDataItems(); i++ ){
					sprintf(floatChar,"%d", (int)evt.getExtraDataFloat(i));
					strcat( eventPacket, floatChar );
					if( i < evt.getExtraDataItems() - 1 )
						strcat( eventPacket, "," ); // Spacer
					else
						strcat( eventPacket, " " ); // Spacer
				}
				strcat( eventPacket, "\n" ); // Spacer
				//printf(eventPacket);
				
				if( evt.getType() != Event::Update )
				{
					printf("%d \n",evt.getFlags());
				}

				return true;
				break;
			default:
				break;
		}
		

		delete[] eventPacket;
		return false;
	}
private:

	private:
		float rx;
		float ry;
		float rz;

		float x;
		float y;
		float z;

		float mx;
		float my;
		float mz;

		float lx;
		float ly;
		float lz;

};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void main(int argc, char** argv)
{
	EventViewer app;

	SoundManager* soundManager = new SoundManager();
	//soundManager->connectToServer("131.193.77.51",57120);
	//soundManager->startSoundServer();

	SoundEnvironment* env = soundManager->getSoundEnvironment();
	Sound* testSound = env->createSound("test");
	testSound->loadFromFile("test.wav");

	//soundManager->stopSoundServer();
	
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
