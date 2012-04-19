/**
* @file		InfoItem.h
* @brief	Represents an item of information that is to be displayed to the 
			screen.
*/
#ifndef INFOITEM_H
#define INFOITEM_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "SharedIncludes.h"

#include <math.h>
#include <string>
//#include "RakNetTypes.h"


enum OverlayType
{
	ONE_OT,
	TWO_OT,
	THREE_OT,
	FOUR_OT,
	FIVE_OT,
	RO_OT,
	ONE_MINUTE_OT,
	ROUND_OVER_OT,
	NEW_ROUND_OT,
	SCOREBOARD_TO_WINNER_OT
};

class InfoItem
{
public:
					InfoItem(OverlayType ot, RakNet::Time startTime, RakNet::Time endTime);
					InfoItem(OverlayType ot, RakNet::Time startTime, int seconds);
					InfoItem(OverlayType ot, int delay, int seconds);

	RakNet::Time	getStartTime();
	RakNet::Time	getEndTime();
	OverlayType		getOverlayType();
	void			setDrawn();
	bool			getDrawn();
	void			sendPacket();
private:
	OverlayType		mOT;
	RakNet::Time	mStartTime;
	RakNet::Time	mEndTime;
	bool			mDrawn;
	
};

#endif