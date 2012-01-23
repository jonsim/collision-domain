/**
 * @file	NetworkCore.cpp
 * @brief 	Takes notifications and deal with them however networking needs to
 */

#include "stdafx.h"
#include "GameIncludes.h"


RakNet::RakPeerInterface* NetworkCore::m_pRak;
RakNet::RPC4 NetworkCore::m_RPC;
bool NetworkCore::bConnected = false;
RakNet::TimeMS NetworkCore::timeLastUpdate = 0;


/// @brief  Constructor, initialising all resources.
NetworkCore::NetworkCore()
{
	// Get our main interface to RakNet
	m_pRak = RakNet::RakPeerInterface::GetInstance();
	m_pRak->Startup( 1, &RakNet::SocketDescriptor(), 1 );

	bConnected = false;

	RegisterRPCSlots();
}

bool NetworkCore::Connect( const char *szHost, int iPort, char *szPass )
{
	// Connect to the specified server
	RakNet::ConnectionAttemptResult bCon = m_pRak->Connect( szHost, iPort, szPass, szPass == NULL ? 0 : strlen(szPass) );
	m_pRak->SetOccasionalPing( true );

	if( bCon == RakNet::ConnectionAttemptResult::CONNECTION_ATTEMPT_STARTED )
	{
		log( "Connecting to %s : %i", szHost, iPort );
		return true;
	}

	log( "Failed to connect to %s : %i", szHost, iPort );
	return false;
}

RakNet::RakPeerInterface* NetworkCore::getRakInterface() { return m_pRak; }


/// @brief  Deconstructor.
NetworkCore::~NetworkCore()
{
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

			// Push the data onto a bitstream
			RakNet::BitStream bitSend;
			unsigned char packetid = ID_PLAYER_INPUT;

			bitSend.Write( packetid );
			bitSend.Write( (char*)&playerInput, sizeof( PLAYER_INPUT_DATA ) );

			// Send to server
			m_pRak->Send( &bitSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverGUID, false );
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
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				log( "Connection to server accepted" );
				serverGUID = pkt->guid;

				RakNet::BitStream bsSend;
				RakNet::RakString strName( "RemotePlayer" );
				RakNet::StringCompressor().EncodeString( strName, 128, &bsSend );
				m_RPC.Signal( "PlayerJoin", &bsSend, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverGUID, false, false );
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

}


/// @brief  Called once a frame and supplies the requested user position so it can be updated onscreen.
/// @param  playerID  An identifier for a specific player in the game.
/// @return NULL if no CarSnapshot has been received since the last time this method was polled.
///         The latest CarSnapshot for the requested player if one has been received (or more than one :)).
CarSnapshot* NetworkCore::getCarSnapshotIfExistsSincePreviousGet(int playerID)
{
    return NULL;
}


void NetworkCore::GameJoin( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	char szNickname[128];

	RakNet::StringCompressor().DecodeString( szNickname, 128, bitStream );

	// Add ourselves to the player pool
	GameCore::mPlayerPool->addLocalPlayer( m_pRak->GetMyGUID(), szNickname );
	log( "GameJoin : local playerid %s", m_pRak->GetMyGUID().ToString() );

	bConnected = true;
	timeLastUpdate = 0;

	// Request to spawn straight away for now
	m_RPC.Signal( "PlayerSpawn", NULL, HIGH_PRIORITY, RELIABLE_ORDERED, 0, pkt->guid, false, false );

	// If we're allowed to spawn, our spawn method will be called by the server automagically.
}

void NetworkCore::PlayerJoin( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	char szNickname[128];

	RakNet::RakNetGUID playerid;
	bitStream->Read( playerid );

	log( "PlayerJoin : playerid %s", playerid.ToString() );

	RakNet::StringCompressor().DecodeString( szNickname, 128, bitStream );

	GameCore::mPlayerPool->addPlayer( playerid, szNickname );
}

void NetworkCore::PlayerQuit( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	RakNet::RakNetGUID playerid;
	bitStream->Read( playerid );

	GameCore::mPlayerPool->delPlayer( playerid );
}

void NetworkCore::PlayerChat( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	char szMessage[512];
	RakNet::RakNetGUID playerid;
	bitStream->Read( playerid );

	RakNet::StringCompressor().DecodeString( szMessage, 512, bitStream );
}

void NetworkCore::PlayerSpawn( RakNet::BitStream *bitStream, RakNet::Packet *pkt )
{
	Player *pPlayer = NULL;
	RakNet::RakNetGUID playerid;
	bitStream->Read( playerid );

	log( "PlayerSpawn : playerid %s", playerid.ToString() );

	if( playerid == GameCore::mPlayerPool->getLocalPlayerID() )
	{
		pPlayer = GameCore::mPlayerPool->getLocalPlayer();
		pPlayer->createPlayer( GameCore::mSceneMgr, MEDIUM, SKIN0, GameCore::mPhysicsCore );
                                pPlayer->attachCamera( GameCore::mGraphicsCore->mCamera );
	}
	else
	{
		pPlayer = GameCore::mPlayerPool->getPlayer( playerid );
		if( pPlayer != NULL )
			pPlayer->createPlayer( GameCore::mSceneMgr, MEDIUM, SKIN0, GameCore::mPhysicsCore );
		else
			log( "..invalid player" );
	}

}

/// @brief Registers the RPC calls for the client
void NetworkCore::RegisterRPCSlots()
{
	m_pRak->AttachPlugin( &m_RPC );

	m_RPC.RegisterSlot( "GameJoin",			GameJoin, 0 );
	m_RPC.RegisterSlot( "PlayerJoin",		PlayerJoin, 0 );
	m_RPC.RegisterSlot( "PlayerQuit",		PlayerQuit, 0 );
	m_RPC.RegisterSlot( "PlayerChat",		PlayerChat, 0 );
	m_RPC.RegisterSlot( "PlayerSpawn",		PlayerSpawn, 0 );
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
