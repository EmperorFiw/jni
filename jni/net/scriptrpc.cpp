#include "../main.h"
#include "../game/game.h"
#include "netgame.h"
#include "../chatwindow.h"
#include "../CAudioStream.h"
#include "../KillList.h"

#include <algorithm>

extern CGame *pGame;
extern CNetGame *pNetGame;
extern CChatWindow *pChatWindow;
extern CAudioStream* pAudioStream;
extern KillList *pKillList;

void ScrDisplayGameText(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrDisplayGameText"));
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	char szMessage[512];
	int iType;
	int iTime;
	int iLength;

	bsData.Read(iType);
	bsData.Read(iTime);
	bsData.Read(iLength);

	if(iLength > 512) return;

	bsData.Read(szMessage,iLength);
	szMessage[iLength] = '\0';

	pGame->DisplayGameText(szMessage, iTime, iType);
}

void ScrSetGravity(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetGravity"));
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fGravity;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(fGravity);

	pGame->SetGravity(fGravity);
}

void ScrForceSpawnSelection(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrForceSpawnSelection"));
	pNetGame->GetPlayerPool()->GetLocalPlayer()->ReturnToClassSelection();
}

void ScrSetPlayerPos(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetPlayerPos"));
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	VECTOR vecPos;
	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);

	if(pLocalPlayer) pLocalPlayer->GetPlayerPed()->TeleportTo(vecPos.X,vecPos.Y,vecPos.Z);
}

void ScrSetCameraPos(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetCameraPos"));
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	VECTOR vecPos;
	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);
	pGame->GetCamera()->SetPosition(vecPos.X, vecPos.Y, vecPos.Z, 0.0f, 0.0f, 0.0f);
}

void ScrSetCameraLookAt(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetCameraLookAt"));
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	VECTOR vecPos;
	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);
	pGame->GetCamera()->LookAtPoint(vecPos.X,vecPos.Y,vecPos.Z,2);	
}

void ScrSetPlayerFacingAngle(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetPlayerFacingAngle"));
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	float fAngle;
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(fAngle);
	pGame->FindPlayerPed()->ForceTargetRotation(fAngle);
}

void ScrSetFightingStyle(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetFightingStyle"));
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID playerId;
	uint8_t byteFightingStyle = 0;
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(playerId);
	bsData.Read(byteFightingStyle);
	
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	CPlayerPed *pPlayerPed = nullptr;

	if(pPlayerPool)
	{
		if(playerId == pPlayerPool->GetLocalPlayerID())
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		else if(pPlayerPool->GetSlotState(playerId)) 
			pPlayerPed = pPlayerPool->GetAt(playerId)->GetPlayerPed();

		if(pPlayerPed)				
				pPlayerPed->SetFightingStyle(byteFightingStyle);
	}
}

void ScrSetPlayerSkin(RPCParameters *rpcParams)
{
	//Log("RPC: ScrSetPlayerSkin");
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	int iPlayerID;
	unsigned int uiSkin;
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(iPlayerID);
	bsData.Read(uiSkin);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	if(iPlayerID == pPlayerPool->GetLocalPlayerID())
		pPlayerPool->GetLocalPlayer()->GetPlayerPed()->SetModelIndex(uiSkin);
	else
	{
		if(pPlayerPool->GetSlotState(iPlayerID) && pPlayerPool->GetAt(iPlayerID)->GetPlayerPed())
			pPlayerPool->GetAt(iPlayerID)->GetPlayerPed()->SetModelIndex(uiSkin);
	}
}

void ScrApplyPlayerAnimation(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrApplyPlayerAnimation"));
	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID playerId;
	uint8_t byteAnimLibLen;
	uint8_t byteAnimNameLen;
	char szAnimLib[256];
	char szAnimName[256];
	float fS;
	bool opt1, opt2, opt3, opt4;
	int opt5;
	CPlayerPool *pPlayerPool = nullptr;
	CPlayerPed *pPlayerPed = nullptr;

	memset(szAnimLib, 0, 256);
	memset(szAnimName, 0, 256);

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(playerId);
	bsData.Read(byteAnimLibLen);
	bsData.Read(szAnimLib, byteAnimLibLen);
	bsData.Read(byteAnimNameLen);
	bsData.Read(szAnimName, byteAnimNameLen);
	bsData.Read(fS);
	bsData.Read(opt1);
	bsData.Read(opt2);
	bsData.Read(opt3);
	bsData.Read(opt4);
	bsData.Read(opt5);

	szAnimLib[byteAnimLibLen] = '\0';
	szAnimName[byteAnimNameLen] = '\0';

	pPlayerPool = pNetGame->GetPlayerPool();

	if(pPlayerPool)
	{
		if(pPlayerPool->GetLocalPlayerID() == playerId)
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		else if(pPlayerPool->GetSlotState(playerId))
			pPlayerPed = pPlayerPool->GetAt(playerId)->GetPlayerPed();

		Log("Animation: %s, %s", szAnimLib, szAnimName);


		if(pPlayerPed && (szAnimLib != nullptr && szAnimName != nullptr))
			pPlayerPed->ApplyAnimation(szAnimName, szAnimLib, fS, (int)opt1, (int)opt2, (int)opt3, (int)opt4, (int)opt5);
		else
			Log("Continue Bad Animation");
	}
}

void ScrClearPlayerAnimations(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrClearPlayerAnimations"));

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	PLAYERID playerId;
	bsData.Read(playerId);
	MATRIX4X4 mat;

	CPlayerPool *pPlayerPool=NULL;
	CPlayerPed *pPlayerPed=NULL;

	pPlayerPool = pNetGame->GetPlayerPool();

	if(pPlayerPool) {
		// Get the CPlayerPed for this player
		if(playerId == pPlayerPool->GetLocalPlayerID()) 
		{
			pPlayerPed = pPlayerPool->GetLocalPlayer()->GetPlayerPed();
		}
		else 
		{
			if(pPlayerPool->GetSlotState(playerId))
				pPlayerPed = pPlayerPool->GetAt(playerId)->GetPlayerPed();
		}
		
		if(pPlayerPed) 
		{
			pPlayerPed->GetMatrix(&mat);
			pPlayerPed->TeleportTo(mat.pos.X, mat.pos.Y, mat.pos.Z);
		}
	}
}

void ScrSetSpawnInfo(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetSpawnInfo"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYER_SPAWN_INFO SpawnInfo;

	RakNet::BitStream bsData(Data, (iBitLength/8)+1, false);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	bsData.Read((char*)&SpawnInfo, sizeof(PLAYER_SPAWN_INFO));

	pPlayerPool->GetLocalPlayer()->SetSpawnInfo(&SpawnInfo);
}

void ScrCreateExplosion(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrCreateExplosion"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	float X, Y, Z, Radius;
	int   iType;

	bsData.Read(X);
	bsData.Read(Y);
	bsData.Read(Z);
	bsData.Read(iType);
	bsData.Read(Radius);

	ScriptCommand(&create_explosion_with_radius, X, Y, Z, iType, Radius);
}

