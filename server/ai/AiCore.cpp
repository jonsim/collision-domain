
#include "AiCore.h"
#include <sstream>

using namespace std;
using namespace Ogre;

void AiCore::createNewAiAgent()
{
    int flags = 0;
    createNewAiAgent(flags);
}

void AiCore::createNewAiAgent(int flags)
{
    if( GameCore::mPlayerPool->getNumberOfPlayers() >= MAX_PLAYERS )
        return;

    //first get the total number of ai agents
    int total = getNumberOfAiPlayers();
    //create the aiplayer's name
    std::stringstream name;
    name << "AiPlayer" << (total + 1);
    AiPlayer player = AiPlayer(name.str(), Vector3(0,0,0), GameCore::mSceneMgr, flags);
    
    mAiPlayers.push_back(player);

}

void AiCore::frameEvent(double timeSinceLastFrame)
{
    std::vector<AiPlayer>::iterator i;

    for(i = mAiPlayers.begin();i != mAiPlayers.end();i++)
    {
        i->Update(timeSinceLastFrame);
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