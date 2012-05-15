/**
 * @file        Player.cpp
 * @brief       Contains the player car and the related data specific to each player.
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "Player.h"
#include "GameCore.h"
#include <sstream>
#include "MeshDeformer.h"
#include "Car.h"
#include "SimpleCoupeCar.h"
#include "SmallCar.h"
#include "TruckCar.h"

#define NEWCAM 1

#define MAX_DAMAGE 400
#define BIG_CRASH_THRESHOLD 80
/*-------------------- METHOD DEFINITIONS --------------------*/

/// @brief  Constructor, setting the player constants and zeroing the PlayerState.
Player::Player (bool isAI) 
  : cameraRotationConstant(0.08f),
    mAlive(true),
    mIsVIP(false), 
    mTeam(NO_TEAM),
    mCarSnapshot(NULL),
    mSnapshots(NULL),
    mCar(NULL),
    mIsAI(isAI)
{
    // PlayerState state configures constants and zeros values upon creation.
        //processingCollision = false;
        numCollisionDataPoints = 0;

        numCameraViews = 3;
        cameraView = 0;

        cameraViews[0]     = btVector3( 0.f, 5.f,  -10.f ); //default
        cameraViews[1]     = btVector3( 0.f, 3.8f, -9.f ); //lower+closer
        cameraViews[2]     = btVector3( 0.f, 2.8f, 0.f ); //windshield

        cameraLookViews[0] = btVector3( 0.f, 0.f, 3.0f );
        cameraLookViews[1] = btVector3( 0.f, 3.8f, 3.0f );
        cameraLookViews[2] = btVector3( 0.f, 2.8f, 8.f );

    std::stringstream ss;
    ss << "car type: " << mCarType << "\n";
    ss << "our team: " << mTeam << "\n";
    mFirstLaunch = true;

    for (int i = 0; i < NUM_POWERUP_BOARDS; i++)
    {
        mPowerupBoards[i] = GameCore::mSceneMgr->createBillboardSet( 1 );
        mPowerupBoards[i]->setDefaultDimensions( 1.0f, 1.0f );
        mPowerupBoards[i]->setCastShadows( false );
        mPowerupBoards[i]->setMaterialName( "board_powerup_good_heavy" );

        mPowerupStates[i] = PowerupBoardState(POWERUP_BOARD_HEAVY, -1.0f);
    }

    mPowerupBars[0] = mPowerupBoards[0]->createBillboard( -0.65f, 3.3f, 0, Ogre::ColourValue(1.0f, 1.0f, 1.0f, 0.0f) );
    mPowerupBars[1] = mPowerupBoards[1]->createBillboard( 0.66f, 3.3f, 0, Ogre::ColourValue(1.0f, 1.0f, 1.0f, 0.0f) );
    mPowerupBars[2] = mPowerupBoards[2]->createBillboard( 0, 4.5f, 0, Ogre::ColourValue(1.0f, 1.0f, 1.0f, 0.0f) );

    mBoards = GameCore::mSceneMgr->createBillboardSet( 5 );
    mBoards->setDefaultDimensions( 1.5f, 0.18f );

    mBacks = GameCore::mSceneMgr->createBillboardSet( 5 );
    mBacks->setDefaultDimensions( 1.5f, 0.18f );
    mBacks->setRenderQueueGroup( Ogre::RENDER_QUEUE_OVERLAY );

    mHealthbar = NULL;
    mHealthBg = NULL;
    mNametag = NULL;

    mLastKiller = NULL;

    camShakeFrames = 0;

    for( int i = 0; i < POWERUP_COUNT; i ++ )
        powerupTimers[i] = 0;

    //OutputDebugString(ss.str().c_str());

        //averageCollisionPoint.setZero();

        // Damage Level Thresholds: how many calls to collisionTickCallback have been seen
        //lowDamageThreshold = 25;
        //mediumDamageThreshold = 55;
        //highDamageThreshold = 80;
        //btVector3::setZero(averageCollisionPoint);
}


/// @brief   Deconstructor.
Player::~Player (void)
{
    if( mCar )
    {
        delete( mCar );
        mCar = NULL;
    }
}

