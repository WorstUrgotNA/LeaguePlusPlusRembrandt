#include "Lucian.h"
#include "Rembrandt.h"

Lucian::~Lucian()
{
	LucianMenu->Remove();
}

Lucian::Lucian(IMenu* Parent, IUnit* Hero) :Champion(Parent, Hero)
{
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	Q->SetSkillshot(0.4f, 65.f, 1600, 675);
	QExtended = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, kCollidesWithNothing);
	QExtended->SetSkillshot(0.4f, 65.f, 1600, 900);

	W = GPluginSDK->CreateSpell2(kSlotW, kLineCast, true, false, kCollidesWithYasuoWall);
	W->SetSkillshot(0.25f, 80.f, 1600.f, 900.f);

	E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, false, false, kCollidesWithNothing);
	E->SetSkillshot(0.25f, 425.f, 1000.f, 425.f);

	R = GPluginSDK->CreateSpell2(kSlotR, kLineCast, true, false, kCollidesWithYasuoWall);
	R->SetSkillshot(0.5f, 110.f, 2800.f, 1200.f);

	LucianMenu = Parent->AddMenu("Lucian PRO++");
	
	//DRAW MENU
	DrawMenu = LucianMenu->AddMenu("++ Drawings");
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
	DrawSpellFarm = DrawMenu->CheckBox("Draw Spell Farm:", true);

	ComboMenu = LucianMenu->AddMenu("++ Combo Menu");
	QInCombo = ComboMenu->CheckBox("Use Q in Combo:", true);
	WInCombo = ComboMenu->CheckBox("Use W in Combo:", true);
	EInCombo = ComboMenu->CheckBox("Use E in Combo:", true);
	RInCombo = ComboMenu->CheckBox("Use R in Combo:", true);
	RLock = ComboMenu->CheckBox("Lock to R Target:", true);

	HarassMenu = LucianMenu->AddMenu("++ Harass");
	HarrasQ = HarassMenu->CheckBox("Use Q for Harass:", true);
	ExtendedQHarass = HarassMenu->CheckBox("Use Extended Q Harass:", true);
	HarassQMana = HarassMenu->AddFloat("Min Mana % to Harass:", 0, 100, 40);

	LaneClearMenu = LucianMenu->AddMenu("++ Lane Clear");
	SpellFarmToggle = LaneClearMenu->AddKey("Toggle Spell Farm:", 4);
	SpellFarmEnabled = LaneClearMenu->CheckBox("Enable Spell Farming:", false);
	QSpellFarm = LaneClearMenu->CheckBox("Use Q to Farm:", true);
	MinimumMinionsToQ = LaneClearMenu->AddInteger("Minimum Minions to Q:", 1, 8, 4);
	QFarmMana = LaneClearMenu->AddFloat("Q Farm Min Mana %:", 0, 100, 50);
	WSpellFarm = LaneClearMenu->CheckBox("Use W to Farm:", false);
	WFarmMana = LaneClearMenu->AddFloat("W Farm Min Mana %:", 0, 100, 70);
	ESpellFarm = LaneClearMenu->CheckBox("Use E to Farm:", false);
	EFarmMana = LaneClearMenu->AddFloat("E Farm Min Mana %:", 0, 100, 50);
	QInJGClear = LaneClearMenu->CheckBox("Use Q in JG Clear:", true);
	QJGMana = LaneClearMenu->AddFloat("Q JG Clear Min Mana %:", 0, 100, 20);
	WInJGClear = LaneClearMenu->CheckBox("Use W in JG Clear:", true);
	WJGMana = LaneClearMenu->AddFloat("W JG Clear Min Mana %:", 0, 100, 20);
	EInJGClear = LaneClearMenu->CheckBox("Use E in JG Clear:", true);
	EJGMana = LaneClearMenu->AddFloat("E JG Clear Min Mana %:", 0, 100, 50);

	SpellFarm = false;
}

