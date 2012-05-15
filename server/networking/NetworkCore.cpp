/**
 * @file	NetworkCore.cpp
 * @brief 	Handles all network traffic and synchronization
 * @author	Jamie Barber
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "stdafx.h"
#include "RakNetTypes.h"
#include "NetworkCore.h"
#include "CarSnapshot.h"
#include "GameCore.h"
#include "Player.h"
#include "PlayerPool.h"
#include <vector>
/*-------------------- METHOD DEFINITIONS --------------------*/

// Lots of static variable initialization
// Needed to be accessed from RPC funcs which have to be static
RakNet::RakPeerInterface* NetworkCore::m_pRak;
RakNet::RPC4* NetworkCore::m_RPC;
bool NetworkCore::bConnected = false;
RakNet::TimeMS NetworkCore::timeLastUpdate = 0;
SERVER_INFO_DATA NetworkCore::serverInfo;

/// @brief  Constructor, initialising all resources.
NetworkCore::NetworkCore()
{
	// Get our main interface to RakNet
	m_pRak = RakNet::RakPeerInterface::GetInstance();
	RakNet::StartupResult iStart = m_pRak->Startup( MAX_PLAYERS, &RakNet::SocketDescriptor(SERVER_PORT, 0), 1 );

	// Report any error starting the server
	if( iStart != RakNet::RAKNET_STARTED )
	{
		log( "Could not start server. Please check the port is not already in use (port %d)\n", SERVER_PORT );
		return;
	}

	RegisterRPCSlots();

    log( "Server seed sent: %u", GameCore::uPublicSeed );
    serverInfo.publicSeed = GameCore::uPublicSeed;
    serverInfo.curMap = 0;
    m_pRak->SetOfflinePingResponse( (char*)&serverInfo, sizeof( SERVER_INFO_DATA ) );
}

/// @brief Initialize the local player and set connected state
void NetworkCore::init( char *szPass )
{
	// Allow incoming connections, turn on occasional ping
	m_pRak->SetMaximumIncomingConnections( MAX_PLAYERS );
	m_pRak->SetOccasionalPing( true );

	// Set the server password if one is specified
	if( szPass != NULL )
		m_pRak->SetIncomingPassword( szPass, strlen( szPass ) );

	bConnected = true;
	// Add the local player
	//GameCore::mPlayerPool->addLocalPlayer( m_pRak->GetMyGUID(), "MRSERVERMAN" );	
	//GameCore::mPlayerPool->getLocalPlayer()->createPlayer(CAR_BANGER, SKIN_DEFAULT);
	//GameCore::mGraphicsApplication->bigScreen->declareNewPlayer(m_pRak->GetMyGUID());
}


/// @brief Get the interface to RakNet (shouldn't be needed if this class is thorough)
/// @return RakPeerInterface pointer
RakNet::RakPeerInterface* NetworkCore::getRakInterface()
{
    return m_pRak;
}


/// @brief  Deconstructor.
NetworkCore::~NetworkCore()
{
    m_pRak->Shutdown( 100, 0 );
	RakNet::RakPeerInterface::DestroyInstance( m_pRak );
}


/// @brief  Called once a frame with the latest user keypresses.
/// @param  inputSnapshot  The object containing the latest user keypresses.
void NetworkCore::frameEvent()
{
    // Called once every frame (each time controls are sampled)
    // Do with this data as you wish - bundle them off in a little packet of joy to the server
	if( !bConnected )
		return;

	RakNet::TimeMS timeNow = RakNet::GetTimeMS();
	if( RakNet::GreaterThan( timeNow, timeLastUpdate + UPDATE_INTERVAL ) )
	{
		BroadcastUpdates();
		timeLastUpdate = RakNet::GetTimeMS();
		
	}

	RakNet::Packet *pkt;

	for( pkt = m_pRak->Receive(); pkt; m_pRak->DeallocatePacket(pkt), pkt=m_pRak->Receive() )
	{
		unsigned char packetid;
		// This apparently broken so no timestamps are used at the moment
		if( (unsigned char)pkt->data[0] == ID_TIMESTAMP )
			packetid = (unsigned char) pkt->data[sizeof(unsigned char) + sizeof(unsigned long)];
		else
			packetid = (unsigned char)pkt->data[0];

		switch( packetid )
		{
			case ID_NEW_INCOMING_CONNECTION:
			{
				log( "New incomming connection to server" );
				break;
			}

			case ID_DISCONNECTION_NOTIFICATION:
			{
				log( "Remote player disconnected" );
                HandlePlayerQuit( pkt->guid, packetid );
				break;
			}

			case ID_CONNECTION_LOST:
				log( "Remote player connection lost" );
                HandlePlayerQuit( pkt->guid, packetid );
				break;

			case ID_PLAYER_INPUT:
				ProcessPlayerState( pkt );
				break;

			default:
				//log( "Something else happened.. %d", pkt->data[0] );
				break;
		}
	}
}

