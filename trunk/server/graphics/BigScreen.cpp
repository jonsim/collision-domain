#include "stdafx.h"
#include "BigScreen.h"

/*
BigScreen::BigScreen(ViewportManager* vpm_P, NetworkCore* networkCore)
{
	mViewportManager	= vpm_P;
	mNetworkCore		= networkCore;
}
*/

BigScreen::BigScreen(ViewportManager* vpm_P)
{
	mViewportManager = vpm_P;
}

bool BigScreen::declareNewPlayer( RakNet::RakNetGUID playerid )
{
	Player* tmpPlayer = NetworkCore::mPlayerPool->getPlayer(playerid);
	if(cameraVector[0] != NULL)
	{
		tmpPlayer->attachCamera(cameraVector[0]);
	}
	return mViewportManager->declareNewPlayer(tmpPlayer);
}

void BigScreen::addCamera(Ogre::Camera* cam) 
{
	cameraVector.push_back(cam);
}