double Lucian::CalcRDamage(IUnit* Target)
{
	if (Hero->GetSpellLevel(kSlotR) == 0 || !Target) return 0;

	auto InitDamage = std::vector<double>({ 400, 875, 1500 }).at(Hero->GetSpellLevel(kSlotR) - 1)
		+ std::vector<double>({ 4, 5, 6 }).at(Hero->GetSpellLevel(kSlotR) - 1) * Hero->TotalPhysicalDamage()
		+ std::vector<double>({ 2, 2.5, 3 }).at(Hero->GetSpellLevel(kSlotR) - 1) * Hero->TotalMagicDamage();

	return GDamage->CalcPhysicalDamage(Hero, Target, InitDamage * Rembrandt::DamageModifierFromMasteries(Hero, Target));

	// TOTAL NON-MINION DAMAGE: 400 / 875 / 1500 (+ 400 / 500 / 600% AD) (+ 200 / 250 / 300% AP) 
}

bool Lucian::CastE(IUnit* Target)
{
	if (!E->IsReady() || !EInCombo->Enabled()) return false;

	if (Target->IsFacing(Hero) && Hero->IsFacing(Target))
	{
		Vec3 NewPosition = Hero->GetPosition().Extend(GGame->CursorPosition(), 10);
		if (Rembrandt::IsInRange(Target->GetPosition(), NewPosition,  Hero->AttackRange() + Hero->BoundingRadius()))
			if (E->CastOnPosition(NewPosition)) return true;
	}

	if (E->CastOnPosition(GGame->CursorPosition())) return true;

	return false;
}

void Lucian::LaneClear()
{
	if (HarrasQ->Enabled() && Hero->ManaPercent() > HarassQMana->GetFloat()) //EXTENDED Q
	{
		if (Q->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 675))) return;

		if (ExtendedQHarass->Enabled() && Q->IsReady())
		{
			auto QTar = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 900);
			if (QTar)
			{
				Vec3 FuturePos;
				GPrediction->GetFutureUnitPosition(QTar, Q->GetDelay(), true, FuturePos);

				auto ExtendedCastPos = Hero->GetPosition().Extend(FuturePos, 1000);
				for (auto Minion : GEntityList->GetAllMinions(false, true, false))
				{
					if (Hero->IsValidTarget(Minion, 675))
						if ((Hero->GetPosition().Extend(Minion->GetPosition(), 1000) - ExtendedCastPos).Length2D() < 65)
							if (Q->CastOnTarget(Minion)) return;
				}
			}
		}
	}

	if (SpellFarmEnabled->Enabled() && Hero->ManaPercent() > QFarmMana->GetFloat())
	{
		if (QSpellFarm->Enabled())
		{
			FarmLocation Output;
			std::vector<Vec3> FarmPositions;
			FarmPositions.push_back(Hero->GetPosition());
			Rembrandt::FindBestLineCastPosition(FarmPositions, QExtended->Range(), Q->Range(), 65, true, true, Output);

			if (Output.HitCount >= MinimumMinionsToQ->GetInteger())
				if (Q->CastOnTarget(Output.CastOnUnit)) return;
		}
	}
}

