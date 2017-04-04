#include "Caitlyn.h"

Caitlyn::Caitlyn(IMenu* Parent)
{
	TrapEnemyCastType = { "Exact Position", "Vector Extension", "Turn Off" };

	Hero = GEntityList->Player();

	//Create Spells
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithYasuoWall));
	Q->SetSkillshot(0.625, 50, 2200, 1300);
	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, false, (kCollidesWithNothing));
	W->SetSkillshot(1.1, 100, 3200, 800);
	E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, true, false, (kCollidesWithYasuoWall, kCollidesWithHeroes, kCollidesWithMinions));
	E->SetSkillshot(0.25, 90, 1600, 950);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, true, false, (kCollidesWithYasuoWall, kCollidesWithHeroes));
	R->SetOverrideDelay(3.0);
	R->SetOverrideSpeed(1000);

	E->SetTriggerEvents(false);

	//E->SetOverrideRange(750);
	SemiManualKey = false;
	ComboTrap = false;
	UseNet = false;
	UseNetCombo = false;

	LastTrapTime = GGame->Time();

	//Menu
	CaitlynMenu = Parent->AddMenu("Caitlyn PRO++");
	
	
	
	
	
	//DRAW MENU
	DrawMenu = CaitlynMenu->AddMenu("++ Drawings");
	DrawReady = DrawMenu->CheckBox("Draw Only Ready Spells:", true);
	DrawQ = DrawMenu->CheckBox("Draw Q Range:", true);
	QColor = DrawMenu->AddColor("Q Range Color:", 255, 255, 0, 255);
	DrawW = DrawMenu->CheckBox("Draw W Range:", true);
	WColor = DrawMenu->AddColor("W Range Color:", 255, 255, 0, 255);
	DrawE = DrawMenu->CheckBox("Draw E Range:", true);
	EColor = DrawMenu->AddColor("E Range Color:", 255, 255, 0, 255);
	DrawR = DrawMenu->CheckBox("Draw R Range:", true);
	RColor = DrawMenu->AddColor("R Range Color:", 255, 255, 0, 255);

	//COMBO MENU
	ComboMenu = CaitlynMenu->AddMenu("++ Combo"); 
	QInCombo = ComboMenu->CheckBox("Use Q in Combo:", true);
	SafeQKS = ComboMenu->CheckBox("Safe Q KS:", true);
	ShortQDisableLevel = ComboMenu->AddInteger("Disable Short-Q after level:", 0, 18, 11);
	WAfterE = ComboMenu->CheckBox("Use W in Burst Combo:", true);
	TrapEnemyCast = ComboMenu->AddSelection("Use W on Enemy AA/Spellcast:", 0, TrapEnemyCastType);
	TrapImmobileCombo = ComboMenu->CheckBox("Use W on Immobile Enemies:", true);
	EBeforeLevel = ComboMenu->AddInteger("Disable Long-E After Level:", 0, 18, 18);
	EWhenClose = ComboMenu->CheckBox("Use E on Gapcloser/Close Enemy:", true);
	RInCombo = ComboMenu->CheckBox("Use R in Combo:", true);
	SemiManualMenuKey = ComboMenu->AddKey("R Semi-Manual Key:", 84);
	UltRange = ComboMenu->AddFloat("Dont R if Enemies in Range:", 0, 3000, 900);
	EnemyToBlockR = ComboMenu->CheckBox("Dont R if an Enemy Can Block:", false);

	HarassMenu = CaitlynMenu->AddMenu("++ Harass");
	SafeQHarass = HarassMenu->CheckBox("Use Q Smart Harass:", true);
	SafeQHarassMana = HarassMenu->AddFloat("Q Harass Above Mana Percent:", 0, 100, 60);
	TrapEnemyCastHarass = HarassMenu->AddSelection("Use W on Enemy AA/Spellcast:", 1, TrapEnemyCastType);

	LaneClearMenu = CaitlynMenu->AddMenu("++ Lane Clear");
	LaneClearQ = LaneClearMenu->CheckBox("Use Q to Laneclear:", true);
	LaneClearMana = LaneClearMenu->AddFloat("Q Laneclear Above Mana Percent:", 0, 100, 80);

	ExtraMenu = CaitlynMenu->AddMenu("++ Extra Settings");
	WDelay = ExtraMenu->AddFloat("Minimum Delay Between Traps (W):", 0, 15, 2);
	EToMouse = ExtraMenu->CheckBox("Enable E-to-Cursor:", false);

}

