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
	//omsg("TouchGestureManager: TouchGestureManager()");
	touchListLock = new Lock();
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TouchGestureManager::registerPQService( Service* service )
{
	pqsInstance = service;
	omsg("TouchGestureManager: Registered with " + service->getName());
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
	
	map<int,Touch> swapList;

	map<int,Touch>::iterator it;
	for ( it = touchList.begin() ; it != touchList.end(); it++ ){
		Touch t = (*it).second;
		
		// TEMP: Dead touch time should be variable and less than 1 second
		if( curTime - t.timestamp < 1000 ){
			swapList[t.ID] = t;
		} else {
			t.timestamp = curTime;

			// Touch has expired - likely due to lost up event. Send new up event.
			generatePQServiceEvent( Event::Up, t );
			//printf("Auto Remove Touch ID %d\n", t.ID);
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
// All touches considered for gestures are added by this function.
// This also serves to error correct touch data: invalid ranges, missing events, etc.
bool TouchGestureManager::addTouch(Event::Type eventType, Touch touch)
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
		return false;

	touchListLock->lock();
	
	if( touchList.count(ID) == 0 && eventType == Event::Down ){
		addTouchGroup( eventType, x, y, ID );
		touchList[ID] = touch;
		//printf("New Touch ID %d pos: %f %f width: %f %f\n", ID, x, y, xW, yW);
	} else if( touchList.count(ID) == 1 && eventType == Event::Move ){
		addTouchGroup( eventType, x, y, ID );
		touchList[ID] = touch;
		//printf("Update Touch ID %d pos: %f %f width: %f %f\n", ID, x, y, xW, yW);
	} else if( touchList.count(ID) == 1 && eventType == Event::Up ){
		touchList.erase( ID );
		//printf("Remove Touch ID %d pos: %f %f width: %f %f\n", ID, x, y, xW, yW);
	} else {

		// Error correcting steps for missing touch data, dropped packets, etc.
		if( touchList.count(ID) == 1 && eventType == Event::Down ) {
			printf("TouchGestureManager: Should-Not-Happen-Warning: Touch down received for existing touch.\n");
			touchListLock->unlock();
			return true;
		} else if( touchList.count(ID) == 0 && eventType == Event::Move ) {
			//printf("TouchGestureManager: Should-Not-Happen-Warning: Touch move received for non-existant touch.\n");

			// If a touch move for non-existant touch detected, create the down event
			touchList[ID] = touch;
			generatePQServiceEvent( Event::Down, touch );
			//printf("(DB) New Touch ID %d pos: %f %f width: %f %f\n", ID, x, y, xW, yW);
			touchListLock->unlock();
			return false;
		} else if( touchList.count(ID) == 0 && eventType == Event::Up ) {
			printf("TouchGestureManager: Should-Not-Happen-Warning: Touch up received for non-existant touch.\n");
			touchListLock->unlock();
			return true;
		} else {
			printf("TouchGestureManager: Should-Not-Happen-Warning: Unknown case.\n");
			touchListLock->unlock();
			return true;
		}
		//printf("Touchlist count for ID %d: %d. Received eventType: %d\n", ID, touchList.count(ID), eventType );
	}

	touchListLock->unlock();
	return true;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TouchGestureManager::addTouchGroup( Event::Type eventType, float xPos, float yPos, int ID )
{
	// Check if new touch is inside an existing TouchGroup
	map<int,TouchGroup*>::iterator it;

	for ( it = touchGroupList.begin() ; it != touchGroupList.end(); it++ ){
		TouchGroup* tg = (*it).second;
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void TouchGestureManager::generatePQServiceEvent( Event::Type eventType, Touch touch )
{

	if( pqsInstance ){
		pqsInstance->lockEvents();

		Event* evt = pqsInstance->writeHead();
		switch(eventType)
		{
			case Event::Down:
				evt->reset(Event::Down, Service::Pointer, touch.ID);
				break;
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