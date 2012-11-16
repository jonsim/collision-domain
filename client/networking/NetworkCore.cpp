/**
 * @file	NetworkCore.cpp
 * @brief 	Takes notifications and deal with them however networking needs to
 */

#include "stdafx.h"
#include "RakNetTypes.h"
#include "NetworkCore.h"
#include "Player.h"
#include "GameCore.h"
#include "ClientHooks.h"
#ifndef WIN32
#include <unistd.h>
#include <sys/param.h>
#else
#define MAXHOSTNAMELEN 128
#endif

RakNet::RakPeerInterface* NetworkCore::m_pRak;
RakNet::RPC4* NetworkCore::m_RPC;
bool NetworkCore::bConnected = false;
RakNet::TimeMS NetworkCore::timeLastUpdate = 0;

/// @brief  Constructor, initialising all resources.
NetworkCore::NetworkCore () : m_szHost( NULL )
{
	char hostName[MAXHOSTNAMELEN];
	char* hostSplit;
	//set the player name to the computer name
	if(gethostname(hostName, MAXHOSTNAMELEN) >= 0)
	{
		hostSplit = strtok(&(hostName[0]), ".");
		mPlayerName = hostSplit;
	}
	else
		mPlayerName = "Human";


	// Get our main interface to RakNet
	m_pRak = RakNet::RakPeerInterface::GetInstance();
	m_pRak->Startup( 1, &RakNet::SocketDescriptor(), 1 );

	bConnected = false;

	RegisterRPCSlots();
}

bool NetworkCore::Connect( const char *szHost, int iPort, char *szPass )
{
	// Connect to the specified server
    char *szCon = (szHost == NULL) ? m_szHost : (char*)szHost;
    if( szCon == NULL )
        return false;

	RakNet::ConnectionAttemptResult bCon = m_pRak->Connect( szCon, iPort, szPass, szPass == NULL ? 0 : strlen(szPass) );
	m_pRak->SetOccasionalPing( true );

	//if( bCon == RakNet::ConnectionAttemptResult::CONNECTION_ATTEMPT_STARTED ) // C4482
	if( bCon == RakNet::CONNECTION_ATTEMPT_STARTED )
	{
		log( "Connecting to %s : %i", szCon, iPort );
		return true;
	}

	log( "Failed to connect to %s : %i", szCon, iPort );
	return false;
}

bool NetworkCore::AutoConnect( int iPort )
{
    m_pRak->Ping( "255.255.255.255", SERVER_PORT, true );
    return true;
}

RakNet::RakPeerInterface* NetworkCore::getRakInterface() { return m_pRak; }


/// @brief  Deconstructor.
NetworkCore::~NetworkCore()
{
    m_pRak->CloseConnection( serverGUID, true, 0, HIGH_PRIORITY );
    while( m_pRak->GetConnectionState( serverGUID ) == RakNet::IS_CONNECTED )
        boost::this_thread::sleep( boost::posix_time::milliseconds( 10 ) );
	RakNet::RakPeerInterface::DestroyInstance( m_pRak );
}