void ScrSetPlayerHealth(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetPlayerHealth"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	float fHealth;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(fHealth);
	pLocalPlayer->GetPlayerPed()->SetHealth(fHealth);
}

void ScrSetPlayerArmour(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetPlayerArmour"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	float fHealth;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(fHealth);

	pLocalPlayer->GetPlayerPed()->SetArmour(fHealth);
}

void ScrSetPlayerColor(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetPlayerColor"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	PLAYERID playerId;
	uint32_t dwColor;

	bsData.Read(playerId);
	bsData.Read(dwColor);

	if(playerId == pPlayerPool->GetLocalPlayerID()) 
	{
		pPlayerPool->GetLocalPlayer()->SetPlayerColor(dwColor);
	} 
	else 
	{
		CRemotePlayer *pPlayer = pPlayerPool->GetAt(playerId);
		if(pPlayer)	pPlayer->SetPlayerColor(dwColor);
	}
}

void ScrSetPlayerName(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetPlayerName"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID playerId;
	uint8_t byteNickLen;
	char szNewName[MAX_PLAYER_NAME+1];
	uint8_t byteSuccess;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();

	bsData.Read(playerId);
	bsData.Read(byteNickLen);

	if(byteNickLen > MAX_PLAYER_NAME) return;

	bsData.Read(szNewName, byteNickLen);
	bsData.Read(byteSuccess);

	szNewName[byteNickLen] = '\0';

	Log(OBFUSCATE("byteSuccess = %d"), byteSuccess);
	if (byteSuccess == 1) pPlayerPool->SetPlayerName(playerId, szNewName);
	
	// Extra addition which we need to do if this is the local player;
	if( pPlayerPool->GetLocalPlayerID() == playerId )
		pPlayerPool->SetLocalPlayerName( szNewName );
}

void ScrSetPlayerPosFindZ(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetPlayerPosFindZ"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();

	VECTOR vecPos;

	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);

	vecPos.Z = pGame->FindGroundZForCoord(vecPos.X, vecPos.Y, vecPos.Z);
	vecPos.Z += 1.5f;

	pLocalPlayer->GetPlayerPed()->TeleportTo(vecPos.X, vecPos.Y, vecPos.Z);
}

void ScrSetPlayerInterior(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetPlayerInterior"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;


	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	uint8_t byteInterior;
	bsData.Read(byteInterior);

	pGame->FindPlayerPed()->SetInterior(byteInterior);	
}

void ScrSetMapIcon(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetMapIcon"));
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	uint8_t byteIndex;
	uint8_t byteIcon;
	uint32_t dwColor;
	float fPos[3];
	uint8_t byteStyle;

	bsData.Read(byteIndex);
	bsData.Read(fPos[0]);
	bsData.Read(fPos[1]);
	bsData.Read(fPos[2]);
	bsData.Read(byteIcon);
	bsData.Read(dwColor);
	bsData.Read(byteStyle);

	pNetGame->SetMapIcon(byteIndex, fPos[0], fPos[1], fPos[2], byteIcon, dwColor, byteStyle);
}

void ScrDisableMapIcon(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrDisableMapIcon"));
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	uint8_t byteIndex;

	bsData.Read(byteIndex);

	pNetGame->DisableMapIcon(byteIndex);
}

void ScrSetCameraBehindPlayer(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetCameraBehindPlayer"));

	pGame->GetCamera()->SetBehindPlayer();	
}

void ScrSetPlayerSpecialAction(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetPlayerSpecialAction"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	uint8_t byteSpecialAction;
	bsData.Read(byteSpecialAction);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	if(pPlayerPool) pPlayerPool->GetLocalPlayer()->ApplySpecialAction(byteSpecialAction);
}

void ScrTogglePlayerSpectating(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrTogglePlayerSpectating"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	uint32_t bToggle;
	bsData.Read(bToggle);
	pPlayerPool->GetLocalPlayer()->ToggleSpectating(bToggle);
	Log(OBFUSCATE("toggle: %d"), bToggle);
}

void ScrSetPlayerSpectating(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetPlayerSpectating"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	PLAYERID playerId;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(playerId);
	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	if (pPlayerPool->GetSlotState(playerId))
		pPlayerPool->GetAt(playerId)->SetState(PLAYER_STATE_SPECTATING);
}

#define SPECTATE_TYPE_NORMAL	1
#define SPECTATE_TYPE_FIXED		2
#define SPECTATE_TYPE_SIDE		3

void ScrPlayerSpectatePlayer(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrPlayerSpectatePlayer"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	
	PLAYERID playerId;
    uint8_t byteMode;
	
	bsData.Read(playerId);
	bsData.Read(byteMode);

	switch (byteMode) 
	{
		case SPECTATE_TYPE_FIXED:
			byteMode = 15;
			break;
		case SPECTATE_TYPE_SIDE:
			byteMode = 14;
			break;
		default:
			byteMode = 4;
	}

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	pLocalPlayer->m_byteSpectateMode = byteMode;
	pLocalPlayer->SpectatePlayer(playerId);
}

void ScrPlayerSpectateVehicle(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrPlayerSpectateVehicle"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	VEHICLEID VehicleID;
	uint8_t byteMode;

	bsData.Read(VehicleID);
	bsData.Read(byteMode);

	switch (byteMode) 
	{
		case SPECTATE_TYPE_FIXED:
			byteMode = 15;
			break;
		case SPECTATE_TYPE_SIDE:
			byteMode = 14;
			break;
		default:
			byteMode = 3;
	}

	CLocalPlayer *pLocalPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	pLocalPlayer->m_byteSpectateMode = byteMode;
	pLocalPlayer->SpectateVehicle(VehicleID);
}

void ScrPutPlayerInVehicle(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrPutPlayerInVehicle"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	VEHICLEID vehicleid;
	uint8_t seatid;
	bsData.Read(vehicleid);
	bsData.Read(seatid);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	int iVehicleIndex = pNetGame->GetVehiclePool()->FindGtaIDFromID(vehicleid);
	CVehicle *pVehicle = pNetGame->GetVehiclePool()->GetAt(vehicleid);

	if(iVehicleIndex && pVehicle)
		 pGame->FindPlayerPed()->PutDirectlyInVehicle(iVehicleIndex, seatid);
}

void ScrVehicleParams(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetVehicleParams"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	VEHICLEID VehicleID;
	uint8_t byteObjectiveVehicle;
	uint8_t byteDoorsLocked;

	bsData.Read(VehicleID);
	bsData.Read(byteObjectiveVehicle);
	bsData.Read(byteDoorsLocked);

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	pVehiclePool->AssignSpecialParamsToVehicle(VehicleID,byteObjectiveVehicle,byteDoorsLocked);
}

#pragma pack(1)
typedef struct _VEHICLE_PARAMS_STATUS_EX
{
	uint8_t engine, lights, alarm, doors, bonnet, boot, objective;
	uint8_t unk1;
	uint8_t door_driver;
	uint8_t unk[3];
	uint8_t windor_driver;
} VEHICLE_PARAMS_STATUS_EX;

