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

#include "omicron/MocapGestureManager.h"

using namespace omicron;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mocap Gesture Manager
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MocapGestureManager::MocapGestureManager()
{
	//omsg("MocapGestureManager: MocapGestureManager()");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// registerServiceManager is used to get events for processing
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MocapGestureManager::registerServiceManager( ServiceManager* sm )
{
	serviceManager = sm;
	//omsg("MocapGestureManager: Registered with " + serviceManager->getName());
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MocapGestureManager::poll()
{
	int av = serviceManager->getAvailableEvents();
	if(av != 0)
	{
		// TODO: Instead of copying the event list, we can lock the main one.
		Event evts[OMICRON_MAX_EVENTS];
		serviceManager->getEvents(evts, OMICRON_MAX_EVENTS);
		for( int evtNum = 0; evtNum < av; evtNum++ )
		{
			processEvent(evts[evtNum]);
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MocapGestureManager::processEvent( const Event& e )
{
	if( e.getServiceId() == Service::Mocap ){
		int ID = e.getSourceId();
		int userID = e.getExtraDataInt(0);
		Vector3f pos = e.getPosition();
		Quaternion ori = e.getOrientation();
		
		Vector3f lastPos = pos;
		Quaternion lastOri = ori;

		// New user
		if( userList.count(userID) == 0 )
		{
			MocapUser newUser;
			newUser.userID = userID;

			Joint newJoint;
			newJoint.ID = ID;
			newJoint.userID = userID;
			newJoint.position = pos;
			newJoint.orientation = ori;

			newUser.joints[ID] = newJoint;

			userList[userID] = newUser;
		}
		else // Existing user
		{
			MocapUser user = userList[userID];

			// New joint
			if( user.joints.count(ID) == 0 )
			{
				Joint newJoint;
				newJoint.ID = ID;
				newJoint.userID = userID;
				newJoint.position = pos;
				newJoint.orientation = ori;

				user.joints[ID] = newJoint;
			}
			else // Existing joint
			{
				Joint joint;
			
				joint.lastPosition = joint.position;
				joint.lastOrientation = joint.orientation;

				joint.position = pos;
				joint.orientation = ori;

				user.joints[ID] = joint;
				lastPos = joint.lastPosition;
				lastOri = joint.lastOrientation;
			}

			userList[userID] = user;
		}
		// Single joint gestures
	}
}