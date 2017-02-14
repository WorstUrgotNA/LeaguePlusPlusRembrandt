#pragma once
#include "PluginSDK.h"

IMenu* DTR;
IMenuOption* TowerRangeEnabled;

class DrawTowerRange
{
public:
	void InitMenu(IMenu* MenuSpot)
	{
		DTR = MenuSpot->AddMenu("Turret Range Indicators");
		TowerRangeEnabled = DTR->CheckBox("Draw Tower Ranges:", true);
	}

public:
	void UpdateTowerRanges()
	{
		if (TowerRangeEnabled->Enabled())
		{
			for (auto turret : GEntityList->GetAllTurrets(false, true))
			{
				if (turret == nullptr || turret->IsDead() || turret->GetHealth() < 1.f)
					continue;

				float range = turret->BoundingRadius() + 775.f;

				GRender->DrawOutlinedCircle(turret->GetPosition(), Vec4(255, 0, 0, 140), range);
			}
		}
	}
};