/// @brief Process a new snapshot of a player's user input
/// @params *pkt  Packet containing the snapshot data
void NetworkCore::ProcessPlayerState( RakNet::Packet *pkt )
{
// UNUSED VARIABLE	unsigned char bHasTime;
// UNUSED VARIABLE	RakNet::Time timestamp;
	unsigned char bPacketID;
	PLAYER_INPUT_DATA playerInput;

	RakNet::BitStream bitStream( pkt->data, pkt->length, false );

	//bitStream.Read( bHasTime );
	//bitStream.Read( timestamp );
	bitStream.Read( bPacketID );
	bitStream.Read( (char*)&playerInput, sizeof( PLAYER_INPUT_DATA ) );

	// Create a new InputState object from received data
	InputState *inputState = new InputState( playerInput.frwdPressed, 
		playerInput.backPressed, playerInput.leftPressed, playerInput.rghtPressed, playerInput.hndbPressed );

	// Delete any old unused input state
	if( GameCore::mPlayerPool->getPlayer( pkt->guid )->newInput != NULL )
		delete( GameCore::mPlayerPool->getPlayer( pkt->guid )->newInput );

	// Store the new state in the player's object
	GameCore::mPlayerPool->getPlayer( pkt->guid )->newInput = inputState;

}

/// @brief Broadcase all player snapshots to connected clients
void NetworkCore::BroadcastUpdates()
{
	// For now, this just sends updates on everyone after a certain time elapse
	// Once snapshots have timestamps in, only send client x's new position if
	// client x's position hasn't been sent for n ms.

	Player *sendPlayer;
	int size = GameCore::mPlayerPool->getNumberOfPlayers();

	// should really package multiple updates into one packet also though
	// RakNet does tend to do that a bit by itself to stop UDP breaking

	int j = 0;
	for( j = 0; j < size; j ++ )
	{

		sendPlayer = GameCore::mPlayerPool->getPlayer( j );
		if( sendPlayer == NULL )
			continue;

		if( sendPlayer->getCar() == NULL )
			continue;

		RakNet::BitStream bitSend;
		unsigned char packetid = ID_PLAYER_SNAPSHOT;
		RakNet::Time curTime = RakNet::GetTime();

		bitSend.Write( packetid );
		bitSend.Write( GameCore::mPlayerPool->getPlayerGUID( j ) );
		
		PLAYER_SYNC_DATA playerState;
		CarSnapshot *playerSnap = sendPlayer->getCar()->getCarSnapshot();

		playerState.timestamp = curTime;
		playerState.playerid = GameCore::mPlayerPool->getPlayerGUID( j );	
		playerState.vPosition = playerSnap->mPosition;
		playerState.fWheelPos = playerSnap->mWheelPosition;
		playerState.qRotation = playerSnap->mRotation;
		playerState.vAngVel = playerSnap->mAngularVelocity;
		playerState.vLinVel = playerSnap->mLinearVelocity;

		bitSend.Write( (char*)&playerState, sizeof( PLAYER_SYNC_DATA ) );

        if( sendPlayer->lastsenthp != sendPlayer->getHP() )
        {
            bitSend.Write( true );
            bitSend.Write( sendPlayer->getHP() );
            sendPlayer->lastsenthp = sendPlayer->getHP();
            RakNet::BitStream bitDmgUpdate;
            bitDmgUpdate.Write( (unsigned char) ID_PLAYER_DAMAGE );
            bitDmgUpdate.Write( (char*)&(sendPlayer->damageLoc), sizeof( PLAYER_DAMAGE_LOC ) );
            m_pRak->Send( &bitDmgUpdate, HIGH_PRIORITY, RELIABLE_ORDERED, 0, sendPlayer->getPlayerGUID(), false );
        }
        else
        {
            bitSend.Write( false );
        }

		m_pRak->Send( &bitSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, m_pRak->GetMyGUID(), true );

		delete( playerSnap );
	}
}

