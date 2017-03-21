#include "Kogmaw.h"

Kogmaw::Kogmaw(IMenu* Parent)
{
	Hero = GEntityList->Player();

	//Create Spells
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithYasuoWall, kCollidesWithHeroes, kCollidesWithMinions));
	Q->SetSkillshot(0.25, 70, 1650, 1200);

	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, (kCollidesWithNothing));
	W->SetSkillshot(0.25, 50, 1400, FLT_MAX);

	E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, true, false, (kCollidesWithYasuoWall));
	E->SetSkillshot(0.25, 120, 1400, 1360);

	R = GPluginSDK->CreateSpell2(kSlotR, kCircleCast, true, false, (kCollidesWithNothing));
	R->SetSkillshot(0.1, 225, 1000, 1800);

	SemiManualKey = false;

	//Menu
	KogmawMenu = Parent->AddMenu("KogMaw PRO++");

	QinCombo = KogmawMenu->CheckBox("Use Q in Combo:", true);
	EinCombo = KogmawMenu->CheckBox("Use E in Combo:", true);
	WinCombo = KogmawMenu->CheckBox("Use W in Combo:", true);
	RinCombo = KogmawMenu->CheckBox("Use R in Combo:", true);

	SemiManualMenuKey = KogmawMenu->AddKey("Semi-Manual R Key:", 84);

	RMaxStacks = KogmawMenu->AddInteger("R Max Stacks Combo:", 0, 10, 3);
	RMaxStacksHarass = KogmawMenu->AddInteger("R Max Stacks Harass", 0, 10, 1);
	RHarassMana = KogmawMenu->AddFloat("R Harass Min Mana %", 0, 100, 60);
	ECastDistance = KogmawMenu->AddFloat("E Cast Range:", 0, 1350, 1000);

	DrawReady = KogmawMenu->CheckBox("Draw Only Ready Spells:", true);
	DrawQ = KogmawMenu->CheckBox("Draw Q Range:", true);
	DrawW = KogmawMenu->CheckBox("Draw W Range:", true);
	DrawE = KogmawMenu->CheckBox("Draw E Range:", true);
	DrawR = KogmawMenu->CheckBox("Draw R Range:", true);

}

Kogmaw::~Kogmaw()
{

}

float Kogmaw::GetWRange()
{
	return std::vector<double>({ 630, 630, 650, 670, 690, 710 }).at(Hero->GetSpellLevel(kSlotW)) + Hero->BoundingRadius();
}
float Kogmaw::GetRRange()
{
	return std::vector<double>({ 1200, 1200, 1500, 1800 }).at(Hero->GetSpellLevel(kSlotR));
}

float Kogmaw::CalcRDamage(IUnit* Target)
{
	auto InitDamage = std::vector<double>({ 100, 140, 180 }).at(Hero->GetSpellLevel(kSlotR) - 1) + (0.65 * Hero->BonusDamage()) + (0.25 * Hero->TotalMagicDamage());

	if (Target->HealthPercent() > 40)
		InitDamage *= (1 + ((100 - Target->HealthPercent()) * 0.83) / 100);
	else
		InitDamage *= 2;

	// MAGIC DAMAGE: 100 / 140 / 180 (+ 65% bonus AD) (+ 25% AP)

	return GDamage->CalcMagicDamage(Hero, Target, InitDamage);
}


void Kogmaw::Combo()
{
	for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (Enemy->IsValidTarget() && !Enemy->IsDead())
		{
			auto Target = GTargetSelector->GetFocusedTarget() != nullptr ? GTargetSelector->GetFocusedTarget() : Enemy;
			auto flDistance = (Hero->GetPosition() - Target->GetPosition()).Length();

			if (flDistance <= GetWRange() && WinCombo->Enabled() && W->IsReady() && WinCombo->Enabled())
				W->CastOnPlayer();
			if (flDistance > Hero->AttackRange() + Hero->BoundingRadius() && Q->IsReady() && QinCombo->Enabled())
			{
				Q->CastOnTarget(Target, kHitChanceVeryHigh);
			}

			if (flDistance < GetRRange() && Target != nullptr && R->IsReady() && flDistance > Hero->AttackRange() + Hero->BoundingRadius()
				&& Target->HealthPercent() < 40 && (Hero->GetBuffCount("kogmawlivingartillerycost") < RMaxStacks->GetInteger() || CalcRDamage(Target) > Target->GetHealth()))
			{
				Vec3 CastPos;
				GPrediction->GetFutureUnitPosition(Target, 0.5, true, CastPos);
				R->CastOnPosition(CastPos);
				//GRender->Notification(Vec4(255, 255, 255, 255), 0, "%.f", CalcRDamage(Target));
			}

			if (E->IsReady() && flDistance < ECastDistance->GetFloat() && Hero->GetMana() - E->ManaCost() > W->ManaCost())
			{
				E->CastOnTarget(Target, kHitChanceVeryHigh);
			}
		}
	}
}

