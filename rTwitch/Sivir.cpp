#include "Sivir.h"

Sivir::Sivir(IMenu* Parent)
{
	Hero = GEntityList->Player();

	//Create Spells
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithYasuoWall));
	Q->SetSkillshot(0.25, 90, 1350, 1250);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, false, false, (kCollidesWithNothing));
	W->SetSkillshot(0.25, 0, 1000, FLT_MAX);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, (kCollidesWithNothing));
	E->SetSkillshot(0.25, 0, 1000, FLT_MAX);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, false, true, (kCollidesWithNothing));
	R->SetSkillshot(0.25, 0, 1000, FLT_MAX);

	HitChance = { "Low", "Medium", "High", "Very High" };

	//Menu
	SivirMenu = Parent->AddMenu("Sivir PRO++");

	SpellFarmToggle = SivirMenu->AddKey("Toggle Spell Farm:", 4);
	FarmWithSpells = SivirMenu->CheckBox("Farm with Spells:", true);
	QHitChance = SivirMenu->AddSelection("Boomerang (Q) Hitchance:", 2, HitChance);
	QLaneClear = SivirMenu->CheckBox("Laneclear with Q:", true);
	WLaneClear = SivirMenu->CheckBox("Laneclear with W:", true);

	SpellFarm = false;

	DrawReady = SivirMenu->CheckBox("Draw Only Ready Spells:", true);
	DrawQ = SivirMenu->CheckBox("Draw Q Range:", true);

}

Sivir::~Sivir()
{
	SivirMenu->Remove();
}

void Sivir::HandleKeyPress()
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
				if (FarmWithSpells->GetInteger() == 0) { FarmWithSpells->UpdateInteger(1); }
				else { FarmWithSpells->UpdateInteger(0); }
				SpellFarm = true;
			}
		}
		else
		{
			// key is up . . .
			SpellFarm = false;
		}
	}
}

int Sivir::EnemiesInRange(IUnit* Source, float range)
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

void Sivir::LaneClear()
{
	if (FarmWithSpells->Enabled() && Q->IsReady())
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

void Sivir::Combo()
{
	if (GTargetSelector->GetFocusedTarget() != nullptr && Q->IsReady())
	{
		Q->CastOnTarget(GTargetSelector->GetFocusedTarget());
		return;
	}
	else if (Q->IsReady())
	{
		Q->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, Q->Range()), QHitChance->GetInteger() + 3);
	}
}

void Sivir::OnGameUpdate()
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		Combo();
	else if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
		LaneClear();

	HandleKeyPress();
}

void Sivir::OnRender()
{

	Vec2 TextPos;
	if (GGame->Projection(Hero->GetPosition(), &TextPos))
	{
		TextPos.y += 100;
		GRender->DrawTextW(TextPos, Vec4(255, 255, 255, 255), "%s", FarmWithSpells->Enabled() ? "SPELLFARM: ON" : "SPELLFARM: OFF");
	}

	if (DrawReady->Enabled())
	{
		if (Q->IsReady() && DrawQ->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	}
	else
	{
		if (DrawQ->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }
	}
}

void Sivir::OnSpellCast(CastedSpell const& Args)
{
	if (E->IsReady() && strstr(Args.Name_, "TwitchExpunge") && Args.Caster_->IsEnemy(Hero) && Hero->HasBuff("twitchdeadlyvenom"))
		E->CastOnPlayer();
	
}

void Sivir::OnOrbwalkAttack(IUnit* Source, IUnit* Target)
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear && FarmWithSpells->Enabled() && W->IsReady())
		W->CastOnPlayer();
	else if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && W->IsReady() && Target->IsHero())
		W->CastOnPlayer();
}