/// @brief	Send an update of the entire gamestate to a particular player
///			Includes all player positions, any other important stuff in the future
///			Might not actually be needed..
/// @params	playerid  unique GUID of player to update
void NetworkCore::GamestateUpdatePlayer( RakNet::RakNetGUID playerid )
{
	Player *sendPlayer;
	int size = GameCore::mPlayerPool->getNumberOfPlayers();

	for( int j = 0; j < size; j ++ )
	{
		sendPlayer = GameCore::mPlayerPool->getPlayer( j );
		if( sendPlayer == NULL )
			continue;

		if( sendPlayer->getCar() == NULL )
			continue;

		RakNet::BitStream bitSend;
		unsigned char packetid = ID_PLAYER_SNAPSHOT;
		RakNet::Time curTime = RakNet::GetTime();

		bitSend.Write( packetid );
		bitSend.Write( GameCore::mPlayerPool->getPlayerGUID( j ) );
		
		PLAYER_SYNC_DATA playerState;
		CarSnapshot *playerSnap = sendPlayer->getCar()->getCarSnapshot();

		playerState.timestamp = curTime;
		playerState.playerid = GameCore::mPlayerPool->getPlayerGUID( j );		
		playerState.fWheelPos = playerSnap->mWheelPosition;
		playerState.qRotation = playerSnap->mRotation;
		playerState.vAngVel = playerSnap->mAngularVelocity;
		playerState.vLinVel = playerSnap->mLinearVelocity;

		bitSend.Write( (char*)&playerState, sizeof( PLAYER_SYNC_DATA ) );

        bitSend.Write( true );
        bitSend.Write( sendPlayer->getHP() );
        sendPlayer->lastsenthp = sendPlayer->getHP();

		m_pRak->Send( &bitSend, HIGH_PRIORITY, UNRELIABLE_SEQUENCED, 0, playerid, false );

		delete( playerSnap );
	}
}

/// @brief	Set up the game for a particular player. Sends PlayerJoin for each 
///			connected client, then an overall update on gamestate
void NetworkCore::SetupGameForPlayer( RakNet::RakNetGUID playerid )
{
	Player *playerSend;

	int size = GameCore::mPlayerPool->getNumberOfPlayers();

	for( int j = 0; j < size; j ++ )
	{
		// Don't send a new client PlayerJoin of themselves
		if( GameCore::mPlayerPool->getPlayerGUID( j ) == playerid )
			continue;

		playerSend = GameCore::mPlayerPool->getPlayer( j );
		if( playerSend )
		{
			RakNet::BitStream bsJoin;
			bsJoin.Write( GameCore::mPlayerPool->getPlayerGUID( j ) );
            bsJoin.Write( playerSend->getTeam() );
            //RakNet::RakString *strName = new RakNet::RakString("RemotePlayer");
            RakNet::StringCompressor().EncodeString( playerSend->getNickname(), 128, &bsJoin );
			m_RPC->Signal( "PlayerJoin", &bsJoin, HIGH_PRIORITY, RELIABLE_ORDERED, 0, playerid, false, false );

            if( playerSend->getCar() )
            {
                unsigned char packetid;
                RakNet::BitStream bsSpawn;
                packetid = ID_SPAWN_SUCCESS;
                bsSpawn.Write( packetid );
				bsSpawn.Write( GameCore::mPlayerPool->getPlayerGUID( j ) );
                bsSpawn.Write( playerSend->getCarType() );
				m_RPC->Signal( "PlayerSpawn", &bsSpawn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, playerid, false, false );
            }
		}
	}

    for( int j = 0; j < MAX_POWERUPS; j ++ )
    {
        if( GameCore::mPowerupPool->getPowerup( j ) )
        {
            RakNet::BitStream bsSend;
            bsSend.Write( j );
            bsSend.Write( GameCore::mPowerupPool->getPowerup( j )->getType()     );
            bsSend.Write( GameCore::mPowerupPool->getPowerup( j )->getPosition() );

            m_RPC->Signal( "PowerupCreate", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0,  playerid, false, false );
        }
    }

	GamestateUpdatePlayer( playerid );
}

void NetworkCore::HandlePlayerQuit( RakNet::RakNetGUID playerid, unsigned char reason )
{
    // Check the player exists first (don't want to send quit twice)
    Player *pPlayer = GameCore::mPlayerPool->getPlayer( playerid );
    if( pPlayer )
        GameCore::mGui->outputToConsole( "Player '%s' disconnected.\n", pPlayer->getNickname() );

    if( GameCore::mPlayerPool->delPlayer( playerid ) == false )
        return;

    RakNet::BitStream bsSend;
    bsSend.Write( playerid );
    bsSend.Write( reason );
    m_RPC->Signal( "PlayerQuit", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0,  playerid, true, false );
}


