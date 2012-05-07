/**
* @file		Gameplay.cpp
* @brief	Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "InfoItem.h"
#include "GetTime.h"
#include "GameCore.h"
//#include "BitStream.h"
InfoItem::InfoItem(OverlayType ot, RakNet::Time startTime, RakNet::Time endTime)
{
	mOT			= ot;
	mStartTime	= startTime;
	mEndTime	= endTime;
	mDrawn		= false;
	//this->sendPacket();
}

InfoItem::InfoItem(OverlayType ot, RakNet::Time startTime, int seconds)
{
	mOT			 = ot;
	mStartTime	 = startTime;
	mEndTime	 = startTime + RakNet::Time(seconds);
	mDrawn		= false;
	//this->sendPacket();
}

InfoItem::InfoItem(OverlayType ot, int delay, int seconds)
{
	mOT		   = ot;
	mStartTime = RakNet::GetTime() + RakNet::Time(delay);
	mEndTime   = mStartTime + RakNet::Time(seconds);
	mDrawn		= false;
	//this->sendPacket();
}

void InfoItem::sendPacket()
{
	#ifdef COLLISION_DOMAIN_SERVER
		GameCore::mNetworkCore->sendInfoItem(this);
	#endif
}

RakNet::Time InfoItem::getStartTime()
{
	return mStartTime;
}

RakNet::Time InfoItem::getEndTime()
{
	return mEndTime;
}

OverlayType	InfoItem::getOverlayType()
{
	return mOT;
}

void InfoItem::setDrawn()
{
	mDrawn = true;
}

bool InfoItem::getDrawn()
{
	return mDrawn;
}
