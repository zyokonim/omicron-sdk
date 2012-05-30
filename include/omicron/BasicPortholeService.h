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
#ifndef __TABLET_SERVICE_H__
#define __TABLET_SERVICE_H__

#include "omicron/osystem.h"
#include "omicron/ServiceManager.h"
#include "omicron/Tcp.h"

namespace omicron {
    
	class BasicPortholeService;

	///////////////////////////////////////////////////////////////////////////////////////////////////
	class TabletConnection: public TcpConnection
	{
	public:
		TabletConnection(ConnectionInfo ci, BasicPortholeService* service);

		virtual void handleData();
		virtual void handleClosed();
		virtual void handleConnected();

	private:
		void handleUiEvent();
		void handleTouchEvent();
		bool withinAnchor( float x , float y , float tolerance );
		void genSimpleEvent( Event::Type evtType ,Service::ServiceType servType , float x , float y);

	private:
		static const int BufferSize = 1024;
		char myBuffer[BufferSize];
        
		BasicPortholeService* myService;
		Vector2i myTouchPosition;
		Vector2f anchor;
		bool ltClick;
		bool rtClick;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////////
	class TabletServer: public TcpServer
	{
	public:
		TabletServer(BasicPortholeService* service):
			myService(service)
		{}
        
		///////////////////////////////////////////////////////////////////////////////////////////
		virtual TcpConnection* createConnection(const ConnectionInfo& ci)
		{
			TabletConnection* conn = new TabletConnection(ci, myService);
			return conn;
		}
        
	private:
		BasicPortholeService* myService;
	};

	///////////////////////////////////////////////////////////////////////////////////////////////
	class OMICRON_API BasicPortholeService: public Service
	{
        friend class TabletConnection;
        friend class TabletServer;
	public:
		//! Allocator function (will be used to register the service inside SystemManager)
		static BasicPortholeService* New() { return new BasicPortholeService(); }
        
	public:
		BasicPortholeService();
		~BasicPortholeService();
        
		virtual void setup(Setting& settings);
		virtual void poll();

		TabletConnection* getConnection(int id);

	private:
		// NOTE: this class is using the obsolete version of using a TcpServer.
		// The correct way is to derive BasicPortholeService from TcpServer directly 
		// (TcpServer derives from Service now)
		TabletServer* myServer;
        void genSimpleEvent( Event::Type evtType ,Service::ServiceType servType , float x , float y);
        bool withinAnchor( float x , float y , float tolerance );
	};
}; // namespace omicron

#endif