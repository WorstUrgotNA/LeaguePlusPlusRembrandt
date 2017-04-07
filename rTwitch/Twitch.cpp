#include "Twitch.h"
#include "Rembrandt.h"

Twitch::~Twitch()
{
	TwitchMenu->Remove();
}

Twitch::Twitch(IMenu* Parent, IUnit* Hero) :Champion(Parent, Hero)
{
	//Create Spells
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	W = GPluginSDK->CreateSpell2(kSlotW, kLineCast, true, true, kCollidesWithYasuoWall);
	W->SetSkillshot(0.25f, 275.f, 1400.f, 950.f);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, true, false, kCollidesWithNothing);
	E->SetSkillshot(0.25f, 0.f, 1000.f, 1200.f);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);
	R->SetOverrideRange(850.f + Hero->BoundingRadius());

	ComboWType = { "Instant", "Smart Logic", "Off" };
	ComboEType = { "Kill Only", "Custom Stacks", "Off" };
	ComboRType = { "Off", "1", "2" , "3" , "4" , "5" };
	HarassEType = { "Off", "1", "2" , "3" , "4" , "5", "6" };
	//MAIN Menu
	TwitchMenu = Parent->AddMenu("Twitch PRO++");

	//DRAW MENU
	DrawMenu = TwitchMenu->AddMenu("++ Drawings");
	DrawReady = DrawMenu->CheckBox("Draw Only Ready Spells:", true);
	DrawW = DrawMenu->CheckBox("Draw W Range:", true);
	WColor = DrawMenu->AddColor("W Range Color:", 255, 255, 0, 255);
	DrawE = DrawMenu->CheckBox("Draw E Range:", true);
	EColor = DrawMenu->AddColor("E Range Color:", 255, 255, 0, 255);
	DrawR = DrawMenu->CheckBox("Draw R Range:", true);
	RColor = DrawMenu->AddColor("R Range Color:", 255, 255, 0, 255);
	DrawEDamage = DrawMenu->CheckBox("Draw E Damage:", true);
	DrawEDamageColor = DrawMenu->AddColor("E Damage Color:", 255, 255, 0, 150);

	//COMBO MENU
	ComboMenu = TwitchMenu->AddMenu("++ Combo");
	ComboQOption = ComboMenu->CheckBox("Use Q in Combo:", true);
	QInRange = ComboMenu->AddFloat("-> Use Q if Enemy in X Range:", 0, 3000, 600);
	ComboWOption = ComboMenu->CheckBox("Use W in Combo:", true);
	ComboETypeOption = ComboMenu->AddSelection("Use E in Combo:", 0, ComboEType);
	ComboECustomStacks = ComboMenu->AddInteger("-> Custom E Stacks:", 1, 6, 6);
	EEnemyLeaving = ComboMenu->CheckBox("Use E if Enemy Escaping Range:", true);
	ComboR = ComboMenu->AddSelection("Use R if X Enemies in Range:", 0, ComboRType);

	//HARASS MENU
	HarassMenu = TwitchMenu->AddMenu("++ Harass");
	HarassW = HarassMenu->CheckBox("Use W for Harass:", false);
	HarassWMana = HarassMenu->AddFloat("-> Min Mana % W Harass:", 0, 100, 75);
	HarassE = HarassMenu->AddSelection("Use E Harass on X Stacks:", 0, HarassEType);

	//LANE CLEAR MENU
	LaneClearMenu = TwitchMenu->AddMenu("++ Jungle Clear");
	EJungleKS = LaneClearMenu->CheckBox("Use E to KS Jungle Mobs:", true);

	//EXTRA MENU
	ExtraMenu = TwitchMenu->AddMenu("++ Extra Settings");
	QRecall = ExtraMenu->CheckBox("Stealth Recall:", true);
	WUnderTurret = ExtraMenu->CheckBox("Use W if Under Enemy Turret:", false);
	SaveManaForE = ExtraMenu->CheckBox("Save Mana for E:", true);
}

int Twitch::EnemiesInRange(IUnit* Source, float range)
{
	auto Targets = GEntityList->GetAllHeros(false, true);
	auto enemiesInRange = 0;

	for (auto target : Targets)
	{
		if (target && !target->IsDead())
		{
			if (Rembrandt::IsInRange(target->GetPosition(), Source->GetPosition(), range))
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
	
	float BonusStackDamage = ((0.25 * Hero->BonusDamage()) + (0.2 * Hero->TotalMagicDamage())) * StackCount;

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

	return GDamage->CalcPhysicalDamage(Hero, Target, InitDamage) * Rembrandt::DamageModifierFromMasteries(Hero, Target);
}


void Twitch::Combo()
{
	if (ComboR->GetInteger() > 0 && EnemiesInRange(Hero, R->Range()) >= ComboR->GetInteger())
		if (R->CastOnPlayer()) return;

	for (auto const& Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (Enemy->IsValidTarget() && !Enemy->IsClone())
		{
			auto flDistance = (Hero->GetPosition() - Enemy->GetPosition()).Length2D();

			if (Enemy->HasBuff("twitchdeadlyvenom"))
			{
				if (flDistance <= E->Range()) // Use E in Combo
				{
					if (ComboETypeOption->GetInteger() == 1 && (Enemy->GetBuffCount("twitchdeadlyvenom") >= ComboECustomStacks->GetInteger() || CalcEDamage(Enemy) > Enemy->GetHealth()))
						if (E->CastOnPlayer()) { return; }
					if (ComboETypeOption->GetInteger() == 0 && CalcEDamage(Enemy) > Enemy->GetHealth())
						if (E->CastOnPlayer()) { return; }
					if (EEnemyLeaving->Enabled() && flDistance > E->Range() - 20 && !Enemy->IsFacing(Hero))
						if (E->CastOnPlayer()) { return; }
				}
			}

			if (ComboQOption->Enabled() && Q->IsReady() && flDistance < QInRange->GetFloat())
			{
				if (SaveManaForE->Enabled() && Hero->GetMana() - Q->ManaCost() < E->ManaCost())
					continue;
				if (Q->CastOnPlayer()) { return; }
			}
		}
	}
	if (EnemiesInRange(Hero, Hero->AttackRange() + Hero->BoundingRadius()) == 0 && ComboWOption->Enabled() && !Hero->HasBuff("globalcamouflage") && !Hero->HasBuff("twitchhideinshadowsbuff"))
	{
		if (SaveManaForE->Enabled() && Hero->GetMana() - W->ManaCost() < E->ManaCost())
			return;

		if (!WUnderTurret->Enabled() && GUtility->IsPositionUnderTurret(Hero->GetPosition()))
			return;
		
		if (W->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, W->Range()))) return;
	}
}

