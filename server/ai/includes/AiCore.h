#ifndef AICORE_H
#define AICORE_H

#include <iostream>
#include <vector>
#include "AiPlayer.h"
#include "GameIncludes.h"

using namespace Ogre;
using namespace std;

enum level;
class AiPlayer;

class AiCore
{
public:
	AiCore() { srand(time(NULL)); };
	~AiCore() {};
	void createNewAiAgent();
	void createNewAiAgent(int flags, level diff);
	int getNumberOfAiPlayers() { return mAiPlayers.size(); } ;
	void frameEvent(double timeSinceLastFrame);
	AiPlayer* getPlayer(string name);


private:
	int numAgents;
	std::vector<AiPlayer> mAiPlayers;
};

#endif
