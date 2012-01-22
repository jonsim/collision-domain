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

	//Ogre::LogManager::getSingleton().logMessage("Starting BigScreen");
	OutputDebugString("BigScreen Loaded \n");
}

bool BigScreen::declareNewPlayer( RakNet::RakNetGUID playerid )
{
	Player* tmpPlayer = NetworkCore::mPlayerPool->getPlayer(playerid);
	
	ViewCamera* bestCanidateVC = NULL;
	std::vector<ViewCamera*>::iterator itr;
	for(itr = viewCameraVector.begin(); itr<viewCameraVector.end(); ++itr)
	{
		ViewCamera* tmp = *itr;
		if(bestCanidateVC != NULL)
		{
			//Check to see if this new canidate is smaller than the previous canidate
			if(RakNet::LessThan(tmp->getLastUpdateTime(),bestCanidateVC->getLastUpdateTime()))
			{
				bestCanidateVC =  tmp;
			}
		}
		else
		{
			bestCanidateVC=  tmp;
		}
	}
	
	if(bestCanidateVC != NULL)
	{
		//This is important. If you add the camera to two points it dies
		bestCanidateVC->getCamera()->detachFromParent(); 
		tmpPlayer->attachCamera(bestCanidateVC->getCamera());
		OutputDebugString("Attached Camera to new player\n");
		bestCanidateVC->setLastUpdateTime();
	}
	
	return mViewportManager->declareNewPlayer(tmpPlayer);
}

void BigScreen::addCamera(Ogre::Camera* cam) 
{
	ViewCamera* tmpVC = new ViewCamera(cam);
	viewCameraVector.push_back(tmpVC);
}