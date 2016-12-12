#pragma once
#include <nite/NiTE.h>
#include <vector>

using namespace openni;
using namespace nite;
using namespace std;

class NiTEHandTracker
{
	HandTracker *handTracker;
	nite::Status niteRc;
	nite::HandTrackerFrameRef handTrackerFrame;
	openni::Status openniStatus;
	nite::HandId newId; 

	openni::Status m_status;
	Device m_device;

public:
	NiTEHandTracker(void);
	~NiTEHandTracker(void);

	bool openDeviceForTracking();
	void init();
	vector<CPoint> getHandPosition();
};

