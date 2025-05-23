#include "CJavaWrapper.h"
#include "../main.h"

extern "C" JavaVM* javaVM;

#include "..//keyboard.h"
#include "..//chatwindow.h"
#include "..//CSettings.h"
#include "../net/netgame.h"
#include "../game/game.h"
#include "../str_obfuscator_no_template.hpp"
#include "../dialog.h"
#include "..//scoreboard.h"

extern CScoreBoard* pScoreBoard;
extern CKeyBoard* pKeyBoard;
extern CChatWindow* pChatWindow;
extern CSettings* pSettings;
extern CNetGame* pNetGame;
extern CGame* pGame;
extern CDialogWindow *pDialogWindow;

JNIEnv* CJavaWrapper::GetEnv()
{
	JNIEnv* env = nullptr;
	int getEnvStat = javaVM->GetEnv((void**)& env, JNI_VERSION_1_4);

	if (getEnvStat == JNI_EDETACHED)
	{
		Log(OBFUSCATE("GetEnv: not attached"));
		if (javaVM->AttachCurrentThread(&env, NULL) != 0)
		{
			Log(OBFUSCATE("Failed to attach"));
			return nullptr;
		}
	}
	if (getEnvStat == JNI_EVERSION)
	{
		Log(OBFUSCATE("GetEnv: version not supported"));
		return nullptr;
	}

	if (getEnvStat == JNI_ERR)
	{
		Log(OBFUSCATE("GetEnv: JNI_ERR"));
		return nullptr;
	}

	return env;
}

std::string CJavaWrapper::GetClipboardString()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return std::string("");
	}

	auto retn = (jbyteArray)env->CallObjectMethod(activity, s_GetClipboardText);

	if ((env)->ExceptionCheck())
	{
		(env)->ExceptionDescribe();
		(env)->ExceptionClear();
		return std::string("");
	}

	if (!retn)
	{
		return std::string("");
	}

	jboolean isCopy = true;

	jbyte* pText = env->GetByteArrayElements(retn, &isCopy);
	jsize length = env->GetArrayLength(retn);

	std::string str((char*)pText, length);

	env->ReleaseByteArrayElements(retn, pText, JNI_ABORT);
	
	return str;
}

void CJavaWrapper::CallLauncherActivity(int type)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	env->CallVoidMethod(activity, s_CallLauncherActivity, type);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowInputLayout()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	env->CallVoidMethod(activity, s_ShowInputLayout);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::HideInputLayout()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	env->CallVoidMethod(activity, s_HideInputLayout);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::ShowClientSettings()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	env->CallVoidMethod(activity, s_ShowClientSettings);

	EXCEPTION_CHECK(env);
}

void CJavaWrapper::MakeDialog(int dialogId, int dialogTypeId, char* caption, char* content, char* leftBtnText, char* rightBtnText)
{
    JNIEnv* env = GetEnv();
    if (!env)
    {
	Log("No env");
	return;
    }
    jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");
    jbyteArray bytes = env->NewByteArray(strlen(caption));
    env->SetByteArrayRegion(bytes, 0, strlen(caption), (jbyte*)caption);
    jstring str1 = (jstring)env->NewObject(strClass, ctorID, bytes, encoding);
    //
    jclass strClass1 = env->FindClass("java/lang/String");
    jmethodID ctorID1 = env->GetMethodID(strClass1, "<init>", "([BLjava/lang/String;)V");
    jstring encoding1 = env->NewStringUTF("UTF-8");
    jbyteArray bytes1 = env->NewByteArray(strlen(content));
    env->SetByteArrayRegion(bytes1, 0, strlen(content), (jbyte*)content);
    jstring str2 = (jstring)env->NewObject(strClass1, ctorID1, bytes1, encoding1);
    //
    jclass strClass2 = env->FindClass("java/lang/String");
    jmethodID ctorID2 = env->GetMethodID(strClass2, "<init>", "([BLjava/lang/String;)V");
    jstring encoding2 = env->NewStringUTF("UTF-8");
    jbyteArray bytes2 = env->NewByteArray(strlen(leftBtnText));
    env->SetByteArrayRegion(bytes2, 0, strlen(leftBtnText), (jbyte*)leftBtnText);
    jstring str3 = (jstring)env->NewObject(strClass2, ctorID2, bytes2, encoding2);
    //
    jclass strClass3 = env->FindClass("java/lang/String");
    jmethodID ctorID3 = env->GetMethodID(strClass3, "<init>", "([BLjava/lang/String;)V");
    jstring encoding3 = env->NewStringUTF("UTF-8");
    jbyteArray bytes3 = env->NewByteArray(strlen(rightBtnText));
    env->SetByteArrayRegion(bytes3, 0, strlen(rightBtnText), (jbyte*)rightBtnText);
    jstring str4 = (jstring)env->NewObject(strClass3, ctorID3, bytes3, encoding3);

    env->CallVoidMethod(activity, s_MakeDialog, dialogId, dialogTypeId, str1, str2, str3, str4);

    EXCEPTION_CHECK(env);
}

