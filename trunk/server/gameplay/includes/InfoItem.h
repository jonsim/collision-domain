/**
* @file		InfoItem.h
* @brief	Represents an item of information that is to be displayed to the 
			screen.
*/
#ifndef INFOITEM_H
#define INFOITEM_H

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"

#include <math.h>
#include <string>
#include "RakNetTypes.h"


enum OverlayType
{
	ONE_OT,
	TWO_OT,
	THREE_OT
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
private:
	OverlayType		mOT;
	RakNet::Time	mStartTime;
	RakNet::Time	mEndTime;
	bool			mDrawn;
};

#endif