/**
 * @file	NetworkCore.cpp
 * @brief 	Handles all network traffic and synchronization
 * @author	Jamie Barber
 */

/*-------------------- INCLUDES --------------------*/
#include "stdafx.h"
#include "GameIncludes.h"
/*-------------------- METHOD DEFINITIONS --------------------*/

// Lots of static variable initialization
// Needed to be accessed from RPC funcs which have to be static
RakNet::RakPeerInterface* NetworkCore::m_pRak;
RakNet::RPC4* NetworkCore::m_RPC;
bool NetworkCore::bConnected = false;
RakNet::TimeMS NetworkCore::timeLastUpdate = 0;


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
	GameCore::mPlayerPool->addLocalPlayer( m_pRak->GetMyGUID(), "SERVER" );	
	GameCore::mGraphicsCore->bigScreen->declareNewPlayer(m_pRak->GetMyGUID());
}

/// @brief Get the interface to RakNet (shouldn't be needed if this class is thorough)
/// @return RakPeerInterface pointer
RakNet::RakPeerInterface* NetworkCore::getRakInterface() { return m_pRak; }


/// @brief  Deconstructor.
NetworkCore::~NetworkCore()
{
    m_pRak->Shutdown( 100, 0 );
	RakNet::RakPeerInterface::DestroyInstance( m_pRak );
}


/// @brief  Called once a frame with the latest user keypresses.
/// @param  inputSnapshot  The object containing the latest user keypresses.
void NetworkCore::frameEvent(InputState *inputSnapshot)
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

			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			{
				log( "Remote player disconnected" );
				break;
			}

			case ID_REMOTE_CONNECTION_LOST:
				log( "Remote player connection lost" );
				break;

			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				log( "New incoming remote connection" );
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

	// should really package multiple updates into one packet also though
	// RakNet does tend to do that a bit by itself to stop UDP breaking

	int j = 0;
	for( j = 0; j < MAX_PLAYERS; j ++ )
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

		m_pRak->Send( &bitSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, GameCore::mPlayerPool->getLocalPlayerID(), true );

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

	for( int j = 0; j < MAX_PLAYERS; j ++ )
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

		m_pRak->Send( &bitSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, playerid, false );

		delete( playerSnap );
	}
}

/// @brief	Set up the game for a particular player. Sends PlayerJoin for each 
///			connected client, then an overall update on gamestate
void NetworkCore::SetupGameForPlayer( RakNet::RakNetGUID playerid )
{
	Player *playerSend;

	for( int j = 0; j < MAX_PLAYERS; j ++ )
	{
		// Don't send a new client PlayerJoin of themselves
		if( GameCore::mPlayerPool->getPlayerGUID( j ) == playerid )
			continue;

		playerSend = GameCore::mPlayerPool->getPlayer( j );
		if( playerSend )
		{
			RakNet::BitStream bsJoin;
			bsJoin.Write( GameCore::mPlayerPool->getPlayerGUID( j ) );
            //RakNet::RakString *strName = new RakNet::RakString("RemotePlayer");
			//RakNet::StringCompressor().EncodeString( strName, 128, &bsJoin );
			m_RPC->Signal( "PlayerJoin", &bsJoin, HIGH_PRIORITY, RELIABLE_ORDERED, 0, playerid, false, false );
		}
	}

	GamestateUpdatePlayer( playerid );
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
	
	// Add the player to server player pool
	GameCore::mPlayerPool->addPlayer( pkt->guid, szNickname );

	RakNet::BitStream bsNewPlayer;
	RakNet::BitStream bsSend;

	// Alert other players that someone new has joined
	bsNewPlayer.Write( pkt->guid );
	RakNet::StringCompressor().EncodeString( szNickname, 128, &bsNewPlayer );
	m_RPC->Signal( "PlayerJoin", &bsNewPlayer, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pkt->guid, true, false );

	// Send them a GameJoin RPC so they can get set up
	// This is where any game specific initialization can go
	RakNet::StringCompressor().EncodeString( szNickname, 128, &bsSend );
	m_RPC->Signal( "GameJoin", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pkt->guid, false, false );

	SetupGameForPlayer( pkt->guid );

}

