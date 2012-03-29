/********************************************************************************************************************** 
 * THE OMEGA LIB PROJECT
 *---------------------------------------------------------------------------------------------------------------------
 * Copyright 2010								Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti							febret@gmail.com
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
#include "omicron/WandService.h"
#include "omicron/Config.h"
#include "omicron/ServiceManager.h"
#include "omicron/StringUtils.h"

using namespace omicron;

///////////////////////////////////////////////////////////////////////////////////////////////////
WandService::WandService():
	myDebug(false),
	myRaySourceId(0)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WandService::setup(Setting& settings)
{
	String inputType;

	myDebug = Config::getBoolValue("debug", settings);

	myRaySourceId = Config::getIntValue("raySourceId", settings);
	myPointerSourceId = Config::getIntValue("pointerSourceId", settings);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WandService::initialize()
{
	setPollPriority(Service::PollLast);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WandService::poll()
{
	lockEvents();
	int numEvts = getManager()->getAvailableEvents();
	for(int i = 0; i < numEvts; i++)
	{
		Event* evt = getEvent(i);
		// Process mocap events.
		if(evt->getServiceType() == Service::Mocap && evt->getSourceId() == myRaySourceId)
		{
			evt->setProcessed();
			const Quaternion& quat = evt->getOrientation();
			Vector3f forward = quat * (-Vector3f::UnitZ());
			myRay.setOrigin(evt->getPosition());
			myRay.setDirection(forward);
			if(myDebug)
			{
				ofmsg("Wand ray origin %1%  direction %2%", %myRay.getOrigin() %myRay.getDirection());
			}
		}
		// Attach the mocap ray to pointer.
		if(evt->getServiceType() == Service::Pointer
			&& evt->getSourceId() == myPointerSourceId)
		{
			evt->setExtraDataType(Event::ExtraDataVector3Array);
			evt->setExtraDataVector3(0, myRay.getOrigin());
			evt->setExtraDataVector3(1, myRay.getDirection());
		}
	}

	unlockEvents();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WandService::dispose()
{
}