/// @brief  Called once a frame and supplies the requested user position so it can be updated onscreen.
/// @param  playerID  An identifier for a specific player in the game.
/// @return NULL if no CarSnapshot has been received since the last time this method was polled.
///         The latest CarSnapshot for the requested player if one has been received (or more than one :)).
CarSnapshot* NetworkCore::getCarSnapshotIfExistsSincePreviousGet(int playerID)
{
    return NULL;
}

void NetworkCore::PlayerJoin( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	char szNickname[128];
	RakNet::StringCompressor().DecodeString( szNickname, 128, bitStream );
	GameCore::mGui->outputToConsole("Player '%s' connected.\n", szNickname);

	// Add the player to server player pool
	int index = GameCore::mPlayerPool->addPlayer( pkt->guid, szNickname );
    if( index == -1 )
    {
        // There aren't any free slots in the playerpool
        // However, there must be some AI players because if the server was full of properly
        // connected human players, RakNet would've already sent ID_NO_FREE_INCOMING_CONNECTIONS

        return;
    }

    GameCore::mPlayerPool->getPlayer( pkt->guid )->setPlayerState( PLAYER_STATE_TEAM_SEL );

	RakNet::BitStream bsNewPlayer;
	RakNet::BitStream bsSend;

	// Alert other players that someone new has joined
	bsNewPlayer.Write( pkt->guid );
    bsNewPlayer.Write( NO_TEAM );
	RakNet::StringCompressor().EncodeString( szNickname, 128, &bsNewPlayer );
	m_RPC->Signal( "PlayerJoin", &bsNewPlayer, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pkt->guid, true, false );

	// Send them a GameJoin RPC so they can get set up
	// This is where any game specific initialization can go
    bsSend.Write( GameCore::mGameplay->getGameMode() );
    bsSend.Write( GameCore::mGameplay->getArenaID() );
	RakNet::StringCompressor().EncodeString( szNickname, 128, &bsSend );
	m_RPC->Signal( "GameJoin", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pkt->guid, false, false );

	SetupGameForPlayer( pkt->guid );

    GameCore::mNetworkCore->sendSyncScores();
    GameCore::mNetworkCore->sendTimeSinceRoundStart(GameCore::mGameplay->startTime);

}

void NetworkCore::PlayerQuit( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	// Check and make sure things from Player are deleted
	// send an RPC to tell other clients they quit
    HandlePlayerQuit( pkt->guid, ID_REMOTE_DISCONNECTION_NOTIFICATION );
}

void NetworkCore::PlayerChat( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	// Receive the message
	char szMessage[128];
	RakNet::StringCompressor().DecodeString( szMessage, 128, bitStream );

    // This is where some checks can be done for team-only messages etc

    // Send the message to all players
    RakNet::BitStream bsSend;
    bsSend.Write( pkt->guid );
    RakNet::StringCompressor().EncodeString( szMessage, 128, &bsSend );
    m_RPC->Signal( "PlayerChat", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_pRak->GetMyGUID(), true, false );

    // Add the message to the console
    GameCore::mGui->outputToConsole("[colour='FFED9DAA']%s:[colour='FFFFFFFF'] %s\n", GameCore::mPlayerPool->getPlayer(pkt->guid)->getNickname(), szMessage);
}

void NetworkCore::InfoItemTransmit( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	char szMessage[128];
    RakNet::BitStream bsSend;
	RakNet::StringCompressor().DecodeString( szMessage, 128, bitStream );
    bsSend.Write( pkt->guid );
    RakNet::StringCompressor().EncodeString( szMessage, 128, &bsSend );

    m_RPC->Signal( "InfoItemTransmit", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_pRak->GetMyGUID(), true, false );
}

