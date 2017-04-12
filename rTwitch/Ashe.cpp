#include "Ashe.h"
#include "Rembrandt.h"


Ashe::~Ashe()
{
	AsheMenu->Remove();
}

Ashe::Ashe(IMenu* Parent, IUnit* Hero) :Champion(Parent, Hero)
{
	EnemyHeros = GEntityList->GetAllHeros(false, true);

	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, (kCollidesWithYasuoWall));
	Q->SetSkillshot(0.25f, 0.f, 2500.f, 0.f);

	W = GPluginSDK->CreateSpell2(kSlotW, kLineCast, true, true, (kCollidesWithYasuoWall | kCollidesWithHeroes | kCollidesWithMinions));
	W->SetSkillshot(0.2f, 57.5f, 2000.f, 1200.f);

	E = GPluginSDK->CreateSpell2(kSlotE, kLineCast, true, false, (kCollidesWithNothing));
	E->SetSkillshot(0.25f, 300.f, 1400.f, 25000.f);

	R = GPluginSDK->CreateSpell2(kSlotR, kLineCast, true, false, (kCollidesWithYasuoWall | kCollidesWithHeroes));
	R->SetSkillshot(0.25f, 130.f, 1600.f, 0.f);

	AsheMenu = Parent->AddMenu("Ashe PRO++");

	//DRAW MENU
	DrawMenu = AsheMenu->AddMenu("++ Drawings");
	DrawReady = DrawMenu->CheckBox("Draw Only Ready Spells:", true);
	DrawQ = DrawMenu->CheckBox("Draw Q Range:", true);
	QColor = DrawMenu->AddColor("Q Range Color:", 255, 255, 0, 255);
	DrawW = DrawMenu->CheckBox("Draw W Range:", true);
	WColor = DrawMenu->AddColor("W Range Color:", 255, 255, 0, 255);
	DrawE = DrawMenu->CheckBox("Draw E Range:", true);
	EColor = DrawMenu->AddColor("E Range Color:", 255, 255, 0, 255);
	DrawR = DrawMenu->CheckBox("Draw R Range:", true);
	RColor = DrawMenu->AddColor("R Range Color:", 255, 255, 0, 255);

	ComboMenu = AsheMenu->AddMenu("++ Combo Menu");
	QInCombo = ComboMenu->CheckBox("Use Q in Combo:", true);
	WInCombo = ComboMenu->CheckBox("Use W in Combo:", true);
	RInCombo = ComboMenu->CheckBox("Use R in Combo:", true);
	RInComboRange = ComboMenu->AddFloat("Auto R if Enemy in X Range:", 100, 2000, 600);
	SemiManualMenuKey = ComboMenu->AddKey("R Semi-Manual Key:", 84);

	HarassMenu = AsheMenu->AddMenu("++ Harass");
	WInHarass = HarassMenu->CheckBox("Use W for Harass:", true);
	WInHarassMana = HarassMenu->AddFloat("Min Mana % W Harass:", 0, 100, 60);

	LaneClearMenu = AsheMenu->AddMenu("++ Lane Clear");
	QInLaneClear = LaneClearMenu->CheckBox("Use Q to Farm:", false);
	QInLaneClearNearbyUnits = LaneClearMenu->AddInteger("Only Q if X Minions Nearby:", 1, 10, 3);
	QInLaneClearMana = LaneClearMenu->AddFloat("Min Mana % to Q Farm:", 0, 100, 60);

	ExtraMenu = AsheMenu->AddMenu("++ Bonus Features");
	EFow = ExtraMenu->CheckBox("Use E on Enemy Fleeing into FoW:", true);

	SemiManualKey = false;
	LastAttackTime = 0;
	ComboTarget = nullptr;
}

void Ashe::Combo()
{
	bool EnemesInAttackRange = Rembrandt::UnitsInRange(Hero->GetPosition(), EnemyHeros, Hero->AttackRange() + Hero->BoundingRadius());

	if (QInCombo->Enabled() && EnemesInAttackRange && Q->CastOnPlayer()) return;

	if (WInCombo->Enabled() && !EnemesInAttackRange)
		if (W->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, W->Range()))) return;

	if (RInCombo->Enabled() && R->CastOnTarget(GTargetSelector->FindTarget(ClosestPriority, PhysicalDamage, RInComboRange->GetFloat()))) return;
}

void Ashe::LaneClear()
{
	if (QInLaneClear->Enabled() && Hero->ManaPercent() > QInLaneClearMana->GetFloat() && Q->IsReady()
		&& Rembrandt::UnitsInRange(Hero->GetPosition(), GEntityList->GetAllMinions(false, true, true), 1000) >= QInLaneClearNearbyUnits->GetInteger()
		&& Q->CastOnPlayer()) return;

	if (WInHarass->Enabled() && Hero->ManaPercent() > WInHarassMana->GetFloat())
		if (W->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, W->Range()))) return;
}

void Ashe::OnGameUpdate()
{
	//key press
	keystate = GetAsyncKeyState(SemiManualMenuKey->GetInteger()); //menu key

	if (GUtility->IsLeagueWindowFocused() && !GGame->IsChatOpen())
	{
		if (keystate < 0) // If most-significant bit is set...
		{
			// key is down . . .
			if (!SemiManualKey)
			{
				R->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, FLT_MAX));
				SemiManualKey = true;
			}
		}
		else
		{
			// key is up . . .
			SemiManualKey = false;
		}
	}

	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
		LaneClear();
	else if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		Combo();
}

void Ashe::OnRender()
{
	
}

void Ashe::OnSpellCast(CastedSpell const& Args)
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && Args.Caster_ == Hero && Args.AutoAttack_ && Args.Target_->IsHero() && Args.Target_->IsValidTarget())
	{
		if (WInCombo->Enabled())
			if (W->CastOnTarget(Args.Target_)) return;

		ComboTarget = Args.Target_;
		LastAttackTime = GGame->Time();
	}
}

void Ashe::OnExitVisible(IUnit* Source)
{
	if (EFow->Enabled() && ComboTarget != nullptr && ComboTarget == Source && Hero->IsFacing(Source) && GGame->Time() - LastAttackTime < 3 && E->CastOnPosition(Source->GetPosition())) return;
}