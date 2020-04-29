#pragma once
#include <stdio.h>
#include <string.h>
#include <string>
#include <functional>

#include "RakPeerInterface.h"
#include "MessageIdentifiers.h"
#include "BitStream.h"
#include "RakNetTypes.h"  // Need MessageID;

#define MAX_CLIENTS 8
#define SERVER_PORT 60000

// A struct to hold Clients when they are connected;
struct ConnectedClient
{

public:

	ConnectedClient(){}
	~ConnectedClient(){}

public:

	bool m_isValid = false;
	std::string m_username;
	RakNet::RakNetGUID m_guid;
	RakNet::SystemAddress m_systemAddress;
	bool m_readyToStartGame = false;
	bool m_purchasePhaseComplete = false;
	bool m_battlePhaseComplete = false;
	bool m_marketplaceLocked = false;
};

// Distinction between applications that are running as the Server vs. Client;
enum class ConnectionType
{
	NONE,
	SERVER,
	CLIENT
};

// The first bit of a packet message with have a type;
enum GameMessageIDTypes
{
	// The first letter is who can process the message;
	// S = Messages for the Server from a Client;
	// C = Message for the Client(s) from the Server;
	S_LOBBYMESSAGE = ID_USER_PACKET_ENUM, // For the user to use.  Start your first enumeration at ID_USER_PACKET_ENUM.
	S_CLIENTUSERNAME,
	S_READYFLAG,
	S_REQUESTMARKETPLACECARDS,
	S_REQUESTHANDCARDS,
	S_CLEARMARKETPLACECARDS,
	S_CLIENTCOMPLETEPURCHASEPHASE,
	S_CLIENTCOMPLETEBATTLEPHASE,
	S_CLIENTMARKETPLACELOCKED,
	S_CLIENTPURCHASEDMARKETCARD,
	S_CLIENTSOLDHANDCARD,
	S_CLIENTPLACEDUNITFROMHANDCARD,
	S_WINNEROFMATCHBEINGREPORTED,
	C_LOBBYMESSAGE,
	C_LOBBYMESSAGEGAMESTARTING,
	C_STARTMULTIPLAYERGAME,
	C_PLAYERID,
	C_SWITCHPHASE,
	C_RECEIVECARDTYPESFORMARKETPLACE,
	C_RECEIVECARDTYPESFORHAND,
	C_RECEIVEUNITTYPESFORFIELD,
	C_RECEIVEENEMYUNITTYPESFORENEMYFIELD,
	C_RECIEVEGOLDAMOUNT,
	C_RECIEVEGOESFIRSTFORBATTLE,
	C_RECIEVESEEDFORBATTLE,
	C_RECIEVEENEMYPLAYERINFOFORBATTLE,
	C_RECIEVEMATCHIDFORBATTLE,
	C_RECIEVEUPDATEDPLAYERHEALTH,
	C_YOUWINTHEGAME,
	C_YOULOSETHEGAME,

	ID_GAMEMESSAGE_COUNT
};

class RakNetInterface
{

public:

	RakNetInterface();
	~RakNetInterface();

	// Flow;
	void Startup();
	void Shutdown();

	// Process Packets;
	void ProcessIncomingPackets();

	// Server/Client Connections;
	bool CreateServer();
	bool JoinServerAsClient(const std::string& hostIP);
	void CloseConnectionToServer();
	void CloseConnectionWithClient(int playerID_);
	void SendClientUsername(const std::string& username);
	void SendIsReadyFlag(bool isReady);
	void SendPlayerIDAndUsernameToPlayer(const int playerID_, std::string username_);

	// Updating Client Information;
	void AddClientToClientList(RakNet::Packet* packet);
	void RemoveClientFromClientList(RakNet::Packet* packet);
	void UpdateClientReadyFlag(RakNet::Packet* packet);
	void UpdateAllClientsNotCompleteWithPhases();
	void UpdateClientCompleteWithPurchasePhase(RakNet::Packet* packet);
	void UpdateClientCompleteWithBattlePhase(RakNet::Packet* packet);
	void UpdateClientMarketplaceLocked(RakNet::Packet* packet);
	void UpdateCardsBasedOnPurchaseByPlayer(RakNet::Packet* packet_);
	void UpdateCardsBasedOnSellingByPlayer(RakNet::Packet* packet_);
	void UpdateUnitsBasedOnPlacingCardInUnitSlotByPlayer(RakNet::Packet* packet_);

	// Updating Server;
	void ProcessMatchReport(RakNet::Packet* packet_);

	// Checks;
	bool CheckAllClientsReadyStatus();
	bool CheckAllClientsPurchasePhaseComplete();
	bool CheckAllClientsBattlePhaseComplete();

	// Sending Packets and Things;
	void SendBitStreamToAllClients(RakNet::BitStream* bs);
	void SendBitStreamToClient(RakNet::BitStream* bs, const ConnectedClient& client);
	void SendBitStreamToClient(RakNet::BitStream* bs, const RakNet::RakNetGUID& guid_);
	void SendBitStreamToClient(RakNet::BitStream* bs, const RakNet::SystemAddress& systemAddress_);
	void SendBitStreamToClient(RakNet::BitStream* bs, const int playerID_);
	void SendBitStreamToServer(RakNet::BitStream* bs);
	void SendLobbyMessageToServer(const std::string& message);
	void SendLobbyMessageToClients(RakNet::Packet* packet);
	void SendGameStartingMessageToClients();
	void SendGameCountdownMessageToClients(int timer_);
	void SendStartMultiplayerGameMessageToClients();
	void SendSwitchPhaseMessageToClients();
	

public:

	RakNet::RakPeerInterface* m_peer = nullptr;

	ConnectionType m_connection = ConnectionType::NONE;

	int m_connectedClientCount = 0;
	//std::vector<ConnectedClient> m_clientList;
	ConnectedClient m_clientList[MAX_CLIENTS];
	RakNet::RakNetGUID m_serverGUID;
	RakNet::SystemAddress m_serverAddress;

	std::function<void(RakNet::Packet*)> m_gamePacketCallback;
};

extern RakNetInterface* g_theRakNetInterface;