/// @brief  Called once a frame with the latest user keypresses.
/// @param  inputSnapshot  The object containing the latest user keypresses.
void NetworkCore::frameEvent(InputState *inputSnapshot)
{
    // Called once every frame (each time controls are sampled)
    // Do with this data as you wish - bundle them off in a little packet of joy to the server

	// Send our input
	RakNet::TimeMS timeNow = RakNet::GetTimeMS();
	if( RakNet::GreaterThan( timeNow, timeLastUpdate + UPDATE_INTERVAL ) )
	{
		// If we're not connected, don't do anything here
		if( bConnected && GameCore::mPlayerPool->getLocalPlayer()->getCar() )
		{
			// Bools are written as 1 bit by raknet :D

			// Create a player input struct to send to server
			PLAYER_INPUT_DATA playerInput;
			playerInput.backPressed = inputSnapshot->isBack();
			playerInput.frwdPressed = inputSnapshot->isForward();
			playerInput.leftPressed = inputSnapshot->isLeft();
			playerInput.rghtPressed = inputSnapshot->isRight();
            playerInput.hndbPressed = inputSnapshot->isHandbrake();

			// Push the data onto a bitstream
			RakNet::BitStream bitSend;
			unsigned char packetid = ID_PLAYER_INPUT;

			bitSend.Write( packetid );
			bitSend.Write( (char*)&playerInput, sizeof( PLAYER_INPUT_DATA ) );

			// Send to server
			m_pRak->Send( &bitSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, serverGUID, false );
		}
			// Update the time the last update was performed
			timeLastUpdate = RakNet::GetTimeMS();
	}

	// Create and receive a packet
	RakNet::Packet *pkt;
	for( pkt = m_pRak->Receive(); pkt; m_pRak->DeallocatePacket(pkt), pkt=m_pRak->Receive() )
	{
		// Get the packet identifier
		unsigned char packetid;
		// Check if there's a timestamp and adjust identidier pointer accordingly
		if( (unsigned char)pkt->data[0] == ID_TIMESTAMP )
			packetid = (unsigned char) pkt->data[sizeof(unsigned char) + sizeof(RakNet::TimeMS)];
		else
			packetid = (unsigned char)pkt->data[0];

		switch( packetid )
		{
            case ID_UNCONNECTED_PONG:
            {
                //char szSeed[32];
                m_szHost = strdup( pkt->systemAddress.ToString() );
                SERVER_INFO_DATA serverInfo;
                RakNet::TimeMS time;
				RakNet::BitStream bsIn( pkt->data, pkt->length, false );
				bsIn.IgnoreBytes( 1 );
				bsIn.Read( time );
                bsIn.Read( (char*)&serverInfo, sizeof( SERVER_INFO_DATA ) );
                GameCore::uPublicSeed = serverInfo.publicSeed;
                log( "Client seed received: %u", GameCore::uPublicSeed );
                srand( GameCore::uPublicSeed );
                break;
            }
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				log( "Connection to server accepted" );
				serverGUID = pkt->guid;

				RakNet::BitStream bsSend;

				RakNet::StringCompressor().EncodeString( mPlayerName.c_str(), 128, &bsSend );

				m_RPC->Signal( "PlayerJoin", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverGUID, false, false );
				break;
			}

			case ID_CONNECTION_ATTEMPT_FAILED:
				log( "Connection to server FAILED" );
				break;
			case ID_ALREADY_CONNECTED:
				log( "Already connected to specified server" );
				break;
			case ID_NO_FREE_INCOMING_CONNECTIONS:
				log( "Server appears to be full" );
				break;

			case ID_INVALID_PASSWORD:
				log( "Invalid password specified" );
				break;
			case ID_CONNECTION_BANNED:
				log( "Banned from this server" );
				break;

			case ID_DISCONNECTION_NOTIFICATION:
				log( "Disconnected from server" );
				break;
			case ID_CONNECTION_LOST:
				log( "Lost connection to server" );
				break;

			case ID_PLAYER_SNAPSHOT:
				ProcessPlayerState( pkt );
				break;

            case ID_PLAYER_DAMAGE:
            {
                unsigned char packetid;
                PLAYER_DAMAGE_LOC damageIn;
                RakNet::BitStream bitStream( pkt->data, pkt->length, false );
                bitStream.Read( packetid );
                bitStream.Read( (char*)&damageIn, sizeof( PLAYER_DAMAGE_LOC ) );
                GameCore::mPlayerPool->getLocalPlayer()->processDamage( damageIn );
                break;
            }

			default:
				break;
				//log( "Something else happened.. %d", packetid );
		}
	}
}

