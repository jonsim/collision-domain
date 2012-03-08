/**
* @file		Gameplay.cpp
* @brief	Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"
#include <sstream>
#include <math.h>

Gameplay::Gameplay()
{
	mSB = new ScoreBoard();
	mHUD = new HUD();
	this->setNumberOfTeams(2); //Might as well default it to 2
}

void Gameplay::setNumberOfTeams(int num)
{
	numberOfTeams = num;
	//Create the new teams
	for(int i=0;i<num;i++)
	{
		Team* tmpTeam = this->createTeam("");
	}
}

Team* Gameplay::createTeam(std::string teamName)
{
	Team* tmpTeam = new Team(teamName);
	teams.push_back(tmpTeam);
	return tmpTeam;
}

Ogre::Real Gameplay::getScorePercentage(std::string identifier)
{
	return 0.0;
}

int	Gameplay::getScoreValue(std::string identifier)
{
	return 0;
}

void Gameplay::addPlayerToTeam(Team* team, Player* player)
{
	team->addPlayer(player);
}

bool Gameplay::gameOver()
{
	if(mGameplayMode == VIP_MODE) {
		vipModeGameWon();
	}
	return false;
}

bool Gameplay::hasWon(Team* team)
{
	return false;
}

void Gameplay::setGameplayMode(int gameplayMode)
{
	mGameplayMode = gameplayMode;
}

bool Gameplay::vipModeGameWon()
{
	std::vector<Team*>::iterator itr;
	for(itr = teams.begin(); itr<teams.end(); ++itr)
	{

	}
	return false;
}

Player* Gameplay::setNewVIP(Team* team)
{
	Player* pPlayer = team->getRandomPlayer();
	if(team == teams[0])
		pPlayer->attachCamera(GameCore::mGraphicsApplication->mViewCam1);
	else if(team == teams[1])
		pPlayer->attachCamera(GameCore::mGraphicsApplication->mViewCam2);
	return team->setNewVIP(pPlayer);
}

void Gameplay::setAllNewVIP()
{
	std::vector<Team*>::iterator itr;
	for(itr = teams.begin(); itr<teams.end(); ++itr)
	{
		Team* team = *itr;
		Player* vipPlayer = team->getRandomPlayer();
		//Do VIP Camera assignment stuff
		//Clear the previous assignments
		GameCore::mGraphicsApplication->mViewCam1->detachFromParent();
		GameCore::mGraphicsApplication->mViewCam2->detachFromParent();
		//Assign to the new VIPS
		if(team == teams[0])
			vipPlayer->attachCamera(GameCore::mGraphicsApplication->mViewCam1);
		else if(team == teams[1])
			vipPlayer->attachCamera(GameCore::mGraphicsApplication->mViewCam2);
		team->setNewVIP(vipPlayer);
	}
}

Team* Gameplay::declareNewPlayer( RakNet::RakNetGUID playerid )
{
	Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(playerid);
	Team* teamToJoin = getTeamToJoin();
	teamToJoin->addPlayer(tmpPlayer);

	//Check to see if we need to start game
	if(GameCore::mPlayerPool->getNumberOfPlayers() >= NUM_PLAYERS_TO_START)
	{
		this->startGame();
	}

	return teamToJoin;
}

//Gets which team makes sense to join (Aims to balance)
Team* Gameplay::getTeamToJoin()
{
	Team*	lowestTeam;
	int		lowestNumOfPlayers;

	//Check to see that there is some teams to join
	if(teams.size() > 0)
	{
		//Set initial size to an actual value. Who knows it might be the correct value
		lowestTeam = teams[0];
		lowestNumOfPlayers = lowestTeam->getTeamSize();

		//Loop through all teams and find the one with the lowest value
		std::vector<Team*>::iterator itr;
		for(itr = teams.begin(); itr<teams.end(); ++itr)
		{
			Team* team = *itr;
			if(team->getTeamSize() < lowestNumOfPlayers)
			{
				lowestTeam = team;
				lowestNumOfPlayers = team->getTeamSize();
			}
		}
	}

	return lowestTeam;
}

void Gameplay::notifyDamage(Player* player)
{
	this->printTeamStats();
}

void Gameplay::printTeamStats()
{
	std::stringstream tmpOutputString;
	tmpOutputString << "Team Stats \n";
	//Loop through all teams and find the one with the lowest value
	std::vector<Team*>::iterator itr;
	int i=0;
	for(itr = teams.begin(); itr<teams.end(); ++itr)
	{
		Team* tmpTeam = *itr;
		tmpOutputString << "Team " << i << ": " << tmpTeam->getTotalTeamHP() << "\n";
		i++;
	}
	OutputDebugString(tmpOutputString.str().c_str());
}

Team* Gameplay::checkIfGameOver()
{
	Team* winningTeam = NULL;
	//Loop through 
	std::vector<Team*>::iterator itr;
	for(itr = teams.begin(); itr<teams.end(); ++itr)
	{
		Team* tmpTeam = *itr;
		if(tmpTeam->getTotalTeamHP() > 0)
		{
			//If we already had a winning team it means we didn't as there are two
			if(winningTeam != NULL)
			{
				return NULL;
			}
			else
			{
				winningTeam = tmpTeam;
			}
		}
	}

	return winningTeam;
}

void Gameplay::preparePlayers()
{
	resetAllHP();
	positionPlayers();
}

//Bah this does nothing yet
void Gameplay::resetAllHP()
{

}

void Gameplay::positionPlayers()
{
	int totalNumberOfPlayers = GameCore::mPlayerPool->getNumberOfPlayers();
	int hypo = 25; //The hypotonuse. Increase to spread out
	//Calculate segment angle
	Ogre::Real segSize = (2*Ogre::Math::PI)/totalNumberOfPlayers;

	for(int i=0;i<MAX_PLAYERS;i++)
	{
		Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(i);
		if(tmpPlayer != NULL)
		{
			//Calcualte the correct positions
			Ogre::Real omega = (Ogre::Real)i*segSize;
			//Calculate which sector of the circle it's in
			int sector = floor(omega/ (Ogre::Math::PI/2));
			//Adjust to keep omega under 90
			omega-=(Ogre::Math::PI/2)*sector;

			Ogre::Real x = 0.0;
			Ogre::Real y = 0.0;

			switch(sector)
			{
				case 0:
					y = cos(omega)*hypo;
					x = sin(omega)*hypo;
					break;
				case 1:
					x = cos(omega)*hypo;
					y = -sin(omega)*hypo;
					break;
				case 2:
					y = -cos(omega)*hypo;
					x = -sin(omega)*hypo;
					break;
				case 3:
					x = -cos(omega)*hypo;
					y = sin(omega)*hypo;
					break;
			}

			//TODO - Move cars to correct positions
			std::stringstream debugString;
			debugString << "X: "<< x << " Y: "<<y<<"\n";
			OutputDebugString(debugString.str().c_str());
			tmpPlayer->getCar()->moveTo(btVector3(x,1,y));
		}
	}	
}

void Gameplay::startGame()
{
	this->positionPlayers();
	this->setAllNewVIP(); //TODO - Change this once we have multiple game modes
	this->scheduleCountDown();
}

void Gameplay::drawInfo()
{
	std::vector<InfoItem*>::iterator itr;
	for(itr = mInfoItems.begin(); itr<mInfoItems.end(); ++itr)
	{
		InfoItem* tmpInfoItem = *itr;
		if(RakNet::LessThan(tmpInfoItem->getStartTime(),RakNet::GetTime()))
		{
			
			//if(tmpInfoItem->getDrawn())
			//{
				//if(RakNet::LessThan(tmpInfoItem->getEndTime(),RakNet::GetTime()))
				//{
					//handleInfoItem(tmpInfoItem,false);
					//mInfoItems.erase(itr);
				//}
			//}
			//else
			if(!tmpInfoItem->getDrawn())
			{
				handleInfoItem(tmpInfoItem,true);
				tmpInfoItem->setDrawn();
				//mInfoItems.erase(itr);
			}
			else
			{
				if(RakNet::GreaterThan(RakNet::GetTime(),tmpInfoItem->getEndTime()))
				{
					handleInfoItem(tmpInfoItem,false);
					mInfoItems.erase(itr);
				}
			}
			break;
		}
	}
}

void Gameplay::handleInfoItem(InfoItem* item, bool show)
{
	Ogre::OverlayElement* tmpOLE = 
		Ogre::OverlayManager::getSingleton().getOverlayElement("ONE_OT",false);

	if(show)
	{
		switch(item->getOverlayType())
		{
			case ONE_OT:
				tmpOLE->setDimensions(0.1f, 0.1f);
				tmpOLE->setMaterialName( "gear1" );
				tmpOLE->setPosition(0.45f, 0.1f);
				tmpOLE->show();
				break;
			case TWO_OT:
				tmpOLE->setDimensions(0.1f,0.1f);
				tmpOLE->setMaterialName( "gear2" );
				tmpOLE->setPosition(0.45f, 0.1f);
				tmpOLE->show();
				break;
			case THREE_OT:
				tmpOLE->setDimensions(0.1f, 0.1f);
				tmpOLE->setMaterialName( "gear3" );
				tmpOLE->setPosition(0.45f, 0.1f);
				tmpOLE->show();
				break;
			case FOUR_OT:
				tmpOLE->setDimensions(0.1f, 0.1f);
				tmpOLE->setMaterialName( "gear4" );
				tmpOLE->setPosition(0.45f, 0.1f);
				tmpOLE->show();
				break;
			case FIVE_OT:
				tmpOLE->setDimensions(0.1f, 0.1f);
				tmpOLE->setMaterialName( "gear5" );
				tmpOLE->setPosition(0.45f, 0.1f);
				tmpOLE->show();
				break;
			case GAME_OVER_OT:
				tmpOLE->setDimensions(0.1f, 0.1f);
				tmpOLE->setMaterialName( "map_top_1" );
				tmpOLE->setPosition(0.45f, 0.1f);
				tmpOLE->show();
				break;
		}
	}
	else
	{
		tmpOLE->hide();
	}
}

void Gameplay::scheduleCountDown()
{
	InfoItem* threeII = new InfoItem(THREE_OT, 1000, 1000);
	InfoItem* twoII = new InfoItem(TWO_OT, 2000, 1000);
	InfoItem* oneII = new InfoItem(ONE_OT, 3000, 1000);


	mInfoItems.push_back(threeII);
	mInfoItems.push_back(twoII);
	mInfoItems.push_back(oneII);

	//Countdown Timer
	InfoItem* fiveEII = new InfoItem(FIVE_OT,115000,1000);
	InfoItem* fourEII = new InfoItem(FOUR_OT,116000,1000);
	InfoItem* threeEII = new InfoItem(THREE_OT,117000,1000);
	InfoItem* twoEII = new InfoItem(TWO_OT,118000,1000);
	InfoItem* oneEII = new InfoItem(ONE_OT,119000,1000);

	mInfoItems.push_back(fiveEII);
	mInfoItems.push_back(fourEII);
	mInfoItems.push_back(threeEII);
	mInfoItems.push_back(twoEII);
	mInfoItems.push_back(oneEII);

	//GAME OVER
	InfoItem* goEII = new InfoItem(GAME_OVER_OT,120000,3000);
	mInfoItems.push_back(goEII);

	//Send packets
	#ifdef COLLISION_DOMAIN_SERVER
		threeII->sendPacket();
		twoII->sendPacket();
		oneII->sendPacket();
		fiveEII->sendPacket();
		fourEII->sendPacket();
		threeEII->sendPacket();
		twoEII->sendPacket();
		oneEII->sendPacket();
		goEII->sendPacket();
	#endif
}

void Gameplay::setupOverlay()
{
	Ogre::Overlay *olInfo = 
		Ogre::OverlayManager::getSingleton().create( "OVERLAY_INFO" );
	olInfo->setZOrder(600);
	olInfo->show();
	
	
	Ogre::OverlayContainer* olContainer = static_cast<Ogre::OverlayContainer*> ( 
		Ogre::OverlayManager::getSingleton().
			createOverlayElement( "Panel", "INFOCONT" ) );
	//olContainer = olContainerTmp;

	olContainer->setMetricsMode( Ogre::GMM_RELATIVE );
	olContainer->setHorizontalAlignment( Ogre::GHA_LEFT);
	olContainer->setVerticalAlignment(Ogre::GVA_TOP);
	//Set the map
	olContainer->setDimensions(1.0f, 1.0f);
	olInfo->add2D(olContainer);
	
	Ogre::OverlayElement* tmpOLE = 
					Ogre::OverlayManager::getSingleton().createOverlayElement(
					"Panel",
					"ONE_OT");
			
	tmpOLE->setMetricsMode( Ogre::GMM_RELATIVE );
	tmpOLE->setDimensions(0.1f, 0.1f);
	tmpOLE->setMaterialName( "gear3" );
	tmpOLE->setPosition(0.45f, 0.1f);
	tmpOLE->hide();
	olContainer->addChild(tmpOLE);
}

Team* Gameplay::getTeam(int i)
{
	return teams[i];
}

void Gameplay::drawDeathInfo()
{
	mHUD->drawDeathMessage(NULL,NULL);
}

void Gameplay::initialize()
{
	mHUD->initialize();
}
