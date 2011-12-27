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
#include "omicron/Config.h"
#include "omicron/DataManager.h"
#include "omicron/FilesystemDataSource.h"
#include "omicron/StringUtils.h"

namespace omicron
{
	//////////////////////////////////////////////////////////////////////////////////////////////////
	FILE* sLogFile = NULL;
	List<ILogListener*> sLogListeners;

	//////////////////////////////////////////////////////////////////////////////////////////////////
	void ologaddlistener(ILogListener* listener)
	{
		sLogListeners.push_back(listener);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	void ologremlistener(ILogListener* listener)
	{
		sLogListeners.remove(listener);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	void ologopen(const char* filename)
	{
		if(!filename) filename = "./log.txt";
		sLogFile = fopen(filename, "w");
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	void ologclose()
	{
		if(sLogFile)
		{
			fflush(sLogFile);
			fclose(sLogFile);
			sLogFile = NULL;
		}
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	void omsg(const String& str)
	{
		printf("%s\n", str.c_str());
		if(sLogFile)
		{
			fprintf(sLogFile, "%s\n", str.c_str());
			fflush(sLogFile);
		}
		foreach(ILogListener* ll, sLogListeners) ll->addLine(str);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	void owarn(const String& str)
	{
		printf("!!! %s\n", str.c_str());
		if(sLogFile)
		{
			fprintf(sLogFile, "!!! %s\n", str.c_str());
			fflush(sLogFile);
		}
		foreach(ILogListener* ll, sLogListeners) ll->addLine(str);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	void oerror(const String& str)
	{
		printf("*** %s\n", str.c_str());
		if(sLogFile)
		{
			fprintf(sLogFile, "*** %s\n", str.c_str());
			fflush(sLogFile);
		}
		foreach(ILogListener* ll, sLogListeners) ll->addLine(str);
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	void oabort(const char* file, int line, const char* reason)
	{
		String filename;
		String path;

		StringUtils::splitFilename(file, filename, path);

		ofmsg("Assertion failed at %1%:%2% - %3%", %file %line %reason);

		abort();
	}

	//////////////////////////////////////////////////////////////////////////////////////////////////
	void oexit(int code)
	{
		exit(code);
	}
}