/// @brief Process a new snapshot of a player. Could be remote or local player
/// @params Packet containing the snapshot data
void NetworkCore::ProcessPlayerState( RakNet::Packet *pkt )
{
	// Here marks the beginning of crazy interpolation to 
	// smoothly "rewind and replay" previous physics frames

	unsigned char bPacketID;
	PLAYER_SYNC_DATA playerState;

	RakNet::BitStream bitStream( pkt->data, pkt->length, false );

	bitStream.Read( bPacketID );
	RakNet::RakNetGUID playerid;
	bitStream.Read( playerid );
	bitStream.Read( (char*)&playerState, sizeof( PLAYER_SYNC_DATA ) );

	Player *pUpdate;

	if( playerid == GameCore::mPlayerPool->getLocalPlayerID() )
		pUpdate = GameCore::mPlayerPool->getLocalPlayer();
	else
		pUpdate = GameCore::mPlayerPool->getPlayer( playerid );

	if( pUpdate == NULL )
		return;

	CarSnapshot *carSnapshot = new CarSnapshot( playerState.vPosition, playerState.qRotation, 
		playerState.vAngVel, playerState.vLinVel, playerState.fWheelPos );

	if( pUpdate->mSnapshots != NULL )
		delete( pUpdate->mSnapshots );

	pUpdate->mSnapshots = carSnapshot;
    bool hasHP; bitStream.Read( hasHP );
    if( hasHP )
    {
        int newHP;
        bitStream.Read( newHP );
        pUpdate->serverSaysHealthChangedTo( (float) newHP );
    }

    bool isAlive;
    bitStream.Read( isAlive );
    if( !isAlive )
    {
        if( pUpdate->getCar() )
            pUpdate->getCar()->loadDestroyedModel();
    }

}

void NetworkCore::setNicknameChange( const char *newNickname )
{
    RakNet::BitStream bsSend;
    RakNet::StringCompressor().EncodeString( newNickname, 128, &bsSend );

    m_RPC->Signal( "UpdateNickname", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverGUID, false, false );
}

void NetworkCore::sendTeamSelect( TeamID t )
{
    RakNet::BitStream bsSend;
    bsSend.Write( t );

    m_RPC->Signal( "PlayerTeamSelect", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverGUID, false, false );
}

void NetworkCore::sendSpawnRequest( CarType iCarType )
{
    RakNet::BitStream bsSend;
    bsSend.Write( iCarType );

    m_RPC->Signal( "PlayerSpawn", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverGUID, false, false );
}


void NetworkCore::sendChatMessage( const char *szMessage )
{
    RakNet::BitStream bsSend;
    RakNet::StringCompressor().EncodeString( szMessage, 128, &bsSend );
    m_RPC->Signal( "PlayerChat", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverGUID, false, false );
}

void NetworkCore::GameJoin( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
    GameMode gm; ArenaID aid;
	char szNickname[128];
    bitStream->Read( gm );
    bitStream->Read( aid );
	RakNet::StringCompressor().DecodeString( szNickname, 128, bitStream );

	// Add ourselves to the player pool
	GameCore::mPlayerPool->addLocalPlayer( m_pRak->GetMyGUID(), szNickname );
	log( "GameJoin : local playerid %s", m_pRak->GetMyGUID().ToString() );
	bConnected = true;
	timeLastUpdate = 0;

    // Set the gameplay parameters.
    GameCore::mGameplay->setGameMode( gm );
    GameCore::mGameplay->setArenaID( aid );

    // Configure the game for the player.
    GameCore::mGui->showSpawnScreenPage1( gm );
    GameCore::mClientGraphics->loadArena( aid );

	// Request to spawn straight away for now
	//m_RPC->Signal( "PlayerSpawn", NULL, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pkt->guid, false, false );

    // Show the spawn screen
    //GameCore::mClientGraphics->mSpawnScreen = new SpawnScreen( GameCore::mClientGraphics->mCamera );

	// If we're allowed to spawn, our spawn method will be called by the server automagically.
    
    //Set the projector map
    if(GameCore::mClientGraphics->getGraphicsState() == PROJECTOR)
    {
        //OutputDebugString("Changing map title on client join
        GameCore::mClientGraphics->mBigScreen->refreshMap();
    }
}

void NetworkCore::PlayerJoin( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	char szNickname[128];

	RakNet::RakNetGUID playerid;
    TeamID team;
	bitStream->Read( playerid );
    bitStream->Read( team );

	log( "PlayerJoin : playerid %s", playerid.ToString() );

	RakNet::StringCompressor().DecodeString( szNickname, 128, bitStream );

	GameCore::mPlayerPool->addPlayer( playerid, szNickname );
    GameCore::mPlayerPool->getPlayer( playerid )->setTeam( team );
    
    if (GameCore::mGui) GameCore::mGui->updateLocalPlayerRank();
}

