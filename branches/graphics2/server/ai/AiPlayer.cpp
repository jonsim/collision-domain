/**
* @file		AiPlayer.cpp
* @brief	Creates an AI player
*/

/*-------------------- INCLUDES --------------------*/
#include "AiPlayer.h"

//constructor
AiPlayer::AiPlayer(string name, Ogre::Vector3 startPos, Ogre::SceneManager* sceneManager, int flags)
{
	mTolerance = 1.05;
	//set the players name
	mName = name;
	//create a player
	mPlayer = new Player();
	//add the aiplayer via the networkcore
	RakNet::BitStream bsAiPlayer;
	RakNet::StringCompressor().EncodeString(name.c_str(), 128, &bsAiPlayer);
	mPacket = new RakNet::Packet();
	mPacket->guid.g = GameCore::mPlayerPool->getNumberOfPlayers() + 1;
	GameCore::mNetworkCore->PlayerJoin(&bsAiPlayer, mPacket);
	mCarType = CAR_SMALL;
	RakNet::BitStream bsCarType;
	bsCarType.Write(mCarType);
	GameCore::mNetworkCore->PlayerSpawn(&bsCarType, mPacket);
	mPlayer = GameCore::mPlayerPool->getPlayer(mPacket->guid);
	//create a steering behaviour
	mSteeringBehaviour = new SteeringBehaviour(mPlayer);
	
	//set steering behaviours
	if((flags & wander) == wander)
		mSteeringBehaviour->WanderOn();
	if((flags & flee) == flee)
	{
        // Check for player 1 and set them as the flee target, otherwise give up and make them wander instead.
        // As this is only temporary it doesn't matter it's a bit of a hack.
        Player* fleeTarget = GameCore::mPlayerPool->getPlayer(0);
        if (fleeTarget && fleeTarget->getCar())
        {
		    mSteeringBehaviour->FleeOn();
		    mSteeringBehaviour->SetFleeTarget(fleeTarget);
        }
        else
        {
            GameCore::mGui->outputToConsole("AI could not find a player to flee from, wandering instead.");
		    mSteeringBehaviour->WanderOn();
        }
	}
	if((flags & seek) == seek)
	{
        // Check for player 1 and set them as the seek target, otherwise give up and make them wander instead.
        // As this is only temporary it doesn't matter it's a bit of a hack.
        Player* seekTarget = GameCore::mPlayerPool->getPlayer(0);
        if (seekTarget && seekTarget->getCar())
        {
		    mSteeringBehaviour->SeekOn();
		    mSteeringBehaviour->SetSeekTarget(seekTarget);
        }
        else
        {
            GameCore::mGui->outputToConsole("AI could not find a player to seek, wandering instead.");
		    mSteeringBehaviour->WanderOn();
        }
	}

	mFeelerDectionLength = 40.0;
	direction = turn = 0;
	targetDistance = 1000000.0;

}

void AiPlayer::CreateFeelers()
{
	double range = mFeelerDectionLength + (mPlayer->getCar()->getCarMph()/mPlayer->getCar()->getMaxSpeed()) * mFeelerDectionLength;
	double theta = mPlayer->getCar()->GetHeading().getYaw().valueRadians();
	Ogre::Vector3 heading = Ogre::Vector3(cos(theta), 0, sin(theta));
	Ogre::Vector3 pos = GetPos();
	mFeelers[0] = pos + range * heading;
	mFeelers[1] = pos + range/2.0f * heading.crossProduct( heading.perpendicular());
	mFeelers[2] = pos + range/2.0f * heading.crossProduct(-heading.perpendicular());
}

