
#include "stdafx.h"
#include "GameIncludes.h"

#define BASIC_INTERP 1

PlayerPool::PlayerPool() : mLocalPlayer(0)
{

}

int PlayerPool::addPlayer( RakNet::RakNetGUID playerid, char *szNickname )
{
	int iNew = mPlayers.size();
	mPlayers.push_back(new Player());
	mPlayers[iNew]->setPlayerGUID(playerid);
	mPlayers[iNew]->setGUID(playerid);
	mGUID.push_back(playerid);
    mPlayers[iNew]->setNickname( szNickname );

	return iNew;
}

int PlayerPool::getNumberOfPlayers()
{
	return mPlayers.size();
}

void PlayerPool::addLocalPlayer( RakNet::RakNetGUID playerid, char *szNickname )
{
	mLocalPlayer = new Player();
	mLocalPlayer->setPlayerGUID(playerid);
	mLocalPlayer->setNickname(szNickname);
	mLocalGUID = playerid;
	
	mPlayers.push_back(mLocalPlayer);
	mGUID.push_back(playerid);
}

void PlayerPool::delPlayer( RakNet::RakNetGUID playerid )
{
	int iRemove = getPlayerIndex( playerid );
	if( iRemove != -1 )
	{
		delete mPlayers[iRemove];
		mPlayers[iRemove] = NULL;
	}
}

int PlayerPool::getPlayerIndex( RakNet::RakNetGUID playerid )
{
	int i = 0;
	for (int i = 0; i < GameCore::mPlayerPool->getNumberOfPlayers(); i ++ )
	{
		if( mGUID[i] == playerid )
			return i;
	}

	return -1;
}

bool PlayerPool::cmp(Player* a, Player* b)
{
	if(a == NULL)
		return false;
	if(b==NULL)
		return true;
	return a->getRoundScore() < b->getRoundScore();
}

//This returns all the players in the pool in sorted score order.
std::vector<Player*> PlayerPool::getScoreOrderedPlayers()
{
	//Put all the players from the array into a vector
	std::vector<Player*> tmp;
	for(int i=0;i<mPlayers.size();i++)
	{
		if(mPlayers[i] != NULL)
			tmp.push_back(mPlayers[i]);
	}
	
	//And sort the vector
	std::sort(tmp.begin(),tmp.end(),PlayerPool::cmp);
	
	//return this->mPlayers;
	return tmp;
}

Player* PlayerPool::getLocalPlayer() { return mLocalPlayer; }
RakNet::RakNetGUID PlayerPool::getLocalPlayerID() { return mLocalGUID; }
Player* PlayerPool::getPlayer( int index ) { return mPlayers[index]; }
Player* PlayerPool::getPlayer( RakNet::RakNetGUID playerid )
{
    if( playerid == mLocalGUID )
        return mLocalPlayer;

	int index = getPlayerIndex( playerid );
	if( index != -1 )
		return mPlayers[index];

	return NULL;
}

void PlayerPool::frameEvent( const float timeSinceLastFrame )
{
	int i = 0;

	for( i = 0; i < mPlayers.size(); i ++ )
	{
		processPlayer( mPlayers[i] );
        // Since we don't have access to other player's input we won't do this for now.
        //mPlayers[i]->updateGlobalGraphics( mPlayers[i]->newInput, timeSinceLastFrame );
		// TODO: add timestamps to snapshots
	}

    processPlayer( getLocalPlayer() );
}

