#include "Twitch.h"

Twitch::Twitch(IMenu* Parent)
{
	Hero = GEntityList->Player();

	//Create Spells
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, true, true, kCollidesWithYasuoWall);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);
	//RECALL = GPluginSDK->CreateSpell2(kSlotRecall, kTargetCast, false, false, kCollidesWithNothing);

	ComboEType = { "Kill Only", "Max Stacks", "Off" };
	//Menu
	TwitchMenu = Parent->AddMenu("Twitch PRO++");
	ComboQOption = TwitchMenu->CheckBox("Use Q in Combo:", true);
	ComboWOption = TwitchMenu->CheckBox("Use W in Combo:", true);
	ComboETypeOption = TwitchMenu->AddSelection("Use E in Combo:", 0, ComboEType);
	SaveManaForE = TwitchMenu->CheckBox("Save Mana for E:", true);
	QRecall = TwitchMenu->CheckBox("Stealth Recall:", true);
	DrawReady = TwitchMenu->CheckBox("Draw Only Ready Spells:", true);
	DrawW = TwitchMenu->CheckBox("Draw W Range:", true);
	DrawE = TwitchMenu->CheckBox("Draw E Range:", true);
	DrawR = TwitchMenu->CheckBox("Draw R Range:", true);
}

Twitch::~Twitch()
{

}

int Twitch::EnemiesInRange(IUnit* Source, float range)
{
	auto Targets = GEntityList->GetAllHeros(false, true);
	auto enemiesInRange = 0;

	for (auto target : Targets)
	{
		if (target != nullptr && !target->IsDead())
		{
			auto flDistance = (target->GetPosition() - Source->GetPosition()).Length();
			if (flDistance <= range)
			{
				enemiesInRange++;
			}
		}
	}
	return enemiesInRange;
}

float Twitch::CalcEDamage(IUnit* Target)
{
	float InitDamage = 0;
	int StackCount = Target->GetBuffCount("twitchdeadlyvenom");

	if (StackCount == 0) return 0;

	float BonusStackDamage = ((0.25 * Hero->BonusDamage()) + (0.2 * Hero->MagicDamage())) * StackCount;

	if (Hero->GetSpellLevel(kSlotE) == 1)
		InitDamage += 20 + (15 * StackCount) + BonusStackDamage;
	else if (Hero->GetSpellLevel(kSlotE) == 2)
		InitDamage += 35 + (20 * StackCount) + BonusStackDamage;
	else if (Hero->GetSpellLevel(kSlotE) == 3)
		InitDamage += 50 + (25 * StackCount) + BonusStackDamage;
	else if (Hero->GetSpellLevel(kSlotE) == 4)
		InitDamage += 65 + (30 * StackCount) + BonusStackDamage;
	else if (Hero->GetSpellLevel(kSlotE) == 5)
		InitDamage += 80 + (35 * StackCount) + BonusStackDamage;

		/*
		BASE PHYSICAL DAMAGE: 20 / 35 / 50 / 65 / 80
		BONUS DAMAGE PER STACK: 15 / 20 / 25 / 30 / 35 (+ 25% bonus AD) (+ 20% AP)
		*/

	return GDamage->CalcPhysicalDamage(Hero, Target, InitDamage);
}


void Twitch::Combo()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		auto flDistance = (GEntityList->Player()->GetPosition() - Enemy->GetPosition()).Length();
		if (Enemy->IsValidTarget() && !Enemy->IsDead() && !Enemy->IsClone())
		{
			if (Enemy->HasBuff("twitchdeadlyvenom"))
			{
				if (E->IsReady() && flDistance < E->Range()) // Use E in Combo
				{
					if (ComboETypeOption->GetInteger() == 1 && (Enemy->GetBuffCount("twitchdeadlyvenom") == 6 || CalcEDamage(Enemy) > Enemy->GetHealth()))
						E->CastOnPlayer();
					else if (ComboETypeOption->GetInteger() == 0 && CalcEDamage(Enemy) > Enemy->GetHealth())
						E->CastOnPlayer();
				}
			}

			if (ComboQOption->Enabled() && Q->IsReady() && flDistance < 600)
			{
				if (SaveManaForE->Enabled() && Hero->GetMana() - Q->ManaCost() < E->ManaCost())
					continue;
				Q->CastOnPlayer();
			}

			if (ComboWOption->Enabled() && W->IsReady() && flDistance < W->Range() && !Hero->HasBuff("globalcamouflage") && !Hero->HasBuff("twitchhideinshadowsbuff"))
			{
				if (SaveManaForE->Enabled() && Hero->GetMana() - W->ManaCost() < E->ManaCost())
					continue;
				W->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, W->Range()));
			}
		}
	}
}

void Twitch::OnGameUpdate()
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		Combo();
}

void Twitch::OnRender()
{
	/*Vec2 ScreenPos;
	if (GTargetSelector->GetFocusedTarget() != nullptr && GGame->Projection(GGame->CursorPosition(), &ScreenPos))
		GRender->DrawTextW(Vec2(ScreenPos.x + 100, ScreenPos.y + 100), Vec4(255, 255, 255, 255), "%i", static_cast<int>(CalcEDamage(GTargetSelector->GetFocusedTarget())));*/

	if (DrawReady->Enabled())
	{
		if (W->IsReady() && DrawW->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }

		if (E->IsReady() && DrawE->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }

		if (R->IsReady() && DrawR->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }

	}
	else
	{
		if (DrawW->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }

		if (DrawE->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }

		if (DrawR->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	}
}

bool Twitch::OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition)
{
	if (Slot == kSlotRecall && QRecall->Enabled() && Q->IsReady() && !Hero->HasBuff("globalcamouflage"))
	{
		Q->CastOnPlayer();
	}

	return true;
}

void Twitch::OnSpellCast(CastedSpell const& Args)
{
		
}

void Twitch::OnOrbwalkAttack(IUnit* Source, IUnit* Target)
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			auto flDistance = (GEntityList->Player()->GetPosition() - Enemy->GetPosition()).Length();
			if (Enemy->IsValidTarget() && !Enemy->IsDead() && !Enemy->IsClone())
			{
				if (ComboWOption->Enabled() && W->IsReady() && flDistance < W->Range() && !Hero->HasBuff("globalcamouflage"))
				{
					if (SaveManaForE->Enabled() && Hero->GetMana() - W->ManaCost() < E->ManaCost())
						continue;
					W->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, W->Range()));
				}
			}
		}
	}
}

