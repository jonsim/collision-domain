/**
 * @file	Death.h
 * @brief 	Takes notifications and deals with them however networking needs to
 */
#ifndef DEATH_H
#define DEATH_H
	
class Player;

struct DEATH
{
	Player* player;
	Player* causedBy;
};

#endif
