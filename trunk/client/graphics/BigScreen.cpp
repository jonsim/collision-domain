#include "stdafx.h"
#include "BigScreen.h"
#include "GameCore.h"


#define MAP_WIDTH	1.0f
#define MAP_HEIGHT	1.0f // 0.8

#define MARKER_WIDTH 0.03f
#define MARKER_HEIGHT 0.04f

BigScreen::BigScreen()
  : viewCameraVector(NULL),
  olcMap(NULL),
  oleCar(NULL),
  mapCorner(0,0,0),
  mapSize(0,0,0),
  oleVIP1(NULL),
  oleVIP2(NULL),
  isTimerSetup(false)
{
}

void BigScreen::addCamera(Ogre::Camera* cam) 
{
        ViewCamera* tmpVC = new ViewCamera(cam);
        viewCameraVector.push_back(tmpVC);
}

void BigScreen::refreshMap()
{
    ArenaID currentArena = GameCore::mGameplay->getArenaID();

    switch(currentArena)
    {
        case COLOSSEUM_ARENA:
            olcMap->setMaterialName( "map_top_1" );
            break;
        case FOREST_ARENA:
            olcMap->setMaterialName( "map_top_2" );
            break;
        case QUARRY_ARENA:
            olcMap->setMaterialName( "map_top_3" );
            break;
    }
}

void BigScreen::setupMapView()
{
        Ogre::Overlay *olMap = 
                Ogre::OverlayManager::getSingleton().create( "OVERLAY_MAP" );
        olMap->setZOrder(500);
        olMap->show();

        olcMap = static_cast<Ogre::OverlayContainer*> ( Ogre::OverlayManager::getSingleton().
                createOverlayElement( "Panel", "MAP" ) );

        olcMap->setMetricsMode( Ogre::GMM_RELATIVE );
        olcMap->setHorizontalAlignment( Ogre::GHA_LEFT);
        olcMap->setVerticalAlignment(Ogre::GVA_TOP);
        
        //Set the map
        olcMap->setDimensions(MAP_WIDTH,MAP_HEIGHT);
        olcMap->setMaterialName( "map_top_1" );
        this->refreshMap();
        olMap->add2D(olcMap);

    //Set the dimensions
    try
    {
        Ogre::Entity*    ae = GameCore::mSceneMgr->getEntity("Arena");
        Ogre::SceneNode* an = GameCore::mSceneMgr->getSceneNode("ArenaNode");
        setMapCorner(an->getPosition());
        setMapSize(ae->getBoundingBox().getSize() * an->getScale());
    }
    catch (Exception&)
    {
        OutputDebugString("Exception caught while creating the bigScreen view, arena not fully initialised.\n");
    }

    //Create the overlay elements for the cars, VIPS and anything else
    oleVIP1 = Ogre::OverlayManager::getSingleton().createOverlayElement( 
                    "Panel", "VIP1" );
    oleVIP2 = Ogre::OverlayManager::getSingleton().createOverlayElement( 
                    "Panel", "VIP2" );

    oleVIP1->setMetricsMode( Ogre::GMM_RELATIVE );
    oleVIP2->setMetricsMode( Ogre::GMM_RELATIVE );

    oleVIP1->setDimensions(0.03f,0.03f);
    oleVIP2->setDimensions(0.03f,0.03f);
        
    oleVIP1->setMaterialName( "VIPIcon" );
    oleVIP2->setMaterialName( "VIPIcon" );

    oleVIP1->setPosition(0.0f,2.0f);
    oleVIP2->setPosition(0.0f,2.0f);

    olcMap->addChild(oleVIP1);
    olcMap->addChild(oleVIP2);

    //Create the round timer
    this->createRoundTimer();
}