void NetworkCore::sendPlayerSpawn( Player *pPlayer )
{
    unsigned char packetid;
    RakNet::BitStream bsSpawn;

    pPlayer->createPlayer( (CarType) pPlayer->getCarType(), (TeamID) pPlayer->getTeam(), (ArenaID) GameCore::mGameplay->getArenaID() );
    GameCore::mGui->outputToConsole( "Player '%s' spawned.\n", pPlayer->getNickname() );

	packetid = ID_SPAWN_SUCCESS;
    bsSpawn.Write( packetid );
	bsSpawn.Write( pPlayer->getPlayerGUID() );
    bsSpawn.Write( pPlayer->getCarType() );
    bsSpawn.Write( GameCore::mGameplay->mGameActive ); // Send the game active state

	m_RPC->Signal( "PlayerSpawn", &bsSpawn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_pRak->GetMyGUID(), true, false );   
    pPlayer->setPlayerState( PLAYER_STATE_INGAME );
}

void NetworkCore::sendPowerupCreate( int pwrID, PowerupType pwrType, Ogre::Vector3 pwrLoc )
{
    RakNet::BitStream bsSend;
    bsSend.Write( pwrID );
    bsSend.Write( pwrType );
    bsSend.Write( pwrLoc );

    m_RPC->Signal( "PowerupCreate", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_pRak->GetMyGUID(), true, false );
}

void NetworkCore::sendPowerupCollect( int pwrID, Player *player, float extraData )
{
    RakNet::BitStream bsSend;
    bsSend.Write( pwrID );

    if( player != NULL )
    {
        bsSend.Write( true );
        bsSend.Write( player->getPlayerGUID() );
        bsSend.Write( extraData );
    }
    else
    {
        bsSend.Write( false );
    }

    m_RPC->Signal( "PowerupCollect", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_pRak->GetMyGUID(), true, false );
}

void NetworkCore::sendChatMessage( const char *szMessage )
{
    RakNet::BitStream bsSend;
    bsSend.Write( m_pRak->GetMyGUID() );
    RakNet::StringCompressor().EncodeString( szMessage, 128, &bsSend );
    m_RPC->Signal( "PlayerChat", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_pRak->GetMyGUID(), true, false );
}

void NetworkCore::PlayerTeamSelect( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
    TeamID t;
    bitStream->Read( t );

    bool bResult = GameCore::mGameplay->addPlayer( pkt->guid, t );

    RakNet::BitStream bsSend;
    bsSend.Write( pkt->guid );
    bsSend.Write( GameCore::mPlayerPool->getPlayer(pkt->guid)->getTeam() );
    bsSend.Write( bResult );

    if( bResult )
    {
        m_RPC->Signal( "PlayerTeamSelect", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_pRak->GetMyGUID(), true, false );
        GameCore::mPlayerPool->getPlayer(pkt->guid)->setPlayerState( PLAYER_STATE_SPAWN_SEL );
    }
    else
    {
        m_RPC->Signal( "PlayerTeamSelect", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pkt->guid, false, false );
    }
}

void NetworkCore::PlayerSpawn( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	// Do some checking here to make sure the player is allowed to spawn before sending the RPC back
    CarType iCarType;
    bitStream->Read( iCarType );
	int size = GameCore::mPlayerPool->getNumberOfPlayers();

	Player *pPlayer = GameCore::mPlayerPool->getPlayer( pkt->guid );

    unsigned char packetid;
    RakNet::BitStream bsSpawn;

    if( pPlayer->getPlayerState() != PLAYER_STATE_SPAWN_SEL )
    {
        return;
    }

    // Don't allow spawn if they haven't selected a team yet
    if( pPlayer->getTeam() == NO_TEAM )
    {
        packetid = ID_SPAWN_NO_TEAM;
		bsSpawn.Write( packetid );
        m_RPC->Signal( "PlayerSpawn", &bsSpawn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pkt->guid, false, false );
        pPlayer->setPlayerState( PLAYER_STATE_TEAM_SEL );
        return;
    }

    if( GameCore::mGameplay->mGameActive == false )
    {
        packetid = ID_SPAWN_GAME_INACTIVE;
		bsSpawn.Write( packetid );
        m_RPC->Signal( "PlayerSpawn", &bsSpawn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pkt->guid, false, false );
        pPlayer->setCarType( iCarType );
        pPlayer->setPlayerState( PLAYER_STATE_WAIT_SPAWN );

        // Check how many players we now have spawned
        int size = GameCore::mPlayerPool->getNumberOfPlayers();
        int c = 0;
	    for(int i=0;i<size;i++)
	    {
		    Player* tmpPlayer = GameCore::mPlayerPool->getPlayer(i);
            if( tmpPlayer->getPlayerState() == PLAYER_STATE_INGAME )
                c ++;
        }

        // Start the game (will cause all waiting players to spawn)
        if( GameCore::mGameplay->mGameActive == false && c >= NUM_PLAYERS_TO_START )
            GameCore::mGameplay->startGame();

       // return;
    }

    pPlayer->setCarType( iCarType );
    GameCore::mNetworkCore->sendPlayerSpawn( pPlayer );

    return;
}