void Lucian::OnGameUpdate()
{
	//key press
	keystate = GetAsyncKeyState(SpellFarmToggle->GetInteger()); //smite key

	if (GUtility->IsLeagueWindowFocused() && !GGame->IsChatOpen())
	{
		if (keystate < 0) // If most-significant bit is set...
		{
			// key is down . . .
			if (SpellFarm == false)
			{
				//toggle
				if (SpellFarmEnabled->GetInteger() == 0) { SpellFarmEnabled->UpdateInteger(1); }
				else { SpellFarmEnabled->UpdateInteger(0); }
				SpellFarm = true;
			}
		}
		else
		{
			// key is up . . .
			SpellFarm = false;
		}
	}

	//Credits OKTW & Sebby
	if (RInCombo->Enabled() && R->IsReady())
	{
		auto Target = GTargetSelector->FindTarget(ClosestPriority, PhysicalDamage, R->Range());

		if (Target && !Rembrandt::IsInRange(Hero->GetPosition(), Target->GetPosition(), 500))
		{
			auto rDmg = CalcRDamage(Target);
			auto flDistance = (Hero->GetPosition() - Target->GetPosition()).Length2D();

			if (rDmg * 0.8 > Target->GetHealth() && flDistance < 700 && !Q->IsReady())
				if (R->CastOnPosition(Target->GetPosition())) return;
			else if (rDmg * 0.7 > Target->GetHealth() && flDistance < 800)
				if (R->CastOnPosition(Target->GetPosition())) return;
			else if (rDmg * 0.6 > Target->GetHealth() && flDistance < 900)
				if (R->CastOnPosition(Target->GetPosition())) return;
			else if (rDmg * 0.5 > Target->GetHealth() && flDistance < 1000)
				if (R->CastOnPosition(Target->GetPosition())) return;
			else if (rDmg * 0.4 > Target->GetHealth() && flDistance < 1100)
				if (R->CastOnPosition(Target->GetPosition())) return;
			else if (rDmg * 0.3 > Target->GetHealth() && flDistance < 1200)
				if (R->CastOnPosition(Target->GetPosition())) return;
		}
	}
	

	if (RLock->Enabled() && Hero->HasBuff("lucianr"))
	{
		auto LockTar = GTargetSelector->FindTarget(ClosestPriority, PhysicalDamage, R->Range());

		if (LockTar && LockTar->IsValidTarget() && Hero->IsFacing(LockTar))
		{
			auto flDist = (Hero->GetPosition() - LockTar->GetPosition()).Length2D();
			auto flDistToCursor = (LockTar->GetPosition() - GGame->CursorPosition()).Length2D();
			if (flDistToCursor > flDist)
				flDist += flDist / 3;
			else
				flDist -= flDist / 3;

			Vec3 pos;
			GPrediction->GetFutureUnitPosition(LockTar, 0.11f, true, pos);
			if (GGame->IssueOrder(Hero, kMoveTo, pos.Extend(Hero->GetPosition(), flDist))) return;
		}
	}

	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
		LaneClear();
}

void Lucian::OnRender()
{
	Vec2 TextPos;
	if (DrawSpellFarm->Enabled() && GGame->Projection(Hero->GetPosition(), &TextPos))
	{
		TextPos.y += 100;
		GRender->DrawTextW(TextPos, Vec4(255, 255, 255, 255), "%s", SpellFarmEnabled->Enabled() ? "SPELLFARM: ON" : "SPELLFARM: OFF");
	}

	if (R->IsReady() && DrawRDamage->Enabled())
	{
		Vec4 BarColor;
		DrawRDamageColor->GetColor(&BarColor);

		for (auto Enemy : GEntityList->GetAllHeros(false, true))
		{
			if (Enemy->IsOnScreen() && Enemy->IsVisible() && Hero->IsValidTarget(Enemy, R->Range()))
				Rembrandt::DrawDamageOnChampionHPBar(Enemy, CalcRDamage(Enemy), "R", BarColor);
		}
	}

	if (DrawReady->Enabled())
	{
		Vec4 CircleColor;
		QColor->GetColor(&CircleColor);
		if (Q->IsReady() && DrawQ->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, 575); }
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
		if (DrawQ->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, 575); }
		WColor->GetColor(&CircleColor);
		if (DrawW->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, W->Range()); }
		EColor->GetColor(&CircleColor);
		if (DrawE->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, E->Range()); }
		RColor->GetColor(&CircleColor);
		if (DrawR->Enabled()) { GRender->DrawOutlinedCircle(Hero->GetPosition(), CircleColor, R->Range()); }
	}
}

