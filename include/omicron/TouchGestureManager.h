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
#ifndef __TOUCH_GESTURE_MANAGER_H__
#define __TOUCH_GESTURE_MANAGER_H__

#include "osystem.h"
#include "ServiceManager.h"

using namespace std;

struct Touch{
	int ID;
	float xPos;
	float yPos;
	float xWidth;
	float yWidth;
	int timestamp;

	// Gestures
	int gestureType;
};

namespace omicron {
	

	// Holds the initial and current center of mass of touches
	// as well as all touches in the group
	class TouchGroup
	{
		private:
			//TouchGestureManager* parent;

			int ID;
			float xPos;
			float yPos;
			float init_xPos;
			float init_yPos;
			
			float initialDiameter;
			float longRangeDiameter;
			float diameter;


			map<int,Touch> touchList;
			map<int,Touch> longRangeTouchList;

			// Maybe replace these with a function call to generate on demand?
			map<int,Touch> idleTouchList;
			map<int,Touch> movingTouchList;
		public:
			TouchGroup(){
				initialDiameter = 500; // Currently in pixels -> TODO: Change to screen ratio
				longRangeDiameter = 4000;
				diameter = initialDiameter;
			}

			bool isInsideGroup( Event::Type eventType, float x, float y, int id ){
				// Check if touch is inside radius of TouchGroup
				if( x > xPos - diameter/2 && x < xPos + diameter/2 && y > yPos - diameter/2 && y < yPos + diameter/2 ){
				  addTouch( eventType, x, y, ID );
				  return true;
				} else if( x > xPos - longRangeDiameter/2 && x < xPos + longRangeDiameter/2 && y > yPos - longRangeDiameter/2 && y < yPos + longRangeDiameter/2 ){
				  addLongRangeTouch( eventType, x, y, ID );
				  return false;
				} else {
				  if( longRangeTouchList.count( ID ) > 0 )
					longRangeTouchList.erase( ID );
				  return false;
				}
			}
			
			void addTouch( Event::Type eventType, float x, float y, int ID ){
			}

			void addLongRangeTouch( Event::Type eventType, float x, float y, int ID ){
			}
	};

	class TouchGestureManager
	{

	public:
		TouchGestureManager();
		void registerPQService(Service*);
		void poll();
		
		bool addTouch(Event::Type eventType, Touch touch);
	private:
		Service* pqsInstance;
		Lock* touchListLock;
		map<int,Touch> touchList;
		map<int,TouchGroup*> touchGroupList;

		static int deadTouchDelay; 

		void addTouchGroup( Event::Type eventType, float xPos, float yPos, int id );

		void generatePQServiceEvent(Event::Type eventType, Touch touch);
	};
}

#endif