void NetworkCore::PlayerQuit( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	// Check and make sure things from Player are deleted
	// send an RPC to tell other clients they quit

	GameCore::mPlayerPool->delPlayer( pkt->guid );
}

void NetworkCore::PlayerChat( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	// Send to all players

	char szMessage[128];
	RakNet::StringCompressor().DecodeString( szMessage, 128, bitStream );

    // This is where some checks can be done for team-only messages etc

    RakNet::BitStream bsSend;

    bsSend.Write( pkt->guid );
    RakNet::StringCompressor().EncodeString( szMessage, 128, &bsSend );

    m_RPC->Signal( "PlayerChat", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, GameCore::mPlayerPool->getLocalPlayerID(), true, false );
}

void NetworkCore::InfoItemTransmit( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	char szMessage[128];
    RakNet::BitStream bsSend;
	RakNet::StringCompressor().DecodeString( szMessage, 128, bitStream );
    bsSend.Write( pkt->guid );
    RakNet::StringCompressor().EncodeString( szMessage, 128, &bsSend );

    m_RPC->Signal( "InfoItemTransmit", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, GameCore::mPlayerPool->getLocalPlayerID(), true, false );
}

	
void NetworkCore::PlayerSpawn( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	// Do some checking here to make sure the player is allowed to spawn before sending the RPC back
    CarType iCarType;
    bitStream->Read( iCarType );

    // TODO: something with iCarType

	Player *pPlayer = GameCore::mPlayerPool->getPlayer( pkt->guid );
	pPlayer->createPlayer( GameCore::mSceneMgr, iCarType, SKIN0, GameCore::mPhysicsCore );

	// Alert the BigScreen we've had a player spawned
	GameCore::mGraphicsCore->bigScreen->declareNewPlayer(pkt->guid);
    GameCore::mGraphicsCore->mGameplay->declareNewPlayer(pkt->guid);

	RakNet::BitStream bsSpawn;
	bsSpawn.Write( pkt->guid );
    bsSpawn.Write( iCarType );
	m_RPC->Signal( "PlayerSpawn", &bsSpawn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, GameCore::mPlayerPool->getLocalPlayerID(), true, false );

	// Spawn all other players (here for now but will be moved to SetupGameForPlayer)
	for( int i = 0; i < MAX_PLAYERS; i ++ )
	{
		if( GameCore::mPlayerPool->getPlayerGUID( i ) == pkt->guid )
			continue;

		Player *pRemote = GameCore::mPlayerPool->getPlayer( i );
		if( pRemote )
		{
			if( pRemote->getCar() )
			{
				RakNet::BitStream bsSpawn;
				bsSpawn.Write( GameCore::mPlayerPool->getPlayerGUID( i ) );
                bsSpawn.Write( GameCore::mPlayerPool->getPlayer( i )->getCarType() );
				m_RPC->Signal( "PlayerSpawn", &bsSpawn, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pkt->guid, false, false );
			}
		}
	}
}



/// @brief Registers the RPC calls for the client
void NetworkCore::RegisterRPCSlots()
{
    m_RPC = RakNet::RPC4::GetInstance();
	m_pRak->AttachPlugin( m_RPC );

	m_RPC->RegisterSlot( "PlayerJoin",		PlayerJoin, 0 );
	m_RPC->RegisterSlot( "PlayerQuit",		PlayerQuit, 0 );
	m_RPC->RegisterSlot( "PlayerChat",		PlayerChat, 0 );
	m_RPC->RegisterSlot( "PlayerSpawn",		PlayerSpawn, 0 );
	m_RPC->RegisterSlot( "InfoItemTransmit",InfoItemTransmit, 0);

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

// This links the big screen view into the networkcore so we can push updates
/* 
void NetworkCore::linkBigScreen(BigScreen* bigScreen_P)
{
	bigScreen = bigScreen_P;
}
*/