void NetworkCore::UpdateNickname( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
    char szNickname[128];
	RakNet::StringCompressor().DecodeString( szNickname, 128, bitStream );
	
    Player* tmpPlayer = GameCore::mPlayerPool->getPlayer( pkt->guid);
    GameCore::mGui->outputToConsole("%s changed nickname to '%s'.\n",tmpPlayer->getNickname(),szNickname);

    tmpPlayer->setNickname(szNickname);
}

/// @brief Registers the RPC calls for the client
void NetworkCore::RegisterRPCSlots()
{
    m_RPC = RakNet::RPC4::GetInstance();
	m_pRak->AttachPlugin( m_RPC );

	m_RPC->RegisterSlot( "PlayerJoin",		    PlayerJoin, 0 );
	m_RPC->RegisterSlot( "PlayerQuit",		    PlayerQuit, 0 );
	m_RPC->RegisterSlot( "PlayerChat",		    PlayerChat, 0 );
    m_RPC->RegisterSlot( "PlayerTeamSelect",    PlayerTeamSelect, 0 );
	m_RPC->RegisterSlot( "PlayerSpawn",		    PlayerSpawn, 0 );
    m_RPC->RegisterSlot( "UpdateNickname",	    UpdateNickname, 0 );
}


// --------------------------------------------------------------------

// THIS SHOULDN'T BE HERE, FOR LATER

/// @brief Simple log output to file
/// @param Same as printf - format, variable list
void log( char *data, ... )
{
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

void NetworkCore::sendInfoItem(InfoItem* ii, bool show)
{
	//OutputDebugString("Sending Info Item\n");
	RakNet::BitStream bs;
	bs.Write(ii->getOverlayType());
	bs.Write(ii->getStartTime());
	bs.Write(ii->getEndTime());
    bs.Write(show);
	m_RPC->Signal( "InfoItemReceive", &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_pRak->GetMyGUID(), true, false);
}

void NetworkCore::sendPlayerDeath(Player* player, Player* causedBy)
{
	//OutputDebugString("Send Player Death\n");
	RakNet::BitStream bs;
	bs.Write(player->getPlayerGUID());
    bs.Write(causedBy->getPlayerGUID());
	m_RPC->Signal( "PlayerDeath", &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_pRak->GetMyGUID(), true, false);
}

void NetworkCore::declareNewVIP(Player* player)
{
	//OutputDebugString("Sending new VIP decleartion\n");
	RakNet::BitStream bs;
	bs.Write(player->getPlayerGUID());
	m_RPC->Signal( "DeclareVIP", &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_pRak->GetMyGUID(), true, false);
}

void NetworkCore::sendSyncScores()
{
    //OutputDebugString("Sending sync of scores\n");
    RakNet::BitStream bs;
    //std::vector<Player*> players = GameCore::mPlayerPool->getPlayers();
    int playerPoolSize = GameCore::mPlayerPool->getNumberOfPlayers();
    bs.Write(playerPoolSize); //Send the size
    for(int i=0;i<playerPoolSize;i++)
    {
        //Write player GUID then round score and then score, then if the player is AI
        bs.Write(GameCore::mPlayerPool->getPlayer(i)->getPlayerGUID());
        bs.Write(GameCore::mPlayerPool->getPlayer(i)->getRoundScore());
        bs.Write(GameCore::mPlayerPool->getPlayer(i)->getGameScore());
    }
    m_RPC->Signal( "SyncScores", &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_pRak->GetMyGUID(), true, false);
}

void NetworkCore::sendGameSync( GameMode gameMode, ArenaID arenaID )
{
    RakNet::BitStream bs;
    bs.Write(gameMode);
    bs.Write(arenaID);
    m_RPC->Signal( "GameSync", &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_pRak->GetMyGUID(), true, false);
}

void NetworkCore::sendTimeSinceRoundStart(time_t startTime)
{
    RakNet::BitStream bs;

    time_t timeOffset = time(NULL) - GameCore::mGameplay->startTime;
    bs.Write(timeOffset);

    m_RPC->Signal( "TimeSync", &bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_pRak->GetMyGUID(), true, false);
}