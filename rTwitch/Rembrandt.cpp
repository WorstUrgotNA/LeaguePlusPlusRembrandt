#include "Rembrandt.h"

/*				UPDATED PATCH #7.7				*/
auto Rembrandt::DamageModifierFromMasteries(IUnit* Hero, IUnit* Target) -> double
{
	double Modifier = 1;
	std::vector<HeroMastery> MyMasteryBuffer;

	if (Hero->GetMasteries(MyMasteryBuffer))
	{
		for (auto Mastery : MyMasteryBuffer)
		{
			//SORCERY: Increases ability and spell damage by 0.4 / 0.8 / 1.2 / 1.6 / 2 %
			if (Mastery.MasteryId == 40)
			{
				Modifier += (0.4 * Mastery.Points) / 100;
			}
			//DOUBLE EDGED SWORD: You deal 3% increased damage from all sources, but take 1.5% increased damage from all sources.
			else if (Mastery.MasteryId == 12)
			{
				Modifier += 0.03;
			}
			//ASSASSAIN: Grants 2% increased damage against enemy champions while no allied champions are nearby - 800 range
			else if (Mastery.MasteryId == 186)
			{
				bool IsActive = true;
				for (auto Friend : GEntityList->GetAllHeros(true, false))
				{
					if (Friend != Hero && (Hero->GetPosition() - Friend->GetPosition()).Length() <= 800)
					{
						IsActive = false;
						break;
					}
				}

				if (IsActive) { Modifier += 0.02; }
			}
			//MERCILESS: Grants 0.6 / 1.2 / 1.8 / 2.4 / 3 % increased damage against champions below 40 % health.
			else if (Mastery.MasteryId == 232)
			{
				if (Target->HealthPercent() < 40)
					Modifier += (0.6 * Mastery.Points) / 100;
			}
		}
	}

	//check if enemy has double edged sword
	std::vector<HeroMastery> TarMasteryBuffer;
	if (Target->GetMasteries(TarMasteryBuffer))
	{
		for (auto Mastery : TarMasteryBuffer)
		{
			//DOUBLE EDGED SWORD: You deal 3% increased damage from all sources, but take 1.5% increased damage from all sources.
			if (Mastery.MasteryId == 12)
			{
				Modifier += 0.015;
				break;
			}
		}
	}

	return Modifier;
}

auto Rembrandt::DrawDamageOnChampionHPBar(IUnit* Hero, double Damage, const char *Text, Vec4 BarColor) -> void
{
	Vec2 HPBarPos;
	if (Hero->GetHPBarPosition(HPBarPos))
	{
		Vec2 HPBarSize = Vec2(103 * (Damage / Hero->GetMaxHealth()), 8);
		HPBarPos = Vec2(HPBarPos.x + 10, HPBarPos.y += 20);

		Vec2 LinePos1 = Vec2(HPBarPos.x + HPBarSize.x, HPBarPos.y);
		Vec2 LinePos2 = Vec2(HPBarPos.x + HPBarSize.x - 5, HPBarPos.y - 7);

		GRender->DrawFilledBox(HPBarPos, HPBarSize, BarColor);
		GRender->DrawLine(LinePos1, LinePos2, Vec4(255, 255, 255, 255));
		GRender->DrawLine(LinePos1, LinePos1 + Vec2(0, 8), Vec4(255, 255, 255, 255));
		GRender->DrawTextW(LinePos2 - Vec2(13, 13), Vec4(255, 255, 255, 255), Text);
	}
}