void NetworkCore::PlayerQuit( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
    unsigned char reason;
	RakNet::RakNetGUID playerid;
	bitStream->Read( playerid );
    bitStream->Read( reason );

	GameCore::mPlayerPool->delPlayer( playerid );
    
    if (GameCore::mGui) GameCore::mGui->updateLocalPlayerRank();
}

void NetworkCore::PlayerChat( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
    RakNet::RakNetGUID playerid;
	char szMessage[128];

	bitStream->Read( playerid );
	RakNet::StringCompressor().DecodeString( szMessage, 128, bitStream );

    if( playerid == pkt->guid )
        GameCore::mGui->chatboxAddMessage( "Admin", szMessage );
    else
        GameCore::mGui->chatboxAddMessage( GameCore::mPlayerPool->getPlayer( playerid )->getNickname(), szMessage );
}

void NetworkCore::PlayerTeamSelect( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
    Player *pPlayer = NULL;
	RakNet::RakNetGUID playerid;
	TeamID teamID;
    bool bResult;

    bitStream->Read( playerid );
    bitStream->Read( teamID );
    bitStream->Read( bResult );

    if( playerid == GameCore::mPlayerPool->getLocalPlayerID() )
    {
        if( !bResult )
        {
            GameCore::mGui->showSpawnScreenErrorText( "Error: selected team too full!" );
            return;
        }

        pPlayer = GameCore::mPlayerPool->getLocalPlayer();
        pPlayer->setTeam( teamID );

        GameCore::mGui->showSpawnScreenPage2(GameCore::mGameplay->getGameMode(), teamID);
    }
    else
    {
        pPlayer = GameCore::mPlayerPool->getPlayer( playerid );

        if( !pPlayer )
            return;

        pPlayer->setTeam( teamID );
    }
}

void NetworkCore::PlayerSpawn( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	Player *pPlayer = NULL;

    unsigned char packetid;
	RakNet::RakNetGUID playerid;
    CarType iCarType;
    //bool isVIP;
    bitStream->Read( packetid );
	bitStream->Read( playerid );
    bitStream->Read( iCarType );
    //bitStream->Read( isVIP );
    bitStream->Read( GameCore::mGameplay->mGameActive ); // Recieve the game active state

    //OutputDebugString("ClientSpawn\n");
	log( "PlayerSpawn : playerid %s", playerid.ToString() );

    switch( packetid )
    {
    case ID_SPAWN_SUCCESS:

	    if( playerid == GameCore::mPlayerPool->getLocalPlayerID() )
	    {
		    pPlayer = GameCore::mPlayerPool->getLocalPlayer();
            pPlayer->createPlayer( iCarType, pPlayer->getTeam(), GameCore::mGameplay->getArenaID() );
            pPlayer->attachCamera( GameCore::mClientGraphics->mCamera );
            ClientHooks::localPlayerNowInArena(pPlayer);
	    }
	    else
	    {
		    pPlayer = GameCore::mPlayerPool->getPlayer( playerid );
		    if( pPlayer != NULL )
		    {
                pPlayer->createPlayer( iCarType, pPlayer->getTeam(), GameCore::mGameplay->getArenaID() );
                ClientHooks::nonLocalPlayerNowInArena(pPlayer);
		    }
		    else
            {
			    log( "..invalid player" );
            }
	    }
        
//	    pPlayer->setVIP(isVIP);

        break;

    case ID_SPAWN_NO_TEAM:
        // This shouldn't really be possible
        break;

    case ID_SPAWN_GAME_INACTIVE:
        // Switch to freeroam camera or something
        break;
    }
    
    if (GameCore::mGui) GameCore::mGui->updateLocalPlayerRank();
}