void PlayerPool::processPlayer( Player *pPlayer )
{
	if( pPlayer->mSnapshots != NULL && pPlayer->getCar() != NULL )
	{
#if BASIC_INTERP

        CarSnapshot *currentSnap = pPlayer->getCar()->getCarSnapshot();
        CarSnapshot *restoreSnap = NULL;

        btScalar dist = currentSnap->mPosition.distance( pPlayer->mSnapshots->mPosition );

        if( dist > 3.00f )
        {
            restoreSnap = (CarSnapshot*)malloc( sizeof( CarSnapshot ) );
            memcpy( restoreSnap, pPlayer->mSnapshots, sizeof( CarSnapshot ) );
        }
        else if( dist > 0.20f )
        {          
            btVector3 interpPos = pPlayer->mSnapshots->mPosition.lerp( currentSnap->mPosition, 0.9f );
            btQuaternion interpRot = pPlayer->mSnapshots->mRotation.slerp( currentSnap->mRotation, 0.9f );

            //btVector3 interpLin = pPlayer->mSnapshots->mLinearVelocity.lerp( currentSnap->mLinearVelocity, 0.9 );
            //btVector3 interpAng = pPlayer->mSnapshots->mAngularVelocity.lerp( currentSnap->mAngularVelocity, 0.9 );
                
            //restoreSnap = new CarSnapshot( interpPos, interpRot, interpAng, interpLin, currentSnap->mWheelPosition );

            restoreSnap = new CarSnapshot( 
                interpPos, 
                interpRot, 
                pPlayer->mSnapshots->mAngularVelocity,
                pPlayer->mSnapshots->mLinearVelocity, 
                pPlayer->mSnapshots->mWheelPosition );
        }

        if( restoreSnap != NULL )
        {
	        pPlayer->getCar()->restoreSnapshot( restoreSnap );

            delete( restoreSnap );
            restoreSnap = NULL;
        }

        delete( currentSnap );
        currentSnap = NULL;

#else

        pPlayer->getCar()->restoreSnapshot( pPlayer->mSnapshots );
#endif
        delete( pPlayer->mSnapshots );
        pPlayer->mSnapshots = NULL;

        
        if (pPlayer->getVIP())
            GameCore::mClientGraphics->updateVIPLocation(pPlayer->getTeam(), pPlayer->getCar()->mBodyNode->getPosition());
    }
}

void PlayerPool::setSpectating( RakNet::RakNetGUID playerid )
{
    if( playerid == mLocalGUID )
    {
        GameCamera *cam = mLocalPlayer->getCamera();
        cam->setTarget( mLocalPlayer->getCar()->mBodyNode );
    }
    int idx = getPlayerIndex( playerid );
    if( idx != -1 )
    {
        mSpectating = playerid;
        setSpectating( idx );
    }
}

void PlayerPool::setSpectating( int idx )
{
    // THIS IS ABSOLUTELY FILTHY
    // TAKE THE CAMERA OUT OF PLAYER ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! ! 
    GameCamera *cam = mLocalPlayer->getCamera();
    cam->setTarget( mPlayers[idx]->getCar()->mBodyNode );
}

void PlayerPool::spectateNext()
{
    int curIdx = getPlayerIndex( mSpectating );
    int nextIdx = -1;

    // Check for an alive player further forward in player array
	for( int i = ++curIdx; i < mPlayers.size(); i ++ )
    {
        if( mPlayers[i] )
        {
            if( mPlayers[i]->getAlive() )
            {
                nextIdx = i;
                break;
            }
        }
    }

    // Check for an alive player wrap-around
    if( nextIdx == -1 )
    {
        for( int i = 0; i < curIdx; i ++ )
        {
            if( mPlayers[i] )
            {
                if( mPlayers[i]->getAlive() )
                {
                    nextIdx = i;
                    break;
                }
            }
        }
    }

    // Spectate new player if one was found
    if( nextIdx != -1 )
    {
        setSpectating( nextIdx );
        mSpectating = mGUID[nextIdx];
    }
}

std::vector<Player*> PlayerPool::getPlayers() {
	//Put all the players from the array into a vector
	std::vector<Player*> tmp;
	for(int i=0;i<mPlayers.size();i++)
	{
		if(mPlayers[i] != NULL)
			tmp.push_back(mPlayers[i]);
	}
	
	//return this->mPlayers;
	return tmp;
}