void CJavaWrapper::SetUseFullScreen(int b)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log(OBFUSCATE("No env"));
		return;
	}

	env->CallVoidMethod(activity, s_SetUseFullScreen, b);

	EXCEPTION_CHECK(env);
}
extern int g_iStatusDriftChanged;
#include "..//CDebugInfo.h"
extern "C"
{
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendButton(JNIEnv* pEnv, jobject thiz, jint action)
	{
		CPlayerPed *pPlayerPed = pGame->FindPlayerPed();
		if (pPlayerPed)
		{
			if (pNetGame && !pDialogWindow->m_bIsActive && pGame->IsToggledHUDElement(HUD_ELEMENT_BUTTONS))
			{
				switch(action) {
					case 0:
					{
						g_pJavaWrapper->ShowClientSettings();
						break;
					}
					case 1:
					{
						CTextDrawPool *pTextDrawPool = pNetGame->GetTextDrawPool();
						RakNet::BitStream bsClick;
						bsClick.Write(0xFFFF);
						pNetGame->GetRakClient()->RPC(&RPC_ClickTextDraw, &bsClick, HIGH_PRIORITY, RELIABLE_ORDERED, 0, false, UNASSIGNED_NETWORK_ID, nullptr);
						pTextDrawPool->SetSelectState(false, 0);
						break;
					}
					case 2:
					{
						if (!pScoreBoard->m_bToggle) 
						{
							pScoreBoard->Toggle();
						}
						else 
						{
							pScoreBoard->Toggle();
						}
						break;
					}
					case 3:
					{
						if(pPlayerPed->IsInVehicle()) LocalPlayerKeys.bKeys[ePadKeys::KEY_FIRE] = true;
						else LocalPlayerKeys.bKeys[ePadKeys::KEY_WALK] = true;
						break;
					}
					case 4:
					{
						LocalPlayerKeys.bKeys[ePadKeys::KEY_SECONDARY_ATTACK] = true;
						break;
					}
					case 5:
					{
						LocalPlayerKeys.bKeys[ePadKeys::KEY_CTRL_BACK] = true;

						CPlayerPool *pPlayerPool = pNetGame->GetPlayerPool();
						CLocalPlayer *pLocalPlayer = pPlayerPool->GetLocalPlayer();
						int iWeapon = pPlayerPed->GetCurrentWeapon();
						if(iWeapon != WEAPON_FIST) 
						{
							if(!pLocalPlayer->m_bPassengerDriveByMode) 
							{
								if(pPlayerPed->StartPassengerDriveByMode(true)) 
									pLocalPlayer->m_bPassengerDriveByMode = true;
							}
							else
							{
								pGame->FindPlayerPed()->TogglePlayerControllable(false);
								pGame->FindPlayerPed()->TogglePlayerControllable(true);
								pLocalPlayer->m_bPassengerDriveByMode = false;
							}
						}
						break;
					}
					case 6:
					{
						LocalPlayerKeys.bKeys[ePadKeys::KEY_YES] = true;
						break;
					}
					case 7:
					{
						LocalPlayerKeys.bKeys[ePadKeys::KEY_NO] = true;
						break;
					}
					case 8:
					{
						LocalPlayerKeys.bKeys[ePadKeys::KEY_CROUCH] = true;
						break;
					}
				}
			}
		}
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onInputEnd(JNIEnv* pEnv, jobject thiz, jbyteArray str)
	{
		if (pKeyBoard)
		{
			pKeyBoard->OnNewKeyboardInput(pEnv, thiz, str);
		}
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendDialogResponse(JNIEnv* pEnv, jobject thiz, jint i3, jint i, jint i2, jbyteArray str)
	{
		jboolean isCopy = true;

		jbyte* pMsg = pEnv->GetByteArrayElements(str, &isCopy);
		jsize length = pEnv->GetArrayLength(str);

		std::string szStr((char*)pMsg, length);

		if(pNetGame) {
			pNetGame->SendDialogResponse(i, i3, i2, (char*)szStr.c_str());
			pGame->FindPlayerPed()->TogglePlayerControllable(true);
		}

		pEnv->ReleaseByteArrayElements(str, pMsg, JNI_ABORT);
	}
		JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendCommand(JNIEnv* pEnv, jobject thiz, jbyteArray str)
	{
		jboolean isCopy = true;

		jbyte* pMsg = pEnv->GetByteArrayElements(str, &isCopy);
		jsize length = pEnv->GetArrayLength(str);

		std::string szStr((char*)pMsg, length);

		if(pNetGame) {
			pNetGame->SendChatCommand((char*)szStr.c_str());
		}

		pEnv->ReleaseByteArrayElements(str, pMsg, JNI_ABORT);
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_sendRPC(JNIEnv* pEnv, jobject thiz, jint type, jbyteArray str, jint action)
	{
		switch(type) {
			case 1:
				switch(action) {
					case 398:
						pNetGame->SendChatCommand("/gps");
						break;
					case 1:
						pNetGame->SendChatCommand("/gps");
						break;
					case 2:
						pNetGame->SendChatCommand("/mn");
						break;
					case 4: 
						pNetGame->SendChatCommand("/inv");
						break;
					case 5: {
						pNetGame->SendChatCommand("/anim");
						pNetGame->SendChatCommand("/anim");	
						break;
					}
					case 6:
						pNetGame->SendChatCommand("/donate");
						break;
					case 7:
						pNetGame->SendChatCommand("/car");
						break;
					case 8:
						pNetGame->SendChatCommand("/");		
						break;		
				}
			case 2:
				switch(action) {
					case 0:
						//pNetGame = new CNetGame(cryptor::create("194.169.160.244", 20).decrypt(), atoi(cryptor::create("7777", 4).decrypt()), pSettings->GetReadOnly().szNickName, pSettings->GetReadOnly().szPassword);
						//pSettings->GetWrite().last_server = 0;
						break;
				}
		}
		
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_togglePlayer(JNIEnv* pEnv, jobject thiz, jint toggle) {
		if(toggle)
			pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable(false);
		else
			pNetGame->GetPlayerPool()->GetLocalPlayer()->GetPlayerPed()->TogglePlayerControllable(true);
	}
	/*JNIEXPORT jint JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getLastServer(JNIEnv* pEnv, jobject thiz)
	{
		return (jint)pSettings->GetReadOnly().last_server;
	}*/
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onEventBackPressed(JNIEnv* pEnv, jobject thiz)
	{
		if (pKeyBoard)
		{
			if (pKeyBoard->IsOpen())
			{
				Log(OBFUSCATE("Closing keyboard"));
				pKeyBoard->Close();
			}
		}
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onNativeHeightChanged(JNIEnv* pEnv, jobject thiz, jint orientation, jint height)
	{
		if (pChatWindow)
		{
			pChatWindow->SetLowerBound(height);
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeCutoutSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iCutout = b;
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeKeyboardSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iAndroidKeyboard = b;
		}

		if (pKeyBoard && b)
		{
			pKeyBoard->EnableNewKeyboard();
		}
		else if(pKeyBoard)
		{
			pKeyBoard->EnableOldKeyboard();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeFpsCounterSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iFPSCounter = b;
		}

		CDebugInfo::SetDrawFPS(b);
	}
	
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeDialog(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iDialog = b;
		}
	}
	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHud(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iHud = b;
			if(!b)
			{
				*(uint8_t*)(g_libGTASA+0x7165E8) = 1;
				g_pJavaWrapper->HideHud();
			}
			else
			{
				*(uint8_t*)(g_libGTASA+0x7165E8) = 0;
				g_pJavaWrapper->ShowHud();
			}
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHpArmourText(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			if (!pSettings->GetWrite().iHPArmourText && b)
			{
				if (CAdjustableHudColors::IsUsingHudColor(HUD_HP_TEXT) == false)
				{
					CAdjustableHudColors::SetHudColorFromRGBA(HUD_HP_TEXT, 255, 0, 0, 255);
				}
				if (CAdjustableHudPosition::GetElementPosition(HUD_HP_TEXT).X == -1 || CAdjustableHudPosition::GetElementPosition(HUD_HP_TEXT).Y == -1)
				{
					CAdjustableHudPosition::SetElementPosition(HUD_HP_TEXT, 500, 500);
				}
				if (CAdjustableHudScale::GetElementScale(HUD_HP_TEXT).X == -1 || CAdjustableHudScale::GetElementScale(HUD_HP_TEXT).Y == -1)
				{
					CAdjustableHudScale::SetElementScale(HUD_HP_TEXT, 400, 400);
				}

				if (CAdjustableHudColors::IsUsingHudColor(HUD_ARMOR_TEXT) == false)
				{
					CAdjustableHudColors::SetHudColorFromRGBA(HUD_ARMOR_TEXT, 255, 0, 0, 255);
				}
				if (CAdjustableHudPosition::GetElementPosition(HUD_ARMOR_TEXT).X == -1 || CAdjustableHudPosition::GetElementPosition(HUD_ARMOR_TEXT).Y == -1)
				{
					CAdjustableHudPosition::SetElementPosition(HUD_ARMOR_TEXT, 300, 500);
				}
				if (CAdjustableHudScale::GetElementScale(HUD_ARMOR_TEXT).X == -1 || CAdjustableHudScale::GetElementScale(HUD_ARMOR_TEXT).Y == -1)
				{
					CAdjustableHudScale::SetElementScale(HUD_ARMOR_TEXT, 400, 400);
				}
			}

			pSettings->GetWrite().iHPArmourText = b;
		}

		CInfoBarText::SetEnabled(b);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeOutfitGunsSettings(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iOutfitGuns = b;

			CWeaponsOutFit::SetEnabled(b);
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativePcMoney(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iPCMoney = b;
		}

		CGame::SetEnabledPCMoney(b);
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeRadarrect(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
		{
			pSettings->GetWrite().iRadarRect = b;

			CRadarRect::SetEnabled(b);
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeNameTag(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
			pSettings->GetWrite().iNameTag = b;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNative3DText(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
			pSettings->GetWrite().i3DText = b;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeSkyBox(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
			pSettings->GetWrite().iSkyBox = b;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeFixedTextDraw(JNIEnv* pEnv, jobject thiz, jboolean b)
	{
		if (pSettings)
			pSettings->GetWrite().iTextDraw = b;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeFixedTextDraw(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
			return pSettings->GetReadOnly().iTextDraw;

		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeSkyBox(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
			return pSettings->GetReadOnly().iSkyBox;

		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeCutoutSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iCutout;
		}
		return 0;
	}
	
	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeDialog(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iDialog;
		}
		return 0;
	}
	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHud(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iHud;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeKeyboardSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iAndroidKeyboard;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeFpsCounterSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iFPSCounter;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHpArmourText(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iHPArmourText;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeOutfitGunsSettings(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iOutfitGuns;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativePcMoney(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iPCMoney;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeRadarrect(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			return pSettings->GetReadOnly().iRadarRect;
		}
		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNative3DText(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
			return pSettings->GetReadOnly().i3DText;

		return 0;
	}

	JNIEXPORT jboolean JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeNameTag(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
			return pSettings->GetReadOnly().iNameTag;

		return 0;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSettingsWindowSave(JNIEnv* pEnv, jobject thiz)
	{
		if (pSettings)
		{
			pSettings->Save();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_onSettingsWindowDefaults(JNIEnv* pEnv, jobject thiz, jint category)
	{
		if (pSettings)
		{
			pSettings->ToDefaults(category);
			if (pChatWindow)
			{
				pChatWindow->m_bPendingReInit = true;
			}
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHudElementColor(JNIEnv* pEnv, jobject thiz, jint id, jint a, jint r, jint g, jint b)
	{
		CAdjustableHudColors::SetHudColorFromRGBA((E_HUD_ELEMENT)id, r, g, b, a);
	}

	JNIEXPORT jbyteArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHudElementColor(JNIEnv* pEnv, jobject thiz, jint id)
	{
		char pTemp[9];
		jbyteArray color = pEnv->NewByteArray(sizeof(pTemp));

		if (!color)
		{
			return nullptr;
		}

		pEnv->SetByteArrayRegion(color, 0, sizeof(pTemp), (const jbyte*)CAdjustableHudColors::GetHudColorString((E_HUD_ELEMENT)id).c_str());

		return color;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHudElementPosition(JNIEnv* pEnv, jobject thiz, jint id, jint x, jint y)
	{
		if (id == 7)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fChatPosX = x;
				pSettings->GetWrite().fChatPosY = y;
				if (pChatWindow)
				{
					pChatWindow->m_bPendingReInit = true;
				}
				return;
			}
			return;
		}
		if (id == HUD_SNOW)
		{
			if (pSettings)
			{
				pSettings->GetWrite().iSnow = x;
			}
			CSnow::SetCurrentSnow(pSettings->GetReadOnly().iSnow);
			return;
		}
		CAdjustableHudPosition::SetElementPosition((E_HUD_ELEMENT)id, x, y);

		if (id >= HUD_WEAPONSPOS && id <= HUD_WEAPONSROT)
		{
			CWeaponsOutFit::OnUpdateOffsets();
		}
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeHudElementScale(JNIEnv* pEnv, jobject thiz, jint id, jint x, jint y)
	{
		CAdjustableHudScale::SetElementScale((E_HUD_ELEMENT)id, x, y);

		if (id >= HUD_WEAPONSPOS && id <= HUD_WEAPONSROT)
		{
			CWeaponsOutFit::OnUpdateOffsets();
		}
	}

	JNIEXPORT jintArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHudElementScale(JNIEnv* pEnv, jobject thiz, jint id)
	{
		jintArray color = pEnv->NewIntArray(2);

		if (!color)
		{
			return nullptr;
		}
		int arr[2];
		arr[0] = CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)id).X;
		arr[1] = CAdjustableHudScale::GetElementScale((E_HUD_ELEMENT)id).Y;
		pEnv->SetIntArrayRegion(color, 0, 2, (const jint*)& arr[0]);

		return color;
	}

	JNIEXPORT void JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_setNativeWidgetPositionAndScale(JNIEnv* pEnv, jobject thiz, jint id, jint x, jint y, jint scale)
	{
		if (id == 0)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fButtonMicrophoneX = x;
				pSettings->GetWrite().fButtonMicrophoneY = y;
				pSettings->GetWrite().fButtonMicrophoneSize = scale;
			}

			if (g_pWidgetManager)
			{
				if (g_pWidgetManager->GetSlotState(WIDGET_MICROPHONE))
				{
					g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetPos(x, y);
					g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetHeight(scale);
					g_pWidgetManager->GetWidget(WIDGET_MICROPHONE)->SetWidth(scale);
				}
			}
		}
		
		if (id == 1)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fButtonEnterPassengerX = x;
				pSettings->GetWrite().fButtonEnterPassengerY = y;
				pSettings->GetWrite().fButtonEnterPassengerSize = scale;
			}
		}

		if (id == 2)
		{
			if (pSettings)
			{
				pSettings->GetWrite().fButtonCameraCycleX = x;
				pSettings->GetWrite().fButtonCameraCycleY = y;
				pSettings->GetWrite().fButtonCameraCycleSize = scale;
			}

			if (g_pWidgetManager)
			{
				if (g_pWidgetManager->GetSlotState(WIDGET_CAMERA_CYCLE))
				{
					g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE)->SetPos(x, y);
					g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE)->SetHeight(scale);
					g_pWidgetManager->GetWidget(WIDGET_CAMERA_CYCLE)->SetWidth(scale);
				}
			}
		}
	}

	JNIEXPORT jintArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeHudElementPosition(JNIEnv* pEnv, jobject thiz, jint id)
	{
		jintArray color = pEnv->NewIntArray(2);

		if (!color)
		{
			return nullptr;
		}
		int arr[2];

		if (id == 7 && pSettings)
		{
			arr[0] = pSettings->GetReadOnly().fChatPosX;
			arr[1] = pSettings->GetReadOnly().fChatPosY;
		}
		else if (id == HUD_SNOW && pSettings)
		{
			arr[0] = CSnow::GetCurrentSnow();
			arr[1] = CSnow::GetCurrentSnow();
		}
		else
		{
			arr[0] = CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)id).X;
			arr[1] = CAdjustableHudPosition::GetElementPosition((E_HUD_ELEMENT)id).Y;
		}

		pEnv->SetIntArrayRegion(color, 0, 2, (const jint*)&arr[0]);

		return color;
	}

	JNIEXPORT jintArray JNICALL Java_com_nvidia_devtech_NvEventQueueActivity_getNativeWidgetPositionAndScale(JNIEnv* pEnv, jobject thiz, jint id)
	{
		jintArray color = pEnv->NewIntArray(3);

		if (!color)
		{
			return nullptr;
		}
		int arr[3] = { -1, -1, -1 };
		

		if (pSettings)
		{
			if (id == 0)
			{
				arr[0] = pSettings->GetWrite().fButtonMicrophoneX;
				arr[1] = pSettings->GetWrite().fButtonMicrophoneY;
				arr[2] = pSettings->GetWrite().fButtonMicrophoneSize;
			}
			if (id == 1)
			{
				arr[0] = pSettings->GetWrite().fButtonEnterPassengerX;
				arr[1] = pSettings->GetWrite().fButtonEnterPassengerY;
				arr[2] = pSettings->GetWrite().fButtonEnterPassengerSize;
			}
			if (id == 2)
			{
				arr[0] = pSettings->GetWrite().fButtonCameraCycleX;
				arr[1] = pSettings->GetWrite().fButtonCameraCycleY;
				arr[2] = pSettings->GetWrite().fButtonCameraCycleSize;
			}
		}
		

		pEnv->SetIntArrayRegion(color, 0, 3, (const jint*)& arr[0]);

		return color;
	}
}

void CJavaWrapper::ShowHud()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showHud);
}

void CJavaWrapper::HideHud()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideHud);
}

void CJavaWrapper::SetPauseState(bool a1)
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_setPauseState, a1);
}

void CJavaWrapper::UpdateHudInfo(int health, int armour, int hunger, int weaponid, int ammo, int ammoinclip, int money, int wanted, int score)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_updateHudInfo, health, armour, hunger, weaponid, ammo, ammoinclip, money, wanted, score);
}

void CJavaWrapper::ShowTabWindow()
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
	env->CallVoidMethod(this->activity, this->s_showTabWindow);
}

void CJavaWrapper::ShowSpeed()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showSpeed);
}

