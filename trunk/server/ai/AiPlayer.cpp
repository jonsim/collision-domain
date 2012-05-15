/**
* @file		AiPlayer.cpp
* @brief	Creates an AI player
*/

/*-------------------- INCLUDES --------------------*/
#include "AiPlayer.h"
#include "GameCore.h"
#include "Gameplay.h"

//constructor
AiPlayer::AiPlayer(string name, Ogre::Vector3 startPos, Ogre::SceneManager* sceneManager, int flags, level diff)
{
	mTolerance = 1.05;
	//set the players name
	mName = name;
	difficulty = diff;
	//create a player
	//add the aiplayer via the networkcore
	RakNet::BitStream bsAiPlayer;
	RakNet::StringCompressor().EncodeString(name.c_str(), 128, &bsAiPlayer);
	mPacket = new RakNet::Packet();
	mPacket->guid.g = GameCore::mNetworkCore->getRakInterface()->Get64BitUniqueRandomNumber();
    mPacket->guid.systemIndex = -1;// = GameCore::mNetworkCore->m_pRak->GetMyGUID().systemIndex;
	GameCore::mNetworkCore->PlayerJoin(&bsAiPlayer, mPacket);
	mPlayer = GameCore::mPlayerPool->getPlayer(mPacket->guid);
    mPlayer->setPlayerState( PLAYER_STATE_TEAM_SEL );
    Spawn();
	//create a steering behaviour
	mSteeringBehaviour = new SteeringBehaviour(mPlayer);
	
	mSteeringBehaviour->WanderOn();

	mFeelerDectionLength = 40.0;
	direction = turn = 0;
	targetDistance = 1000000.0;

    //Used for stuck detection
    timeSinceNotableChange = 0;
    oldPosition = Vector3(0.0f);
    stuckMode = 0;
    timeInChangeOver = 0;
}