void ScrVehicleParamsEx(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data, (iBitLength / 8) + 1, false);
	VEHICLE_PARAMS_STATUS_EX vehParams;
	memset(&vehParams, 0, sizeof(vehParams));

	VEHICLEID VehicleId;
	bsData.Read(VehicleId);
	bsData.Read((char*)& vehParams, sizeof(vehParams));

	if (pNetGame && pNetGame->GetVehiclePool())
	{
		if (pNetGame->GetVehiclePool()->GetSlotState(VehicleId))
		{
			CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt(VehicleId);
			if (pVehicle)
			{
				// doors
				pVehicle->SetDoorState(vehParams.doors);
				// engine
				pVehicle->SetEngineState(vehParams.engine);
				// lights
				pVehicle->SetLightsState(vehParams.lights);
				// bonnet and boot
				pVehicle->SetBonnetAndBootStatus((vehParams.bonnet == 0xFF) ? 0 : vehParams.bonnet, (vehParams.boot == 0xFF) ? 0 : vehParams.boot);
				// alarm
				pVehicle->SetSirenState((vehParams.alarm == 0xFF) ? 0 : vehParams.alarm);
			}
		}
	}
}

void ScrHaveSomeMoney(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrHaveSomeMoney"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	int iAmmount;
	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bsData.Read(iAmmount);
	pGame->AddToLocalMoney(iAmmount);
}

void ScrResetMoney(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrResetMoney"));

	pGame->ResetLocalMoney();
}

void ScrLinkVehicle(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrLinkVehicle"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	VEHICLEID VehicleID;
	uint8_t byteInterior;

	bsData.Read(VehicleID);
	bsData.Read(byteInterior);

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	if(pVehiclePool)
			pVehiclePool->LinkToInterior(VehicleID, (int)byteInterior);
}

void ScrRemovePlayerFromVehicle(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrRemovePlayerFromVehicle"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
	pPlayerPool->GetLocalPlayer()->GetPlayerPed()->ExitCurrentVehicle();
}

void ScrSetVehicleHealth(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetVehicleHealth"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	float fHealth;
	VEHICLEID VehicleID;

	bsData.Read(VehicleID);
	bsData.Read(fHealth);

	if(pNetGame->GetVehiclePool()->GetSlotState(VehicleID))
		pNetGame->GetVehiclePool()->GetAt(VehicleID)->SetHealth(fHealth);
}

void ScrSetVehiclePos(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetVehiclePos"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	VEHICLEID VehicleId;
	float fX, fY, fZ;
	bsData.Read(VehicleId);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);

	if(pNetGame && pNetGame->GetVehiclePool())
	{
		if(pNetGame->GetVehiclePool()->GetSlotState(VehicleId))
			pNetGame->GetVehiclePool()->GetAt(VehicleId)->TeleportTo(fX, fY, fZ);
	}
}

void ScrSetVehicleVelocity(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetVehicleVelocity"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	uint8_t turn = false;
	VECTOR vecMoveSpeed;
	bsData.Read(turn);
	bsData.Read(vecMoveSpeed.X);
	bsData.Read(vecMoveSpeed.Y);
	bsData.Read(vecMoveSpeed.Z);
	Log(OBFUSCATE("X: %f, Y: %f, Z: %f"), vecMoveSpeed.X, vecMoveSpeed.Y, vecMoveSpeed.Z);

	CVehiclePool *pVehiclePool = pNetGame->GetVehiclePool();
	CPlayerPed *pPlayerPed = pGame->FindPlayerPed();

	if(pPlayerPed)
	{
		CVehicle *pVehicle = pVehiclePool->GetAt( pVehiclePool->FindIDFromGtaPtr(pPlayerPed->GetGtaVehicle()));
		if(pVehicle)
			pVehicle->SetMoveSpeedVector(vecMoveSpeed);
	}
}

void ScrNumberPlate(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrNumberPlate"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	VEHICLEID VehicleID;
	char len;
	char szNumberPlate[32+1];

	/*bsData.Read(VehicleID);
	bsData.Read(len);
	bsData.Read(szNumberPlate, len);
	szNumberPlate[len] = '\0';*/
}

void ScrInterpolateCamera(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("ScrInterpolateCamera"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	bool bSetPos = true;
	VECTOR vecFrom, vecDest;
	int time;
	uint8_t mode;

	bsData.Read(bSetPos);
	bsData.Read(vecFrom.X);
	bsData.Read(vecFrom.Y);
	bsData.Read(vecFrom.Z);
	bsData.Read(vecDest.X);
	bsData.Read(vecDest.Y);
	bsData.Read(vecDest.Z);
	bsData.Read(time);
	bsData.Read(mode);

	if(mode < 1 || mode > 2)
		mode = 2;

	if(time > 0)
	{
		pNetGame->GetPlayerPool()->GetLocalPlayer()->m_bSpectateProcessed = true;
		if(bSetPos)
		{
			pGame->GetCamera()->InterpolateCameraPos(&vecFrom, &vecDest, time, mode);
		}
		else
			pGame->GetCamera()->InterpolateCameraLookAt(&vecFrom, &vecDest, time, mode);
	}
}

void ScrAddGangZone(RPCParameters *rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool)
	{
		uint16_t wZoneID;
		float minx, miny, maxx, maxy;
		uint32_t dwColor;
		bsData.Read(wZoneID);
		bsData.Read(minx);
		bsData.Read(miny);
		bsData.Read(maxx);
		bsData.Read(maxy);
		bsData.Read(dwColor);
		pGangZonePool->New(wZoneID, minx, miny, maxx, maxy, dwColor);
	}
}

void ScrRemoveGangZone(RPCParameters *rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();

	if (pGangZonePool)
	{
		uint16_t wZoneID;
		bsData.Read(wZoneID);
		pGangZonePool->Delete(wZoneID);
	}
}

void ScrFlashGangZone(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrFlashGangZone"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool)
	{
		uint16_t wZoneID;
		uint32_t dwColor;
		bsData.Read(wZoneID);
		bsData.Read(dwColor);
		pGangZonePool->Flash(wZoneID, dwColor);
	}
}

void ScrStopFlashGangZone(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrStopFlashGangZone"));

	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	CGangZonePool* pGangZonePool = pNetGame->GetGangZonePool();
	if (pGangZonePool)
	{
		uint16_t wZoneID;
		bsData.Read(wZoneID);
		pGangZonePool->StopFlash(wZoneID);
	}
}

int iTotalObjects = 0;

RwTexture* LoadTextureForReTexture(const char* szTexture)
{
	RwTexture* tex;

	std::vector<const char *> badTextures = { OBFUSCATE("none"), "", " " };

	for (auto & badTexture : badTextures)
	{
		if (strncmp(szTexture, badTexture, 5u) == 0)
			return nullptr;
	}

	tex = (RwTexture*)LoadTexture(szTexture);

	// -- Converting texture name to lower register and loading it
	if (!tex)
	{
		std::string str = szTexture;
		std::transform(str.begin(), str.end(), str.begin(), ::tolower);

		tex = (RwTexture*)LoadTexture(str.c_str());
	}

	if (!tex)
		return nullptr;

	return tex;
}

