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

}

void Gameplay::setNumberOfTeams(int num)
{
	numberOfTeams = num;
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
	return team->setNewVIP(team->getRandomPlayer());
}

void Gameplay::setAllNewVIP()
{
	std::vector<Team*>::iterator itr;
	for(itr = teams.begin(); itr<teams.end(); ++itr)
	{
		Team* team = *itr;
		Player* vipPlayer = team->getRandomPlayer();
		team->setNewVIP(vipPlayer);
		//vipPlayer->getCar()->
	}
}

Team* Gameplay::declareNewPlayer( RakNet::RakNetGUID playerid )
{
	Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(playerid);
	Team* teamToJoin = getTeamToJoin();
	teamToJoin->addPlayer(tmpPlayer);
	OutputDebugString("Joined player to team");
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
	OutputDebugString("Preparing Players");
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
	int hypo = 500; //The hypotonuse. Increase to spread out
	//Calculate segment angle
	Ogre::Real segSize = (2*M_PI)/totalNumberOfPlayers;

	for(int i=0;i<MAX_PLAYERS;i++)
	{
		if(GameCore::mPlayerPool->getPlayer(i) != NULL)
		{
			Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(i);
		
			//Calcualte the correct positions
			Ogre::Real omega = (Ogre::Real)i*segSize;
			//Calculate which sector of the circle it's in
			int sector = floor(omega/ (M_PI/2));
			//Adjust to keep omega under 90
			omega-=(M_PI/2)*sector;

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