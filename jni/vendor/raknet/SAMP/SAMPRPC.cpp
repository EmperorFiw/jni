// https://github.com/P3ti/RakSAMP/blob/master/raknet/SAMP/SAMPRPC.cpp

/*
	Updated to 0.3.7 by P3ti
*/

int RPC_ServerJoin = 137;
int RPC_ServerQuit = 138;
int RPC_InitGame = 139;
int RPC_ClientJoin = 25;
int RPC_NPCJoin = 54;
int RPC_Death = 53;
int RPC_RequestClass = 128;
int RPC_RequestSpawn = 129;
int RPC_SetInteriorId = 118;
int RPC_Spawn = 52;
int RPC_Chat = 101;
int RPC_EnterVehicle = 26;
int RPC_ExitVehicle = 154;
int RPC_VehicleDamage = 106;
int RPC_MenuSelect = 132;
int RPC_MenuQuit = 140;
int RPC_ScmEvent = 96;
int RPC_AdminMapTeleport = 255;
int RPC_WorldPlayerAdd = 32;
int RPC_WorldPlayerDeath = 166;
int RPC_WorldPlayerRemove = 163;
int RPC_WorldVehicleAdd = 164;
int RPC_WorldVehicleRemove = 165;
int RPC_SetCheckpoint = 107;
int RPC_DisableCheckpoint = 37;
int RPC_SetRaceCheckpoint = 38;
int RPC_DisableRaceCheckpoint = 39;
int RPC_UpdateScoresPingsIPs = 155;
int RPC_SvrStats = 102;
int RPC_GameModeRestart = 40;
int RPC_ConnectionRejected = 130;
int RPC_ClientMessage = 93;
int RPC_WorldTime = 94;
int RPC_Pickup = 95;
int RPC_DestroyPickup = 63;
int RPC_DestroyWeaponPickup = 97;
int RPC_Weather = 152;
int RPC_SetTimeEx = 29;//255;
int RPC_ToggleClock = 30;
int RPC_ServerCommand = 50;
int RPC_PickedUpPickup = 131;
int RPC_PickedUpWeapon = 255;
int RPC_VehicleDestroyed = 136;
int RPC_DialogResponse = 62;
int RPC_PlayAudioStream = 41;
int RPC_StopAudioStream = 42;
int RPC_ClickPlayer = 23;
int RPC_PlayerUpdate = 60;
int RPC_ClickTextDraw = 83;
int RPC_MapMarker = 119;
int RPC_PlayerGiveTakeDamage = 115;
int RPC_EnterEditObject = 27;
int RPC_EditObject = 117;
int RPC_RemoveBuilding = 43;
int RPC_ShowActor = 171;
int RPC_HideActor = 172;
int RPC_SetActorHealth = 178;
int RPC_SetActorPos = 176;
int RPC_SetActorFacingAngle = 175;
int RPC_SetActorAnimation = 173;
int RPC_ClearActorAnimations = 174;

int RPC_ScrSetPlayerObjectMaterial = 0x54;
int RPC_ScrSetSpawnInfo = 68;
int RPC_ScrSetPlayerTeam = 69;
int RPC_ScrSetPlayerSkin = 153;
int RPC_ScrSetPlayerName = 11;
int RPC_ScrSetPlayerPos = 12;
int RPC_ScrSetPlayerPosFindZ = 13;
int RPC_ScrSetPlayerHealth = 14;
int RPC_ScrPutPlayerInVehicle = 70;
int RPC_ScrRemovePlayerFromVehicle = 71;
int RPC_ScrSetPlayerColor = 72;
int RPC_SetPlayerChatBubble = 59;
int RPC_ScrDisplayGameText = 73;
int RPC_ScrSetInterior = 156;
int RPC_ScrSetCameraPos = 157;
int RPC_ScrSetCameraLookAt = 158;
int RPC_ScrSetVehiclePos = 159;
int RPC_ScrSetVehicleZAngle = 160;
int RPC_ScrVehicleParams = 161;
int RPC_ScrSetCameraBehindPlayer = 162;
int RPC_ScrTogglePlayerControllable = 15;
int RPC_ScrPlaySound = 16;
int RPC_ScrSetWorldBounds = 17;
int RPC_ScrHaveSomeMoney = 18;
int RPC_ScrSetPlayerFacingAngle = 19;
int RPC_ScrResetMoney = 20;
int RPC_ScrResetPlayerWeapons = 21;
int RPC_ScrGivePlayerWeapon = 22;
int RPC_ScrRespawnVehicle = 255;
int RPC_ScrSetPlayerAttachedObject = 113;
int RPC_ScrLinkVehicle = 65;
int RPC_ScrSetPlayerArmour = 66;
int RPC_SetArmedWeapon = 67;
int RPC_ScrDeathMessage = 55;
int RPC_ScrSetMapIcon = 56;
int RPC_ScrDisableMapIcon = 144;
int RPC_ScrSetWeaponAmmo = 145;
int RPC_ScrSetGravity = 146;
int RPC_ScrSetVehicleHealth = 147;
int RPC_ScrAttachTrailerToVehicle = 148;
int RPC_ScrDetachTrailerFromVehicle = 149;
int RPC_ScrCreateObject = 44;
int RPC_ScrSetObjectPos = 45;
int RPC_ScrSetObjectRotation = 46;
int RPC_ScrDestroyObject = 47;
int RPC_ScrCreateExplosion = 79;
int RPC_ScrShowNameTag = 80;
int RPC_ScrMoveObject = 99;
int RPC_ScrStopObject = 122;
int RPC_ScrNumberPlate = 123;
int RPC_ScrTogglePlayerSpectating = 124;
int RPC_ScrSetPlayerSpectating = 255;
int RPC_ScrPlayerSpectatePlayer = 126;
int RPC_ScrPlayerSpectateVehicle = 127;
int RPC_ScrRemoveComponent = 57;
int RPC_ScrForceSpawnSelection = 74;
int RPC_ScrAttachObjectToPlayer = 75;
int RPC_ScrInitMenu = 76;
int RPC_ScrShowMenu = 77;
int RPC_ScrHideMenu = 78;
int RPC_ScrSetPlayerWantedLevel = 133;
int RPC_ScrSetPlayerSkillLevel = 34;
int RPC_ScrShowTextDraw = 134;
int RPC_ScrHideTextDraw = 135;
int RPC_ScrEditTextDraw = 105;
int RPC_ScrAddGangZone = 108;
int RPC_ScrRemoveGangZone = 120;
int RPC_ScrFlashGangZone = 121;
int RPC_ScrStopFlashGangZone = 85;

int RPC_ScrApplyPlayerAnimation = 86;
int RPC_ScrClearPlayerAnimations = 87;
int RPC_ScrApplyActorAnimation = 173;
int RPC_ScrClearActorAnimations = 174;

int RPC_ScrSetSpecialAction = 88;
int RPC_ScrEnableStuntBonus = 104;
int RPC_ScrSetFightingStyle = 89;
int RPC_ScrSetPlayerVelocity = 90;
int RPC_ScrSetVehicleVelocity = 91;
int RPC_ScrToggleWidescreen = 255;
int RPC_ScrSetVehicleTireStatus = 255;
int RPC_ScrSetPlayerDrunkLevel = 35;
int RPC_ScrDialogBox = 61;
int RPC_ScrCreate3DTextLabel = 36;
int RPC_ScrDestroy3DTextLabel = 58;
int RPC_ScrVehicleParamsEx = 24;
int RPC_ScrInterpolateCamera = 82;

int RPC_CustomJoin = 201;
int RPC_CustomHash = 202;