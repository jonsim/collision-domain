
#include "AiCore.h"
#include "GameCore.h"
#include <sstream>

using namespace std;
using namespace Ogre;

void AiCore::createNewAiAgent()
{
	int flags = 0;
	createNewAiAgent(flags, normal);
}

void AiCore::createNewAiAgent(int flags, level diff)
{
    if( GameCore::mPlayerPool->getNumberOfPlayers() >= MAX_PLAYERS )
        return;

	//first get the total number of ai agents
	int total = getNumberOfAiPlayers();
	//create the aiplayer's name
	std::stringstream name;
	name << "AiPlayer" << (total + 1);
	AiPlayer player = AiPlayer(name.str(), Vector3(0,0,0), GameCore::mSceneMgr, flags, diff);
	
	mAiPlayers.push_back(player);

}

void AiCore::frameEvent(double timeSinceLastFrame)
{
	//check if the game has started
	//if(GameCore::mGameplay->mGameActive == false)
	//	return;

	std::vector<AiPlayer>::iterator i;

	//#pragma omp parallel num_threads(2)
	{
	for(i = mAiPlayers.begin();i != mAiPlayers.end();i++)
	{
		//#pragma omp single nowait
		{
		i->Update(timeSinceLastFrame);
		}
	}
	}
}

void AiCore::playerQuit(Player *pPlayer)
{    
	std::vector<AiPlayer>::iterator i;

	for(i = mAiPlayers.begin();i != mAiPlayers.end();i++)
	{
        if(i->getSteeringBehaviour()->GetFleeTarget() == pPlayer)
            i->getSteeringBehaviour()->SetFleeTarget(NULL);

       if(i->getSteeringBehaviour()->GetSeekTarget() == pPlayer)
            i->getSteeringBehaviour()->SetSeekTarget(NULL);
	}
}

AiPlayer* AiCore::getPlayer(string name)
{
	std::vector<AiPlayer>::iterator i;
	
	for(i = mAiPlayers.begin();i != mAiPlayers.end();i++)
	{
		if(i->GetName() == name)
			return (AiPlayer*)&(*i);
	}

	return NULL;
}