void ScrCreateObject(RPCParameters* rpcParams)
{
    unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
    int iBitLength = rpcParams->numberOfBitsOfData;

    uint16_t wObjectID;
    unsigned long ModelID;
    float fDrawDistance;
    VECTOR vecPos, vecRot;
    uint8_t byteMatType;
    uint8_t bNoCameraCol;
    int16_t attachedVehicleID;
    int16_t attachedObjectID;
    VECTOR vecAttachedOffset;
    VECTOR vecAttachedRotation;
    uint8_t bSyncRot;
    uint8_t id;

    RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
    bsData.Read(wObjectID);
    bsData.Read(ModelID);

    // Log(OBFUSCATE("RPC_SCRCREATEOBJECT(%d) %d"), iTotalObjects, ModelID);
	if(wObjectID < 0 || wObjectID >= MAX_OBJECTS)
		return;
	
    bsData.Read(vecPos.X);
    bsData.Read(vecPos.Y);
    bsData.Read(vecPos.Z);
	
    bsData.Read(vecRot.X);
    bsData.Read(vecRot.Y);
    bsData.Read(vecRot.Z);

    bsData.Read(fDrawDistance);

    bsData.Read(bNoCameraCol);
    bsData.Read(attachedVehicleID);
    bsData.Read(attachedObjectID);
    if (attachedObjectID != INVALID_OBJECT_ID || attachedVehicleID != INVALID_VEHICLE_ID)
    {
        bsData.Read(vecAttachedOffset);
        bsData.Read(vecAttachedRotation);
        bsData.Read(bSyncRot);
    }
	uint8_t byteMaterialsCount = 0;
	bsData.Read(byteMaterialsCount);

	// Log("id: %d model: %d x: %f y: %f z: %f", wObjectID, ModelID, vecPos.X, vecPos.Y, vecPos.Z);

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	// Log("trying loading %d", ModelID);
	
	if (ModelID == INVALID_OBJECT_ID) {
		Log("Invalid ModelID: %d. Using fallback model.", ModelID);
		pObjectPool->New(wObjectID, 18631, vecPos, vecRot, fDrawDistance);
	} else {
		pObjectPool->New(wObjectID, ModelID, vecPos, vecRot, fDrawDistance);
	}

    CObject *pObject = pNetGame->GetObjectPool()->GetAt(wObjectID);
    if (pObject && attachedVehicleID != INVALID_VEHICLE_ID)
    {
        pObject->AttachToVehicle(attachedVehicleID, &vecAttachedOffset, &vecAttachedRotation);
    }
	if(pObject)
	{
		// read obj.
		if(byteMaterialsCount > 0)
		{
			char txdname[256];
			char texturename[256];
			uint8_t byteType;
			uint8_t byteMaterialIndex;

			// Material
			uint16_t wMaterialModel;
			uint8_t byteLength;
			uint32_t dwColor;

			// Material Text
			uint8_t byteMaterialSize;
			uint8_t byteFontNameLength;
			char szFontName[32];
			uint8_t byteFontSize;
			uint8_t byteFontBold;
			uint32_t dwFontColor;
			uint32_t dwBackgroundColor;
			uint8_t byteAlign;
			char szText[2048];

			for(int i=0; i<byteMaterialsCount; i++)
			{
				// Material
				memset(txdname, 0, sizeof(txdname));
				memset(texturename, 0, sizeof(texturename));

				// Material Text
				memset(szFontName, 0, sizeof(szFontName));
				memset(szText, 0, sizeof(szText));

				bsData.Read(byteType);

				if(byteType == 1)
				{
					bsData.Read(byteMaterialIndex);
					bsData.Read(wMaterialModel);
					bsData.Read(byteLength);
					bsData.Read(txdname, byteLength);
					txdname[byteLength] = 0;
					bsData.Read(byteLength);
					bsData.Read(texturename, byteLength);
					texturename[byteLength] = 0;
					bsData.Read(dwColor);

					if(strlen(txdname) < 32 && strlen(texturename) < 32)
					{
						if(pObject)
						{
							pObject->SetMaterial(wMaterialModel, byteMaterialIndex, txdname, texturename, dwColor);
						}
					}
				}
				else if(byteType == 2)
				{
					bsData.Read(byteMaterialIndex);
					bsData.Read(byteMaterialSize);
					bsData.Read(byteFontNameLength);
					bsData.Read(szFontName, byteFontNameLength);
					szFontName[byteFontNameLength] = '\0';
					bsData.Read(byteFontSize);
					bsData.Read(byteFontBold);
					bsData.Read(dwFontColor);
					bsData.Read(dwBackgroundColor);
					bsData.Read(byteAlign);
					stringCompressor->DecodeString(szText, 2048, &bsData);

					if(strlen(szFontName) <= 32)
					{
						if(pObject)
						{
							pObject->SetMaterialText(byteMaterialIndex, byteMaterialSize, szFontName, byteFontSize, byteFontBold, dwFontColor, dwBackgroundColor, byteAlign, szText);
						}
					}
				}
			}
		}
  	}
	iTotalObjects++;
}

void ScrDestroyObject(RPCParameters *rpcParams)
{

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint16_t wObjectID;
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	bsData.Read(wObjectID);

	//LOGI("id: %d", wObjectID);
	iTotalObjects--;

	CObjectPool* pObjectPool =	pNetGame->GetObjectPool();
	if(pObjectPool->GetAt(wObjectID))
		pObjectPool->Delete(wObjectID);
}

void ScrSetObjectPos(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC_SCRSETOBJECTPOS"));

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	uint16_t wObjectID;
	float fRotation;
	VECTOR vecPos, vecRot;
	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	bsData.Read(wObjectID);
	bsData.Read(vecPos.X);
	bsData.Read(vecPos.Y);
	bsData.Read(vecPos.Z);
	bsData.Read(fRotation);

	//LOGI("id: %d x: %.2f y: %.2f z: %.2f", wObjectID, vecPos.X, vecPos.Y, vecPos.Z);
	//LOGI("VecRot x: %.2f y: %.2f z: %.2f", vecRot.X, vecRot.Y, vecRot.Z);

	CObjectPool*	pObjectPool =	pNetGame->GetObjectPool();
	CObject*		pObject		=	pObjectPool->GetAt(wObjectID);
	if(pObject)
	{
		pObject->SetPos(vecPos.X, vecPos.Y, vecPos.Z);
	}
}