void BigScreen::updateMapView()
{
    // Loop through all possible players
    std::vector<Player*> players = GameCore::mPlayerPool->getPlayers();
    for(std::vector<Player*>::iterator it = players.begin();it != players.end();it++)
    {
        if ( (*it)->getCar()
            && (*it)->getCar()->getBigScreenOverlayElement() )
        {
            updatePlayer( (*it)->getCar(), (*it), (*it)->getCar()->getBigScreenOverlayElement() );
        }
    }

    // Loop through all possible powerups
    std::vector<Powerup *> powerups = GameCore::mPowerupPool->getPowerups();
    for (unsigned int i = 0; i < powerups.size(); i++)
    {
        updatePowerup(
            powerups[i]->getPosition(),
            powerups[i]->getRotation(),
            powerups[i]->getBigScreenOverlayElement());
    }

    updateRoundTimer();
}

inline float BigScreen::convertWorldToScreenX(float xPos)
{
    xPos = xPos/(mapSize.x);
    // This value lines up perfectly so the point of the arrow hits the wall if going straight on,
    // and if riding sideways along the wall you are pretty much displayed on top of the boundary
    xPos *= 0.76f;

    xPos += MAP_WIDTH/2;
    xPos *= MAP_WIDTH;
    xPos -= 0.5 * MARKER_WIDTH;

    return xPos;
}

inline float BigScreen::convertWorldToScreenY(float yPos)
{
    yPos = yPos/(mapSize.z);
    // This value lines up perfectly so the point of the arrow hits the wall if going straight on,
    // and if riding sideways along the wall you are pretty much displayed on top of the boundary
    yPos *= 0.74f;

    yPos += MAP_HEIGHT/2;
    yPos *= MAP_HEIGHT;
    yPos -= 0.5 * MARKER_HEIGHT;

    return yPos;
}

void BigScreen::updatePlayer(Car *car, Player *player, Ogre::OverlayElement* carOverlay)
{
    if (!car || !player || !carOverlay) return;

    CarSnapshot *playerSnap = car->getCarSnapshot();
    if (!playerSnap) return;

    btVector3 localPlayerPos = playerSnap->mPosition;
    float xPos = convertWorldToScreenX(localPlayerPos.getX()); 
    float yPos = convertWorldToScreenY(localPlayerPos.getZ()); //Z as we're doing a 2D projection

    // Calculate rotation from the car's chassis. This is projected straight to +Z. This can be done by passing through
    // Ogre's quaternions and using the getYaw() function (as in r314), it just seemed overly complicated.
    btQuaternion q = playerSnap->mRotation;
    Ogre::Radian rot = Ogre::Math::ATan2(      (2.0 * (q.getZ()*q.getX() + q.getY()*q.getW())),
                                1.0 - (2.0 * (q.getX()*q.getX() + q.getY()*q.getY())) );
    //rot = (rot.valueRadians() > 0) ? rot.valueRadians() - Ogre::Math::PI : rot.valueRadians() + Ogre::Math::PI;
                
    // Rotate the arrow.
    Ogre::Material* matMarker = carOverlay->getMaterial().get();
    Ogre::TextureUnitState* texMarker = matMarker->getTechnique(0)->getPass(0)->getTextureUnitState(0);
    texMarker->setTextureRotate(rot);

    // This is not really needed if the above code is working
    if(xPos > 1)
        xPos = 1.0f;
    if(yPos > 1)
        yPos = 1.0f;

    //Only check if get VIP
    if(player->getVIP())
    {
        if(player->getTeam() == 1)
        {
                oleVIP1->setPosition(xPos,yPos-0.03f);
        }
        else
        {
                oleVIP2->setPosition(xPos,yPos-0.03f);
        }
    }

    carOverlay->setPosition(xPos,yPos);

    delete playerSnap; //Fixes the memory leak
}