void CJavaWrapper::HideSpeed()
{
    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_hideSpeed);
}

void CJavaWrapper::UpdateSpeedInfo(int speed, int fuel, int hp, int mileage, int engine, int light, int belt, int lock)
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_updateSpeedInfo, speed, fuel, hp, mileage, engine, light, belt, lock);
}

void CJavaWrapper::ShowMenu() 
{
	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	env->CallVoidMethod(this->activity, this->s_showMenu);
}

void CJavaWrapper::SetTabStat(int id, char* name, int score, int ping) {

	JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}

	jclass strClass = env->FindClass("java/lang/String");
    jmethodID ctorID = env->GetMethodID(strClass, "<init>", "([BLjava/lang/String;)V");
    jstring encoding = env->NewStringUTF("UTF-8");

    jbyteArray bytes = env->NewByteArray(strlen(name));
    env->SetByteArrayRegion(bytes, 0, strlen(name), (jbyte*)name);
    jstring jname = (jstring) env->NewObject(strClass, ctorID, bytes, encoding);

	env->CallVoidMethod(this->activity, this->s_setTabStat, id, jname, score, ping);
}

void CJavaWrapper::ShowWelcome(bool a) {

    JNIEnv* env = GetEnv();

	if (!env)
	{
		Log("No env");
		return;
	}
    env->CallVoidMethod(this->activity, this->s_showWelcome, a);
}


