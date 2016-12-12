#include "stdafx.h"
#include "NiTEHandTracker.h"

#include <iostream>

NiTEHandTracker::NiTEHandTracker(void)
{
	handTracker = new HandTracker;
}

NiTEHandTracker::~NiTEHandTracker(void)
{
	delete handTracker;
}

bool NiTEHandTracker::openDeviceForTracking()
{
	m_status = openni::STATUS_OK;
	const char* deviceURI = ANY_DEVICE;

	m_status = OpenNI::initialize();

	if(m_status != openni::STATUS_OK)
	{
		throw "OpenNI failed to initialize !";
	}

	// open the device
	m_status = m_device.open(deviceURI);

	if(m_status != openni::STATUS_OK)
	{
		OpenNI::shutdown();
		throw "Device open failed !";
	}
}

void NiTEHandTracker::init()
{
	nite::NiTE::initialize();

	if (handTracker->create(&m_device) != nite::STATUS_OK)     
	{
		throw "HandTracker failed to recognize device !";
	}

	handTracker->startGestureDetection(GESTURE_CLICK);
	handTracker->startGestureDetection(GESTURE_WAVE);
}

vector<CPoint> NiTEHandTracker::getHandPosition()
{
	CPoint currentPosition;
	vector<CPoint> posVector;
	handTracker->readFrame(&handTrackerFrame);

	const nite::Array<GestureData>& aGestures = handTrackerFrame.getGestures();

	for( int i = 0; i < aGestures.getSize(); ++ i )
	{
		const GestureData& rGesture = aGestures[i];

		switch( rGesture.getType() )
		{
			case GESTURE_WAVE:
			break;

			case GESTURE_CLICK:
			break;

			case GESTURE_HAND_RAISE:
			break;
		}

		const Point3f& rPos = rGesture.getCurrentPosition();

		currentPosition.x = rPos.x;
		currentPosition.x = rPos.y;
	}

	posVector.push_back(currentPosition);
	return posVector;		
}