void ScrAttachObjectToPlayer(RPCParameters* rpcParams)
{
	Log(OBFUSCATE("RPC_SCRATTACHOBJECTTOPLAYER"));

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	PLAYERID wObjectID, wPlayerID;
	float OffsetX, OffsetY, OffsetZ, rX, rY, rZ;

	bsData.Read(wObjectID);
	bsData.Read(wPlayerID);

	bsData.Read(OffsetX);
	bsData.Read(OffsetY);
	bsData.Read(OffsetZ);

	bsData.Read(rX);
	bsData.Read(rY);
	bsData.Read(rZ);

	CObject* pObject = pNetGame->GetObjectPool()->GetAt(wObjectID);
	if (!pObject) return;

	if (wPlayerID == pNetGame->GetPlayerPool()->GetLocalPlayerID())
	{
		CLocalPlayer* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
		ScriptCommand(&attach_object_to_actor, pObject->m_dwGTAId, pPlayer->GetPlayerPed()->m_dwGTAId,
			OffsetX, OffsetY, OffsetZ, rX, rY, rZ);
	}
	else {
		CRemotePlayer* pPlayer = pNetGame->GetPlayerPool()->GetAt(wPlayerID);

		if (!pPlayer)
			return;

		ScriptCommand(&attach_object_to_actor, pObject->m_dwGTAId, pPlayer->GetPlayerPed()->m_dwGTAId,
			OffsetX, OffsetY, OffsetZ, rX, rY, rZ);
	}
}


void ScrPlaySound(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrPlaySound"));

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);

	int iSound;
	float fX, fY, fZ;
	bsData.Read(iSound);
	bsData.Read(fX);
	bsData.Read(fY);
	bsData.Read(fZ);
	pGame->PlaySound(iSound, fX, fY, fZ);
}

void ScrSetPlayerWantedLevel(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetPlayerWantedLevel"));

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData((unsigned char*)Data,(iBitLength/8)+1,false);
	
	if(!pGame) return;

	uint8_t byteLevel;
	bsData.Read(byteLevel);
	pGame->SetWantedLevel(byteLevel);
}

void ScrGivePlayerWeapon(RPCParameters* rpcParams)
{

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	PLAYERID wPlayerID;
	int iWeaponID;
	int iAmmo;
	bsData.Read(iWeaponID);
	bsData.Read(iAmmo);

	CLocalPlayer* pPlayer = pNetGame->GetPlayerPool()->GetLocalPlayer();
	pPlayer->GetPlayerPed()->GiveWeapon(iWeaponID, iAmmo);
}

void ScrTogglePlayerControllable(RPCParameters *rpcParams)
{
	Log(OBFUSCATE("RPC: TogglePlayerControllable"));

	unsigned char * Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data,(iBitLength/8)+1,false);
	uint8_t byteControllable;
	bsData.Read(byteControllable);
	//Log("controllable = %d", byteControllable);
	pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->LockControllable(!byteControllable);
	pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable((int)byteControllable);
}

void ScrSetPlayerSkillLevel(RPCParameters *rpcParams)
{
	int iBitLength = rpcParams->numberOfBitsOfData;

	CPlayerPool * pPlayerPool = pNetGame->GetPlayerPool();
	if (!pPlayerPool) return;

	RakNet::BitStream bsData(rpcParams->input, (iBitLength / 8) + 1, false);

	PLAYERID bytePlayerID;
	unsigned int ucSkillType;
	unsigned short uiSkillLevel;

	bsData.Read(bytePlayerID);
	bsData.Read(ucSkillType);
	bsData.Read(uiSkillLevel);

	if (ucSkillType < 0 || ucSkillType > 10) return;
	if (uiSkillLevel < 0 || uiSkillLevel > 1000) return;

	switch (ucSkillType)
	{
	case 0:
		ucSkillType = WEAPONTYPE_PISTOL_SKILL;
		break;
	case 1:
		ucSkillType = WEAPONTYPE_PISTOL_SILENCED_SKILL;
		break;
	case 2:
		ucSkillType = WEAPONTYPE_DESERT_EAGLE_SKILL;
		break;
	case 3:
		ucSkillType = WEAPONTYPE_SHOTGUN_SKILL;
		break;
	case 4:
		ucSkillType = WEAPONTYPE_SAWNOFF_SHOTGUN_SKILL;
		break;
	case 5:
		ucSkillType = WEAPONTYPE_SPAS12_SHOTGUN_SKILL;
		break;
	case 6:
		ucSkillType = WEAPONTYPE_MICRO_UZI_SKILL;
		break;
	case 7:
		ucSkillType = WEAPONTYPE_MP5_SKILL;
		break;
	case 8:
		ucSkillType = WEAPONTYPE_AK47_SKILL;
		break;
	case 9:
		ucSkillType = WEAPONTYPE_M4_SKILL;
		break;
	case 10:
		ucSkillType = WEAPONTYPE_SNIPERRIFLE_SKILL;
		break;
	default:
		return;
	}

	if (bytePlayerID == pPlayerPool->GetLocalPlayerID())
	{
		float* StatsTypesFloat = (float*)(SA_ADDR(0x8C41A0));
		StatsTypesFloat[ucSkillType] = uiSkillLevel;
	}
	else
	{
		CRemotePlayer* pRemotePlayer = pPlayerPool->GetAt(bytePlayerID);
		if (!pRemotePlayer) return;

		CPlayerPed* pPlayerPed = pRemotePlayer->GetPlayerPed();
		if (!pPlayerPed) return;

		pPlayerPed->SetFloatStat(ucSkillType, uiSkillLevel);
	}
}

void ScrResetPlayerWeapons(RPCParameters* rpcParams)
{
	uint8_t* Data = reinterpret_cast<uint8_t*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	CPlayerPed* pPlayerPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
	pPlayerPed->ClearAllWeapons();
}

void ScrShowTextDraw(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
	if (pTextDrawPool)
	{
		uint16_t wTextID;
		uint16_t wTextSize;
		TEXT_DRAW_TRANSMIT TextDrawTransmit;
		char cText[MAX_TEXT_DRAW_LINE];

		bsData.Read(wTextID);
		bsData.Read((char*)& TextDrawTransmit, sizeof(TEXT_DRAW_TRANSMIT));
		bsData.Read(wTextSize);
		bsData.Read(cText, wTextSize);
		cText[wTextSize] = 0;
		pTextDrawPool->New(wTextID, &TextDrawTransmit, cText);
	}
}

void ScrHideTextDraw(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	CTextDrawPool* pTextDrawPool = pNetGame->GetTextDrawPool();
	if (pTextDrawPool)
	{
		uint16_t wTextID;
		bsData.Read(wTextID);
		pTextDrawPool->Delete(wTextID);
	}
}

void ScrEditTextDraw(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	uint16_t wTextID;
	uint16_t wLen;
	bsData.Read(wTextID);
	bsData.Read(wLen);
	uint8_t pStr[256];
	if (wLen >= 255) return;

	bsData.Read((char*)pStr, wLen);
	pStr[wLen] = 0;
	CTextDraw* pTextDraw = pNetGame->GetTextDrawPool()->GetAt(wTextID);
	if (pTextDraw)
	{
		pTextDraw->SetText((const char*)pStr);
	}
}

#define ATTACH_BONE_SPINE	1
#define ATTACH_BONE_HEAD	2
#define ATTACH_BONE_LUPPER	3
#define ATTACH_BONE_RUPPER	4
#define ATTACH_BONE_LHAND	5
#define ATTACH_BONE_RHAND	6
#define ATTACH_BONE_LTHIGH	7
#define ATTACH_BONE_RTHIGH	8
#define ATTACH_BONE_LFOOT	9
#define ATTACH_BONE_RFOOT	10
#define ATTACH_BONE_RCALF	11
#define ATTACH_BONE_LCALF	12
#define ATTACH_BONE_LFARM	13
#define ATTACH_BONE_RFARM	14
#define ATTACH_BONE_LSHOULDER	15
#define ATTACH_BONE_RSHOULDER	16
#define ATTACH_BONE_NECK	17
#define ATTACH_BONE_JAW		18

