
#include "stdafx.h"
#include "SharedIncludes.h"

ViewCamera::ViewCamera(Ogre::Camera* cam)
{
	mCam = cam;
	mLastUpdate = RakNet::GetTimeMS(); //Set it to the current time
}

Ogre::Camera* ViewCamera::getCamera()
{
	return mCam;
}

RakNet::TimeMS ViewCamera::getLastUpdateTime()
{
	return mLastUpdate;
}

void ViewCamera::setLastUpdateTime(RakNet::TimeMS lastUpdate)
{
	mLastUpdate = lastUpdate;
}

void ViewCamera::setLastUpdateTime()
{
	mLastUpdate = RakNet::GetTimeMS();
}