/// @brief  Set the player's nickname and update their nametag
/// @param  szNick  String for player nickname
void Player::setNickname (const char *szNick)
{
    mNickname = strdup( szNick );
    if( mNametag != NULL )
        mNametag->setCaption( szNick );
}

/// @brief  Creates and positions the 3D player object (and space for a camera).
/// @param  sm  The SceneManager to which the 3D player object is attached.
/// @param  t   The car model to load as the player object.
/// @param  s   The texture to apply to the car model.
/// @param  physicsCore   The class containing the physics world.
void Player::createPlayer (CarType carType, TeamID tid, ArenaID aid)
{
    if( mCar )
        delCar();

    mCarType = carType;

    switch (carType) {
        case CAR_BANGER:
            mCar = (Car*) new SimpleCoupeCar(GameCore::mPhysicsCore->getUniqueEntityID(), tid, aid);
            rearDamageBoundary = -2.1f;
            frontDamageBoundary = 1.6f;
            break;
        case CAR_SMALL:
            mCar = (Car*) new SmallCar(GameCore::mPhysicsCore->getUniqueEntityID(), tid, aid);
            rearDamageBoundary = -1.4f;
            frontDamageBoundary = 0.8f;
            break;
        case CAR_TRUCK:
            mCar = (Car*) new TruckCar(GameCore::mPhysicsCore->getUniqueEntityID(), tid, aid);
            rearDamageBoundary = -2.15f;
            frontDamageBoundary = 1.3f;
            break;
        default:
            throw Ogre::Exception::ERR_INVALIDPARAMS;
            break;
    }
    
    bool isLocalPlayer = this == GameCore::mPlayerPool->getLocalPlayer();
    if (mFirstLaunch)
    {
        if(isLocalPlayer)
        {
            mCar->louderLocalSounds();
            GameCore::mGui->setupDamageDisplay(carType, tid);
        }
        //else
        {
            // Create the healthbar
            float boardHeight;
            switch( carType )
            {
            case CAR_BANGER:
            case CAR_SMALL:
                boardHeight = 2.f;
                break;
            case CAR_TRUCK:
                boardHeight = 3.f;
                break;
            }
            mHealthbar = mBoards->createBillboard( 0, boardHeight, 0, Ogre::ColourValue::Green );
            mBoards->setCastShadows( false );
            mBoards->setMaterialName( "board_health" );

            // Create healthbar border
            mHealthBg = mBacks->createBillboard( 0, boardHeight, 0, Ogre::ColourValue(0,0,0) );
            mBacks->setCastShadows( false );
            mBacks->setMaterialName( "board_health_bg" );

            // Create nametag
            std::string txtIdent = "TXT_";
            txtIdent.append( mPlayerGUID.ToString() );
            mNametag = new MovableText( txtIdent, mNickname, "DejaVuSans", 0.3f );
            mNametag->setTextAlignment( MovableText::H_CENTER, MovableText::V_ABOVE );
            mNametag->setGlobalTranslation( Ogre::Vector3(0, boardHeight, 0) );
            mNametag->setCastShadows( false );

            // Set nametag colour according to team
#ifdef TEAM_COLOURED_NAMEPLATES
            switch( mTeam )
            {
            case BLUE_TEAM:
                mNametag->setColor( Ogre::ColourValue::Blue );
                break;
            case RED_TEAM:
                mNametag->setColor( Ogre::ColourValue::Red );
                break;
            default:
                mNametag->setColor( Ogre::ColourValue::White );
            }
#else
            mNametag->setColor( Ogre::ColourValue::White );
#endif
        }

        mFirstLaunch = false;
    }
    
    for (int i = 0; i < NUM_POWERUP_BOARDS; i++)
    {
        mCar->mBodyNode->attachObject( mPowerupBoards[i] );
    }

    //if( !isLocalPlayer )
    {
        //if( mBoards->isAttached() )
        //    mBoards->detachFromParent();
        //if( mBacks->isAttached() )
        //    mBacks->detachFromParent();
        
        mCar->mBodyNode->attachObject( mBoards );
        mCar->mBodyNode->attachObject( mBacks );
        mCar->mBodyNode->attachObject( mNametag );
    }
    if( isLocalPlayer )
        GameCore::mClientGraphics->mGameCam->setTransform( btVector3( 0, -100, 0 ) );

    hp                    = INITIAL_HEALTH;
    initialHP             = INITIAL_HEALTH;
    damageShareTL = 0.05f;
    damageShareBL = 0.2f;
    damageShareML = 0.25f;
    damageShareTR = 0.05f;
    damageShareBR = 0.25f;
    damageShareMR = 0.2f;

        roundScore = 0;
    mCar->attachCollisionTickCallback(this);
    
    mCar->moveTo(btVector3(0,0.5,0));
    crashCount = 0;
}

