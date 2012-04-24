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

#include "omicron/TouchGestureManager.h"

using namespace omicron;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int TouchGestureManager::deadTouchDelay = 1500; 

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
TouchGestureManager::TouchGestureManager()
{
	omsg("TouchGestureManager: TouchGestureManager()");
	touchListLock = new Lock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TouchGestureManager::registerPQService( Service* service )
{
	pqsInstance = service;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TouchGestureManager::poll()
{
	touchListLock->lock();

	// Get time in milliseconds
	timeb tb;
	ftime( &tb );
	int curTime = tb.millitm + (tb.time & 0xfffff) * 1000;

	//printf("TouchGestureManager: poll()\n");
	//printf("TouchGestureManager: poll() Time: %d\n", curTime);
	
	std::map<int,Touch> swapList;

	std::map<int,Touch>::iterator it;
	for ( it = touchList.begin() ; it != touchList.end(); it++ ){
		Touch t = (*it).second;
		
		// TEMP: Dead touch time should be variable and less than 1 second
		if( curTime - t.timestamp < 1000 ){
			swapList[t.ID] = t;
		} else {
			t.timestamp = curTime;

			// Touch has expired - likely due to lost up event. Send new up event.
			generatePQServiceEvent( Event::Up, t );
			printf("Auto Remove Touch ID %d\n", t.ID);
		}
		//printf( "TL: Touch ID %d \n", t.timestamp );
	}
	touchListLock->unlock();
	
	touchListLock->lock();
	touchList = swapList;
	touchListLock->unlock();

	//printf("TouchList size: %d\n", touchList.size());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TouchGestureManager::addTouch(Event::Type eventType, Touch touch)
{
	float x = touch.xPos;
	float y = touch.yPos;
	float xW = touch.xWidth;
	float yW = touch.yWidth;
	int ID = touch.ID;
	int timestamp = touch.timestamp;
	
	// Expand coords for testing
	x *= 8160.0;
	y *= 2304.0;
	xW *= 8160.0;
	yW *= 2304.0;

	// Width data should be > 0 - Else likely bad data
	if( xW <= 0 || yW <= 0 )
		return;
	
	touchListLock->lock();

	if( touchList.count(ID) == 0 ){
		addTouchGroup( eventType, x, y, ID );
		touchList[ID] = touch;
		printf("New Touch ID %d pos: %f %f width: %f %f\n", ID, x, y, xW, yW);
	} else {
		if( eventType == Event::Move ){
			addTouchGroup( eventType, x, y, ID );
			touchList[ID] = touch;
			//printf("Update Touch ID %d pos: %f %f width: %f %f\n", ID, x, y, xW, yW);
		} else if( eventType == Event::Up ){
			touchList.erase( ID );
			printf("Remove Touch ID %d pos: %f %f width: %f %f\n", ID, x, y, xW, yW);
		}
	}

	touchListLock->unlock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TouchGestureManager::addTouch( Event::Type eventType, float x, float y, float xW, float yW, int ID)
{

}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TouchGestureManager::addTouchGroup( Event::Type eventType, float xPos, float yPos, int ID )
{
	
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TouchGestureManager::generatePQServiceEvent( Event::Type eventType, Touch touch )
{

	if( pqsInstance ){
		pqsInstance->lockEvents();

		Event* evt = pqsInstance->writeHead();
		switch(eventType)
		{
			case Event::Up:
				evt->reset(Event::Up, Service::Pointer, touch.ID);
				break;
		}		
		evt->setPosition(touch.xPos, touch.yPos);

		evt->setExtraDataType(Event::ExtraDataFloatArray);
		evt->setExtraDataFloat(0, touch.xWidth);
		evt->setExtraDataFloat(1, touch.yWidth);

		pqsInstance->unlockEvents();
	} else {
		printf("TouchGestureManagerL No PQService Registered\n");
	}
}