#include <string>
#include <cstring>

std::string TIS620ToUTF8(const char* tis620Str) {
    std::string utf8Str;
    unsigned char c;

    while ((c = *tis620Str++)) {
        if (c < 0x80) {
            utf8Str += c; // ASCII ส่วนใหญ่เหมือนกับ TIS-620
        } else {
            unsigned short unicode = 0x0E00 + (c - 0xA0);
            utf8Str += 0xE0 | (unicode >> 12); // First byte
            utf8Str += 0x80 | ((unicode >> 6) & 0x3F); // Second byte
            utf8Str += 0x80 | (unicode & 0x3F); // Third byte
        }
    }

    return utf8Str;
}

void CJavaWrapper::ShowTwitter(bool a, const char* caption, const char*url) {
    JNIEnv* env = GetEnv();

    if (!env) {
        Log("No env twitter");
        return;
    }

    // แปลง const char* จาก TIS-620 เป็น UTF-8
    std::string utf8Caption = TIS620ToUTF8(caption);
    jstring jCaption = env->NewStringUTF(utf8Caption.c_str());

    jstring jUrl = env->NewStringUTF(url);

    // เรียกใช้ method showTwitter
    env->CallVoidMethod(this->activity, this->s_showTwitter, a, jCaption, jUrl);

    // ตรวจสอบ exception หลังจากเรียกใช้ JNI method
    EXCEPTION_CHECK(env);
	env->DeleteLocalRef(jCaption);
}