Caitlyn::~Caitlyn()
{
	CaitlynMenu->Remove();
}

int Caitlyn::AlliesInRange(IUnit* Source, float range)
{
	auto Targets = GEntityList->GetAllHeros(false, true);
	auto AlliesInRange = 0;

	for (auto target : Targets)
	{
		if (target != nullptr && !target->IsDead())
		{
			auto flDistance = (target->GetPosition() - Source->GetPosition()).Length();
			if (flDistance <= range)
			{
				AlliesInRange++;
			}
		}
	}
	return AlliesInRange;
}

int Caitlyn::EnemiesInRange(IUnit* Source, float range)
{
	auto Targets = GEntityList->GetAllHeros(false, true);
	auto enemiesInRange = 0;

	for (auto target : Targets)
	{
		if (target != nullptr && !target->IsDead())
		{
			auto flDistance = (target->GetPosition() - Source->GetPosition()).Length();
			if (flDistance < range)
			{
				enemiesInRange++;
			}
		}
	}
	return enemiesInRange;
}

float Caitlyn::CalcSpellDamage(IUnit* Target, eSpellSlot Slot)
{
	if (!Target || Hero->GetSpellLevel(Slot) == 0) return 0;
	float InitDamage;

	if (Slot == kSlotR)
	{
		InitDamage = 2 * Hero->BonusDamage();

		if (Hero->GetSpellLevel(kSlotR) == 1)
			InitDamage += 250;
		else if (Hero->GetSpellLevel(kSlotR) == 2)
			InitDamage += 475;
		else if (Hero->GetSpellLevel(kSlotR) == 3)
			InitDamage += 700;
	}
	else if (Slot == kSlotQ)
	{
		InitDamage = std::vector<double>({ 1.3, 1.4, 1.5, 1.6, 1.7 }).at(Hero->GetSpellLevel(kSlotQ) - 1) * Hero->TotalPhysicalDamage() + std::vector<double>({ 30 , 70 , 110 , 150 , 190 }).at(Hero->GetSpellLevel(kSlotQ) - 1);
	}
	
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
			else if (Mastery.PageId == 62 && Mastery.MasteryId == 254)
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
			else if (Mastery.PageId == 62 && Mastery.MasteryId == 119)
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

	/*
	PageId: 193 = Ferocity, 62 = Cunning
	
	PageId 193 - MasteryId 127 - FRESH BLOOD: Basic attacks versus enemy champions deal (10 + 1 * level) bonus damage (6 second per-target cooldown).
	PageId 193 - MasteryId 249 - NATURAL TALENT
	
	PageId 193 - MasteryId 103 - BATTERING BLOWS: %arm pen
	PageId 193 - MasteryId 52 - FERVOR OF BATTLE
	PageId 62 - MasteryId 44 - SAVAGERY: Basic attacks and single target spells deal 1 / 2 / 3 / 4 / 5 bonus damage to minions and monsters.
	
	
	PageId 62 - MasteryId 238 - DANGEROUS GAME
	
	*/
	
	return FinalDamage;
}

void Caitlyn::LaneClear()
{
	if (LaneClearQ->Enabled() && Hero->ManaPercent() > LaneClearMana->GetFloat())
	{
		Vec3 CastPosition;
		int EnemiesHit;
		GPrediction->FindBestCastPosition(Q->Range(), Q->Radius(), true, true, true, CastPosition, EnemiesHit);

		if (EnemiesHit > 3)
		{
			if (Q->CastOnPosition(CastPosition)) { return; }
		}
	}
}

