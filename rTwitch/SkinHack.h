#pragma once
#include "PluginSDK.h"

IMenuOption* SkinID;

IMenu* SkinMenu;

class SkinHack
{
public: 
	void InitMenu(IMenu* MenuSpot)
	{

		SkinMenu = MenuSpot->AddMenu("Skin Changer");
		{
			SkinID = SkinMenu->AddInteger("Select Skin ID:", 1, 30, 1);
		}
	}

public: 
	void UpdateSkin()
	{
		int SkinNum = SkinID->GetInteger();
		GEntityList->Player()->SetSkinId(SkinNum);
	}
};