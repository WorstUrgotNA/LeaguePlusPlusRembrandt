#pragma once
#include "PluginSDK.h"

IMenuOption* SkinID;
IMenuOption* SkinEnable;

IMenu* SkinMenu;

class SkinHack
{
public: 
	void InitMenu(IMenu* MenuSpot)
	{

		SkinMenu = MenuSpot->AddMenu("Skin Changer");
		{
			SkinEnable = SkinMenu->CheckBox("Enable:", false);
			SkinID = SkinMenu->AddInteger("Select Skin ID:", 1, 30, 1);
		}
	}

public: 
	void UpdateSkin()
	{
		if (SkinEnable->Enabled())
		{
			int SkinNum = SkinID->GetInteger();
			GEntityList->Player()->SetSkinId(SkinNum);
		}
	}
};