void Caitlyn::Combo()
{
	for (auto const& Enemy : GEntityList->GetAllHeros(false, true))
	{
		auto flDistance = (Enemy->GetPosition() - Hero->GetPosition()).Length();

		if (!Enemy->IsClone() && Enemy->IsValidTarget())
		{
			if (SafeQKS->Enabled() && flDistance > 675 && EnemiesInRange(Hero, 650) == 0 && (CalcSpellDamage(Enemy, kSlotQ) > Enemy->GetHealth() || (R->IsReady() && CalcSpellDamage(Enemy, kSlotQ) + CalcSpellDamage(Enemy, kSlotR) > Enemy->GetHealth())))
			{
				if (Q->CastOnTarget(Enemy, kHitChanceVeryHigh)) { return; }
			}

			if (RInCombo->Enabled() && flDistance < R->Range() && flDistance > UltRange->GetFloat() && R->IsReady() && Enemy->GetHealth() - CalcSpellDamage(Enemy, kSlotR) < 0 && EnemiesInRange(Hero, UltRange->GetFloat()) == 0)
			{
				if (EnemyToBlockR->Enabled() && AlliesInRange(Enemy, 500) > 0)
					continue;
				if (R->CastOnTarget(Enemy)) { return; }
			}

			if (TrapImmobileCombo->Enabled() && flDistance < W->Range() && (Enemy->HasBuffOfType(BUFF_Snare) || Enemy->HasBuffOfType(BUFF_Stun) || Enemy->HasBuffOfType(BUFF_Suppression) || Enemy->HasBuffOfType(BUFF_Knockup)))
			{
				if (GGame->Time() - LastTrapTime > WDelay->GetFloat())
				{
					if (W->CastOnTarget(Enemy))
					{
						LastTrapTime = GGame->Time();
						return;
					}
				}
			}

			if (EWhenClose->Enabled() && flDistance < 300)
			{
				if (E->CastOnUnit(Enemy))
				{
					ComboTarget = Enemy;
					UseNetCombo = true;
				}
			}
		}
	}
}

void Caitlyn::OnGameUpdate()
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		Combo();

	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
		LaneClear();
	//key press
	keystate = GetAsyncKeyState(SemiManualMenuKey->GetInteger()); //smite key

	if (GUtility->IsLeagueWindowFocused() && !GGame->IsChatOpen())
	{
		if (keystate < 0) // If most-significant bit is set...
		{
			// key is down . . .
			if (SemiManualKey == false)
			{
				R->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, R->Range()));
				//GRender->Notification(Vec4(255, 255, 255, 255), 0, "%.1f", CalcRDamage(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, R->Range())));
				SemiManualKey = true;
			}
		}
		else
		{
			// key is up . . .
			SemiManualKey = false;
		}
	}


	if (E->IsReady() && UseNet && EToMouse->Enabled() && !UseNetCombo)
	{
		Vec3 CastPosition = Hero->ServerPosition() - (GGame->CursorPosition() - Hero->ServerPosition());
		E->CastOnPosition(CastPosition);
	}
}

void Caitlyn::OnRender()
{
	/* Collision Drawing
	auto pLocal = GEntityList->Player();

	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
	if ((Enemy->GetPosition() - pLocal->GetPosition()).Length() < 950)
	{
	//Vec3 EndPosition = (pLocal->GetPosition()).Extend(Enemy->GetPosition, 950);
	Vec3 EstimatedEnemyPos;
	GPrediction->GetFutureUnitPosition(Enemy, 0.35, true, EstimatedEnemyPos);
	Vec3 EndPosition = pLocal->GetPosition() + (EstimatedEnemyPos - pLocal->GetPosition()).VectorNormalize() * 925;

	Vec2 vecMyPosition;
	Vec2 vecProjectedPosition;

	if (GGame->Projection(pLocal->GetPosition(), &vecMyPosition))
	{
	GGame->Projection(EndPosition, &vecProjectedPosition);
	if (GNavMesh->IsPointWall(EndPosition))
	GRender->DrawLine(vecMyPosition, vecProjectedPosition, Vec4(255, 0, 0, 255));
	else
	GRender->DrawLine(vecMyPosition, vecProjectedPosition, Vec4(0, 0, 0, 255));
	}
	}
	}*/
	//GRender->DrawCircle(GGame->CursorPosition(), circletestradius->GetFloat(), Vec4(255, 255, 255, 255), 1);


	if (DrawReady->Enabled())
	{
		Vec4 CircleColor;
		QColor->GetColor(&CircleColor);
		if (Q->IsReady() && DrawQ->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, Q->Range()); }
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
		QColor->GetColor(&CircleColor);
		if (DrawQ->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, Q->Range()); }
		WColor->GetColor(&CircleColor);
		if (DrawW->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, W->Range()); }
		EColor->GetColor(&CircleColor);
		if (DrawE->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, E->Range()); }
		RColor->GetColor(&CircleColor);
		if (DrawR->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, R->Range()); }
	}
}