#if _WIN32
void BigScreen::updatePowerup(Ogre::Vector3 location, btQuaternion &quat, Ogre::OverlayElement* powerupOverlay)
#else
void BigScreen::updatePowerup(Ogre::Vector3 location, btQuaternion quat, Ogre::OverlayElement* powerupOverlay)
#endif
{
    if (!powerupOverlay) return;

    float xPos = convertWorldToScreenX(location.x); 
    float yPos = convertWorldToScreenY(location.z); //Z as we're doing a 2D projection
    
    // Calculate rotation from the car's chassis. This is projected straight to +Z. This can be done by passing through
    // Ogre's quaternions and using the getYaw() function (as in r314), it just seemed overly complicated.
    Ogre::Radian rot = Ogre::Math::ATan2( ( 2.0 * ( quat.z() * quat.x() + quat.y() * quat.w() ) ),
                                    1.0 - ( 2.0 * ( quat.x() * quat.x() + quat.y() * quat.y() ) ) );
    
    // Rotate the arrow.
    Ogre::Material* matMarker = powerupOverlay->getMaterial().get();
    Ogre::TextureUnitState* texMarker = matMarker->getTechnique(0)->getPass(0)->getTextureUnitState(0);
    texMarker->setTextureRotate(rot);

    // This is not really needed if the above code is working
    if(xPos > 1)
        xPos = 1.0f;
    if(yPos > 1)
        yPos = 1.0f;

    powerupOverlay->setPosition(xPos,yPos);
}

void BigScreen::setMapCorner(Ogre::Vector3 corner)
{
        mapCorner = corner;
}

void BigScreen::setMapSize(Ogre::Vector3 size)
{
        mapSize = size;
}

void BigScreen::hideScreen()
{
        this->olcMap->hide();
}

void BigScreen::showScreen()
{
        this->olcMap->show();
}

// THIS MUST REMOVE IT, the calling class will try to destroy the overlay
// once this is done, so if it is still used shit will explode
void BigScreen::removePowerupOverlayFromMap(int uniqueID)
{
    std::stringstream overlayNameSS;
    overlayNameSS << "PowerupOverlay" << uniqueID;  
        
    olcMap->removeChild(overlayNameSS.str());
}

void BigScreen::removeCarOverlayFromMap(int uniqueID)
{
    std::stringstream overlayNameSS;
    overlayNameSS << "PlayerOverlay" << uniqueID;  
        
    olcMap->removeChild(overlayNameSS.str());
}

Ogre::OverlayElement* BigScreen::createPowerupOverlayElement(Ogre::Vector3 powerupPosition, int uniqueID)
{
        std::stringstream overlayNameSS;
        overlayNameSS << "PowerupOverlay" << uniqueID;  
        
        Ogre::OverlayElement* tmpOLE
            = Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", overlayNameSS.str());
    
        tmpOLE->setMetricsMode( Ogre::GMM_RELATIVE );
        tmpOLE->setDimensions(MARKER_WIDTH,MARKER_HEIGHT);

        // Get a different material per powerup as it will be rotated
        Ogre::MaterialPtr powerupMaterial = Ogre::MaterialManager::getSingleton().getByName("powerupCrateIcon");
        
        // Clone a new instance then we can assign the new material with a new name
        std::stringstream newMaterialName;
        newMaterialName << "PowerupIcon" << uniqueID;

        powerupMaterial->clone(newMaterialName.str());
        tmpOLE->setMaterialName(newMaterialName.str());
        
        float tmpX = convertWorldToScreenX(powerupPosition.x);
        float tmpY = convertWorldToScreenY(powerupPosition.z);

        tmpOLE->setPosition(tmpX,tmpY);
        olcMap->addChild(tmpOLE);

    return tmpOLE;
}

//type 0=plain, 1=blue, 2=red, 3=dead
void BigScreen::changeArrow(int uniqueID, int type)
{
    Ogre::MaterialPtr arrowMaterial;
    std::stringstream newMaterialName;
    
    //Load in the correct material
    switch(type)
    {
        case 0:
            arrowMaterial = Ogre::MaterialManager::getSingleton().getByName("DefaultIcon");
            newMaterialName << "DefaultIcon" << uniqueID;
            break;
        case 1:
            arrowMaterial = Ogre::MaterialManager::getSingleton().getByName("BlueIcon");
            newMaterialName << "BlueIcon" << uniqueID;
            break;
        case 2:
            arrowMaterial = Ogre::MaterialManager::getSingleton().getByName("RedIcon");
            newMaterialName << "RedIcon" << uniqueID;
            break;
        case 3:
            arrowMaterial = Ogre::MaterialManager::getSingleton().getByName("DeadIcon");
            newMaterialName << "DeadIcon" << uniqueID;
            break;
    }
    
    //Remove the old material
    std::stringstream overlayNameSS;
    overlayNameSS << "PlayerOverlay" << uniqueID;  
    
    //Get the overlay element
    Ogre::OverlayElement* tmpOLE = olcMap->getChild(overlayNameSS.str());

    //Get the old materials name
    String oldMaterialName = tmpOLE->getMaterialName();
    
    //Create the new material
    arrowMaterial->clone(newMaterialName.str());
    tmpOLE->setMaterialName(newMaterialName.str());

    //Delete the old material
    MaterialManager::getSingleton().remove(oldMaterialName);
}

