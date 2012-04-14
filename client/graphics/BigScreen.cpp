#include "stdafx.h"
#include "GameIncludes.h"

#define MAP_WIDTH	1.0f
#define MAP_HEIGHT	0.8f

#define MARKER_WIDTH 0.03f
#define MARKER_HEIGHT 0.04f

BigScreen::BigScreen()
{

}

bool BigScreen::declareNewPlayer( RakNet::RakNetGUID playerid )
{
	Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(playerid);
	//Manage new player for bigscreen view
	this->manageNewPlayer(tmpPlayer);
	//return mViewportManager->declareNewPlayer(tmpPlayer);
	return true;
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

void BigScreen::manageNewPlayer(Player* player)
{
	//Timestamp used to fix duplicate named overlays.
	std::stringstream overlayNameSS;
	overlayNameSS << "PlayerOverlay" << player->getGUID() << RakNet::GetTime();	
	
	Ogre::OverlayElement* tmpOLE = 
		Ogre::OverlayManager::getSingleton().createOverlayElement(
		"Panel",
		overlayNameSS.str());


	tmpOLE->setMetricsMode( Ogre::GMM_RELATIVE );
	tmpOLE->setDimensions(MARKER_WIDTH,MARKER_HEIGHT);

	//Get a reference to the main material
	Ogre::MaterialPtr arrowMaterial = Ogre::MaterialManager::getSingleton().getByName("DefaultIcon");
	//Build a new name with GUID and the time so it should be unique
	std::stringstream newMaterialName;
	newMaterialName << "DefaultIcon" << player->getPlayerGUID().ToString() << RakNet::GetTime();
	
	//Clone a new instance
	arrowMaterial->clone(newMaterialName.str());
	//We can now assign the new material with a new name
	tmpOLE->setMaterialName(newMaterialName.str());
	
	tmpOLE->setPosition(0.5f, 0.5f);
	olcMap->addChild(tmpOLE);

	player->setOverlayElement(tmpOLE);
}

void BigScreen::updateMapView()
{
	//Handle powerups
	this->handlePowerups();

	std::vector<Player*> players = GameCore::mPlayerPool->getPlayers();
	//Loop through all possible players
	for(std::vector<Player*>::iterator it = players.begin();it != players.end();it++)
	{
		/*
		if((*it)->getOverlayElement() == NULL)
		{
			this->manageNewPlayer((*it));
		}
		*/
		//if((*it)->getOverlayElement() != NULL)
			updatePlayer((Player*)(*it), (*it)->getOverlayElement());
	}
}

inline float BigScreen::convertWorldToScreenX(float xPos)
{
	xPos = xPos/(mapSize.x);
	xPos += MAP_WIDTH/2;
	xPos *= MAP_WIDTH;
	xPos -= MARKER_WIDTH;

	//Some fluff fixing 
	xPos *= 0.9f;
	//xPos += 0.1;

	return xPos;
}

inline float BigScreen::convertWorldToScreenY(float yPos)
{
	yPos = yPos/(mapSize.z);
	yPos += MAP_HEIGHT/2;
	yPos *= MAP_HEIGHT;
	yPos -= MARKER_HEIGHT;
	
	//Some fluff fixing
	yPos *= 0.8f;
	yPos += 0.2f;

	return yPos;
}

void BigScreen::updatePlayer(Player* player, Ogre::OverlayElement* carOverlay)
{
	CarSnapshot *playerSnap = NULL;

    if(player->getCar() != NULL)
        playerSnap = player->getCar()->getCarSnapshot();	

	if(playerSnap != NULL)
	{
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

void BigScreen::handlePowerups()
{
	std::vector<Powerup*> powerups = GameCore::mPowerupPool->getPowerups();
	for(int i=0;i<powerups.size();i++)
	{
		//Create new OLE's if the is a new element
		if(powerups[i]->getOverlayElement() == NULL)
			manageNewPowerup(powerups[i]);
	}

}

void BigScreen::manageNewPowerup(Powerup* pu)
{
	//Timestamp used to fix duplicate named overlays.
	std::stringstream overlayNameSS;
	overlayNameSS << "PowerupOverlay" << pu->getIndex() << RakNet::GetTime();	
	
	Ogre::OverlayElement* tmpOLE = 
		Ogre::OverlayManager::getSingleton().createOverlayElement(
		"Panel",
		overlayNameSS.str());


	tmpOLE->setMetricsMode( Ogre::GMM_RELATIVE );
	tmpOLE->setDimensions(MARKER_WIDTH,MARKER_HEIGHT);

	//Get a reference to the main material
	tmpOLE->setMaterialName("powerupCrateIcon");
	
	Ogre::Vector3 pos = pu->getPosition();
	float tmpX = convertWorldToScreenX(pos.x);
	float tmpY = convertWorldToScreenY(pos.y);

	tmpOLE->setPosition(tmpX,tmpY);
	olcMap->addChild(tmpOLE);

	pu->setOverlayElement(tmpOLE);
}