void AiPlayer::Update(double timeSinceLastFrame)
{
	//get the steering force
	Ogre::Vector3 targetPos = mSteeringBehaviour->Calculate();
	//get angle between current heading and desired heading
	Ogre::Quaternion heading = GetHeading();
	Ogre::Vector3 pos = GetPos();
//  UNUSED VARIABLE    double tempDis;
//  UNUSED VARIABLE    Ogre::Vector3 tempPos;
//  UNUSED VARIABLE    int aiDirection;

	double distance = pos.distance(targetPos);
	double theta = heading.getYaw().valueRadians();
	
	if(mPlayer->getAlive())
	{
		if(distance > 10)
			mPlayer->getCar()->accelInputTick(true, false, false, timeSinceLastFrame);
		else
			mPlayer->getCar()->accelInputTick(false, false, false, timeSinceLastFrame);

		double angle = sin((pos.x-targetPos.x) / (pos.z - targetPos.z));

		if(pos.z > targetPos.z && pos.x > targetPos.x)
		{
			if(theta < 0)
			{
				theta = fabs(theta);
				angle += HalfPi;

				if(fabs(theta-angle) < 0.03)
					return;
				else if(theta > angle)
					mPlayer->getCar()->steerInputTick(true, false, timeSinceLastFrame);
				else
					mPlayer->getCar()->steerInputTick(false, true, timeSinceLastFrame);
			}
			else
				mPlayer->getCar()->steerInputTick(false, true, timeSinceLastFrame);

			return;
		}

		if(pos.z < targetPos.z && pos.x > targetPos.x)
		{
			if(theta < 0)
			{
				//angle = fabs(angle);
				//theta = fabs(theta);
				//angle += HalfPi;

				if(fabs(theta-angle) < 0.03)
					return;
				else if(theta < angle)
					mPlayer->getCar()->steerInputTick(true, false, timeSinceLastFrame);
				else
					mPlayer->getCar()->steerInputTick(false, true, timeSinceLastFrame);
			}
			else
				mPlayer->getCar()->steerInputTick(false, true, timeSinceLastFrame);

			return;

		}

		if(targetPos.x > pos.x && targetPos.z > pos.z)
		{
			if(theta >= 0)
			{
				if(fabs(theta - angle) < 0.03)
					return;
				else if(angle > theta)
					mPlayer->getCar()->steerInputTick(true, false, timeSinceLastFrame);
				else
					mPlayer->getCar()->steerInputTick(false, true, timeSinceLastFrame);
			}
			else
				mPlayer->getCar()->steerInputTick(false, true, timeSinceLastFrame);

			return;

		}

		if(theta > 0)
		{
			angle = fabs(angle);
			angle += HalfPi;

			if(fabs(theta-angle) < 0.03)
				return;
			else if(angle > theta)
				mPlayer->getCar()->steerInputTick(true, false, timeSinceLastFrame);
			else
				mPlayer->getCar()->steerInputTick(false, true, timeSinceLastFrame);
		}
		else
			mPlayer->getCar()->steerInputTick(false, true, timeSinceLastFrame);
	}
    else
    {
        mPlayer->getCar()->steerInputTick(false, false, timeSinceLastFrame);
        mPlayer->getCar()->accelInputTick(false, false, false, timeSinceLastFrame);
    }
}

Ogre::Vector3 AiPlayer::GetPos()
{ 
	return mPlayer->getCar()->GetPos(); 
}

Ogre::Quaternion AiPlayer::GetHeading()
{
	return mPlayer->getCar()->GetHeading();
}

/*Ogre::Vector3 AiPlayer::FindWalls()
{
	double DistToThisIP    = 0.0;
	double DistToClosestIP = MaxDouble;
	Ogre::Vector3 point, ClosestPoint;
	point = ClosestPoint = Vector3::ZERO;
	String entName;
	Entity *wallEnt;
	SceneNode *feeler = NULL;

	//create the three feelers are cast infront of the vehicle to detect walls
	CreateFeelers();
	
	for(int i = 0; i < mFeelers.size(); i++)
	{
		Ogre::Vector3 heading = Vector3::ZERO;
		heading = (mFeelers[i]+GetPos()).normalisedCopy();

		if(mCollision->RaycastFromPoint(mFeelers[i], heading, wall_type, point, entName))
		{
			DistToThisIP = mFeelers[i].distance(point);
			
			if(DistToThisIP < DistToClosestIP)
			{
				DistToClosestIP = DistToThisIP;

				if(DistToClosestIP < mFeelerDectionLength)
				{
					wallEnt = GameCore::mSceneMgr->getEntity(entName);
					mWallHitPosition = point;
					mWallNormal = wallEnt->;
					mFeelerPos = mFeelers[i];
				}	
				
				ClosestPoint = point;
			}
		}
	}

	if(DistToClosestIP  < mFeelerDectionLength)
		return ClosestPoint;
	else 
		return Vector3::ZERO;

}*/