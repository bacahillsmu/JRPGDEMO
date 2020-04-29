#include "ThirdParty/RakNet/RakNetInterface.hpp"
#include "Engine/Core/DevConsole.hpp"

#include "Game/Framework/App.hpp"
#include "Game/Framework/Interface.hpp"
#include "Game/Gameplay/PlayerFilters.hpp"

//-----------------------------------------------------------------------------------------------
RakNetInterface* g_theRakNetInterface = nullptr;

//-----------------------------------------------------------------------------------------------
RakNetInterface::RakNetInterface()
{
	m_peer = RakNet::RakPeerInterface::GetInstance();
}

//-----------------------------------------------------------------------------------------------
RakNetInterface::~RakNetInterface()
{
	RakNet::RakPeerInterface::DestroyInstance(m_peer);
}

//-----------------------------------------------------------------------------------------------
// Flow;
//-----------------------------------------------------------------------------------------------
void RakNetInterface::Startup()
{

}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::Shutdown()
{

}

//-----------------------------------------------------------------------------------------------
// Process Packets;
//-----------------------------------------------------------------------------------------------
void RakNetInterface::ProcessIncomingPackets()
{
	RakNet::Packet* packet = nullptr;

	for (packet = m_peer->Receive(); packet; m_peer->DeallocatePacket(packet), packet = m_peer->Receive())
	{
		switch (packet->data[0])
		{
			// RakNet Messages;
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
			{
				g_theDevConsole->AddStringToTextOutput(Rgba::RED, "Another client has disconnected.");
				RemoveClientFromClientList(packet);
				break;
			}			
			case ID_REMOTE_CONNECTION_LOST:
			{
				g_theDevConsole->AddStringToTextOutput(Rgba::RED, "Another client has lost the connection.");
				RemoveClientFromClientList(packet);
				break;
			}
			case ID_REMOTE_NEW_INCOMING_CONNECTION:
			{
				g_theDevConsole->AddStringToTextOutput(Rgba::GREEN, "Another client has connected.");
				break;
			}
			case ID_CONNECTION_REQUEST_ACCEPTED:
			{
				g_theDevConsole->AddStringToTextOutput(Rgba::GREEN, "Our connection request has been accepted.");
				break;
			}
			case ID_NEW_INCOMING_CONNECTION:
			{
				g_theDevConsole->AddStringToTextOutput(Rgba::YELLOW, "A connection is incoming.");
				break;
			}
			case ID_NO_FREE_INCOMING_CONNECTIONS:
			{
				g_theDevConsole->AddStringToTextOutput(Rgba::RED, "The server is full.");
				break;
			}
			case ID_DISCONNECTION_NOTIFICATION:
			{
				if(m_connection == ConnectionType::SERVER)
				{
					g_theDevConsole->AddStringToTextOutput(Rgba::RED, "A client has disconnected.");
					RemoveClientFromClientList(packet);
				}
				else
				{
					g_theDevConsole->AddStringToTextOutput(Rgba::RED, "We have been disconnected.");
				}
				break;
			}
			case ID_CONNECTION_LOST:
			{
				if(m_connection == ConnectionType::SERVER)
				{
					g_theDevConsole->AddStringToTextOutput(Rgba::RED, "A client lost the connection.");
					RemoveClientFromClientList(packet);
				}
				else
				{
					g_theDevConsole->AddStringToTextOutput(Rgba::RED, "Connection lost.");
				}
				break;
			}
			case ID_CONNECTION_ATTEMPT_FAILED:
			{
				g_theDevConsole->AddStringToTextOutput(Rgba::RED, "Connection Attempt Failed.");
				break;
			}

			// Pass to the Game Messages;
			default:
			{
				// Pass to game;
				m_gamePacketCallback(packet);
				break;
			}
		}
	}
}

//-----------------------------------------------------------------------------------------------
// Server/Client Connections;
//-----------------------------------------------------------------------------------------------
bool RakNetInterface::CreateServer()
{
	m_connection = ConnectionType::SERVER;

	RakNet::SocketDescriptor socketDescriptor(SERVER_PORT, 0);
	m_peer->Startup(MAX_CLIENTS, &socketDescriptor, 1);
	m_peer->SetMaximumIncomingConnections(MAX_CLIENTS);

	return true;
}

//-----------------------------------------------------------------------------------------------
bool RakNetInterface::JoinServerAsClient(const std::string& hostIP)
{
	m_connection = ConnectionType::CLIENT;

	RakNet::SocketDescriptor socketDescriptor;
	m_peer->Startup(1, &socketDescriptor, 1);
	RakNet::ConnectionAttemptResult connectionAttempResult = m_peer->Connect(hostIP.c_str(), SERVER_PORT, 0, 0);

	return connectionAttempResult == RakNet::CONNECTION_ATTEMPT_STARTED;
}

