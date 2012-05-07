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
  oleVIP2(NULL)
{
}

void BigScreen::addCamera(Ogre::Camera* cam) 
{
        ViewCamera* tmpVC = new ViewCamera(cam);
        viewCameraVector.push_back(tmpVC);
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

        //Start thinking about the cars
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

        /*
        //Go through all the players which will add anything pre existing players in
        std::vector<Player*> tmpPlayers = GameCore::mPlayerPool->getPlayers();
        for(int i=0;i<tmpPlayers.size();i++)
        {
                this->manageNewPlayer(tmpPlayers[i]);
        }
        */
}

void BigScreen::updateMapView()
{
    std::vector<Player*> players = GameCore::mPlayerPool->getPlayers();

    //Loop through all possible players
    for(std::vector<Player*>::iterator it = players.begin();it != players.end();it++)
    {
        if ( (*it)->getCar()
            && (*it)->getCar()->getBigScreenOverlayElement() )
        {
            updatePlayer( (*it)->getCar(), (*it), (*it)->getCar()->getBigScreenOverlayElement() );
        }
    }
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
    if (!car || !player) return;

    CarSnapshot *playerSnap = car->getCarSnapshot();
    if (!playerSnap) return;

    btVector3 localPlayerPos = playerSnap->mPosition;
    float xPos = this->convertWorldToScreenX(localPlayerPos.getX()); 
    float yPos = this->convertWorldToScreenY(localPlayerPos.getZ()); //Z as we're doing a 2D projection

    //Correct the position to be relative of top left corner.
    //xPos -= mapCorner.x;
    //yPos -= mapCorner.z;
    //Ogre::Entity* arenaEntity = GameCore::mGraphicsApplication->getArenaEntity();
    //Ogre::Vector3 maxArena = arenaEntity->getBoundingBox().getMaximum();
        

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

Ogre::OverlayElement* BigScreen::createPowerupOverlayElement(Ogre::Vector3 powerupPosition, int uniqueID)
{
        std::stringstream overlayNameSS;
        overlayNameSS << "PowerupOverlay" << uniqueID;  
        
        Ogre::OverlayElement* tmpOLE
            = Ogre::OverlayManager::getSingleton().createOverlayElement("Panel", overlayNameSS.str());
    
        tmpOLE->setMetricsMode( Ogre::GMM_RELATIVE );
        tmpOLE->setDimensions(MARKER_WIDTH,MARKER_HEIGHT);

        //Get a reference to the main material
        tmpOLE->setMaterialName("powerupCrateIcon");
        
        float tmpX = convertWorldToScreenX(powerupPosition.x);
        float tmpY = convertWorldToScreenY(powerupPosition.z);

        tmpOLE->setPosition(tmpX,tmpY);
        olcMap->addChild(tmpOLE);

    return tmpOLE;
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
        //Build a new name with GUID and the time so it should be unique
        std::stringstream newMaterialName;
        newMaterialName << "DefaultIcon" << uniqueID;
        
        //Clone a new instance
        arrowMaterial->clone(newMaterialName.str());
        //We can now assign the new material with a new name
        tmpOLE->setMaterialName(newMaterialName.str());
        
        float tmpX = convertWorldToScreenX(0);
        float tmpY = convertWorldToScreenY(0);

        tmpOLE->setPosition(tmpX,tmpY);
        tmpOLE->show();

        olcMap->addChild(tmpOLE);

        return tmpOLE;
}