void AiPlayer::Spawn()
{
    if( mPlayer->getPlayerState() == PLAYER_STATE_TEAM_SEL )
    {
        RakNet::BitStream bsAiTeam;
        bsAiTeam.Write( NO_TEAM );
        GameCore::mNetworkCore->PlayerTeamSelect(&bsAiTeam, mPacket);
    }
	mCarType = CAR_BANGER;
	RakNet::BitStream bsCarType;
	bsCarType.Write(mCarType);
	GameCore::mNetworkCore->PlayerSpawn(&bsCarType, mPacket);
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

void AiPlayer::isStuck(float timeSinceLastFrame)
{
    //GameCore::mServerGraphics->mRoot->get
    if(timeSinceNotableChange >= TIME_BEFORE_STUCK/timeSinceLastFrame)
    {
        //In here we've decide we're stuck
        StringStream tmpSS;
        tmpSS << mName << " player stuck\n";
        OutputDebugString(tmpSS.str().c_str());
        //GameCore::mGui->outputToConsole(tmpSS.str().c_str());
        //GameCore::mGui->outputToConsole("Ai Player stuck!\n");
        this->stuckMode = 1;
        this->timeInStuckMode = 0; // Lets get going
        timeSinceNotableChange = 0;
    }

}

void AiPlayer::updateStuckDetection()
{
    float currentSpeed = mPlayer->getCar()->getCarMph();
    
    StringStream tmpSS;
    tmpSS << mName << " " << currentSpeed << "mph\n";
    //GameCore::mGui->outputToConsole(tmpSS.str().c_str());
    //OutputDebugString(tmpSS.str().c_str());
    unsigned int timeSinceStart = (time(NULL) - GameCore::mGameplay->startTime);

    if( currentSpeed < 3.0f && timeSinceStart > 5)
    {
        timeSinceNotableChange++;
    }
}

void AiPlayer::Update(double timeSinceLastFrame)
{

    if( mPlayer->getPlayerState() == PLAYER_STATE_TEAM_SEL || mPlayer->getPlayerState() == PLAYER_STATE_SPAWN_SEL )
    {
        Spawn();
        return;
    }

    if( GameCore::mGameplay->mGameActive == false )
        return;

    if( !mPlayer->getCar() )
        return;

	//get the steering force
	Ogre::Vector3 targetPos = mSteeringBehaviour->Calculate();
	//get angle between current heading and desired heading
	Ogre::Quaternion heading = GetHeading();
	Ogre::Vector3 pos = GetPos();

	double distance = pos.distance(targetPos);
	double theta = heading.getYaw().valueRadians();

	if(mPlayer->getAlive())
	{
		float currentSpeed = mPlayer->getCar()->getCarMph();
		unsigned int timeSinceStart = (time(NULL) - GameCore::mGameplay->startTime);

        if(this->stuckMode == 1)
        {
            // Go Backwards
            mPlayer->getCar()->accelInputTick(false,true,false,timeSinceLastFrame);
            timeInStuckMode++;
            if(timeInStuckMode > TIME_BEFORE_UNSTUCK)
            {
                this->timeInStuckMode = false;
                timeInStuckMode = 0;
                mPlayer->getCar()->accelInputTick(true,false,false,timeSinceLastFrame);
                
                /*
                StringStream tmpSS;
                tmpSS << "Taking " << mName << " out of stuck mode\n";
                OutputDebugString(tmpSS.str().c_str());
                */

                stuckMode = 2;
            }
            stuckMode = 2;
            return;
        }
        else if(stuckMode == 2)
        {
            timeInChangeOver++;
            if(timeInChangeOver > 10000)
            {
                stuckMode = 0;
                timeInChangeOver = 0;
            }
        }
        else
        {
            //This is stuck stuff
            updateStuckDetection(); // Update the stuck detection stuff
            isStuck(timeSinceLastFrame);
        }
        
		//first check if were about to crash into a player on our own team
		/*if(GameCore::mGameplay->getGameMode() != FFA_MODE)
		{
			int numPlayers = GameCore::mPlayerPool->getNumberOfPlayers();
			Player* tempPlayer;
			Ogre::Vector3 tempPos;
			double angleBetween;
			for(int i = 0;i < numPlayers;i++)
			{
				//check if the player is in our way
				tempPlayer = GameCore::mPlayerPool->getPlayer(i);
				if(tempPlayer->getTeam() != mPlayer->getTeam())
				{
					continue;
				}
				else
				{
					tempPos = tempPlayer->getCar()->GetPos();
					angleBetween = pos.angleBetween(tempPos).valueRadians();
					if(InRange(theta-QuarterPi, theta+QuarterPi, angleBetween))
					{
						//if the distance is very close, just break
						if(pos.distance(tempPos) <= 5.0 && mPlayer->getCar()->getCarMph() > 20.0)
						{
							mPlayer->getCar()->accelInputTick(false, false, true, timeSinceLastFrame);
							return;
						}

						if(pos.distance(tempPos) <= 15.0)
						{
							//steer out of the way (random direction) and slow down
							mPlayer->getCar()->accelInputTick(true, false, false, timeSinceLastFrame);
							if(RandBool())
								mPlayer->getCar()->steerInputTick(false, true, timeSinceLastFrame);
							else
								mPlayer->getCar()->steerInputTick(true, false, timeSinceLastFrame);
							return;
						}
					}

				}
			}
		}*/


		//check if we need to set a target
		if(mSteeringBehaviour->On(seek))
		{	
			if(mSteeringBehaviour->GetSeekTarget() == NULL || mSteeringBehaviour->GetSeekTarget()->getAlive() == false)
			{
				//get a random player on other team
				Player* seekPlayer;
				do{
					seekPlayer = GameCore::mPlayerPool->getRandomPlayer();
				}while(mPlayer->getTeam() == seekPlayer->getTeam() && GameCore::mGameplay->getGameMode() != FFA_MODE);

				mSteeringBehaviour->SetSeekTarget(seekPlayer);
			}
		}


        //REMOVED BY ASH ON 11/05/12, works it seems but not great for gameplay
        /*
		//get out health our run away if someone is chasing us
		if(mPlayer->getHP() < 100 && difficulty >= normal)
		{
			//set flee target as closest person on opposite team
			Player* fleePlayer;
			fleePlayer = GameCore::mPlayerPool->getClosestPlayer(mPlayer);
			mSteeringBehaviour->FleeOn();
			mSteeringBehaviour->SeekOff();
			mSteeringBehaviour->SetFleeTarget(fleePlayer);
		}
        */

		if(difficulty == hard)
		{
			if(mSteeringBehaviour->On(flee))
			{
				//see if a powerup is nearer to us than flee target
				Ogre::Vector3 powerupPos = GameCore::mPowerupPool->getNearestPowerUp(GetPos());
				if(mSteeringBehaviour->GetFleeTarget() && powerupPos.distance(GetPos()) < mSteeringBehaviour->GetFleeTarget()->getCar()->GetPos().distance(GetPos()))
				{
					mSteeringBehaviour->PowerupOn();
					mSteeringBehaviour->SetPowerupTarget(powerupPos);
				}
			}
			else if(mSteeringBehaviour->On(seek))
			{
				//see if were in vip mode
				if(GameCore::mGameplay->getGameMode() == VIP_MODE)
				{
					//see if the enemy vip is near
					Player* enemyVIP = GameCore::mPlayerPool->getEnemyVip(mPlayer->getTeam());


				}
			}

		}


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
