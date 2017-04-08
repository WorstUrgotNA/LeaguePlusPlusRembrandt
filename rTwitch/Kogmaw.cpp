#include "Kogmaw.h"
#include "Rembrandt.h"

Kogmaw::~Kogmaw()
{
	KogmawMenu->Remove();
}

Kogmaw::Kogmaw(IMenu* Parent, IUnit* Hero) :Champion(Parent, Hero)
{
	Hero = GEntityList->Player();

	//Create Spells
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithYasuoWall | kCollidesWithHeroes | kCollidesWithMinions));
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

	//DRAW MENU
	DrawMenu = KogmawMenu->AddMenu("++ Drawings");
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

	ComboMenu = KogmawMenu->AddMenu("++ Combo");
	QinCombo = ComboMenu->CheckBox("Use Q in Combo:", true);
	WinCombo = ComboMenu->CheckBox("Use W in Combo:", true);
	EinCombo = ComboMenu->CheckBox("Use E in Combo:", true);
	ECastDistance = ComboMenu->AddFloat("E Maximum Cast Range:", 0, 1350, 1000);
	RinCombo = ComboMenu->CheckBox("Use R in Combo:", true);
	SemiManualMenuKey = ComboMenu->AddKey("Semi-Manual R Key:", 84);
	RMaxStacks = ComboMenu->AddInteger("R Max Stacks Combo:", 0, 10, 3);
	RKillSteal = ComboMenu->CheckBox("Force R KillSteal:", true);

	HarassMenu = KogmawMenu->AddMenu("++ Harass");
	RHarassEnable = HarassMenu->CheckBox("Use R on Enemy AA/Spellcast:", true);
	RMaxStacksHarass = HarassMenu->AddInteger("R Max Stacks Harass", 0, 10, 1);
	RHarassMana = HarassMenu->AddFloat("R Harass Min Mana %", 0, 100, 60);

	LaneClearMenu = KogmawMenu->AddMenu("++ Lane Clear");

	ExtraMenu = KogmawMenu->AddMenu("++ Extra Settings");
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

	InitDamage *= Target->HealthPercent() > 40 ? (1 + ((100 - Target->HealthPercent()) * 0.83) / 100) : 2;
	
	// MAGIC DAMAGE: 100 / 140 / 180 (+ 65% bonus AD) (+ 25% AP)

	return GDamage->CalcMagicDamage(Hero, Target, InitDamage) * Rembrandt::DamageModifierFromMasteries(Hero, Target);
}