void RakNetInterface::CloseConnectionToServer()
{
	m_peer->CloseConnection(m_serverAddress, true, 0, HIGH_PRIORITY);
	m_connection = ConnectionType::NONE;
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::CloseConnectionWithClient(int playerID_)
{
	m_peer->CloseConnection(m_clientList[playerID_].m_systemAddress, true, 0, HIGH_PRIORITY);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::SendClientUsername(const std::string& username)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)S_CLIENTUSERNAME);
	bsOut.Write(username.c_str());

	SendBitStreamToServer(&bsOut);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::SendIsReadyFlag(bool isReady)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)S_READYFLAG);
	bsOut.Write(isReady);

	SendBitStreamToServer(&bsOut);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::SendPlayerIDAndUsernameToPlayer(const int playerID_, std::string username_)
{
	RakNet::RakNetGUID guid = m_clientList[playerID_].m_guid;
	RakNet::BitStream bsOut;
	RakNet::RakString rs_username = username_.c_str();
	bsOut.Write((unsigned char)C_PLAYERID);
	bsOut.Write(playerID_);
	bsOut.Write(rs_username);

	SendBitStreamToClient(&bsOut, guid);
}

//-----------------------------------------------------------------------------------------------
// Updating Client Information;
//-----------------------------------------------------------------------------------------------
void RakNetInterface::AddClientToClientList(RakNet::Packet* packet)
{
	// The data in this packet should be the client's username;
	RakNet::RakString rs;
	RakNet::BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	bsIn.Read(rs);

	std::string username = rs.C_String();
	if (username == "")
	{
		username = Stringf("%s%d", "User", m_connectedClientCount);
	}

	ConnectedClient client;
	client.m_isValid = true;
	client.m_username = username.c_str();
	client.m_guid = packet->guid;
	client.m_systemAddress = packet->systemAddress;

	m_clientList[m_connectedClientCount] = client;
	m_connectedClientCount++;

	// Create a message to be sent out;
	RakNet::BitStream bsOut;
	std::string joinMessage = Stringf("[Server]: %s has joined!", client.m_username.c_str());
	bsOut.Write((unsigned char)C_LOBBYMESSAGE);
	bsOut.Write(joinMessage.c_str());

	SendBitStreamToAllClients(&bsOut);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::RemoveClientFromClientList(RakNet::Packet* packet)
{
	int indexToRemove = 0;
	ConnectedClient removedClient;
	for (int i = 0; i < m_connectedClientCount; ++i)
	{
		if (m_clientList[i].m_guid == packet->guid)
		{
			removedClient = m_clientList[i];
			indexToRemove = i;
			break;
		}
	}

	if(indexToRemove == (int)MAX_CLIENTS
	|| indexToRemove == m_connectedClientCount)
	{
		m_clientList[indexToRemove].m_isValid = false;
		m_clientList[indexToRemove].m_username = "";
		m_clientList[indexToRemove].m_readyToStartGame = false;
	}
	else
	{
		for (int i = indexToRemove; i < m_connectedClientCount - 1; ++i)
		{
			m_clientList[i] = m_clientList[i + 1];
		}
	}
	m_connectedClientCount--;
	m_connectedClientCount = Clamp(m_connectedClientCount, 0, MAX_CLIENTS);

	// Create a message to be sent out;
	RakNet::BitStream bsOut;
	std::string joinMessage = Stringf("[Server]: %s has left!", removedClient.m_username.c_str());
	bsOut.Write((unsigned char)C_LOBBYMESSAGE);
	bsOut.Write(joinMessage.c_str());

	SendBitStreamToAllClients(&bsOut);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::UpdateClientReadyFlag(RakNet::Packet* packet)
{
	// The data in this packet should be the client's ready flag;
	RakNet::BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	bool readyFlag = false;
	bsIn.Read(readyFlag);

	ConnectedClient client;
	for(int i = 0; i < m_connectedClientCount; ++i)
	{
		if(m_clientList[i].m_guid == packet->guid)
		{
			client = m_clientList[i];
			m_clientList[i].m_readyToStartGame = readyFlag;
			break;
		}
	}

	// Create a message to be sent out;
	RakNet::BitStream bsOut;
	std::string readyMessage;
	if(readyFlag)
	{
		readyMessage = Stringf("[Server]: %s is ready to start!", client.m_username.c_str());
	}
	else
	{
		readyMessage = Stringf("[Server]: %s is not ready to start!", client.m_username.c_str());
	}
	
	bsOut.Write((unsigned char)C_LOBBYMESSAGE);
	bsOut.Write(readyMessage.c_str());

	SendBitStreamToAllClients(&bsOut);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::UpdateAllClientsNotCompleteWithPhases()
{
	for (int i = 0; i < m_connectedClientCount; ++i)
	{
		m_clientList[i].m_purchasePhaseComplete = false;
		m_clientList[i].m_battlePhaseComplete = false;
	}
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::UpdateClientCompleteWithPurchasePhase(RakNet::Packet* packet)
{
	for (int i = 0; i < m_connectedClientCount; ++i)
	{
		if (m_clientList[i].m_guid == packet->guid)
		{
			m_clientList[i].m_purchasePhaseComplete = true;
			break;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::UpdateClientCompleteWithBattlePhase(RakNet::Packet* packet)
{
	for (int i = 0; i < m_connectedClientCount; ++i)
	{
		if (m_clientList[i].m_guid == packet->guid)
		{
			m_clientList[i].m_battlePhaseComplete = true;
			break;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::UpdateClientMarketplaceLocked(RakNet::Packet* packet)
{
	// The data in this packet should be the client's ready flag;
	RakNet::BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	bool marketplaceLocked = false;
	bsIn.Read(marketplaceLocked);

	for (int i = 0; i < m_connectedClientCount; ++i)
	{
		if (m_clientList[i].m_guid == packet->guid)
		{
			m_clientList[i].m_marketplaceLocked = marketplaceLocked;
			break;
		}
	}
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::UpdateCardsBasedOnPurchaseByPlayer(RakNet::Packet* packet_)
{
	RakNet::BitStream bsIn(packet_->data, packet_->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	unsigned int cardID = 0u;
	int playerID = 0;
	bsIn.Read(cardID);
	bsIn.Read(playerID);

	g_Interface->server().UpdateMarketplaceCardsBasedOnPurchaseByPlayer(cardID, playerID);
	g_Interface->server().GetMarketplaceCardsForPlayerID(playerID);
	g_Interface->server().GetAndSendCardsInHandForPlayerID(playerID);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::UpdateCardsBasedOnSellingByPlayer(RakNet::Packet* packet_)
{
	RakNet::BitStream bsIn(packet_->data, packet_->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	unsigned int cardID = 0u;
	int playerID = 0;
	bsIn.Read(cardID);
	bsIn.Read(playerID);

	g_Interface->server().RemoveCardFromSellingByPlayer(cardID);
	g_Interface->server().GetAndSendCardsInHandForPlayerID(playerID);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::UpdateUnitsBasedOnPlacingCardInUnitSlotByPlayer(RakNet::Packet* packet_)
{
	RakNet::BitStream bsIn(packet_->data, packet_->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	unsigned int cardID = 0u;
	int unitSlotID = 0;
	int playerID = 0;
	bsIn.Read(cardID);
	bsIn.Read(unitSlotID);
	bsIn.Read(playerID);

	g_Interface->server().RemoveCardFromPlacingUnitByPlayer(cardID);
	g_Interface->server().CreateNewUnitFromCardPlacedByPlayer(cardID, unitSlotID);
	g_Interface->server().GetAndSendCardsInHandForPlayerID(playerID);
	g_Interface->server().GetAndSendUnitsForPlayerID(playerID);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::ProcessMatchReport(RakNet::Packet* packet_)
{
	RakNet::BitStream bsIn(packet_->data, packet_->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	int winningPlayerID = 0;
	int losingPlayerID = 0;
	int damageDealtToLosingPlayer = 0;
	int matchID = 0;
	bsIn.Read(winningPlayerID);
	bsIn.Read(losingPlayerID);
	bsIn.Read(damageDealtToLosingPlayer);
	bsIn.Read(matchID);

	g_Interface->server().CreateMatchReport(winningPlayerID, losingPlayerID, damageDealtToLosingPlayer, matchID);
}

//-----------------------------------------------------------------------------------------------
bool RakNetInterface::CheckAllClientsReadyStatus()
{
	int readyCounter = 0;

	// Go through each connected client, count the ones who are ready;
	for(int i = 0; i < m_connectedClientCount; ++i)
	{
		if(m_clientList[i].m_readyToStartGame)
		{
			readyCounter++;
		}
	}

	// If we have at least one connected client, then check our readyCounter;
	if(m_connectedClientCount > 0)
	{
		// If our readyCounter is equal to the number of connect clients, then all are ready;
		return readyCounter == m_connectedClientCount;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
bool RakNetInterface::CheckAllClientsPurchasePhaseComplete()
{
	int completeCounter = 0;

	// Go through each connected client, count the ones who are complete;
	for (int i = 0; i < m_connectedClientCount; ++i)
	{
		if (m_clientList[i].m_purchasePhaseComplete)
		{
			completeCounter++;
		}
	}

	// If we have at least one connected client, then check our completeCounter;
	if (m_connectedClientCount > 0)
	{
		// If our completeCounter is equal to the number of connect clients, then all are complete;
		return completeCounter == m_connectedClientCount;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
bool RakNetInterface::CheckAllClientsBattlePhaseComplete()
{
	int completeCounter = 0;

	// Go through each connected client, count the ones who are complete;
	for (int i = 0; i < m_connectedClientCount; ++i)
	{
		if (m_clientList[i].m_battlePhaseComplete)
		{
			completeCounter++;
		}
	}

	// If we have at least one connected client, then check our completeCounter;
	if (m_connectedClientCount > 0)
	{
		// If our completeCounter is equal to the number of connect clients, then all are complete;
		return completeCounter == m_connectedClientCount;
	}

	return false;
}

//-----------------------------------------------------------------------------------------------
// Sending Packets, BitStreams, Messages, etc...
//-----------------------------------------------------------------------------------------------
void RakNetInterface::SendBitStreamToAllClients(RakNet::BitStream* bs)
{
	// Send to each client in our client list;
	for (int i = 0; i < m_connectedClientCount; ++i)
	{
		m_peer->Send(bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_clientList[i].m_systemAddress, false);
	}
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::SendBitStreamToClient(RakNet::BitStream* bs, const ConnectedClient& client)
{
	m_peer->Send(bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, client.m_systemAddress, false);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::SendBitStreamToClient(RakNet::BitStream* bs, const RakNet::RakNetGUID& guid_)
{
	m_peer->Send(bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, guid_, false);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::SendBitStreamToClient(RakNet::BitStream* bs, const RakNet::SystemAddress& systemAddress_)
{
	m_peer->Send(bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, systemAddress_, false);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::SendBitStreamToClient(RakNet::BitStream* bs, const int playerID_)
{
	Player* player = g_Interface->query().GetPlayer(HasPlayerID(playerID_));

	if(!player->IsAIPlayer())
	{
		m_peer->Send(bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_clientList[playerID_].m_systemAddress, false);
	}
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::SendBitStreamToServer(RakNet::BitStream* bs)
{
	m_peer->Send(bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, m_serverAddress, false);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::SendLobbyMessageToServer(const std::string& message)
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)S_LOBBYMESSAGE);
	bsOut.Write(message.c_str());

	SendBitStreamToServer(&bsOut);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::SendLobbyMessageToClients(RakNet::Packet* packet)
{
	RakNet::RakString rs;
	RakNet::BitStream bsIn(packet->data, packet->length, false);
	bsIn.IgnoreBytes(sizeof(RakNet::MessageID));
	bsIn.Read(rs);
	std::string message = rs.C_String();

	// We have received a message from a client, this needs to go out to all other clients;
	// Identify who sent this message based on guid;
	ConnectedClient clientWhoSentMessage;
	for(int i = 0; i < m_connectedClientCount; ++i)
	{
		if(packet->guid == m_clientList[i].m_guid)
		{
			clientWhoSentMessage = m_clientList[i];
		}
	}

	// Create a message to be sent out;
	RakNet::BitStream bsOut;
	std::string concatMessage = Stringf("[%s]: %s", clientWhoSentMessage.m_username.c_str(), message.c_str());
	bsOut.Write((unsigned char)C_LOBBYMESSAGE);
	bsOut.Write(concatMessage.c_str());

	SendBitStreamToAllClients(&bsOut);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::SendGameStartingMessageToClients()
{
	std::string message = "Game Starting...";

	// Create a message to be sent out;
	RakNet::BitStream bsOut;
	std::string concatMessage = Stringf("[Server]: %s", message.c_str());
	bsOut.Write((unsigned char)C_LOBBYMESSAGE);
	bsOut.Write(concatMessage.c_str());

	SendBitStreamToAllClients(&bsOut);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::SendGameCountdownMessageToClients(int timer_)
{
	// Create a message to be sent out;
	RakNet::BitStream bsOut;
	std::string concatMessage = Stringf("[Server]: %d...", timer_);
	bsOut.Write((unsigned char)C_LOBBYMESSAGEGAMESTARTING);
	bsOut.Write(concatMessage.c_str());

	SendBitStreamToAllClients(&bsOut);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::SendStartMultiplayerGameMessageToClients()
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)C_STARTMULTIPLAYERGAME);

	SendBitStreamToAllClients(&bsOut);
}

//-----------------------------------------------------------------------------------------------
void RakNetInterface::SendSwitchPhaseMessageToClients()
{
	RakNet::BitStream bsOut;
	bsOut.Write((unsigned char)C_SWITCHPHASE);

	SendBitStreamToAllClients(&bsOut);
}


