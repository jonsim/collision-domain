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
#include <time.h>

class BigScreen
{
public:
								BigScreen();
	void						manageNewPlayer(Player* player);
	void						addCamera(Ogre::Camera* cam);
	void						setupMapView();
	void						updateMapView();
	void						hideScreen();
	void						showScreen();
	Ogre::OverlayElement*       createPowerupOverlayElement(Ogre::Vector3 powerupPosition, int uniqueID);
    Ogre::OverlayElement*       createPlayerOverlayElement(int uniqueID);

    void                        changeArrow(int uniqueID, int type);

    void                        removePowerupOverlayFromMap(int uniqueID);
    void                        removeCarOverlayFromMap(int uniqueID);
    
    void                        createRoundTimer();
    void                        resetRoundTimer();
    void                        updateRoundTimer();
private:
	std::vector<ViewCamera*>	viewCameraVector;
	Ogre::OverlayContainer*		olcMap;
	Ogre::OverlayElement*		oleCar;
	Ogre::Vector3				mapCorner;
	Ogre::Vector3				mapSize;

	Ogre::OverlayElement*		oleVIP1;
	Ogre::OverlayElement*		oleVIP2;

	void						updatePlayer(Car *car, Player *player, Ogre::OverlayElement* carOverlay);
    #if _WIN32
        void                        updatePowerup(Ogre::Vector3 location, btQuaternion &quat, Ogre::OverlayElement* powerupOverlay);
    #else
        void						updatePowerup(Ogre::Vector3 location, btQuaternion quat, Ogre::OverlayElement* powerupOverlay);
    #endif
	
	void						setMapCorner(Ogre::Vector3 corner);
	void						setMapSize(Ogre::Vector3 size);

	inline float				convertWorldToScreenX(float xPos);
	inline float				convertWorldToScreenY(float yPos);

    //The timer elements
    Ogre::OverlayElement    *oleTimerText;
    bool                    isTimerSetup;
    int                     timeLeft;
    time_t                  startTime;
};

#endif // #ifndef BIGSCREENH
