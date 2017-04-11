#include "Caitlyn.h"
#include "Rembrandt.h"

Caitlyn::~Caitlyn()
{
	CaitlynMenu->Remove();
}

Caitlyn::Caitlyn(IMenu* Parent, IUnit* Hero):Champion(Parent, Hero)
{
	
	TrapEnemyCastType = { "Exact Position", "Vector Extension", "Turn Off" };
	
	//Create Spells
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithYasuoWall));
	Q->SetSkillshot(0.625, 50, 2200, 1300);
	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, false, (kCollidesWithNothing));
	W->SetSkillshot(1.1, 100, 3200, 800);
	E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, true, false, (kCollidesWithYasuoWall | kCollidesWithHeroes | kCollidesWithMinions));
	E->SetSkillshot(0.25, 90, 1600, 950);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, true, false, (kCollidesWithYasuoWall | kCollidesWithHeroes));
	R->SetOverrideDelay(3.0);
	R->SetOverrideSpeed(1000);

	E->SetTriggerEvents(false);
	

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
	DrawRDamage = DrawMenu->CheckBox("Draw R Damage:", true);
	DrawRDamageColor = DrawMenu->AddColor("R Damage Color:", 255, 255, 0, 150);

	//COMBO MENU
	ComboMenu = CaitlynMenu->AddMenu("++ Combo"); 
	QInCombo = ComboMenu->CheckBox("Use Q in Combo:", true);
	SafeQKS = ComboMenu->CheckBox("Safe Q KS:", true);
	ShortQDisableLevel = ComboMenu->AddInteger("Disable Short-Q after level:", 0, 18, 11);
	WAfterE = ComboMenu->CheckBox("Use W in Burst Combo:", true);
	//TrapEnemyCast = ComboMenu->AddSelection("Use W on Enemy AA/Spellcast:", 0, TrapEnemyCastType);
	TrapEnemyCast = ComboMenu->AddInteger("W [0]Exact Position [1]Vector Extension [2]Turn Off", 0, 2, 0);
	TrapImmobileCombo = ComboMenu->CheckBox("Use W on Immobile Enemies:", true);
	EBeforeLevel = ComboMenu->AddInteger("Disable Long-E After Level:", 0, 18, 18);
	EWhenClose = ComboMenu->CheckBox("Use E on Gapcloser/Close Enemy:", true);
	SemiManualEMenuKey = ComboMenu->AddKey("E Semi-Manual Key:", 17);
	RInCombo = ComboMenu->CheckBox("Use R in Combo:", true);
	SemiManualMenuKey = ComboMenu->AddKey("R Semi-Manual Key:", 84);
	UltRange = ComboMenu->AddFloat("Dont R if Enemies in Range:", 0, 3000, 900);
	EnemyToBlockR = ComboMenu->CheckBox("Dont R if an Enemy Can Block:", false);
	
	HarassMenu = CaitlynMenu->AddMenu("++ Harass");
	SafeQHarass = HarassMenu->CheckBox("Use Q Smart Harass:", true);
	SafeQHarassMana = HarassMenu->AddFloat("Q Harass Above Mana Percent:", 0, 100, 60);
	TrapEnemyCastHarass = HarassMenu->AddInteger("W [0]Exact Position [1]Vector Extension [2]Turn Off", 0, 2, 0);
	//TrapEnemyCastHarass = HarassMenu->AddSelection("Use W on Enemy AA/Spellcast:", 1, TrapEnemyCastType);

	LaneClearMenu = CaitlynMenu->AddMenu("++ Lane Clear");
	LaneClearQ = LaneClearMenu->CheckBox("Use Q to Laneclear:", true);
	LaneClearMana = LaneClearMenu->AddFloat("Q Laneclear Above Mana Percent:", 0, 100, 80);

	ExtraMenu = CaitlynMenu->AddMenu("++ Extra Settings");
	WDelay = ExtraMenu->AddFloat("Minimum Delay Between Traps (W):", 0, 15, 2);
	EToMouse = ExtraMenu->CheckBox("Enable E-to-Cursor:", false);
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
		if (target && !target->IsDead())
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
	float InitDamage = 0;

	if (Slot == kSlotR)
	{
		InitDamage = 2 * Hero->BonusDamage();
		InitDamage += std::vector<double>({ 250, 475, 700 }).at(Hero->GetSpellLevel(kSlotR) - 1);
	}
	else if (Slot == kSlotQ)
	{
		InitDamage = std::vector<double>({ 1.3, 1.4, 1.5, 1.6, 1.7 }).at(Hero->GetSpellLevel(kSlotQ) - 1) * Hero->TotalPhysicalDamage() + std::vector<double>({ 30 , 70 , 110 , 150 , 190 }).at(Hero->GetSpellLevel(kSlotQ) - 1);
	}
	
	return GDamage->CalcPhysicalDamage(Hero, Target, InitDamage) * Rembrandt::DamageModifierFromMasteries(Hero, Target);
}

void Caitlyn::LaneClear()
{
	if (LaneClearQ->Enabled() && Hero->ManaPercent() > LaneClearMana->GetFloat())
	{
		std::vector<Vec3> CastPos; 
		CastPos.push_back(Hero->GetPosition());
		FarmLocation Farmlocation;
		Rembrandt::FindBestLineCastPosition(CastPos, Q->Range(), Q->Range(), 50, true, true, Farmlocation);

		if (Farmlocation.HitCount > 3)
		{
			if (Q->CastOnPosition(Farmlocation.CastPosition)) { return; }
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
			if (!SemiManualKey)
			{
				R->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, R->Range()));
				SemiManualKey = true;
			}
		}
		else
		{
			// key is up . . .
			SemiManualKey = false;
		}
	}

	//key press
	keystate = GetAsyncKeyState(SemiManualEMenuKey->GetInteger()); //smite key

	if (GUtility->IsLeagueWindowFocused() && !GGame->IsChatOpen())
	{
		if (keystate < 0) // If most-significant bit is set...
		{
			// key is down . . .
			if (!SemiManualKey)
			{
				E->CastOnTarget(GTargetSelector->FindTarget(ClosestPriority, PhysicalDamage, E->Range()));
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
	if (R->IsReady() && DrawRDamage->Enabled())
	{
		Vec4 BarColor;
		DrawRDamageColor->GetColor(&BarColor);

		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (Enemy->IsOnScreen() && Enemy->IsVisible() && Hero->IsValidTarget(Enemy, R->Range()))
				Rembrandt::DrawDamageOnChampionHPBar(Enemy, CalcSpellDamage(Enemy, kSlotR), "R", BarColor);
		}
	}

	/*Vec4 CircleColor;
		for (int i = 0; i < 4; i++)
		{
			colors[i]->GetColor(&CircleColor);
			if (spells[i]->IsReady() && draws[i]->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, spells[i]->Range()); }
		}*/

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

	if (WAfterE->Enabled() && strstr(Args.Name_, "CaitlynEntrapment"))
	{
		if (ComboTarget && ComboTarget->IsValidTarget())
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
		if (Hero->GetLevel() <= EBeforeLevel->GetInteger() && E->CastOnTarget(Target, kHitChanceHigh))
		{
			UseNetCombo = true;
			ComboTarget = Target;
			return;
		}
	}
}

