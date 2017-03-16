#pragma once
#include "PluginSDK.h"

IMenu* AutoLevelMainMenu;
IMenu* ChampNameMenu;

IMenuOption* EnableAutoLevelUp;
IMenuOption* ALUQ;
IMenuOption* ALUW;
IMenuOption* ALUE;
IMenuOption* ALUR;
IMenuOption* ALUOnlyR;
IMenuOption* ALUStartLevel;


class AutoLevel
{
public:
	void InitMenu(IMenu* MenuSpot)
	{
		auto pHero = GEntityList->Player();
		std::string szMenuName = std::string(pHero->ChampionName());
		szMenuName += " Level Builder";

		AutoLevelMainMenu = MenuSpot->AddMenu("Auto Level PRO");
		ChampNameMenu = AutoLevelMainMenu->AddMenu(szMenuName.c_str());
		
		EnableAutoLevelUp = ChampNameMenu->CheckBox("Enable: ", false);
		ALUOnlyR = ChampNameMenu->CheckBox("Only level R (Ultimate):", false);
		ALUR = ChampNameMenu->AddInteger("R: ", 1, 4, 1);
		ALUQ = ChampNameMenu->AddInteger("Q: ", 1, 4, 2);
		ALUW = ChampNameMenu->AddInteger("W: ", 1, 4, 3);
		ALUE = ChampNameMenu->AddInteger("E: ", 1, 4, 4);
		ALUStartLevel = ChampNameMenu->AddInteger("Start at level: ", 1, 16, 4);
		

	}

public:
	void LevelUp(IUnit* Source, int NewLevel)
	{
		if (EnableAutoLevelUp->Enabled() && Source == GEntityList->Player() && NewLevel >= ALUStartLevel->GetInteger()) //auto level
		{
			for (int i = 1; i <= 4; i++)
			{
				if (ALUR->GetInteger() == i) {
					Source->LevelUpSpell(kSlotR);
				}
				if (ALUQ->GetInteger() == i && !ALUOnlyR->Enabled()) {
					Source->LevelUpSpell(kSlotQ);
				}
				if (ALUW->GetInteger() == i && !ALUOnlyR->Enabled()) {
					Source->LevelUpSpell(kSlotW);
				}
				if (ALUE->GetInteger() == i && !ALUOnlyR->Enabled()) {
					Source->LevelUpSpell(kSlotE);
				}

			}
		}
	}
};
