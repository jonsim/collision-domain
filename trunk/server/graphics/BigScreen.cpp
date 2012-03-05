#include "stdafx.h"
#include "GameIncludes.h"

#define MAP_WIDTH	1.0f
#define MAP_HEIGHT	0.8f

#define MARKER_WIDTH 0.05f
#define MARKER_HEIGHT 0.05f
/*
BigScreen::BigScreen(ViewportManager* vpm_P, NetworkCore* networkCore)
{
	mViewportManager	= vpm_P;
	mNetworkCore		= networkCore;
}
*/

BigScreen::BigScreen(ViewportManager* vpm_P)
{
	mViewportManager = vpm_P;
}

bool BigScreen::declareNewPlayer( RakNet::RakNetGUID playerid )
{
	Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(playerid);
	/*
	ViewCamera* bestCanidateVC = NULL;
	std::vector<ViewCamera*>::iterator itr;
	for(itr = viewCameraVector.begin(); itr<viewCameraVector.end(); ++itr)
	{
		ViewCamera* tmp = *itr;
		if(bestCanidateVC != NULL)
		{
			//Check to see if this new canidate is smaller than the previous canidate
			if(RakNet::LessThan(tmp->getLastUpdateTime(),bestCanidateVC->getLastUpdateTime()))
			{
				bestCanidateVC =  tmp;
			}
		}
		else
		{
			bestCanidateVC=  tmp;
		}
	}
	
	if(bestCanidateVC != NULL)
	{
		//This is important. If you add the camera to two points it dies
		bestCanidateVC->getCamera()->detachFromParent(); 
		tmpPlayer->attachCamera(bestCanidateVC->getCamera());
		OutputDebugString("Attached Camera to new player\n");
		bestCanidateVC->setLastUpdateTime();
	}
	*/
	//Manage new player for bigscreen view
	this->manageNewPlayer(tmpPlayer);
	return mViewportManager->declareNewPlayer(tmpPlayer);
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
    catch (int e)
    {
        OutputDebugString("Exception caught while creating the bigScreen view, arena not fully initialised.\n");
    }

	//Start thinking about the cars
	/*
	oleCar = 
		Ogre::OverlayManager::getSingleton().createOverlayElement( 
			"Panel", 
			"Player" );
	oleCar->setMetricsMode( Ogre::GMM_RELATIVE );
	oleCar->setDimensions(0.2f,0.2f);
	oleCar->setMaterialName( "map_top_1" );
	oleCar->setPosition(0.5,0.5);
	olcMap->addChild(oleCar);
	*/
}

void BigScreen::manageNewPlayer(Player* player)
{
	//Timestamp used to fix duplicate named overlays.
	std::stringstream overlayNameSS;
	overlayNameSS << "PlayerOverlay" << RakNet::GetTime();	
	
	Ogre::OverlayElement* tmpOLE = 
		Ogre::OverlayManager::getSingleton().createOverlayElement(
		"Panel",
		overlayNameSS.str());


	tmpOLE->setMetricsMode( Ogre::GMM_RELATIVE );
	tmpOLE->setDimensions(MARKER_WIDTH,MARKER_HEIGHT);

	//Make a copy of the material
	Ogre::MaterialPtr arrowMaterial = Ogre::MaterialManager::getSingleton().getByName("ArrowBlack");
	//Build a new name with GUID so it should be unique
	std::stringstream newMaterialName;
	newMaterialName << "ArrowBlack" << player->getPlayerGUID().ToString();
	//Clone a new instance
	arrowMaterial = arrowMaterial->clone(newMaterialName.str());
	//We can now assign the new material with a new name
	tmpOLE->setMaterialName(newMaterialName.str());
	
	tmpOLE->setPosition(0.5f, 0.5f);
	olcMap->addChild(tmpOLE);

	player->setOverlayElement(tmpOLE);
}

void BigScreen::updateMapView()
{
	//Loop through all possible players
	for(int i=0;i<MAX_PLAYERS;i++)
	{
		Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(i);
		//Check to see if there is an actual player in this section
		if(tmpPlayer != NULL)
		{
			updatePlayer(tmpPlayer,tmpPlayer->getOverlayElement());
		}
	}
}

void BigScreen::updatePlayer(Player* player, Ogre::OverlayElement* carOverlay)
{
	
	if(player->getCar() != NULL && player->getCar()->getCarSnapshot() != NULL)
	{
		btVector3 localPlayerPos = player->getCar()->getCarSnapshot()->mPosition;
		float xPos = localPlayerPos.getX(); 
		float yPos = localPlayerPos.getZ(); //Z as we're doing a 2D projection
	
		//Correct the position to be relative of top left corner.
		//xPos -= mapCorner.x;
		//yPos -= mapCorner.z;
		//Ogre::Entity* arenaEntity = GameCore::mGraphicsApplication->getArenaEntity();
		//Ogre::Vector3 maxArena = arenaEntity->getBoundingBox().getMaximum();
	
		//Correct to make percentage
		xPos = xPos/(mapSize.x);
		yPos = yPos/(mapSize.z);
	
		xPos += MAP_WIDTH/2;
		yPos += MAP_HEIGHT/2;

		//Corrects it for aspect ratio of drawn map
		xPos *= MAP_WIDTH;
		yPos *= MAP_HEIGHT;

		//Correct for the size of the marker
		xPos -= MARKER_WIDTH;
		yPos -= MARKER_HEIGHT;
		
		// Calculate rotation from the car's chassis. This is projected straight to +Z. This can be done by passing through
        // Ogre's quaternions and using the getYaw() function (as in r314), it just seemed overly complicated.
		btQuaternion q = player->getCar()->getCarSnapshot()->mRotation;
        Ogre::Radian rot = Ogre::Math::ATan2(      (2.0 * (q.getZ()*q.getX() + q.getY()*q.getW())),
                                             1.0 - (2.0 * (q.getX()*q.getX() + q.getY()*q.getY())) );
        rot = (rot.valueRadians() > 0) ? rot.valueRadians() - Ogre::Math::PI : rot.valueRadians() + Ogre::Math::PI;
		
        // Rotate the arrow.
		Ogre::Material* matMarker = carOverlay->getMaterial().get();
		Ogre::TextureUnitState* texMarker = matMarker->getTechnique(0)->getPass(0)->getTextureUnitState(0);
		texMarker->setTextureRotate(rot);

		// This is not really needed if the above code is working
		if(xPos > 1)
			xPos = 1.0f;
		if(yPos > 1)
			yPos = 1.0f;

		carOverlay->setPosition(xPos,yPos);
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