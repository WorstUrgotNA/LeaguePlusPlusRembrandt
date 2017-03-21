#include "Caitlyn.h"

Caitlyn::Caitlyn(IMenu* Parent)
{
	TrapEnemyCastType = { "Exact Position", "Vector Extension", "Turn Off" };

	//Create Spells
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithYasuoWall));
	Q->SetSkillshot(0.625, 90, 2200, 1300);
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
	SemiManualMenuKey = CaitlynMenu->AddKey("Semi-Manual Ult Key:", 84);
	EToMouse = CaitlynMenu->CheckBox("Enable E-to-Cursor:", false);
	TrapEnemyCast = CaitlynMenu->AddSelection("Trap on Enemy Auto/Spellcast:", 0, TrapEnemyCastType);
	WDelay = CaitlynMenu->AddFloat("Minimum Delay Between Traps (W):", 0, 15, 2);
	UltRange = CaitlynMenu->AddFloat("Dont R if Enemies Nearby:", 0, 3000, 900);
	EnemyToBlockR = CaitlynMenu->CheckBox("Dont R if an Enemy Can Block:", false);
	DrawReady = CaitlynMenu->CheckBox("Draw Only Ready Spells:", true);
	LaneClearQ = CaitlynMenu->CheckBox("Use Q to Laneclear:", true);
	LaneClearMana = CaitlynMenu->AddFloat("Q Laneclear Above Mana Percent:", 0, 100, 80);
	DrawQ = CaitlynMenu->CheckBox("Draw Q Range:", true);
	DrawW = CaitlynMenu->CheckBox("Draw W Range:", true);
	DrawE = CaitlynMenu->CheckBox("Draw E Range:", true);
	DrawR = CaitlynMenu->CheckBox("Draw R Range:", true);
}