void Caitlyn::OnSpellCast(CastedSpell const& Args)
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && TrapEnemyCast->GetInteger() < 2 && Args.Caster_->IsEnemy(Hero) && Args.Caster_->IsHero() && Args.Caster_->IsValidTarget(Hero, 750))
	{
		if (GGame->Time() - LastTrapTime > WDelay->GetFloat())
		{
			if (TrapEnemyCast->GetInteger() == 0)
			{
				if (W->CastOnPosition(Args.Caster_->GetPosition())) 
				{ 
					LastTrapTime = GGame->Time(); 
					return;
				}
			}
			else
			{
				Vec3 EndPosition = Hero->GetPosition() + (Args.Caster_->GetPosition() - Hero->GetPosition()).VectorNormalize() * ((Args.Caster_->GetPosition() - Hero->GetPosition()).Length() + 50);
				if (W->CastOnPosition(EndPosition))
				{
					LastTrapTime = GGame->Time(); 
					return;
				}
			}
		}
			
	}
	
	if (GOrbwalking->GetOrbwalkingMode() == kModeLastHit && TrapEnemyCastHarass->GetInteger() < 2 && Args.Caster_->IsEnemy(Hero) && Args.Caster_->IsHero() && Args.Caster_->IsValidTarget(Hero, 750))
	{
		if (GGame->Time() - LastTrapTime > WDelay->GetFloat())
		{
			if (TrapEnemyCastHarass->GetInteger() == 0)
			{
				if (W->CastOnPosition(Args.Caster_->GetPosition()))
				{
					LastTrapTime = GGame->Time();
					return;
				}
			}
			else
			{
				Vec3 EndPosition = Hero->GetPosition() + (Args.Caster_->GetPosition() - Hero->GetPosition()).VectorNormalize() * ((Args.Caster_->GetPosition() - Hero->GetPosition()).Length() + 50);
				if (W->CastOnPosition(EndPosition))
				{
					LastTrapTime = GGame->Time();
					return;
				}
			}
		}

	}

	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear && Args.Caster_->IsEnemy(Hero) && Args.Caster_->IsHero() && Args.Caster_->IsValidTarget(Hero, 1250))
	{
		if (SafeQHarass->Enabled() && Hero->ManaPercent() > SafeQHarassMana->GetFloat() && EnemiesInRange(Hero, 800) == 0)
		{
			if (Q->CastOnPosition(Args.Caster_->GetPosition())) { return; }
		}
	}

	if (QInCombo->Enabled() && strstr(Args.Name_, "CaitlynHeadshotMissile") && Args.Target_->IsHero() && Args.Target_->IsValidTarget())
	{
		auto flDistance = (Args.Target_->GetPosition() - Hero->GetPosition()).Length();
		if (flDistance < Q->Range())
		{
			if (flDistance > 650 || Hero->GetLevel() < ShortQDisableLevel->GetInteger())
				if (Q->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range()), kHitChanceVeryHigh)) { return; }
		}
	}
	
	//if ((strstr(Args.Name_, "CaitlynBasicAttack") || strstr(Args.Name_, "CaitlynCritAttack") || strstr(Args.Name_, "CaitlynHeadshotMissile")) && Args.Target_->IsHero() && Args.Target_->IsValidTarget() && !Args.Target_->IsDead())


	if (WAfterE->Enabled() && strstr(Args.Name_, "CaitlynEntrapment"))
	{
		if (ComboTarget != nullptr && ComboTarget->IsValidTarget())
		{
			Vec3 EstimatedEnemyPos;
			GPrediction->GetFutureUnitPosition(ComboTarget, 0.5, true, EstimatedEnemyPos);

			if (W->CastOnPosition(EstimatedEnemyPos)) { return; }
		}
	}
	
}

bool Caitlyn::OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition)
{
	if (UseNetCombo)
	{
		UseNetCombo = false;
		return true;
	}
		

	if (Slot == kSlotE && EToMouse->Enabled())
	{
		if (UseNet)
		{
			UseNet = false;
			return true;
		}
		else
		{
			UseNet = true;

			return false;
		}
	}

	return true;
}

void Caitlyn::OnOrbwalkAttack(IUnit* Source, IUnit* Target)
{	
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && Target->IsHero() && Target->IsValidTarget())
	{
		if (Hero->GetLevel() <= EBeforeLevel->GetInteger() && E->CastOnTarget(Target, kHitChanceVeryHigh))
		{
			UseNetCombo = true;
			ComboTarget = Target;
			return;
		}
	}
}