int GetInternalBoneIDFromSampID(int sampid)
{
	switch (sampid)
	{
	case ATTACH_BONE_SPINE: // 3 or 2
		return 3;
	case ATTACH_BONE_HEAD: // ?
		return 5;
	case ATTACH_BONE_LUPPER: // left upper arm
		return 22;
	case ATTACH_BONE_RUPPER: // right upper arm
		return 32;
	case ATTACH_BONE_LHAND: // left hand
		return 34;
	case ATTACH_BONE_RHAND: // right hand
		return 24;
	case ATTACH_BONE_LTHIGH: // left thigh
		return 41;
	case ATTACH_BONE_RTHIGH: // right thigh
		return 51;
	case ATTACH_BONE_LFOOT: // left foot
		return 43;
	case ATTACH_BONE_RFOOT: // right foot
		return 53;
	case ATTACH_BONE_RCALF: // right calf
		return 52;
	case ATTACH_BONE_LCALF: // left calf
		return 42;
	case ATTACH_BONE_LFARM: // left forearm
		return 33;
	case ATTACH_BONE_RFARM: // right forearm
		return 23;
	case ATTACH_BONE_LSHOULDER: // left shoulder (claviacle)
		return 31;
	case ATTACH_BONE_RSHOULDER: // right shoulder (claviacle)
		return 21;
	case ATTACH_BONE_NECK: // neck
		return 4;
	case ATTACH_BONE_JAW: // jaw ???
		return 8; // i dont know
	}
	return 0;
}

void ScrSetPlayerAttachedObject(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	PLAYERID id;
	uint32_t slot;
	bool create;
	ATTACHED_OBJECT_INFO info;

	bsData.Read(id);
	bsData.Read(slot);
	bsData.Read(create);
	CPlayerPed* pPed = nullptr;
	if (id == pNetGame->GetPlayerPool()->GetLocalPlayerID())
	{
		pPed = pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed();
	}
	else
	{
		if (pNetGame->GetPlayerPool()->GetSlotState(id))
		{
			pPed = pNetGame->GetPlayerPool()->GetAt(id)->GetPlayerPed();
		}
	}
	if (!pPed) return;
	if (!create)
	{
		pPed->DeattachObject(slot);
		return;
	}
	bsData.Read((char*)& info, sizeof(ATTACHED_OBJECT_INFO));
	info.dwBone = GetInternalBoneIDFromSampID(info.dwBone);
	pPed->AttachObject(&info, slot);
}


void ScrSetPlayerObjectMaterial(RPCParameters* rpcParams)
{
	Log("Set Material Object");
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	CObjectPool* pObjectPool = pNetGame->GetObjectPool();
	uint16_t ObjectID;
	uint8_t byteMaterialType;
	uint8_t byteMaterialIndex;

	// Material
	uint16_t wModelID;
	uint8_t byteLength;
	char txdname[256], texname[256];
	uint32_t dwColor;

	// Material Text
	uint8_t byteMaterialSize;
	uint8_t byteFontNameLength;
	char szFontName[32];
	uint8_t byteFontSize;
	uint8_t byteFontBold;
	uint32_t dwFontColor;
	uint32_t dwBackgroundColor;
	uint8_t byteAlign;
	char szText[2048];

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(ObjectID);

	CObject* pObject = pObjectPool->GetAt(ObjectID);

	bsData.Read(byteMaterialType);
	if (byteMaterialType == 1)
	{
		bsData.Read(byteMaterialIndex);
		bsData.Read(wModelID);
		bsData.Read(byteLength);
		bsData.Read(txdname, byteLength);
		txdname[byteLength] = '\0';
		bsData.Read(byteLength);
		bsData.Read(texname, byteLength);
		texname[byteLength] = '\0';
		bsData.Read(dwColor);
		if (strlen(txdname) < 32 && strlen(texname) < 32)
		{
			if (pObject)
			{
				pObject->SetMaterial(wModelID, byteMaterialIndex, txdname, texname, dwColor);
			}
		}
	}
	else if (byteMaterialType == 2)
	{
		bsData.Read(byteMaterialIndex);
		bsData.Read(byteMaterialSize);
		bsData.Read(byteFontNameLength);
		bsData.Read(szFontName, byteFontNameLength);
		szFontName[byteFontNameLength] = '\0';
		bsData.Read(byteFontSize);
		bsData.Read(byteFontBold);
		bsData.Read(dwFontColor);
		bsData.Read(dwBackgroundColor);
		bsData.Read(byteAlign);
		stringCompressor->DecodeString(szText, 2048, &bsData);

		if(strlen(szFontName) <= 32)
		{
			if (pObject)
			{
				pObject->SetMaterialText(byteMaterialIndex, byteMaterialSize, szFontName, byteFontSize, byteFontBold, dwFontColor, dwBackgroundColor, byteAlign, szText);
			}
		}
	}
	
}

void ScrSetVehicleZAngle(RPCParameters* rpcParams)
{
	Log(OBFUSCATE("RPC: ScrSetVehicleZAngle"));

	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	VEHICLEID VehicleId;
	float fZAngle;
	bsData.Read(VehicleId);
	bsData.Read(fZAngle);
	CVehicle* pVeh = pNetGame->GetVehiclePool()->GetAt(VehicleId);
	if (!pVeh) return;
	if (GamePool_Vehicle_GetAt(pVeh->m_dwGTAId))
	{
		ScriptCommand(&set_car_z_angle, pVeh->m_dwGTAId, fZAngle);
	}
}

void ScrAttachTrailerToVehicle(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	VEHICLEID TrailerID, VehicleID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(TrailerID);
	bsData.Read(VehicleID);
	CVehicle* pTrailer = pNetGame->GetVehiclePool()->GetAt(TrailerID);
	CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt(VehicleID);
	if (!pVehicle) return;
	if (!pTrailer) return;
	pVehicle->SetTrailer(pTrailer);
	pVehicle->AttachTrailer();
}

//----------------------------------------------------

void ScrDetachTrailerFromVehicle(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	VEHICLEID VehicleID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(VehicleID);
	CVehicle* pVehicle = pNetGame->GetVehiclePool()->GetAt(VehicleID);
	if (!pVehicle) return;
	pVehicle->DetachTrailer();
	pVehicle->SetTrailer(NULL);
}

void ScrRemoveComponent(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	
	VEHICLEID vehicleId;
	uint16_t component;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(vehicleId);
	bsData.Read(component);

	if (pNetGame->GetVehiclePool())
	{
		if (pNetGame->GetVehiclePool()->GetAt(vehicleId))
		{
			pNetGame->GetVehiclePool()->GetAt(vehicleId)->RemoveComponent(component);
		}
	}
}

