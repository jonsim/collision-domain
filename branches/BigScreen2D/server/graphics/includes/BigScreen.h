/**
 * @file	BigScreen.h
 * @brief 	Handles all BigScreen code
 */
#ifndef BIGSCREEN_H
#define BIGSCREEN_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"

#include "RakNetTypes.h"
#include <vector>
#include "RakNetTime.h"
#include "GetTime.h"
#include "OgreTextAreaOverlayElement.h"
#include "OgreFontManager.h"

class BigScreen
{
public:
								BigScreen(ViewportManager* vpm_P);
	bool						declareNewPlayer( RakNet::RakNetGUID playerid ); //Returns true if the player has been added to big screen
	void						addCamera(Ogre::Camera* cam);
	void						setupMapView();
	void						updateMapView();
	void						setMapCorner(Ogre::Vector3 corner);
	void						setMapSize(Ogre::Vector3 size);
private:
	ViewportManager*			mViewportManager;
	std::vector<ViewCamera*>	viewCameraVector;
	Ogre::OverlayContainer*		olcMap;
	Ogre::OverlayElement*		oleCar;
	Ogre::Vector3				mapCorner;
	Ogre::Vector3				mapSize;
};

#endif // #ifndef BIGSCREENH