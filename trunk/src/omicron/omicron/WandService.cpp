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
	myRaySourceId(0),
	myButton1State(false),
	myButton2State(false),
	myButtonStateChanged(false),
	mySliderState(0),
	myOutputSourceId(0)
{
	setPollPriority(Service::PollLast);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WandService::setup(Setting& settings)
{
	String inputType;

	myUpdateInterval = Config::getFloatValue("updateInterval", settings, 0.1f);
	myDebug = Config::getBoolValue("debug", settings);

	inputType = Config::getStringValue("inputType", settings);
	myRaySourceId = Config::getIntValue("raySourceId", settings);
	myInputSourceId = Config::getIntValue("inputSourceId", settings);
	myOutputSourceId = Config::getIntValue("outputSourceId", settings);

	String inputSvc = Config::getStringValue("inputService", settings);
	myInputService = getManager()->findService(inputSvc);
	if(myInputService == NULL)
	{
		ofwarn("WandService::setup: could not find input service %1%", %inputSvc);
	}

	StringUtils::toLowerCase(inputType);

	if(inputType == "wiimote") myInputType = Wiimote;
	else if(inputType == "mouse") myInputType = Mouse;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WandService::initialize()
{
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
			const Quaternion& quat = evt->getOrientation();
			Vector3f forward = quat * (-Vector3f::UnitZ());
			myRay.setOrigin(evt->getPosition());
			myRay.setDirection(forward);
			if(myDebug)
			{
				ofmsg("Wand ray origin %1%  direction %2%", %myRay.getOrigin() %myRay.getDirection());
			}
		}
		// Process button input events.
		if(myInputService != NULL)
		{
			if(evt->getServiceId() == myInputService->getServiceId())
			{
				bool bt1 = evt->isFlagSet(Event::Button1);
				bool bt2 = evt->isFlagSet(Event::Button1);
				if(bt1 != myButton1State || bt2 != myButton2State)
				{
					myButtonStateChanged = true;
				}
				else
				{
					myButtonStateChanged = false;
				}
				myButton1State = bt1;
				myButton2State = bt2;
				if(evt->getType() == Event::Zoom) mySliderState = evt->getExtraDataInt(0);
			}
		}
	}

	unlockEvents();

	if(myUpdateTimer.getElapsedTime() < myUpdateInterval) return;
	// Reset update timer.
	myUpdateTimer.stop();
	myUpdateTimer.start();

	lockEvents();

	// Generate output event.
	Event* evt = writeHead();
	if(myButtonStateChanged)
	{
		if(myButton1State || myButton2State)
		{
			evt->reset(Event::Down, Service::Pointer, myOutputSourceId);
		}
		else
		{
			evt->reset(Event::Up, Service::Pointer, myOutputSourceId);
		}
	}
	else
	{
		evt->reset(Event::Update, Service::Pointer, myOutputSourceId);
	}
	evt->setExtraDataType(Event::ExtraDataVector3Array);
	evt->setExtraDataVector3(0, myRay.getOrigin());
	evt->setExtraDataVector3(1, myRay.getDirection());

	unlockEvents();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void WandService::dispose()
{
}