void ScrMoveObject(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	unsigned short byteObjectID;
	float curx, cury, curz, newx, newy, newz, speed, rotx, roty, rotz;

	bsData.Read(byteObjectID);
	bsData.Read(curx);
	bsData.Read(cury);
	bsData.Read(curz);
	bsData.Read(newx);
	bsData.Read(newy);
	bsData.Read(newz);
	bsData.Read(speed);
	bsData.Read(rotx);
	bsData.Read(roty);
	bsData.Read(rotz);

	CObject* pObject = pNetGame->GetObjectPool()->GetAt(byteObjectID);
	if (pObject)
	{
		pObject->MoveTo(newx, newy, newz, speed, rotx, roty, rotz);
	}
}

void ScrStopObject(RPCParameters *rpcParams) 
{
	Log("RPC: StopObject");
	unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	RakNet::BitStream bsData((unsigned char*)Data, (iBitLength / 8) + 1, false);
	
	int16_t ObjectID;
	bsData.Read(ObjectID);
	
	CObject* pObject = pNetGame->GetObjectPool()->GetAt(ObjectID);
	if(pObject) pObject->StopMoving();
}

void ScrSetObjectRotation(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);

	uint16_t objectId;
	VECTOR vecRot;
	
	bsData.Read(objectId);
	bsData.Read((char*)&vecRot, sizeof(VECTOR));

	if (pNetGame->GetObjectPool()->GetAt(objectId))
	{
		pNetGame->GetObjectPool()->GetAt(objectId)->InstantRotate(vecRot.X, vecRot.Y, vecRot.Z);
	}
}

void ScrSelectTextDraw(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;

	bool bEnable = false;
	uint32_t dwColor = 0;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(bEnable);
	bsData.Read(dwColor);

	pNetGame->GetTextDrawPool()->SetSelectState(bEnable ? true : false, dwColor);
}

void ScrPlayAudioStream(RPCParameters* rpcParams)
{
    Log("RPC: PlayAudioStreamForPlayer");

    RakNet::BitStream bsData(rpcParams->input, (rpcParams->numberOfBitsOfData / 8) + 1, false);

    uint8_t byteTextLen;
    char szURL[1024];

    float X, Y, Z;
    float fRadius;

    bool bUsePos;

    bsData.Read(byteTextLen);
    bsData.Read(szURL, byteTextLen);

    bsData.Read(X);
    bsData.Read(Y);
    bsData.Read(Z);

    bsData.Read(fRadius);

    bsData.Read(bUsePos);

    szURL[byteTextLen] = '\0';

    if(pAudioStream) pAudioStream->Play(szURL, X, Y, Z, fRadius, bUsePos);

    // if(pChatWindow) {
    //     pChatWindow->AddInfoMessage("{FFFFFF}Audio Stream: %s", szURL);
    // }
}

void ScrStopAudioStream(RPCParameters* rpcParams)
{
    Log("RPC: StopAudioStreamForPlayer");

    if(pAudioStream) pAudioStream->Stop(true);
}

void ScrSetPlayerArmed(RPCParameters* rpcParams)
{
	unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
	int iBitLength = rpcParams->numberOfBitsOfData;
	PlayerID sender = rpcParams->sender;

	uint32_t weaponID;
	RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
	bsData.Read(weaponID);

	pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->SetArmedWeapon(weaponID, 0);
}
void ScrDeathMessage(RPCParameters* rpcParams)
{
    Log(OBFUSCATE("RPC: DeathMessage"));
    unsigned char* Data = reinterpret_cast<unsigned char *>(rpcParams->input);
    int iBitLength = rpcParams->numberOfBitsOfData;
    RakNet::BitStream bsData((unsigned char*)Data, (iBitLength / 8) + 1, false);

    PLAYERID playerId, killerId;
    uint8_t reason;

    bsData.Read(killerId);
    bsData.Read(playerId);
    bsData.Read(reason);

    std::string killername, playername;
    uint32_t killercolor, playercolor;
    killername.clear();
    playername.clear();

    CPlayerPool* pPlayerPool = pNetGame->GetPlayerPool();
    if(pPlayerPool)
    {
        if(pPlayerPool->GetLocalPlayerID() == playerId)
        {
            playername = pPlayerPool->GetLocalPlayerName();
            playercolor = pPlayerPool->GetLocalPlayer()->GetPlayerColorAsARGB();
        }
        else
        {
            if(pPlayerPool->GetSlotState(playerId))
            {
                playername = pPlayerPool->GetPlayerName(playerId);
                playercolor = pPlayerPool->GetAt(playerId)->GetPlayerColorAsARGB();
            }
			else
			{
				playername = "  ";
				
				if(pPlayerPool->GetLocalPlayerID() != killerId && !pPlayerPool->GetSlotState(killerId))
				{
					killername = "  ";
					reason = 255;
				}
			}
        }

        if(pPlayerPool->GetLocalPlayerID() == killerId)
        {
            killername = pPlayerPool->GetLocalPlayerName();
            killercolor = pPlayerPool->GetLocalPlayer()->GetPlayerColorAsARGB();
        }
        else
        {
            if(pPlayerPool->GetSlotState(killerId))
            {
                killername = pPlayerPool->GetPlayerName(killerId);
                killercolor = pPlayerPool->GetAt(killerId)->GetPlayerColorAsARGB();
            }
			else
			{
				killername = "  ";
			}
        }
    }

    pKillList->MakeRecord(playername.c_str(), playercolor, killername.c_str(), killercolor, reason);
}


void ScrSetObjectMaterial(RPCParameters* rpcParams)
{
	Log("RPC: ScrSetObjectMaterial");
    unsigned char* Data = reinterpret_cast<unsigned char*>(rpcParams->input);
    int iBitLength = rpcParams->numberOfBitsOfData;

    CObjectPool* pObjectPool = pNetGame->GetObjectPool();

    uint16_t ObjectID = INVALID_OBJECT_ID;
    uint8_t byteMaterialType = 0;
    uint8_t byteMaterialIndex = 0;
    uint16_t wModelID = 0;
    uint8_t byteLength = 0;
    char txdname[256] = { 0 }, texname[256] = { 0 };
    uint32_t dwColor = 0;

    uint8_t byteMaterialSize;
    uint8_t byteFontNameLength;
    char szFontName[32];
    uint8_t byteFontSize;
    uint8_t byteFontBold;
    uint32_t dwFontColor;
    uint32_t dwBackgroundColor;
    uint8_t byteAlign;
    char szText[2048];

    RakNet::BitStream bsData(Data, (iBitLength / 8) + 1, false);
    bsData.Read(ObjectID);

    if(ObjectID < 0 || ObjectID >= MAX_OBJECTS) {
        return;
    }

    if(!pObjectPool->GetSlotState(ObjectID)) {
        return;
    }

    CObject* pObject = pObjectPool->GetAt(ObjectID);

    bsData.Read(byteMaterialType);

    if (byteMaterialType == 1)
    {
        bsData.Read(byteMaterialIndex);
        bsData.Read(wModelID);
        bsData.Read(byteLength);
        bsData.Read(txdname, byteLength);
        txdname[byteLength] = '\0';
        bsData.Read(byteLength);
        bsData.Read(texname, byteLength);
        texname[byteLength] = '\0';
        bsData.Read(dwColor);
        if (strlen(txdname) < 32 && strlen(texname) < 32)
        {
            if (pObject)
                pObject->SetMaterial(wModelID, byteMaterialIndex, txdname, texname, dwColor);
        }
    }
    else if (byteMaterialType == 2)
    {
        bsData.Read(byteMaterialIndex);
        bsData.Read(byteMaterialSize);
        bsData.Read(byteFontNameLength);
        bsData.Read(szFontName, byteFontNameLength);
        szFontName[byteFontNameLength] = '\0';
        bsData.Read(byteFontSize);
        bsData.Read(byteFontBold);
        bsData.Read(dwFontColor);
        bsData.Read(dwBackgroundColor);
        bsData.Read(byteAlign);
        stringCompressor->DecodeString(szText, 2048, &bsData);

        if(strlen(szFontName) <= 32)
        {
            if (pObject)
            {
                pObject->SetMaterialText(byteMaterialIndex, byteMaterialSize, szFontName, byteFontSize, byteFontBold, dwFontColor, dwBackgroundColor, byteAlign, szText);
            }
        }
    }
}

