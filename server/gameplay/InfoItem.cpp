/**
* @file		Gameplay.cpp
* @brief	Manages the view ports for a window
*/

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"
#include <sstream>
#include <math.h>
#include "InfoItem.h"

InfoItem::InfoItem(OverlayType ot, RakNet::Time startTime, RakNet::Time endTime)
{
	mOT			= ot;
	mStartTime	= startTime;
	mEndTime	= endTime;
	mDrawn		= false;
}

InfoItem::InfoItem(OverlayType ot, RakNet::Time startTime, int seconds)
{
	mOT			 = ot;
	mStartTime	 = startTime;
	mEndTime	 = startTime + RakNet::Time(seconds);
	mDrawn		= false;
}

InfoItem::InfoItem(OverlayType ot, int delay, int seconds)
{
	mOT		   = ot;
	mStartTime = RakNet::GetTime() + RakNet::Time(delay);
	mEndTime   = mStartTime + RakNet::Time(seconds);
	mDrawn		= false;
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