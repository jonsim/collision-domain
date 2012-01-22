/**
 * @file	NetworkCore.h
 * @brief 	Takes notifications and deals with them however networking needs to
 */
#ifndef BIGSCREEN_H
#define BIGSCREEN_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "ViewportManager.h"
#include "RakNetTypes.h"
#include "NetworkCore.h"
#include <vector>
#include "ViewCamera.h"
#include "RakNetTime.h"
#include "GetTime.h"

class BigScreen
{
public:
	BigScreen (ViewportManager* vpm_P);
	bool declareNewPlayer( RakNet::RakNetGUID playerid ); //Returns true if the player has been added to big screen
	void addCamera(Ogre::Camera* cam);
	
private:
	ViewportManager* mViewportManager;
	std::vector<ViewCamera*> viewCameraVector;
};

#endif // #ifndef BIGSCREENH