void RegisterScriptRPCs(RakClientInterface* pRakClient)
{
	Log(OBFUSCATE("Registering ScriptRPC's.."));

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectMaterial, ScrSetObjectMaterial);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText, ScrDisplayGameText);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetGravity, ScrSetGravity);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrForceSpawnSelection,ScrForceSpawnSelection);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos, ScrSetPlayerPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos, ScrSetCameraPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt, ScrSetCameraLookAt);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle, ScrSetPlayerFacingAngle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetFightingStyle, ScrSetFightingStyle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin, ScrSetPlayerSkin);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrApplyPlayerAnimation, ScrApplyPlayerAnimation);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrClearPlayerAnimations, ScrClearPlayerAnimations);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo, ScrSetSpawnInfo);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrCreateExplosion, ScrCreateExplosion);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth, ScrSetPlayerHealth);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour, ScrSetPlayerArmour);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor, ScrSetPlayerColor);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName, ScrSetPlayerName);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPosFindZ, ScrSetPlayerPosFindZ);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetInterior, ScrSetPlayerInterior);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetMapIcon, ScrSetMapIcon);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDisableMapIcon, ScrDisableMapIcon);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetCameraBehindPlayer, ScrSetCameraBehindPlayer);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetSpecialAction, ScrSetPlayerSpecialAction);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating, ScrTogglePlayerSpectating);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSpectating, ScrSetPlayerSpectating);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectatePlayer, ScrPlayerSpectatePlayer);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectateVehicle, ScrPlayerSpectateVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle, ScrPutPlayerInVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParams, ScrVehicleParams);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrVehicleParamsEx, ScrVehicleParamsEx);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney, ScrHaveSomeMoney);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrResetMoney, ScrResetMoney);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle, ScrLinkVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle, ScrRemovePlayerFromVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth, ScrSetVehicleHealth);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos, ScrSetVehiclePos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleVelocity, ScrSetVehicleVelocity);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrNumberPlate, ScrNumberPlate);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrInterpolateCamera, ScrInterpolateCamera);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrAddGangZone,ScrAddGangZone);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone,ScrRemoveGangZone);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrFlashGangZone,ScrFlashGangZone);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone,ScrStopFlashGangZone);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrCreateObject, ScrCreateObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectPos, ScrSetObjectPos);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDestroyObject, ScrDestroyObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrPlaySound, ScrPlaySound);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerWantedLevel, ScrSetPlayerWantedLevel);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon, ScrGivePlayerWeapon);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerControllable, ScrTogglePlayerControllable);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrAttachObjectToPlayer, ScrAttachObjectToPlayer);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons, ScrResetPlayerWeapons);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkillLevel, ScrSetPlayerSkillLevel);
	//Kill list
    pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDeathMessage, ScrDeathMessage);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw, ScrShowTextDraw);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw, ScrHideTextDraw);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrEditTextDraw, ScrEditTextDraw);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerAttachedObject, ScrSetPlayerAttachedObject);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetPlayerObjectMaterial, ScrSetPlayerObjectMaterial);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetVehicleZAngle, ScrSetVehicleZAngle);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrAttachTrailerToVehicle, ScrAttachTrailerToVehicle);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrDetachTrailerFromVehicle, ScrDetachTrailerFromVehicle);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrRemoveComponent, ScrRemoveComponent);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ClickTextDraw, ScrSelectTextDraw);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrStopObject, ScrStopObject);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrSetObjectRotation, ScrSetObjectRotation);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_PlayAudioStream, ScrPlayAudioStream);
	pRakClient->RegisterAsRemoteProcedureCall(&RPC_StopAudioStream, ScrStopAudioStream);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_SetArmedWeapon, ScrSetPlayerArmed);

	pRakClient->RegisterAsRemoteProcedureCall(&RPC_ScrMoveObject, ScrMoveObject);
}

void UnRegisterScriptRPCs(RakClientInterface* pRakClient)
{
	Log(OBFUSCATE("Unregistering ScriptRPC's.."));

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ClickTextDraw);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrAttachTrailerToVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDetachTrailerFromVehicle);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetVehicleZAngle);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerObjectMaterial);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerAttachedObject);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrEditTextDraw);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrShowTextDraw);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrHideTextDraw);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkillLevel);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrResetPlayerWeapons);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrGivePlayerWeapon);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDisplayGameText);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetGravity);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrForceSpawnSelection);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPos);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetCameraPos);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetCameraLookAt);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerFacingAngle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetFightingStyle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSkin);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrApplyPlayerAnimation);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrClearPlayerAnimations);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetSpawnInfo);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrCreateExplosion);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerHealth);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerArmour);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerColor);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerName);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerPosFindZ);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetInterior);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetMapIcon);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDisableMapIcon);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetCameraBehindPlayer);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetSpecialAction);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrTogglePlayerSpectating);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerSpectating);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectatePlayer);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrPlayerSpectateVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrPutPlayerInVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrVehicleParams);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrVehicleParamsEx);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrHaveSomeMoney);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrResetMoney);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrLinkVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrRemovePlayerFromVehicle);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetVehicleHealth);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetVehiclePos);
	//KillList
    pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDeathMessage);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetVehicleVelocity);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrNumberPlate);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrInterpolateCamera);
	
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrAddGangZone);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrRemoveGangZone);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrFlashGangZone);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrStopFlashGangZone);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrCreateObject);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrDestroyObject);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetObjectPos);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrPlaySound);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetPlayerWantedLevel);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrRemoveComponent);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrStopObject);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrSetObjectRotation);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_PlayAudioStream);
	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_StopAudioStream);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_SetArmedWeapon);

	pRakClient->UnregisterAsRemoteProcedureCall(&RPC_ScrMoveObject);
}