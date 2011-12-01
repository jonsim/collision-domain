/**
 * @file	Player.h
 * @brief 	Contains the Player nodes and the related data.
 */
#ifndef PLAYER_H
#define PLAYER_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "PlayerState.h"
#include "InputState.h"
#include "PhysicsCore.h"


/*-------------------- CLASS DEFINITIONS --------------------*/
/// CarType represents the type of car used as the player object (i.e. the model).
enum CarType {SMALL, MEDIUM, LARGE};

/// CarSkin represents the texture applied to the player object.
enum CarSkin {SKIN0, SKIN1, SKIN2, SKIN3, SKIN4, SKIN5, SKIN6, SKIN7, SKIN8, SKIN9};

/**
 *  @brief 	Contains the Player nodes and the related data.
 */
class Player //: public PhysicsEntity
{
 
public:
    OgreBulletDynamics::RaycastVehicle *mVehicle;

    Player (void);
    ~Player (void);
    void createPlayer (Ogre::SceneManager* sm, CarType t, CarSkin s, PhysicsCore *physicsCore);
    void attachCamera (Ogre::Camera* cam);
    void updatePlayer (PlayerState newState);
    void updateWheels (signed char m);
    void updateCamera (int XRotation, int YRotation);
    PlayerState getPlayerState (void);
    
private:
    PlayerState state;              ///< The player's current state.
    const float cameraRotationConstant;

    Ogre::SceneNode* playerNode;	///< The complete player node - this is what should be moved when the car moves.
    Ogre::SceneNode* camNode;		///< The node onto which a camera can be attached to observe the car.
    Ogre::SceneNode* camArmNode;
    Ogre::SceneNode* carNode;		///< The node onto which the car object and its components attach.
    Ogre::SceneNode* carLDoorNode;
    Ogre::SceneNode* carRDoorNode;
    Ogre::SceneNode* carFBumperNode;
    Ogre::SceneNode* carRBumperNode;
    Ogre::SceneNode* carFLWheelNode;
    Ogre::SceneNode* carFRWheelNode;
    Ogre::SceneNode* carRLWheelNode;
    Ogre::SceneNode* carRRWheelNode;

    void Player::createGeometry(Ogre::SceneManager *sm,
                                const std::string &entityName,
                                const std::string &meshName,
                                const std::string &materialName,
                                Ogre::SceneNode *toAttachTo);
};

#endif // #ifndef PLAYER_H