CJavaWrapper::CJavaWrapper(JNIEnv* env, jobject activity)
{
	this->activity = env->NewGlobalRef(activity);

	jclass nvEventClass = env->GetObjectClass(activity);
	if (!nvEventClass)
	{
		Log(OBFUSCATE("nvEventClass null"));
		return;
	}

	//s_CallLauncherActivity = env->GetMethodID(nvEventClass, OBFUSCATE("callLauncherActivity"), OBFUSCATE("(I)V"));
	s_GetClipboardText = env->GetMethodID(nvEventClass, OBFUSCATE("getClipboardText"), OBFUSCATE("()[B"));

	s_ShowInputLayout = env->GetMethodID(nvEventClass, OBFUSCATE("showInputLayout"), OBFUSCATE("()V"));
	s_HideInputLayout = env->GetMethodID(nvEventClass, OBFUSCATE("hideInputLayout"), OBFUSCATE("()V"));

	s_ShowClientSettings = env->GetMethodID(nvEventClass, OBFUSCATE("showClientSettings"), OBFUSCATE("()V"));
	s_SetUseFullScreen = env->GetMethodID(nvEventClass, OBFUSCATE("setUseFullscreen"), OBFUSCATE("(I)V"));
	
	s_MakeDialog = env->GetMethodID(nvEventClass, OBFUSCATE("showDialog"),OBFUSCATE("(IILjava/lang/String;Ljava/lang/String;Ljava/lang/String;Ljava/lang/String;)V"));
	    
	s_updateHudInfo = env->GetMethodID(nvEventClass, OBFUSCATE("updateHudInfo"), OBFUSCATE("(IIIIIIIII)V"));
    s_showHud = env->GetMethodID(nvEventClass, OBFUSCATE("showHud"), OBFUSCATE("()V"));
    s_hideHud = env->GetMethodID(nvEventClass, OBFUSCATE("hideHud"), OBFUSCATE("()V"));
	
	s_updateSpeedInfo = env->GetMethodID(nvEventClass, OBFUSCATE("updateSpeedInfo"), OBFUSCATE("(IIIIIIII)V"));
    s_showSpeed = env->GetMethodID(nvEventClass, OBFUSCATE("showSpeed"), OBFUSCATE("()V"));
    s_hideSpeed = env->GetMethodID(nvEventClass, OBFUSCATE("hideSpeed"), OBFUSCATE("()V"));
	
	
	s_showTabWindow = env->GetMethodID(nvEventClass, OBFUSCATE("showTabWindow"), OBFUSCATE("()V"));	
	s_setTabStat = env->GetMethodID(nvEventClass, OBFUSCATE("setTabStat"), OBFUSCATE("(ILjava/lang/String;II)V"));
	
	s_showMenu = env->GetMethodID(nvEventClass, OBFUSCATE("showMenu"), OBFUSCATE("()V"));
	
	//s_updateSplash = env->GetMethodID(nvEventClass, OBFUSCATE("updateSplash"), OBFUSCATE("(I)V"));
//	s_showSplash = env->GetMethodID(nvEventClass, OBFUSCATE("showSplash"), OBFUSCATE("()V"));
	
	s_showWelcome = env->GetMethodID(nvEventClass, OBFUSCATE("showWelcome"), OBFUSCATE("(Z)V"));
	s_showTwitter = env->GetMethodID(nvEventClass, OBFUSCATE("showTwitter"), OBFUSCATE("(ZLjava/lang/String;Ljava/lang/String;)V"));
	
//	s_showNotification = env->GetMethodID(nvEventClass, OBFUSCATE("showNotification"),OBFUSCATE("(ILjava/lang/String;ILjava/lang/String;Ljava/lang/String;)V"));
	
	
	s_setPauseState = env->GetMethodID(nvEventClass, OBFUSCATE("setPauseState"), OBFUSCATE("(Z)V"));


	env->DeleteLocalRef(nvEventClass);
}

CJavaWrapper::~CJavaWrapper()
{
	JNIEnv* pEnv = GetEnv();
	if (pEnv)
	{
		pEnv->DeleteGlobalRef(this->activity);
	}
}

CJavaWrapper* g_pJavaWrapper = nullptr;