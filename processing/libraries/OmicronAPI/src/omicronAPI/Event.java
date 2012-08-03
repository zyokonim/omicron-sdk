package omicronAPI;

/**************************************************************************************************
 * THE OMICRON PROJECT
 * ----------------------------------------------------------
 * --------------------------------------- Copyright 2010-2012 Electronic
 * Visualization Laboratory, University of Illinois at Chicago Authors: Arthur
 * Nishimoto arthur.nishimoto@gmail.com
 * ------------------------------------------
 * ------------------------------------------------------- Copyright (c)
 * 2010-2011, Electronic Visualization Laboratory, University of Illinois at
 * Chicago All rights reserved. Redistribution and use in source and binary
 * forms, with or without modification, are permitted provided that the
 * following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer. Redistributions in binary
 * form must reproduce the above copyright notice, this list of conditions and
 * the following disclaimer in the documentation and/or other materials provided
 * with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************************************/

public class Event {
	public int sourceID; // Device IDs (touch ids, mocap joints, etc.)
	public int eventID; // Event type i.e. click, move, etc.
	public int serviceID; // Used for pointer to denote types (Mouse, touch)
	public OmicronAPI.ServiceType serviceType; // Pointer, mocap, voice, etc.

	public float[] dataArray;
	int dataArraySize = 0;
	public String dataString;
	public float[] position;
	public float[] rotation;
	public float timestamp;

	Event() {
		dataString = "";
	}

	public float getXPos() {
		if (position != null)
			return position[0];
		else
			return -1;
	}

	public float getYPos() {
		if (position != null)
			return position[1];
		else
			return -1;
	}

	public float getZPos() {
		if (position != null)
			return position[2];
		else
			return -1;
	}

	public OmicronAPI.ServiceType getServiceType() {
		return serviceType;
	}

	public int getSourceID() {
		return sourceID;
	}

	public int getEventID() {
		return eventID;
	}

	public void setTimeStamp(float ts) {
		timestamp = ts;
	}

	public long getTimeStamp() {
		return (long) timestamp;
	}

	public float getFloatData(int index) {
		if (dataArraySize > index)
			return (float) dataArray[index];
		else
			return -1;
	}

	public int getIntData(int index) {
		if (dataArraySize > index)
			return (int) dataArray[index];
		else
			return -1;
	}
	
	public String getDataString(){
		return dataString;
	}
}
