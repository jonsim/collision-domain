/**
 * @file	SimpleCoupeCar.h
 * @brief 	A Car object with specific mesh and texture to create a simple coupe car
 */
#ifndef __SMALLCAR_H_
#define __SMALLCAR_H_

#include "stdafx.h"
#include "SharedIncludes.h"

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
	#include "boost\lexical_cast.hpp"
#else
	#include "boost/lexical_cast.hpp"
#endif

class SmallCar : public Car
{
public:
    SmallCar(int uniqueCarID, TeamID tid, ArenaID aid);
    virtual ~SmallCar(void);
    static void createCollisionShapes();

    void updateAudioPitchFrameEvent();
    void playCarHorn();
    void louderLocalSounds();
    virtual void updateTeam (TeamID tid);
    virtual void updateArena (ArenaID aid);
    virtual void loadDestroyedModel (void);

    virtual void makeBitsFallOff();
    virtual void startEngineSound();
    virtual void removeLDoor();
    virtual void removeRDoor();
    virtual void removeRBumper();
    virtual void removeFBumper();
    void removeLHeadlight();
    void removeRHeadlight();
    virtual void removeCarPart(unsigned int part);

private:
    void initTuning();
    void initNodes();
    void initGraphics(TeamID tid, ArenaID aid);
    void initBody(Ogre::Vector3 carPosition);
    void initWheels();
    
    // Car related Nodes (initNodes())
    Ogre::SceneNode *mLDoorNode;
    Ogre::SceneNode *mRDoorNode;
    Ogre::SceneNode *mFBumperNode;
    Ogre::SceneNode *mRBumperNode;
    Ogre::SceneNode *mLHeadlightNode;
    Ogre::SceneNode *mRHeadlightNode;

    btRigidBody     *mLDoorBody;
    btRigidBody     *mRDoorBody;
    btRigidBody     *mFBumperBody;
    btRigidBody     *mRBumperBody;
    btRigidBody     *mLHeadlightBody;
    btRigidBody     *mRHeadlightBody;
    
    Ogre::SceneNode *mChassisNode;
    Ogre::SceneNode *mFLWheelNode;
    Ogre::SceneNode *mFRWheelNode;
    Ogre::SceneNode *mRLWheelNode;
    Ogre::SceneNode *mRRWheelNode;
    
    OgreOggISound *mHornSound;
    OgreOggISound *mEngineSound;

    bool mHasLocalSounds;
};

#endif // #ifndef __SMALLCAR_H_

