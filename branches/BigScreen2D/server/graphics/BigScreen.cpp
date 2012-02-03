#include "stdafx.h"
#include "GameIncludes.h"

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

	//Ogre::LogManager::getSingleton().logMessage("Starting BigScreen");
	OutputDebugString("BigScreen Loaded \n");
}

bool BigScreen::declareNewPlayer( RakNet::RakNetGUID playerid )
{
	Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(playerid);
	
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
	olcMap->setDimensions(1.0f,0.8f);
	olcMap->setMaterialName( "map_top_1" );
	olMap->add2D(olcMap);

	//Start thinking about the cars
	oleCar = 
		Ogre::OverlayManager::getSingleton().createOverlayElement( 
			"Panel", 
			"Player" );
	oleCar->setMetricsMode( Ogre::GMM_RELATIVE );
	oleCar->setDimensions(0.2f,0.2f);
	oleCar->setMaterialName( "map_top_1" );
	oleCar->setPosition(0.5,0.5);
	olcMap->addChild(oleCar);
	
}

void BigScreen::updateMapView()
{
	Player* localPlayer = GameCore::mPlayerPool->getLocalPlayer();
	btVector3 localPlayerPos = localPlayer->getCar()->getCarSnapshot()->mPosition;
	float xPos = localPlayerPos.getX(); 
	float yPos = localPlayerPos.getZ(); //Z as we're doing a 2D projection
	
	//Correct the position to be relative of top left corner.
	xPos -= mapCorner.x;
	yPos -= mapCorner.z;
	//Ogre::Entity* arenaEntity = GameCore::mGraphicsApplication->getArenaEntity();
	//Ogre::Vector3 maxArena = arenaEntity->getBoundingBox().getMaximum();
	
	//Correct to make percentage
	xPos = xPos/mapSize.x;
	yPos = yPos/mapSize.z;
	
	/*
	std::stringstream tmpDebugString;
	tmpDebugString << "XPos: " << xPos;
	tmpDebugString << " yPos: " << yPos;
	tmpDebugString << "\n";
	OutputDebugString(tmpDebugString.str().c_str());
	*/
	//This is not really needed if the above code is working
	if(xPos > 1)
		xPos = 1.0f;
	if(yPos > 1)
		yPos = 1.0f;

	oleCar->setPosition(xPos,yPos);
}

void BigScreen::setMapCorner(Ogre::Vector3 corner)
{
	mapCorner = corner;
}

void BigScreen::setMapSize(Ogre::Vector3 size)
{
	mapSize = size;
}