void NetworkCore::PowerupCreate( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{	  
    int id;
    PowerupType pwrType;
    Ogre::Vector3 pwrLoc;

    bitStream->Read( id );
    bitStream->Read( pwrType );
    bitStream->Read( pwrLoc );

    GameCore::mPowerupPool->spawnPowerup( pwrType, pwrLoc, id );
}

void NetworkCore::PowerupCollect( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
    int pwrID;
    bitStream->Read( pwrID );

    Powerup *pwrObject = GameCore::mPowerupPool->getPowerup( pwrID );

    bool hasPlayer;
    bitStream->Read( hasPlayer );

     float extraData = 0;

    Player *pPlayer = NULL;

    if(hasPlayer)
    {
    	RakNet::RakNetGUID playerid;
    	bitStream->Read( playerid );

        bitStream->Read( extraData );
    	pPlayer = GameCore::mPlayerPool->getPlayer( playerid );
    }

    // if pPlayer is null playerCollision will remove the player
    pwrObject->playerCollision( pPlayer, extraData );
}

void NetworkCore::InfoItemReceive( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	//OutputDebugString("Received Info Item\n");

	//Read in all the info need to rebuild the InfoItem
	OverlayType ot;
	bitStream->Read(ot);
	RakNet::Time startTime;
	bitStream->Read(startTime);
	RakNet::Time endTime;
	bitStream->Read(endTime);
    bool show;
    bitStream->Read(show);

	InfoItem* ii = new InfoItem(ot,startTime,endTime);
    //Removed for RPC calls of InfoItems
	//GameCore::mGameplay->mInfoItems.push_back(ii);
    GameCore::mGameplay->handleInfoItem(ii,show);
}

void NetworkCore::PlayerDeath( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	//OutputDebugString("Player Death Notice Received\n");
	RakNet::RakNetGUID deadPlayerGUID;
	bitStream->Read(deadPlayerGUID);

    RakNet::RakNetGUID causedByPlayerGUID;
    bitStream->Read(causedByPlayerGUID);

	Player* deadPlayer = GameCore::mPlayerPool->getPlayer(deadPlayerGUID);
    Player* causedBy   = GameCore::mPlayerPool->getPlayer(causedByPlayerGUID);
    deadPlayer->killPlayer(causedBy);
}

void NetworkCore::DeclareVIP( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	//OutputDebugString("New VIP assignment notice Received\n");
	RakNet::RakNetGUID vipPlayerGUID;
	bitStream->Read(vipPlayerGUID);

	Player* newPlayer = GameCore::mPlayerPool->getPlayer(vipPlayerGUID);
    if (newPlayer == NULL)
    {
        //OutputDebugString("NetworkCore::DeclareVIP, new vipPlayer does not exist. Incoming NULL pointer exception lol.\n");
        return;
    }
    GameCore::mGameplay->setNewVIP(newPlayer->getTeam(), newPlayer);
}

void NetworkCore::SyncScores( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
    //OutputDebugString("Syncing new scores\n");
    
    //Get the number of player scores being transmitted
    int playersSize;
    bitStream->Read(playersSize);
    
    for(int i=0; i<playersSize;i++)
    {
        RakNet::RakNetGUID pGUID;
        bitStream->Read(pGUID);

        int pRoundScore;
        bitStream->Read(pRoundScore);

        int pGameScore;
        bitStream->Read(pGameScore);

        Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(pGUID);
        if(tmpPlayer != NULL) {
            tmpPlayer->setRoundScore(pRoundScore);
            tmpPlayer->setGameScore(pRoundScore);
        } else {
            StringStream tmpSS;
            tmpSS << "Attempted to update score for " << pGUID.ToString() << " non existent player\n";
            OutputDebugString(tmpSS.str().c_str());
        }
    }
    
    if (GameCore::mGui) GameCore::mGui->updateLocalPlayerRank();
}

void NetworkCore::GameSync( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
    GameMode newGameMode;
    bitStream->Read(newGameMode);

    ArenaID newArenaID;
    bitStream->Read(newArenaID);

    //Set the game to this
    GameCore::mGameplay->setGameMode(newGameMode);
    GameCore::mClientGraphics->unloadArena( GameCore::mGameplay->getArenaID() );
    GameCore::mGameplay->setArenaID(newArenaID);
    GameCore::mClientGraphics->loadArena( GameCore::mGameplay->getArenaID() );

    GameCore::mPlayerPool->getLocalPlayer()->setPlayerState( PLAYER_STATE_SPAWN_SEL );

    if(GameCore::mClientGraphics->getGraphicsState() == PROJECTOR)
    {
        GameCore::mClientGraphics->mBigScreen->refreshMap();
    }

    // Update the client's car selection screen jobby.
    GameCore::mPlayerPool->getLocalPlayer()->setPlayerState( PLAYER_STATE_SPAWN_SEL );

    if(GameCore::mClientGraphics->getGraphicsState() != PROJECTOR)
        GameCore::mGui->showSpawnScreenPage2(newGameMode, GameCore::mPlayerPool->getLocalPlayer()->getTeam(), GameCore::mPlayerPool->getLocalPlayer()->getCarType());

    StringStream tmpSS;
    tmpSS << "Server declared game mode: " << newGameMode;
    OutputDebugString(tmpSS.str().c_str());
}

void NetworkCore::TimeSync( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
    time_t timeDelta;
    bitStream->Read(timeDelta);

    time_t newStartTime = time(NULL) - timeDelta;
    if(GameCore::mClientGraphics->getGraphicsState() == PROJECTOR)
    {
        GameCore::mClientGraphics->mBigScreen->setStartTime(newStartTime);
        OutputDebugString("Recived Time Update");
    }
}

void NetworkCore::NicknameChange( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
    OutputDebugString("Changing players name\n");
    RakNet::RakNetGUID guid;
    bitStream->Read(guid); //Read the GUID

    char szNickname[128];
	RakNet::StringCompressor().DecodeString( szNickname, 128, bitStream );
    Player* tmpPlayer = GameCore::mPlayerPool->getPlayer( guid);
    tmpPlayer->setNickname(szNickname);
}

/// @brief Registers the RPC calls for the client
void NetworkCore::RegisterRPCSlots()
{
    m_RPC = RakNet::RPC4::GetInstance();
	m_pRak->AttachPlugin( m_RPC );

	m_RPC->RegisterSlot( "GameJoin",		    GameJoin,           0 );
	m_RPC->RegisterSlot( "PlayerJoin",		    PlayerJoin,         0 );
	m_RPC->RegisterSlot( "PlayerQuit",		    PlayerQuit,         0 );
	m_RPC->RegisterSlot( "PlayerChat",		    PlayerChat,         0 );
    m_RPC->RegisterSlot( "PlayerTeamSelect",    PlayerTeamSelect,   0 );
	m_RPC->RegisterSlot( "PlayerSpawn",		    PlayerSpawn,        0 );
    m_RPC->RegisterSlot( "PowerupCreate",       PowerupCreate,      0 );
    m_RPC->RegisterSlot( "PowerupCollect",      PowerupCollect,     0 );
	m_RPC->RegisterSlot( "InfoItemReceive",     InfoItemReceive,    0 );
	m_RPC->RegisterSlot( "PlayerDeath",		    PlayerDeath,        0 );
	m_RPC->RegisterSlot( "DeclareVIP",		    DeclareVIP,         0 );
    m_RPC->RegisterSlot( "SyncScores",		    SyncScores,         0 );
    m_RPC->RegisterSlot( "GameSync",		    GameSync,           0 );
    m_RPC->RegisterSlot( "TimeSync",		    TimeSync,           0 );
    m_RPC->RegisterSlot( "NicknameChange",	    NicknameChange,     0 );
}


// --------------------------------------------------------------------

// THIS SHOULDN'T BE HERE, FOR LATER

/// @brief Simple log output to file
/// @param Same as printf - format, variable list
void log( char *data, ... )
{
    return;

	char buffer[2048];
	int error = 0;

    va_list va;
	va_start(va, data);
	vsprintf(buffer, data, va);
	va_end(va);
	strcat(buffer, "\n");

	// TODO: add timestamp
	FILE *fLog = fopen( LOG_FILENAME, "a" );
	fwrite( buffer, 1, strlen( buffer ), fLog );
	fclose( fLog );
	
	return;
}

RakNet::RakNetGUID NetworkCore::getServerGUID()
{
	return this->serverGUID;
}