Ogre::OverlayElement* BigScreen::createPlayerOverlayElement(int uniqueID)
{
        //Timestamp used to fix duplicate named overlays.
        std::stringstream overlayNameSS;
        overlayNameSS << "PlayerOverlay" << uniqueID;     
        
        Ogre::OverlayElement* tmpOLE
            = Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", overlayNameSS.str());
        
        tmpOLE->setMetricsMode( Ogre::GMM_RELATIVE );
        tmpOLE->setDimensions(MARKER_WIDTH,MARKER_HEIGHT);

        //Get a reference to the main material
        Ogre::MaterialPtr arrowMaterial = Ogre::MaterialManager::getSingleton().getByName("DefaultIcon");
        
        //Clone a new instance then we can assign the new material with a new name
        std::stringstream newMaterialName;
        newMaterialName << "DefaultIcon" << uniqueID;

        arrowMaterial->clone(newMaterialName.str());
        tmpOLE->setMaterialName(newMaterialName.str());
        
        float tmpX = convertWorldToScreenX(0.5);
        float tmpY = convertWorldToScreenY(0.5);

        tmpOLE->setPosition(tmpX,tmpY);
        tmpOLE->show();

        olcMap->addChild(tmpOLE);

        return tmpOLE;
}

void BigScreen::createRoundTimer()
{
    //Create the overlay
    Ogre::Overlay *textOverlay = 
        Ogre::OverlayManager::getSingleton().create( "TIMER_TEXT_OVERLAY" );
    textOverlay->setZOrder(602);

    //Create the container
    Ogre::OverlayContainer *textScoreContainer = static_cast<Ogre::OverlayContainer*> ( 
		Ogre::OverlayManager::getSingleton().
			createOverlayElement( "Panel", "TIMER_TEXT_CONTAINER" ));
    textOverlay->add2D(textScoreContainer);

    //Create the text element
    this->oleTimerText = Ogre::OverlayManager::getSingleton().
		createOverlayElement("TextArea","OLE_TIMER_TEXT1");
    this->oleTimerText->setDimensions(0.3f, 0.3f);
	this->oleTimerText->setMetricsMode(Ogre::GMM_PIXELS);
    int screenHeight = GameCore::mClientGraphics->screenHeight;
    int screenWidth  = GameCore::mClientGraphics->screenWidth;
    this->oleTimerText->setPosition(screenWidth/2-60,10);

    this->oleTimerText->setParameter("font_name","DejaVuSans");
	this->oleTimerText->setParameter("char_height", "60");
	this->oleTimerText->setColour(Ogre::ColourValue::White);
	//this->oleTimerText->setCaption();
    textScoreContainer->addChild(this->oleTimerText);

    textOverlay->show();
    textScoreContainer->show();
    this->oleTimerText->show();

    this->startTime = time(NULL);
    isTimerSetup = true;
}

void BigScreen::updateRoundTimer()
{
    long int timeLeft = 181-(time(NULL)-startTime);
    long int minutesLeft = timeLeft/60;
    long int secondsLeft = timeLeft - minutesLeft*60;

    char displayText[100];
    sprintf(displayText,"%ld:%ld",minutesLeft,secondsLeft);
    if(181-(time(NULL)-startTime) >= 0 && 181-(time(NULL)-startTime) < 180)
        this->oleTimerText->setCaption(displayText);
    else
        this->oleTimerText->hide();
}


void BigScreen::resetRoundTimer()
{
    this->startTime = time(NULL);
}