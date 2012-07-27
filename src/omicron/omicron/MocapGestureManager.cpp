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
typedef enum XnSkeletonJoint
{
XN_SKEL_HEAD	= 1,
XN_SKEL_NECK	= 2,
XN_SKEL_TORSO	= 3,
XN_SKEL_WAIST	= 4,

XN_SKEL_LEFT_COLLAR	= 5,
XN_SKEL_LEFT_SHOULDER	= 6,
XN_SKEL_LEFT_ELBOW	= 7,
XN_SKEL_LEFT_WRIST	= 8,
XN_SKEL_LEFT_HAND	= 9,
XN_SKEL_LEFT_FINGERTIP	=10,

XN_SKEL_RIGHT_COLLAR	=11,
XN_SKEL_RIGHT_SHOULDER	=12,
XN_SKEL_RIGHT_ELBOW	=13,
XN_SKEL_RIGHT_WRIST	=14,
XN_SKEL_RIGHT_HAND	=15,
XN_SKEL_RIGHT_FINGERTIP	=16,

XN_SKEL_LEFT_HIP	=17,
XN_SKEL_LEFT_KNEE	=18,
XN_SKEL_LEFT_ANKLE	=19,
XN_SKEL_LEFT_FOOT	=20,

XN_SKEL_RIGHT_HIP	=21,
XN_SKEL_RIGHT_KNEE	=22,
XN_SKEL_RIGHT_ANKLE	=23,
XN_SKEL_RIGHT_FOOT	=24	
} XnSkeletonJoint;

#define OMICRON_SKEL_HEAD				XN_SKEL_HEAD
#define OMICRON_SKEL_NECK				XN_SKEL_NECK
#define	OMICRON_SKEL_TORSO			XN_SKEL_TORSO
#define OMICRON_SKEL_WAIST			XN_SKEL_WAIST

#define OMICRON_SKEL_LEFT_COLLAR		XN_SKEL_LEFT_COLLAR
#define OMICRON_SKEL_LEFT_SHOULDER	XN_SKEL_LEFT_SHOULDER
#define OMICRON_SKEL_LEFT_ELBOW		XN_SKEL_LEFT_ELBOW
#define OMICRON_SKEL_LEFT_WRIST		XN_SKEL_LEFT_WRIST
#define OMICRON_SKEL_LEFT_HAND		XN_SKEL_LEFT_HAND
#define OMICRON_SKEL_LEFT_FINGERTIP	XN_SKEL_LEFT_FINGERTIP

#define OMICRON_SKEL_LEFT_HIP			XN_SKEL_LEFT_HIP
#define OMICRON_SKEL_LEFT_KNEE		XN_SKEL_LEFT_KNEE
#define OMICRON_SKEL_LEFT_ANKLE		XN_SKEL_LEFT_ANKLE
#define OMICRON_SKEL_LEFT_FOOT		XN_SKEL_LEFT_FOOT

#define OMICRON_SKEL_RIGHT_COLLAR		XN_SKEL_RIGHT_COLLAR
#define OMICRON_SKEL_RIGHT_SHOULDER	XN_SKEL_RIGHT_SHOULDER
#define OMICRON_SKEL_RIGHT_ELBOW		XN_SKEL_RIGHT_ELBOW
#define OMICRON_SKEL_RIGHT_WRIST		XN_SKEL_RIGHT_WRIST
#define OMICRON_SKEL_RIGHT_HAND		XN_SKEL_RIGHT_HAND
#define OMICRON_SKEL_RIGHT_FINGERTIP	XN_SKEL_RIGHT_FINGERTIP

#define OMICRON_SKEL_RIGHT_HIP		XN_SKEL_RIGHT_HIP
#define OMICRON_SKEL_RIGHT_KNEE		XN_SKEL_RIGHT_KNEE
#define OMICRON_SKEL_RIGHT_ANKLE		XN_SKEL_RIGHT_ANKLE
#define OMICRON_SKEL_RIGHT_FOOT		XN_SKEL_RIGHT_FOOT

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
			
			if( !e.isExtraDataNull(OMICRON_SKEL_HEAD) )
				newUser.head.position = e.getExtraDataVector3(OMICRON_SKEL_HEAD);
			if( !e.isExtraDataNull(OMICRON_SKEL_LEFT_HAND) )
				newUser.leftHand.position = e.getExtraDataVector3(OMICRON_SKEL_LEFT_HAND);
			if( !e.isExtraDataNull(OMICRON_SKEL_RIGHT_HAND) )
				newUser.rightHand.position = e.getExtraDataVector3(OMICRON_SKEL_RIGHT_HAND);

			userList[userID] = newUser;
		}
		else // Existing user
		{
			MocapUser user = userList[userID];
			
			user.head.lastPosition = user.head.position;
			user.leftHand.lastPosition = user.leftHand.position;
			user.rightHand.lastPosition = user.rightHand.position;

			if( !e.isExtraDataNull(OMICRON_SKEL_HEAD) )
				user.head.position = e.getExtraDataVector3(OMICRON_SKEL_HEAD);
			if( !e.isExtraDataNull(OMICRON_SKEL_LEFT_HAND) )
				user.leftHand.position = e.getExtraDataVector3(OMICRON_SKEL_LEFT_HAND);
			if( !e.isExtraDataNull(OMICRON_SKEL_RIGHT_HAND) )
				user.rightHand.position = e.getExtraDataVector3(OMICRON_SKEL_RIGHT_HAND);
			
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

				generateGesture( Event::Click, OMICRON_SKEL_RIGHT_HAND, userID, user.rightHand.position );
			}
			//else if( -zDiff2 > clickThreshold )
			//	omsg("Left Click");

			//ofmsg("Head pos(%1%) diff (%2%) ", %user.head.position %diff );

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