void Twitch::LaneClear()
{
	for (auto const& Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (HarassE->GetInteger() > 0 && Enemy->IsValidTarget() && !Enemy->IsClone())
		{
			auto flDistance = (Hero->GetPosition() - Enemy->GetPosition()).Length2D();

			if (Enemy->HasBuff("twitchdeadlyvenom"))
			{
				if (flDistance <= E->Range()) // Use E in Harass
				{
					if (Enemy->GetBuffCount("twitchdeadlyvenom") >= HarassE->GetInteger() || CalcEDamage(Enemy) > Enemy->GetHealth())
						if (E->CastOnPlayer()) { return; }
					if (EEnemyLeaving->Enabled() && flDistance > E->Range() - 20 && !Enemy->IsFacing(Hero))
						if (E->CastOnPlayer()) { return; }
				}
			}
		}
	}

	if (SaveManaForE->Enabled() && Hero->GetMana() - W->ManaCost() < E->ManaCost())
		return;
	if (!WUnderTurret->Enabled() && GUtility->IsPositionUnderTurret(Hero->GetPosition()))
		return;

	if (HarassW->Enabled() && Hero->ManaPercent() > HarassWMana->GetFloat())
		if (W->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, W->Range()))) return;
}

void Twitch::OnGameUpdate()
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		Combo();
	else if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
		LaneClear();
	
	if (EJungleKS->Enabled() && (GOrbwalking->GetOrbwalkingMode() == kModeCombo || GOrbwalking->GetOrbwalkingMode() == kModeLaneClear))
	{
		for (auto JGCreep : GEntityList->GetAllMinions(false, false, true))
		{
			if (JGCreep->IsValidTarget(Hero, E->Range()) && JGCreep->HasBuff("twitchdeadlyvenom") && CalcEDamage(JGCreep) > JGCreep->GetHealth())
			{
				if (strstr(JGCreep->GetObjectName(), "Red") || strstr(JGCreep->GetObjectName(), "Blue") ||
					strstr(JGCreep->GetObjectName(), "Dragon") || strstr(JGCreep->GetObjectName(), "Rift") || strstr(JGCreep->GetObjectName(), "Baron"))
					if (E->CastOnPlayer()) { return; }
			}
		}
	}

}

void Twitch::OnRender()
{
	if (E->IsReady() && DrawEDamage->Enabled())
	{
		Vec4 BarColor;
		DrawEDamageColor->GetColor(&BarColor);

		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (Enemy->IsOnScreen() && Enemy->IsVisible() && Enemy->IsValidTarget() && Enemy->HasBuff("twitchdeadlyvenom"))
				Rembrandt::DrawDamageOnChampionHPBar(Enemy, CalcEDamage(Enemy), "E", BarColor);
		}
	}

	if (DrawReady->Enabled())
	{
		Vec4 CircleColor;
		WColor->GetColor(&CircleColor);
		if (W->IsReady() && DrawW->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, W->Range()); }
		EColor->GetColor(&CircleColor);
		if (E->IsReady() && DrawE->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, E->Range()); }
		RColor->GetColor(&CircleColor);
		if (R->IsReady() && DrawR->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, R->Range()); }
	}
	else
	{
		Vec4 CircleColor;
		WColor->GetColor(&CircleColor);
		if (DrawW->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, W->Range()); }
		EColor->GetColor(&CircleColor);
		if (DrawE->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, E->Range()); }
		RColor->GetColor(&CircleColor);
		if (DrawR->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, R->Range()); }
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

void Twitch::OnRealSpellCast(CastedSpell const& Args)
{
	if (Args.Caster_->IsEnemy(Hero) && Args.Name_ == "SummonerFlash" && Args.Caster_->HasBuff("twitchdeadlyvenom") && (Hero->GetPosition() - Args.Caster_->GetPosition()).Length2D() < E->Range() && (Hero->GetPosition() - Args.EndPosition_).Length() > E->Range())
	{
		if (E->CastOnPlayer()) { return; }
	}
}

void Twitch::OnOrbwalkAttack(IUnit* Source, IUnit* Target)
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && ComboWOption->Enabled())
	{
		if (Target->IsValidTarget() && Target->IsHero())
		{
			if (SaveManaForE->Enabled() && Hero->GetMana() - W->ManaCost() < E->ManaCost())
				return;

			if (!WUnderTurret->Enabled() && GUtility->IsPositionUnderTurret(Hero->GetPosition()))
				return;

			if (W->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, W->Range()))) return;
		}
	}
}