void Player::angleTest(void) {
    
}
/// @brief  Called back every substep of physics stepSim (so potentially multiple times a frame)
/// @param  hitPoint            Location of the collision point on the collision mesh - in world coordinates
/// @param  speed                       The speed of the impact in the direction of the normal to the collision point
/// @param  causedByPlayer      Pointer to the other player in the collision.
#if _WIN32
    void Player::collisionTickCallback(Ogre::Vector3 &hitPoint, Ogre::Real damage, unsigned int damageSection, int crashType, Player *causedByPlayer) {
#else
        void Player::collisionTickCallback(Ogre::Vector3 hitPoint, Ogre::Real &damage, int& damageSection, int &crashType, Player *&causedByPlayer) {
#endif
    bool isFront = ((damageSection == 0) || (damageSection == 1));
    TeamID tid = (GameCore::mGameplay->getGameMode() == FFA_MODE) ? NO_TEAM : getTeam();
    float shrapnelPlaneOffset = (getCarType() == CAR_SMALL) ? -0.75f : ((getCarType() == CAR_BANGER) ? -0.85f : -1.25f);
    float shrapnelCount    = damage / 10.0f;
    float shrapnelMaxSpeed = damage / 20.0f;

    if(damage > 40.f) getCar()->removeCarPart(damageSection);
    switch(crashType) {
        case 1:
#ifdef PARTICLE_EFFECT_SPARKS
            GameCore::mClientGraphics->generateSparks(hitPoint, Ogre::Vector3(1, 0.6f, 1));
#endif
            break;
        case 2:
#ifdef PARTICLE_EFFECT_SHRAPNEL
            GameCore::mClientGraphics->generateShrapnel(hitPoint, tid, shrapnelCount, shrapnelMaxSpeed, shrapnelPlaneOffset);
#endif
            break;
        case 3:
            GameCore::mClientGraphics->mMeshDeformer->collisonDeform(this->getCar()->mBodyNode, hitPoint, damage * 0.04, isFront);
#ifdef PARTICLE_EFFECT_SHRAPNEL
            GameCore::mClientGraphics->generateShrapnel(hitPoint, tid, shrapnelCount, shrapnelMaxSpeed, shrapnelPlaneOffset);
#endif
            break;
        default:
            // error
            break;
    }
}


void Player::cameraLookLeft(void) {
    GameCore::mClientGraphics->mGameCam->setCollidable(false);
    GameCore::mClientGraphics->mGameCam->setCamType( CAM_FIXED );
        GameCore::mClientGraphics->mGameCam->setOffset( btVector3(5, 0, 0) );
    GameCore::mClientGraphics->mGameCam->setLookOffset( btVector3(0,0,0) );
}

void Player::cameraLookRight(void) {
    GameCore::mClientGraphics->mGameCam->setCollidable(false);
    GameCore::mClientGraphics->mGameCam->setCamType( CAM_FIXED );
        GameCore::mClientGraphics->mGameCam->setOffset( btVector3(-5, 0, 0) );
    GameCore::mClientGraphics->mGameCam->setLookOffset( btVector3(0,0,0) );
}

void Player::cameraLookBack(void) {
    GameCore::mClientGraphics->mGameCam->setCollidable(false);
    GameCore::mClientGraphics->mGameCam->setCamType( CAM_FIXED );
    GameCore::mClientGraphics->mGameCam->setOffset( btVector3(0, 0, 5) );
    GameCore::mClientGraphics->mGameCam->setLookOffset( btVector3(0,0,0) );
    
}

void Player::revertCamera(void) {
    GameCore::mClientGraphics->mGameCam->setOffset( cameraViews[cameraView] );
    GameCore::mClientGraphics->mGameCam->setLookOffset( cameraLookViews[cameraView] );
    GameCore::mClientGraphics->mGameCam->setCollidable(true);
    GameCore::mClientGraphics->mGameCam->setCamType( CAM_CHASE );
}

void Player::cycleCameraView(void) {
        cameraView = (++cameraView) & numCameraViews;
        switch(cameraView) {
                case 0 :
                case 1 :
                        GameCore::mClientGraphics->mGameCam->setCamType( CAM_CHASE );
                        break;
                case 2 :
                        GameCore::mClientGraphics->mGameCam->setCamType( CAM_FIXED );
                        break;
        }
    GameCore::mClientGraphics->mGameCam->setOffset( cameraViews[cameraView] );
    GameCore::mClientGraphics->mGameCam->setLookOffset( cameraLookViews[cameraView] );
}

/// @brief  Attaches a camera to the player.
/// @param  cam   The camera object to attach to the player.
void Player::attachCamera (Ogre::Camera* cam)
{
    // only attach a camera to one of them!! Imagine the carnage if there were more
    camNode = mCar->attachCamNode();
    camArmNode = camNode->getParentSceneNode();
    camNode->translate(0, 0, -20); // zoom in!! (50 is a fair way behind the car, 75 is in the car)

#if NEWCAM

    GameCore::mPlayerPool->setSpectating( GameCore::mPlayerPool->getLocalPlayerID() );
#else
    mCarCam = new CarCam(mCar,cam, camNode, mCar->mBodyNode);
#endif
}


/// @brief  Applies the player controls to the car so it will move on next stepSimulation.
/// @param  userInput               The latest user keypresses.
/// @param  secondsSinceLastFrame   The time in seconds since the last frame, for framerate independence.
void Player::processControlsFrameEvent(
        InputState *userInput,
        Ogre::Real secondsSinceLastFrame)
{
        //Only take input if the player is alive
        if(this->mAlive && this->getCar())
        {
                // process steering and apply acceleration
                mCar->steerInputTick(userInput->isLeft(), userInput->isRight(), secondsSinceLastFrame);
                mCar->accelInputTick(userInput->isForward(), userInput->isBack(), userInput->isHandbrake(), secondsSinceLastFrame);
        }
    else
    {
        mCar->steerInputTick(false, false, secondsSinceLastFrame);
        mCar->accelInputTick(false, false, false, secondsSinceLastFrame);
    }
}

void Player::pushBackNewPowerupBoard(PowerupBoardType type, float fadeOutInSeconds)
{
    int matchedIndex = -1, finishedIndex = -1;
    for (int i = 0; i < NUM_POWERUP_BOARDS; i++)
    {
        // this method will return quickly if the powerup in question is already disabled
        if (mPowerupStates[i].isFinished()) finishedIndex = i;
        if (mPowerupStates[i].isType(type)) matchedIndex = i;
    }

    if (matchedIndex < 0 && finishedIndex < 0)
    {
        OutputDebugString("Never happen case in Player::pushBackNewPowerupBoard\n");
        return;
    }
    else if (matchedIndex >= NUM_POWERUP_BOARDS || finishedIndex >= NUM_POWERUP_BOARDS)
    {
        OutputDebugString("Invalid index in Player::pushBackNewPowerupBoard\n");
        return;
    }

    switch (type)
    {
        case POWERUP_BOARD_HEAVY:
            mPowerupBoards[ matchedIndex >= 0 ? matchedIndex : finishedIndex ]->setMaterialName( "board_powerup_good_heavy" );
            break;
        case POWERUP_BOARD_LIGHT:
            mPowerupBoards[ matchedIndex >= 0 ? matchedIndex : finishedIndex ]->setMaterialName( "board_powerup_bad_light" );
            break;
        case POWERUP_BOARD_HEALTH:
            mPowerupBoards[ matchedIndex >= 0 ? matchedIndex : finishedIndex ]->setMaterialName( "board_powerup_good_health" );
            break;
        case POWERUP_BOARD_SPEED:
            mPowerupBoards[ matchedIndex >= 0 ? matchedIndex : finishedIndex ]->setMaterialName( "board_powerup_good_speed" );
            break;
        default:
            // make it so we can never push back a powerup with invalid type
            return;
    }
    
    mPowerupStates[ matchedIndex >= 0 ? matchedIndex : finishedIndex ] = PowerupBoardState(type, fadeOutInSeconds);
    
    reLayoutPowerupBoards();
}

void Player::reLayoutPowerupBoards()
{
    // Shuffle the powerups around if they have finished
    float remaining0 = mPowerupStates[0].getRemainingTime();
    float remaining1 = mPowerupStates[1].getRemainingTime();
    float remaining2 = mPowerupStates[2].getRemainingTime();

    int orderedIndicesLongtoShort[3];

    if (remaining0 >= remaining1 && remaining0 >= remaining2)
    {
        bool is1bigger = remaining1 > remaining2;
        orderedIndicesLongtoShort[0] = 0;
        orderedIndicesLongtoShort[1] = is1bigger ? 1 : 2;
        orderedIndicesLongtoShort[2] = is1bigger ? 2 : 1;
    }
    else if (remaining1 >= remaining0 && remaining1 >= remaining2)
    {
        bool is0bigger = remaining0 > remaining2;
        orderedIndicesLongtoShort[0] = 1;
        orderedIndicesLongtoShort[1] = is0bigger ? 0 : 2;
        orderedIndicesLongtoShort[2] = is0bigger ? 2 : 0;
    }
    else
    {
        bool is0bigger = remaining0 > remaining1;
        orderedIndicesLongtoShort[0] = 2;
        orderedIndicesLongtoShort[1] = is0bigger ? 0 : 1;
        orderedIndicesLongtoShort[2] = is0bigger ? 1 : 0;
    }

    // 'shuffle down' the layout as powerup icons finish their time
    if ( mPowerupStates[ orderedIndicesLongtoShort[1] ].isFinished() )
    {
        // there is only 1 powerup icon active, center it
        mPowerupBars[ orderedIndicesLongtoShort[0] ]->setPosition( 0, 3.3f, 0 );
    }
    else
    {
        mPowerupBars[ orderedIndicesLongtoShort[0] ]->setPosition(   0.66f, 3.3f, 0 );
        mPowerupBars[ orderedIndicesLongtoShort[1] ]->setPosition( - 0.65f, 3.3f, 0 );
        mPowerupBars[ orderedIndicesLongtoShort[2] ]->setPosition(       0, 4.5f, 0 );
    }
}

void Player::frameEvent(float time)
{
    for (int i = 0; i < NUM_POWERUP_BOARDS; i++)
    {
        // this method will return quickly if the powerup in question is already disabled
        if ( mPowerupStates[i].timeElapsed(mPowerupBars[i], time) )
        {
            reLayoutPowerupBoards();
        }
    }

    // Remove any powerups that have expired
    for( int i = 0; i < POWERUP_COUNT; i ++ )
    {
        if( powerupTimers[i] != 0 && RakNet::GreaterThan( RakNet::GetTimeMS(), powerupTimers[i] ) )
        {
            switch( i )
            {
            case POWERUP_MASS:
                getCar()->resetMass();
                getCar()->resetEngineForce();
                break;
            case POWERUP_SPEED:
                getCar()->resetEngineForce();
                break;
            }

            powerupTimers[i] = 0;
        }
    }

    if( this == GameCore::mPlayerPool->getLocalPlayer() && getPlayerState() != PLAYER_STATE_SPECTATE && mLastKiller != NULL && RakNet::GreaterThan( RakNet::GetTimeMS(), mTimeLastKilled+3000 ) )
    {
        setPlayerState( PLAYER_STATE_SPECTATE );
        GameCore::mPlayerPool->setSpectating( mLastKiller->getPlayerGUID() );
        mLastKiller = NULL;
    }
}

/// @brief  Updates the camera's rotation based on the values given.
/// @param  XRotation   The amount to rotate the camera by in the X direction (relative to its current rotation).
/// @param  YRotation   The amount to rotate the camera by in the Y direction (relative to its current rotation).
void Player::updateCameraFrameEvent (int XRotation, int YRotation, int ZDepth, float time, GameCamera *gameCamera)
{
    //Ogre::SceneNode *camNode = mCar->attachCamNode();
    //Ogre::SceneNode *camArmNode = camNode->getParentSceneNode();
    
    /*if(camShakeCounter > 0) {
        
        camArmNode->yaw(Ogre::Degree((-cameraRotationConstant * XRotation)+(rand()/RAND_MAX)*10), Ogre::Node::TS_PARENT);
            camArmNode->pitch(Ogre::Degree((cameraRotationConstant * 0.5f * -YRotation)+(rand()/RAND_MAX)*10), Ogre::Node::TS_LOCAL);
        camShakeCounter--;
    } else {*/
        camArmNode->yaw(Ogre::Degree(-cameraRotationConstant * XRotation), Ogre::Node::TS_PARENT);
            camArmNode->pitch(Ogre::Degree(cameraRotationConstant * 0.5f * -YRotation), Ogre::Node::TS_LOCAL);
    //}

        Ogre::Vector3 camPosition = camNode->getPosition();
        ZDepth = -ZDepth;
        if ((ZDepth < 0 && camPosition.z > -40) || (ZDepth > 0 && camPosition.z < 90))
                camNode->translate(0, 0, ZDepth * 0.02f);

    if( camShakeFrames > 0 )
    {
        camShakeFrames --;
        if( camShakeFrames > 0 )
        {
            btVector3 camPos = gameCamera->getOffset();
            float curX = camPos.x();
            curX -= 5;
            curX += rand() % 10;
            camPos.setX( curX );
            float curY = camPos.y();
            curY -= 5;
            curY += rand() % 10;
            camPos.setY( curY );
            float curZ = camPos.z();
            curZ -= 5;
            curZ += rand() % 10;
            camPos.setZ( curZ );
            gameCamera->setTempOffset( camPos );
        }
        else
            gameCamera->resetTempOffset();
    }
   
        //Update the camera
        //

        //std::stringstream ssTmp;
        //ssTmp << camShakeCounter << "\n";
        //OutputDebugString(ssTmp.str().c_str());

}

/// @brief  Updates graphics for the local player (with effects that should only be applied from that, for example
///         radial blur, screen cracks etc.
void Player::updateLocalGraphics (void)
{
        // Update radial blur (from vehicle speed).
#ifdef COMPOSITOR_RADIAL_BLUR
        float speedmph = mCar->getCarMph();
        float blurAmount = 0;

        if (speedmph > 40.0f)
        {
                // calculate blurring as a function of speed, then scale it back depending on where you
                // are looking at the car from (effect strongest from behind and infront (3 maxima at
                // +/-180 and 0, hence the double abs() reduction)).
                blurAmount = (speedmph - 40) / 28;
                blurAmount *= abs(abs(GameCore::mPlayerPool->getLocalPlayer()->getCameraYaw()) - 90) / 90;
        }
    GameCore::mClientGraphics->setRadialBlur(GameCore::mClientGraphics->mCamera->getViewport(), blurAmount);
#endif
}

/// @brief  Updates graphics for all players (called individually for each player in player pool), contains graphical
///         effects all players will have.
void Player::updateGlobalGraphics (Ogre::Real secondsSinceLastFrame)
{
    if (mCar != NULL)
        mCar->updateParticleSystems(secondsSinceLastFrame, hp);
}

/// @brief Returns the camera current yawing around the player.
/// @return The yawing, in degrees, around the player. 0 is directly in front of the player, +/-180 is behind.
float Player::getCameraYaw ()
{
        return camArmNode->getOrientation().getYaw().valueDegrees();
}


/// @brief  Supplies the Car object which contains player position and methods on that. 
/// @return The Car object which allows forcing a player to a given CarSnapshot or getting a CarSnapshot.
Car* Player::getCar()
{
    return mCar;
}


void Player::killPlayer()
{
        if(!mCar)
                return;

        this->mAlive = false;
    // Place an explosion at the players position and load the burnt model
#ifdef PARTICLE_EFFECT_EXPLOSION
    GameCore::mClientGraphics->generateExplosion(mCar->mBodyNode->getPosition(), true);
#endif
    mCar->loadDestroyedModel();

    // Blast the stuff out of the car (renders it completely undriveable but since this
    // should only be called on dead cars thats not such a problem).
    // Yeah so turns out this just stuffs everything up and not in a good way.
    mCar->applyForce(mCar->mBodyNode, Ogre::Vector3(0, 500.0f, 0));
}

void Player::setAlive(bool pAlive)
{
        mAlive = pAlive;
}

bool Player::getAlive()
{
        return mAlive;
}

void Player::setGUID(RakNet::RakNetGUID playerGUID) {
        stringGUID = playerGUID.ToString();
}

std::string Player::getGUID(void) {
        return stringGUID;
}

RakNet::RakNetGUID Player::getPlayerGUID()
{
        return mPlayerGUID;
}

void Player::setPlayerGUID(RakNet::RakNetGUID playerGUID)
{
        mPlayerGUID = playerGUID;
}

void Player::addToScore(int amount)
{
        roundScore += amount;
}

int Player::getRoundScore()
{
        return this->roundScore;
}
void Player::addToGameScore(int amount)
{
        this->gameScore += amount;
}

// HEALTH RELATED FUNCTIONS

void Player::serverSaysHealthChangedTo(float newHP)
{
    // NOTE TO WHOM IT MAY CONCERN:
    // I Have removed the early exit if it's not the local player in order to adjust opponent healthbars
    // When GUI stuff is uncomment, this check will need to surround GUI changes

    //if( this != GameCore::mPlayerPool->getLocalPlayer() )
    //{
    //    this->hp = newHP;
    //    return;
    //}

    std::stringstream ss;
    ss << "localplayer health changed to " << newHP << "\n";

    //OutputDebugString(ss.str().c_str());


    // update damage HUD

    float redLimit = (float) INITIAL_HEALTH * 0.25f;
    float yelLimit = (float) INITIAL_HEALTH * 0.66f;

    float healthPercent = newHP / (float) INITIAL_HEALTH;
    if( mHealthbar )
        mHealthbar->setDimensions( 1.5f * healthPercent, 0.18f );
    
    // the last quarter is red
    if (newHP <= redLimit)
    {
        if (this->hp > redLimit)
        {
            // set colour to red, as it was yellow previously

            // leave some components randomly yellow for a "cheat" damage system on demo day :D
            // part 0-body, 1-engine, 2-fl, 3-fr, 4-rl, 5-rr.
            // colour 0-green, 1-yellow, 2-red
            //GameCore::mGui->updateDamage(0, 2-(rand()%2));
            //GameCore::mGui->updateDamage(1, 2-(rand()%2));
            //GameCore::mGui->updateDamage(2, 2-(rand()%2));
            //GameCore::mGui->updateDamage(3, 2-(rand()%2));
            //GameCore::mGui->updateDamage(4, 2-(rand()%2));
            //GameCore::mGui->updateDamage(5, 2-(rand()%2));
            if( mHealthbar )
                mHealthbar->setColour( Ogre::ColourValue::Red );
        }
    }
    else if (newHP <= yelLimit)
    {
        if (this->hp > yelLimit)
        {
            // set colour to yellow, as it was green previously
            //GameCore::mGui->updateDamage(0, 1-(rand()%2));
            //GameCore::mGui->updateDamage(1, 1-(rand()%2));
            //GameCore::mGui->updateDamage(2, 1-(rand()%2));
            //GameCore::mGui->updateDamage(3, 1-(rand()%2));
            //GameCore::mGui->updateDamage(4, 1-(rand()%2));
            //GameCore::mGui->updateDamage(5, 1-(rand()%2));
            if( mHealthbar )
                mHealthbar->setColour( Ogre::ColourValue( 1,1,0) );
        }
    }
    else
    {
        if (this->hp <= yelLimit)
        {
            // change colour back to green, its been red or yellow already
            //GameCore::mGui->updateDamage(0, 0);
            //GameCore::mGui->updateDamage(1, 0);
            //GameCore::mGui->updateDamage(2, 0);
            //GameCore::mGui->updateDamage(3, 0);
            //GameCore::mGui->updateDamage(4, 0);
            //GameCore::mGui->updateDamage(5, 0);
            if( mHealthbar )
                mHealthbar->setColour( Ogre::ColourValue::Green );
        }
    }
    
    this->hp = newHP;
}

/// @brief  This is called when a client receives changes to their own local damage points
void Player::processDamage( PLAYER_DAMAGE_LOC damageIn )
{
    float piss = ((damageIn.damageBL * damageShareBL) + 
           (damageIn.damageBR * damageShareBR) + 
           (damageIn.damageML * damageShareML) + 
           (damageIn.damageMR * damageShareMR) + 
           (damageIn.damageTL * damageShareTL) + 
           (damageIn.damageTR * damageShareTR));

    std::stringstream ss;
    ss << piss << "\n";
    GameCore::mGui->chatboxAddMessage("damage", (char*)ss.str().c_str());

    float damageShares[6] = {
        damageIn.damageTL / ((float)INITIAL_HEALTH * (1-damageShareTL)),  // 
        damageIn.damageTR / ((float)INITIAL_HEALTH * (1-damageShareTR)),  // 
        damageIn.damageML / ((float)INITIAL_HEALTH * (1-damageShareML)),  // 
        damageIn.damageMR / ((float)INITIAL_HEALTH * (1-damageShareMR)),  // 
        damageIn.damageBL / ((float)INITIAL_HEALTH * (1-damageShareBL)),  // 
        damageIn.damageBR / ((float)INITIAL_HEALTH * (1-damageShareBR))   // 
    }; // keep in order!

    int colour = 0;;
    for(unsigned int i = 0; i<6; i++) {
        if(damageShares[i] <= 0.5f) {
            colour = 0;
        } else if(damageShares[i] > 0.5f && damageShares[i] <= 0.8f) {
            colour = 1;
        } else if(damageShares[i] > 0.8f) {
            colour = 2;
        }
        GameCore::mGui->updateDamage(mCarType, i, colour);
    }






    
}

void Player::lowDamageCallBack(std::string causedBy) {

}

void Player::midDamageCallBack(std::string causedBy) {

}

void Player::highDamageCallBack(std::string causedBy) {
}

void Player::applyHealthBonus()
{

}

int Player::getHP()
{
        return hp;
}

void Player::resetHP()
{
        //this->hp = INITIAL_HEALTH;
    serverSaysHealthChangedTo((float) INITIAL_HEALTH);
}

void Player::killPlayer(Player* causedBy)
{
        this->killPlayer();
        GameCore::mGameplay->markDeath(this,causedBy);
    GameCore::mGameplay->handleDeath(this,causedBy);
    mLastKiller = causedBy;
    mTimeLastKilled = RakNet::GetTimeMS();

    if( this == GameCore::mPlayerPool->getLocalPlayer() )
    {
        //InfoItem *spectate = new InfoItem( PLAYER_KILLED_OT, 0, 3000 );
        //GameCore::mGameplay->mInfoItems.push_back( spectate );
    }

    GameCore::mGui->updateLocalPlayerRank();
}

void Player::setTeam(TeamID newTeam) 
{ 
    mTeam = newTeam; 
    if( mCar ) mCar->updateTeam(newTeam); 
    
    //Add it to the gameplay teams
    //GameCore::mGameplay->getTeam(newTeam)->addPlayer(this);
}

// used to place human players more importantly above AI players on the final scoreboards and rank.
bool Player::isAI()
{
    return mIsAI;
}

void Player::addPowerup( PowerupType type, RakNet::TimeMS endtime )
{
    powerupTimers[type] = endtime;
}
