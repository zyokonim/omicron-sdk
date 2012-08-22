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
class GestureService;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Mocap Gesture Manager
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
MocapGestureManager::MocapGestureManager( Service* service )
{
	gestureService = service;
	//omsg("MocapGestureManager: MocapGestureManager()");
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MocapGestureManager::poll()
{
	//ofmsg("UserList size: %1%", %userList.size() );
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MocapGestureManager::processEvent( const Event& e )
{
	if( e.getServiceType() == Service::Mocap && e.getType() == Event::Update ){
		int userID = e.getSourceId();
		int ID = 0;
		Vector3f pos = e.getPosition();
		Quaternion ori = e.getOrientation();
		
		Vector3f lastPos = pos;
		Quaternion lastOri = ori;

		bool useOpenNI = true;

		if( useOpenNI )
		{

		// New user
		if( userList.count(userID) == 0 )
		{
			MocapUser newUser;
			newUser.userID = userID;
			
			if( !e.isExtraDataNull(Event::OMICRON_SKEL_HEAD) )
				newUser.head.position = e.getExtraDataVector3(Event::OMICRON_SKEL_HEAD);
			if( !e.isExtraDataNull(Event::OMICRON_SKEL_LEFT_HAND) )
				newUser.leftHand.position = e.getExtraDataVector3(Event::OMICRON_SKEL_LEFT_HAND);
			if( !e.isExtraDataNull(Event::OMICRON_SKEL_RIGHT_HAND) )
				newUser.rightHand.position = e.getExtraDataVector3(Event::OMICRON_SKEL_RIGHT_HAND);

			userList[userID] = newUser;
		}
		else // Existing user
		{
			MocapUser user = userList[userID];
			
			user.head.lastPosition = user.head.position;
			user.leftHand.lastPosition = user.leftHand.position;
			user.rightHand.lastPosition = user.rightHand.position;

			if( !e.isExtraDataNull(Event::OMICRON_SKEL_HEAD) )
				user.head.position = e.getExtraDataVector3(Event::OMICRON_SKEL_HEAD);
			if( !e.isExtraDataNull(Event::OMICRON_SKEL_LEFT_HAND) )
				user.leftHand.position = e.getExtraDataVector3(Event::OMICRON_SKEL_LEFT_HAND);
			if( !e.isExtraDataNull(Event::OMICRON_SKEL_RIGHT_HAND) )
				user.rightHand.position = e.getExtraDataVector3(Event::OMICRON_SKEL_RIGHT_HAND);
			
			float xDiff = user.rightHand.position[0] - user.rightHand.lastPosition[0];
			float yDiff = user.rightHand.position[1] - user.rightHand.lastPosition[1];
			float zDiff = user.rightHand.position[2] - user.rightHand.lastPosition[2];
			
			float zDiff2 = user.leftHand.position[2] - user.leftHand.lastPosition[2];
			
			float curt = (float)((double)clock() / CLOCKS_PER_SEC);

			float swipeThreshold = 0.08f;
			float clickThreshold = 0.08f;
			if( -xDiff > swipeThreshold )
				ofmsg("%1% Swipe left", %curt);
			else if( xDiff > swipeThreshold )
				ofmsg("%1% Swipe right", %curt);
			else if( -yDiff > swipeThreshold )
				ofmsg("%1% Swipe down", %curt);
			else if( yDiff > swipeThreshold )
				ofmsg("%1% Swipe up", %curt);
			
			//float clickTimeout = 
			if( -zDiff > clickThreshold && curt - lastClickTime > 0.5f )
			{
				lastClickTime = curt;
				ofmsg("%1% Right Click", %curt);

				generateGesture( Event::Click, Event::OMICRON_SKEL_RIGHT_HAND, userID, user.rightHand.position );
			}
			//else if( -zDiff2 > clickThreshold )
			//	omsg("Left Click");

			//ofmsg("RightHand pos(%1%) xDiff (%2%) ", %user.rightHand.position %xDiff );

			userList[userID] = user;
		}
		
		}
		else
		{
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
		}
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void MocapGestureManager::generateGesture( Event::Type gesture, int jointID, int userID, Vector3f position )
{
	Event* evt;

	evt = gestureService->writeHead();
	evt->reset(gesture, Service::Mocap, userID);
	evt->setPosition(position);
	
	evt->setExtraDataType(Event::ExtraDataIntArray);
	evt->setExtraDataInt(0, jointID);
	gestureService->unlockEvents();
}