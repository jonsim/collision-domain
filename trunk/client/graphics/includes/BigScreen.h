/**
 * @file	NetworkCore.h
 * @brief 	Takes notifications and deals with them however networking needs to
 */
#ifndef BIGSCREEN_H
#define BIGSCREEN_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"

#include "ViewCamera.h"
#include "Powerup.h"
#include "Player.h"

#include "RakNetTypes.h"
#include <vector>
#include "RakNetTime.h"
#include "GetTime.h"
#include <sstream>

class BigScreen
{
public:
								BigScreen();
	bool						declareNewPlayer( RakNet::RakNetGUID playerid ); //Returns true if the player has been added to big screen
	void						manageNewPlayer(Player* player);
	void						addCamera(Ogre::Camera* cam);
	void						setupMapView();
	void						updateMapView();
	void						hideScreen();
	void						showScreen();
	Ogre::OverlayElement*       createPowerupOverlayElement(Ogre::Vector3 powerupPosition, int uniqueID);

private:
	//ViewportManager*			mViewportManager;
	std::vector<ViewCamera*>	viewCameraVector;
	Ogre::OverlayContainer*		olcMap;
	Ogre::OverlayElement*		oleCar;
	Ogre::Vector3				mapCorner;
	Ogre::Vector3				mapSize;

	Ogre::OverlayElement*		oleVIP1;
	Ogre::OverlayElement*		oleVIP2;

	void						updatePlayer(Player* player, Ogre::OverlayElement* carOverlay);
	
	void						setMapCorner(Ogre::Vector3 corner);
	void						setMapSize(Ogre::Vector3 size);

	inline float				convertWorldToScreenX(float xPos);
	inline float				convertWorldToScreenY(float yPos);

};

#endif // #ifndef BIGSCREENH