Caitlyn::~Caitlyn()
{

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

float Caitlyn::CalcRDamage(IUnit* Target)
{
	auto Hero = GEntityList->Player();
	float InitDamage = 2 * Hero->BonusDamage();

	if (Hero->GetSpellLevel(kSlotR) == 1)
		InitDamage += 250;
	else if (Hero->GetSpellLevel(kSlotR) == 2)
		InitDamage += 475;
	else if (Hero->GetSpellLevel(kSlotR) == 3)
		InitDamage += 700;

	
	//Initial R Damage Fully Calculated - apply resists
	auto Armor = Target->Armor();
	if (Armor > 0) // Arm pen %
	{
		//Hero->ArmorPenetrationPercent();
	}
	//auto FlatArmorPenetration = Hero->ArmorPenetrationFlat ×(0.6 + 0.4 × Target's level ÷ 18)

	//auto Multiplier = Armor > 0 ? (100/(100+Armor)) : 2 - (100/(100-Armor));
	//auto FinalDamage = InitDamage * Multiplier;
	//
	//GRender->Notification(Vec4(255, 255, 255, 255), 0, "R Damage: %i", static_cast<int>(FinalDamage)); //
	return GDamage->CalcPhysicalDamage(Hero, Target, InitDamage);
}

void Caitlyn::LaneClear()
{
	if (EnemiesInRange(GEntityList->Player(), Q->Range()) > 0)
	{

	}

	if (LaneClearQ->Enabled() && GEntityList->Player()->ManaPercent() > LaneClearMana->GetFloat())
	{
		Vec3 CastPosition;
		int EnemiesHit;
		GPrediction->FindBestCastPosition(Q->Range(), Q->Radius(), true, true, true, CastPosition, EnemiesHit);

		if (EnemiesHit > 3)
		{
			Q->CastOnPosition(CastPosition);
		}
	}
}

void Caitlyn::Combo()
{
	auto MyHero = GEntityList->Player();

	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		auto flDistance = (Enemy->GetPosition() - MyHero->GetPosition()).Length();

		if (!Enemy->IsClone() && Enemy->IsValidTarget() && !Enemy->IsDead())
		{
			if (flDistance < R->Range() && flDistance > UltRange->GetFloat() && R->IsReady() && Enemy->GetHealth() - CalcRDamage(Enemy) < 0 && EnemiesInRange(MyHero, UltRange->GetFloat()) == 0)
			{
				if (EnemyToBlockR->Enabled() && AlliesInRange(Enemy, 500) > 0)
					continue;
				R->CastOnTarget(Enemy);
			}

			if (flDistance < W->Range() && W->IsReady() && (Enemy->HasBuffOfType(BUFF_Snare) || Enemy->HasBuffOfType(BUFF_Stun) || Enemy->HasBuffOfType(BUFF_Suppression) || Enemy->HasBuffOfType(BUFF_Knockup)))
			{
				if (GGame->Time() - LastTrapTime > WDelay->GetFloat())
				{
					W->CastOnTarget(Enemy);
					LastTrapTime = GGame->Time();
				}
			}

			if (flDistance < 300 && E->IsReady())
			{
				ComboTarget = Enemy;
				UseNetCombo = true;
				E->CastOnTarget(Enemy, kHitChanceLow);
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
				//toggle smite
				if (R->IsReady())
				{
					R->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, R->Range()));
					//GRender->Notification(Vec4(255, 255, 255, 255), 0, "%.2f", W->Radius());
				}
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
		Vec3 CastPosition = GEntityList->Player()->ServerPosition() - (GGame->CursorPosition() - GEntityList->Player()->ServerPosition());
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
		if (Q->IsReady() && DrawQ->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }

		if (W->IsReady() && DrawW->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }

		if (E->IsReady() && DrawE->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }

		if (R->IsReady() && DrawR->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	}
	else
	{
		if (DrawQ->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }

		if (DrawW->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }

		if (DrawE->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }

		if (DrawR->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	}
}

void Caitlyn::OnSpellCast(CastedSpell const& Args)
{
	auto MyHero = GEntityList->Player();
	float AttackDamage = GEntityList->Player()->PhysicalDamage() + GEntityList->Player()->BonusDamage();
	

	if (TrapEnemyCast->GetInteger() < 2 && W->IsReady() && Args.Caster_->IsEnemy(MyHero) && Args.Caster_->IsHero() && Args.Caster_->IsValidTarget() && !Args.Caster_->IsDead())
	{
		auto flDist = (Args.Caster_->GetPosition() - MyHero->GetPosition()).Length();
		if (flDist < 750 && GGame->Time() - LastTrapTime > WDelay->GetFloat())
		{
			if (TrapEnemyCast->GetInteger() == 0)
			{
				W->CastOnPosition(Args.Caster_->GetPosition());
			}
			else
			{
				Vec3 EndPosition = MyHero->GetPosition() + (Args.Caster_->GetPosition() - MyHero->GetPosition()).VectorNormalize() * (flDist + 50);
				W->CastOnPosition(EndPosition);
			}
			
			LastTrapTime = GGame->Time();
		}
			
	}


	//if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	//
	//CaitlynHeadshotMissile CaitlynCritAttack
	//	GGame->IssueOrder(MyHero, kStop, MyHero);
	if (strstr(Args.Name_, "CaitlynHeadshotMissile") && Args.Target_->IsHero() && !Args.Target_->IsDead() && Args.Target_->IsValidTarget())
	{
		auto flDistance = (Args.Target_->GetPosition() - MyHero->GetPosition()).Length();
		if (flDistance < Q->Range() && Q->IsReady())
		{
			if (flDistance > 650 || MyHero->AttackSpeed() < 1.25f)
				Q->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range()), kHitChanceVeryHigh);
		}
	}
	
	//if ((strstr(Args.Name_, "CaitlynBasicAttack") || strstr(Args.Name_, "CaitlynCritAttack") || strstr(Args.Name_, "CaitlynHeadshotMissile")) && Args.Target_->IsHero() && Args.Target_->IsValidTarget() && !Args.Target_->IsDead())


	if (strstr(Args.Name_, "CaitlynEntrapment"))
	{
		if (W->IsReady() && ComboTarget != nullptr && ComboTarget->IsValidTarget() && !ComboTarget->IsDead())
		{
			Vec3 EstimatedEnemyPos;
			GPrediction->GetFutureUnitPosition(ComboTarget, 0.5, true, EstimatedEnemyPos);

			W->CastOnPosition(EstimatedEnemyPos);
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
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && E->IsReady() && Target->IsHero())
	{
		AdvPredictionOutput out;
		E->RunPrediction(Target, false, kCollidesWithMinions, &out);

		if (out.HitChance >= kHitChanceMedium)
		{
			UseNetCombo = true;
			E->CastOnTarget(Target);
			ComboTarget = Target;
		}
	}
}