void Kogmaw::OnWaveClear()
{
	/*for (auto Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (Enemy->IsValidTarget() && !Enemy->IsDead())
		{
			auto Target = GTargetSelector->GetFocusedTarget() != nullptr ? GTargetSelector->GetFocusedTarget() : GTargetSelector->FindTarget(LowestHealthPriority, SpellDamage, FLT_MAX);
			auto flDistance = (Hero->GetPosition() - Target->GetPosition()).Length();

			if (Target != nullptr && R->IsReady() && Hero->GetBuffCount("kogmawlivingartillerycost") < 3 && flDistance > Hero->AttackRange() + Hero->BoundingRadius() && Hero->ManaPercent() > 60)
			{
				Vec3 CastPos;
				GPrediction->GetFutureUnitPosition(Target, 0.6, true, CastPos);
				R->CastOnPosition(CastPos);
			}
		}
	}*/
}

void Kogmaw::OnGameUpdate()
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		Combo();

	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
		OnWaveClear();


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
					for (auto Enemy : GEntityList->GetAllHeros(false, true))
					{
						if (Enemy->IsValidTarget() && !Enemy->IsDead())
						{
							auto Target = GTargetSelector->GetFocusedTarget() != nullptr ? GTargetSelector->GetFocusedTarget() : Enemy;
							auto flDistance = (Hero->GetPosition() - Target->GetPosition()).Length();

							if (flDistance < GetRRange() && Target != nullptr)
							{
								Vec3 CastPos;
								GPrediction->GetFutureUnitPosition(Target, 0.5, true, CastPos);
								R->CastOnPosition(CastPos);
							}
						}
					}
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
}

void Kogmaw::OnRender()
{
	if (DrawReady->Enabled())
	{
		if (Q->IsReady() && DrawQ->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }

		if (W->IsReady() && DrawW->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), GetWRange()); }

		if (E->IsReady() && DrawE->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }

		if (R->IsReady() && DrawR->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), GetRRange()); }
	}
	else
	{
		if (DrawQ->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }

		if (DrawW->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), GetWRange()); }

		if (DrawE->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), E->Range()); }

		if (DrawR->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), GetRRange()); }
	}
}

void Kogmaw::OnSpellCast(CastedSpell const& Args)
{
	if (Hero->ManaPercent() > RHarassMana->GetFloat() && GOrbwalking->GetOrbwalkingMode() == kModeLaneClear && Args.Caster_->IsHero() && Args.Caster_->IsEnemy(Hero) && Args.Caster_->IsValidTarget() && !Args.Caster_->IsDead())
	{
		if (Args.AutoAttack_ && R->IsReady() && Hero->GetBuffCount("kogmawlivingartillerycost") < RMaxStacksHarass->GetInteger())
		{
			auto flDistance = (Hero->GetPosition() - Args.Caster_->GetPosition()).Length();
			if (flDistance < GetRRange()) { R->CastOnTarget(Args.Caster_, kHitChanceVeryHigh); }
		}
	}
}

void Kogmaw::OnOrbwalkAttack(IUnit* Source, IUnit* Target)
{
	if (Target->IsHero() && GOrbwalking->GetOrbwalkingMode() == kModeCombo && Target->IsValidTarget() && !Target->IsDead())
	{
		if (QinCombo->Enabled()) { Q->CastOnTarget(Target, kHitChanceVeryHigh); }
	}
}

void Kogmaw::BeforeAttack(IUnit* Target)
{
	if (Target->IsHero() && GOrbwalking->GetOrbwalkingMode() == kModeCombo && Target->IsValidTarget() && !Target->IsDead())
	{
		if (WinCombo->Enabled()) { W->CastOnPlayer(); }
	}
}

void Kogmaw::OnNewPath(IUnit* Source, std::vector<Vec3> const& Path)
{
	/*if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear && Source->IsHero() && !Source->IsDead() && Source->IsEnemy(Hero)) //&& strstr(Args.Name_, "BasicAttack") && strstr(Args.Name_, "CritAttack")
	{
		if (R->IsReady())
		{
			Vec3 CastPos;
			GPrediction->GetFutureUnitPosition(Source, 1.2, true, CastPos);
			R->CastOnTarget(Source);
		}
	}*/
}