void Lucian::OnSpellCast(CastedSpell const& Args)
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && Args.Caster_ == Hero && Args.AutoAttack_ && Args.Target_->IsHero())
	{
		if (CastE(Args.Target_))
		{
			GOrbwalking->ResetAA();
			return;
		}
		if (QInCombo->Enabled() && Q->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range()))) return;
		if (WInCombo->Enabled() && W->CastOnUnit(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, W->Range()))) return;
	}

	if (SpellFarmEnabled->Enabled() && Hero->ManaPercent() > EFarmMana->GetFloat()&& GOrbwalking->GetOrbwalkingMode() == kModeLaneClear && Args.Caster_ == Hero && Args.AutoAttack_ && Args.Target_->IsCreep())
	{
		if (ESpellFarm->Enabled() && E->CastOnPosition(Hero->GetPosition().Extend(GGame->CursorPosition(), 10)))
		{
			GOrbwalking->ResetAA();
			return;
		}
		if (QSpellFarm->Enabled() && Hero->ManaPercent() > QFarmMana->GetFloat())
		{
			FarmLocation Output;
			std::vector<Vec3> FarmPositions;
			FarmPositions.push_back(Hero->GetPosition());
			Rembrandt::FindBestLineCastPosition(FarmPositions, QExtended->Range(), Q->Range(), 65, true, true, Output);

			if (Output.HitCount >= MinimumMinionsToQ->GetInteger())
				if (Q->CastOnTarget(Output.CastOnUnit)) return;
		}
		if (WSpellFarm->Enabled() && Hero->ManaPercent() > WFarmMana->GetFloat() && W->CastOnTarget(Args.Target_)) return;
	}

	if (SpellFarmEnabled->Enabled() && GOrbwalking->GetOrbwalkingMode() == kModeLaneClear && Args.Caster_ == Hero && Args.AutoAttack_ && Args.Target_->IsJungleCreep())
	{
		if (EInJGClear->Enabled() && Hero->ManaPercent() > EJGMana->GetFloat() && E->CastOnPosition(Hero->GetPosition().Extend(GGame->CursorPosition(), 10)))
		{
			GOrbwalking->ResetAA();
			return;
		}
		if (QInJGClear->Enabled() && Hero->ManaPercent() > QJGMana->GetFloat() && Args.Target_->IsValidTarget() && Q->CastOnTarget(Args.Target_)) return;
		if (WInJGClear->Enabled() && Hero->ManaPercent() > WJGMana->GetFloat() && Args.Target_->IsValidTarget() && W->CastOnUnit(Args.Target_)) return;
	}
}

void Lucian::OnLevelUp(IUnit* Source, int NewLevel)
{
		switch (NewLevel)
		{
		case 1:
			Q->SetOverrideDelay(0.4f);
		case 2:
			Q->SetOverrideDelay(0.39f);
		case 3:
			Q->SetOverrideDelay(0.38f);
		case 4:
			Q->SetOverrideDelay(0.37f);
		case 5:
			Q->SetOverrideDelay(0.36f);
		case 6:
			Q->SetOverrideDelay(0.36f);
		case 7:
			Q->SetOverrideDelay(0.35f);
		case 8:
			Q->SetOverrideDelay(0.34f);
		case 9:
			Q->SetOverrideDelay(0.33f);
		case 10:
			Q->SetOverrideDelay(0.32f);
		case 11:
			Q->SetOverrideDelay(0.31f);
		case 12:
			Q->SetOverrideDelay(0.3f);
		case 13:
			Q->SetOverrideDelay(0.29f);
		case 14:
			Q->SetOverrideDelay(0.29f);
		case 15:
			Q->SetOverrideDelay(0.28f);
		case 16:
			Q->SetOverrideDelay(0.27f);
		case 17:
			Q->SetOverrideDelay(0.26f);
		case 18:
			Q->SetOverrideDelay(0.25f);
		default:
			break;
		}

		QExtended->SetOverrideDelay(Q->GetDelay());
}