void Kogmaw::Combo()
{
	for (auto const& Enemy : GEntityList->GetAllHeros(false, true))
	{
		if (Enemy->IsValidTarget())
		{
			auto flDistance = (Hero->GetPosition() - Enemy->GetPosition()).Length2D();

			if (flDistance <= GetWRange() && WinCombo->Enabled() && WinCombo->Enabled())
			{
				if (W->CastOnPlayer()) { return; }
			}
				
			if (flDistance > Hero->AttackRange() + Hero->BoundingRadius() && QinCombo->Enabled())
			{
				if (Q->CastOnTarget(Enemy, kHitChanceVeryHigh)) { return; }
			}

			if (RinCombo->Enabled() && flDistance < GetRRange() && R->IsReady() && flDistance > Hero->AttackRange() + Hero->BoundingRadius()
				&& Enemy->HealthPercent() < 40 && (Hero->GetBuffCount("kogmawlivingartillerycost") < RMaxStacks->GetInteger() || (CalcRDamage(Enemy) > Enemy->GetHealth() && RKillSteal->Enabled())))
			{
				Vec3 CastPos;
				GPrediction->GetFutureUnitPosition(Enemy, 0.5, true, CastPos);
				if ((CastPos - Hero->GetPosition()).Length() < GetRRange())
					if (R->CastOnPosition(CastPos)) { return; }
				//GRender->Notification(Vec4(255, 255, 255, 255), 0, "%.f", CalcRDamage(Target));
			}
		}
	}

	if (EinCombo->Enabled() && Hero->GetMana() - E->ManaCost() > W->ManaCost())
	{
		if (E->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, ECastDistance->GetFloat()), kHitChanceVeryHigh)) { return; }
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
	keystate = GetAsyncKeyState(SemiManualMenuKey->GetInteger()); //ult key

	if (GUtility->IsLeagueWindowFocused() && !GGame->IsChatOpen())
	{
		if (keystate < 0) // If most-significant bit is set...
		{
			// key is down . . .
			if (!SemiManualKey)
			{
				//toggle smite
				if (R->IsReady())
				{
					
					auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, GetRRange());
					if (Target)
					{
						auto flDistance = (Hero->GetPosition() - Target->GetPosition()).Length();

						Vec3 CastPos;
						GPrediction->GetFutureUnitPosition(Target, 0.5, true, CastPos);
						R->CastOnPosition(CastPos);
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
	if (R->IsReady() && DrawRDamage->Enabled())
	{
		Vec4 BarColor;
		DrawRDamageColor->GetColor(&BarColor);

		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (Enemy->IsOnScreen() && Enemy->IsVisible() && Hero->IsValidTarget(Enemy, GetRRange()))
				Rembrandt::DrawDamageOnChampionHPBar(Enemy, CalcRDamage(Enemy), "R", BarColor);
		}
	}

	if (DrawReady->Enabled())
	{
		Vec4 CircleColor;
		QColor->GetColor(&CircleColor);
		if (Q->IsReady() && DrawQ->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, Q->Range()); }
		WColor->GetColor(&CircleColor);
		if (W->IsReady() && DrawW->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, GetWRange()); }
		EColor->GetColor(&CircleColor);
		if (E->IsReady() && DrawE->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, E->Range()); }
		RColor->GetColor(&CircleColor);
		if (R->IsReady() && DrawR->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, GetRRange()); }
	}
	else
	{
		Vec4 CircleColor;
		QColor->GetColor(&CircleColor);
		if (DrawQ->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, Q->Range()); }
		WColor->GetColor(&CircleColor);
		if (DrawW->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, GetWRange()); }
		EColor->GetColor(&CircleColor);
		if (DrawE->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, E->Range()); }
		RColor->GetColor(&CircleColor);
		if (DrawR->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, GetRRange()); }
	}
}

void Kogmaw::OnSpellCast(CastedSpell const& Args)
{
	if (RHarassEnable->Enabled() && Hero->ManaPercent() > RHarassMana->GetFloat() && GOrbwalking->GetOrbwalkingMode() == kModeLaneClear && Args.Caster_->IsHero() && Args.Caster_->IsEnemy(Hero) && Args.Caster_->IsValidTarget() && !Args.Caster_->IsDead())
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
	if (Target->IsHero() && Hero->IsValidTarget(Target, GetRRange()) && RinCombo->Enabled() && R->IsReady() && CalcRDamage(Target) > Target->GetHealth() && RKillSteal->Enabled())
	{
		Vec3 CastPos;
		GPrediction->GetFutureUnitPosition(Target, 0.5, true, CastPos);
		if ((CastPos - Hero->GetPosition()).Length() < GetRRange())
			if (R->CastOnPosition(CastPos)) { return; }
	}

	if (Target->IsHero() && GOrbwalking->GetOrbwalkingMode() == kModeCombo && Target->IsValidTarget() && QinCombo->Enabled())
	{
		if (QinCombo->Enabled())
			if (Q->CastOnTarget(Target, kHitChanceVeryHigh))
				return;
	}
}

void Kogmaw::BeforeAttack(IUnit* Target)
{
	if (Target->IsHero() && GOrbwalking->GetOrbwalkingMode() == kModeCombo && Target->IsValidTarget())
	{
		if (WinCombo->Enabled()) { W->CastOnPlayer(); }
	}
}

