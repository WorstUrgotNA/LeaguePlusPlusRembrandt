#include "Twitch.h"

Twitch::Twitch(IMenu* Parent)
{
	Hero = GEntityList->Player();

	//Create Spells
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	W = GPluginSDK->CreateSpell2(kSlotW, kLineCast, true, true, (kCollidesWithYasuoWall));
	W->SetSkillshot(0.25f, 275.f, 1400.f, 900.f);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, true, false, (kCollidesWithNothing));
	E->SetSkillshot(0.25f, 0.f, 1000.f, 1200.f);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, false, kCollidesWithNothing);
	//RECALL = GPluginSDK->CreateSpell2(kSlotRecall, kTargetCast, false, false, kCollidesWithNothing);

	ComboEType = { "Kill Only", "Max Stacks", "Off" };
	//Menu
	TwitchMenu = Parent->AddMenu("Twitch PRO++");
	ComboQOption = TwitchMenu->CheckBox("Use Q in Combo:", true);
	QInRange = TwitchMenu->AddFloat("Use Combo-Q if Enemy in X Range:", 0, 3000, 600);
	ComboWOption = TwitchMenu->CheckBox("Use W in Combo:", true);
	ComboETypeOption = TwitchMenu->AddSelection("Use E in Combo:", 0, ComboEType);
	EEnemyLeaving = TwitchMenu->CheckBox("Use E if Enemy Escaping Range:", true);
	EJungleKS = TwitchMenu->CheckBox("Use E to KS Jungle Mobs:", true);
	SaveManaForE = TwitchMenu->CheckBox("Save Mana for E:", true);
	QRecall = TwitchMenu->CheckBox("Stealth Recall:", true);
	WUnderTurret = TwitchMenu->CheckBox("Use W if Under Enemy Turret:", false);
	DrawReady = TwitchMenu->CheckBox("Draw Only Ready Spells:", true);
	DrawW = TwitchMenu->CheckBox("Draw W Range:", true);
	DrawE = TwitchMenu->CheckBox("Draw E Range:", true);
	DrawR = TwitchMenu->CheckBox("Draw R Range:", true);
}

Twitch::~Twitch()
{
	TwitchMenu->Remove();
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

	auto FinalDamage = GDamage->CalcPhysicalDamage(Hero, Target, InitDamage);

	std::vector<HeroMastery> MyMasteryBuffer;
	if (Hero->GetMasteries(MyMasteryBuffer))
	{
		double Modifier = 0;

		for (auto Mastery : MyMasteryBuffer)
		{
			//PageId 193 - MasteryId 201 - SORCERY: Increases ability and spell damage by 0.4 / 0.8 / 1.2 / 1.6 / 2 %
			if (Mastery.PageId == 193 && Mastery.MasteryId == 201)
			{
				Modifier += (0.4 * Mastery.Points) / 100;
			}
			//PageId 193 - MasteryId 124 - DOUBLE EDGED SWORD: You deal 3% increased damage from all sources, but take 1.5% increased damage from all sources.
			else if (Mastery.PageId == 193 && Mastery.MasteryId == 124)
			{
				Modifier += 0.03;
			}
			//PageId 62 - MasteryId 254 - ASSASSAIN: Grants 2% increased damage against enemy champions while no allied champions are nearby - 800 range
			else if (Mastery.PageId == 62 && Mastery.MasteryId == 254 && Target->IsHero())
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
			// PageId 62 - MasteryId 119 - MERCILESS: Grants 0.6 / 1.2 / 1.8 / 2.4 / 3 % increased damage against champions below 40 % health.
			else if (Mastery.PageId == 62 && Mastery.MasteryId == 119 && Target->IsHero())
			{
				if (Target->HealthPercent() < 40)
					Modifier += (0.6 * Mastery.Points) / 100;
			}
		}

		FinalDamage += FinalDamage * Modifier;
	}

	//check if enemy has double edged sword
	std::vector<HeroMastery> TarMasteryBuffer;
	if (Target->GetMasteries(TarMasteryBuffer))
	{
		double Modifier = 0;

		for (auto Mastery : TarMasteryBuffer)
		{
			//PageId 193 - MasteryId 124 - DOUBLE EDGED SWORD: You deal 3% increased damage from all sources, but take 1.5% increased damage from all sources.
			if (Mastery.PageId == 193 && Mastery.MasteryId == 124)
			{
				Modifier += 0.015;
			}
		}

		FinalDamage += FinalDamage * Modifier;
	}

	//GRender->Notification(Vec4(255, 255, 255, 255), 0, "%f", FinalDamage);

	return FinalDamage;
}


void Twitch::Combo()
{
	for (auto const& Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (Enemy->IsValidTarget() && !Enemy->IsClone())
		{
			auto flDistance = (Hero->GetPosition() - Enemy->GetPosition()).Length();

			if (Enemy->HasBuff("twitchdeadlyvenom"))
			{
				if (flDistance <= E->Range()) // Use E in Combo
				{
					if (ComboETypeOption->GetInteger() == 1 && (Enemy->GetBuffCount("twitchdeadlyvenom") == 6 || CalcEDamage(Enemy) > Enemy->GetHealth()))
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
		
		W->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, W->Range()));
	}
}

void Twitch::OnGameUpdate()
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		Combo();

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

void Twitch::OnRealSpellCast(CastedSpell const& Args)
{
	if (Args.Caster_->IsEnemy(Hero) && Args.Name_ == "SummonerFlash" && Args.Caster_->HasBuff("twitchdeadlyvenom") && (Hero->GetPosition() - Args.Caster_->GetPosition()).Length() < E->Range() && (Hero->GetPosition() - Args.EndPosition_).Length() > E->Range())
	{
		if (E->CastOnPlayer()) { return; }
	}
}

void Twitch::OnOrbwalkAttack(IUnit* Source, IUnit* Target)
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		if (Target->IsValidTarget() && Target->IsHero())
		{
			if (SaveManaForE->Enabled() && Hero->GetMana() - W->ManaCost() < E->ManaCost())
				return;

			if (!WUnderTurret->Enabled() && GUtility->IsPositionUnderTurret(Hero->GetPosition()))
				return;

			W->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, W->Range()));
		}
	}
}

