/**
 * @file    NetworkCore.h
 * @brief     Takes notifications and deals with them however networking needs to
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
#include <sstream>

class ViewCamera;

class BigScreen
{
public:
                                BigScreen(ViewportManager* vpm_P);
    bool                        declareNewPlayer( RakNet::RakNetGUID playerid ); //Returns true if the player has been added to big screen
    void                        addCamera(Ogre::Camera* cam);
    void                        setupMapView();
    void                        updateMapView();
private:
    ViewportManager*            mViewportManager;
    std::vector<ViewCamera*>    viewCameraVector;
    Ogre::OverlayContainer*        olcMap;
    Ogre::OverlayElement*        oleCar;
    Ogre::Vector3                mapCorner;
    Ogre::Vector3                mapSize;

    Ogre::OverlayElement*        oleVIP1;
    Ogre::OverlayElement*        oleVIP2;

    void                        updatePlayer(Player* player, Ogre::OverlayElement* carOverlay);
    void                        manageNewPlayer(Player* player);
    void                        setMapCorner(Ogre::Vector3 corner);
    void                        setMapSize(Ogre::Vector3 size);

    inline float                convertWorldToScreenX(float xPos);
    inline float                convertWorldToScreenY(float yPos);

};

#endif // #ifndef BIGSCREENH