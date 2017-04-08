#include "Graves.h"
#include "Rembrandt.h"

Graves::~Graves()
{
	GravesMenu->Remove();
}

Graves::Graves(IMenu* Parent, IUnit* Hero):Champion(Parent, Hero)
{
	//Create Spells
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, (kCollidesWithYasuoWall));
	Q->SetSkillshot(0.25, 60, 3000, 825);
	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, true, false, (kCollidesWithYasuoWall));
	W->SetSkillshot(0.25, 250, 1000, 950);
	E = GPluginSDK->CreateSpell2(kSlotE, kCircleCast, false, false, (kCollidesWithNothing));
	E->SetSkillshot(0.25, 100, 1000, 425);
	R = GPluginSDK->CreateSpell2(kSlotR, kLineCast, true, false, (kCollidesWithYasuoWall));
	R->SetSkillshot(0.25, 100, 2100, 1100);
	SemiManualKey = false;

	ComboQType = { "Always", "Wallbang Only", "Off" };

	//Initialize Stack
	Stack = {
		"",
		"",
		"",
		"",
		"",
		""
	};
	StackIndex = -1;

	//Menu
	GravesMenu = Parent->AddMenu("Graves PRO++");

	DrawMenu = GravesMenu->AddMenu("++ Drawings");
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

	ComboMenu = GravesMenu->AddMenu("++ Combo");
	ComboQTypeOption = ComboMenu->AddSelection("Q Combo Usage:", 0, ComboQType);
	UseWCombo = ComboMenu->CheckBox("Use W in Combo:", true);
	UseECombo = ComboMenu->CheckBox("Use E in Combo:", true);
	UseRCombo = ComboMenu->CheckBox("Use R in Combo:", true);
	SemiManualMenuKey = ComboMenu->AddKey("Semi-Manual Ult Key:", 84);

	HarassMenu = GravesMenu->AddMenu("++ Harass");

	LaneClearMenu = GravesMenu->AddMenu("++ Jungle Clear");
	UseQJungle = LaneClearMenu->CheckBox("Use Q in Jungle Clear:", true);
	QJungleMana = LaneClearMenu->AddFloat("Q Minimum Mana Percent:", 0, 100, 40);
	UseWJungle = LaneClearMenu->CheckBox("Use W in Jungle Clear:", true);
	WJungleMana = LaneClearMenu->AddFloat("W Minimum Mana Percent:", 0, 100, 75);
	UseEJungle = LaneClearMenu->CheckBox("Use E in Jungle Clear:", true);
	EJungleMana = LaneClearMenu->AddFloat("E Minimum Mana Percent:", 0, 100, 10);

	ExtraMenu = GravesMenu->AddMenu("++ Extra Settings");
}
 
int Graves::EnemiesInRange(IUnit* Source, float range)
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

void Graves::ClearStack()
{
	for (int j = 0; j < 6; j++) //clear stack
	{
		Stack[j] = "";
	}
	StackIndex = -1;
}

float Graves::CalcRDamage(IUnit* Target)
{
	float InitDamage = 1.5 * Hero->BonusDamage();

	InitDamage += 250 + 150 * (Hero-> GetSpellLevel(kSlotR) - 1);

	if (Rembrandt::LengthSqr(Target->GetPosition() - Hero->GetPosition()) > 1000000)
		InitDamage *= 0.8f;
	
	return GDamage->CalcPhysicalDamage(Hero, Target, InitDamage) * Rembrandt::DamageModifierFromMasteries(Hero, Target);
}

void Graves::Combo()
{
	if (UseRCombo->Enabled() &&	R->IsReady())
	{
		for (auto enemy : GEntityList->GetAllHeros(false, true))
		{
			if (!enemy->IsClone() && !enemy->IsDead() && enemy->IsValidTarget() && (enemy->GetPosition() - Hero->GetPosition()).Length() < 1500 && enemy->GetHealth() - CalcRDamage(enemy) < 0)
				R->CastOnTarget(enemy, kHitChanceVeryHigh);
		}
	}

	if (StackIndex < 0)
	{
		if (W->IsReady() && UseWCombo->Enabled() && EnemiesInRange(Hero, 950) > 0)
		{
			W->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 950), kHitChanceVeryHigh);
		}

		if (ComboQTypeOption->GetInteger() != 2 && Q->IsReady()) //cast if wall to bang
		{
			auto pLocal = Hero;

			for (auto enemy : GEntityList->GetAllHeros(false, true))
			{
				if (!enemy->IsClone() && enemy->IsValidTarget() && (enemy->GetPosition() - pLocal->GetPosition()).Length() < 950)
				{
					Vec3 EstimatedEnemyPos;
					GPrediction->GetFutureUnitPosition(enemy, 0.35, true, EstimatedEnemyPos);
					Vec3 EndPosition = pLocal->GetPosition() + (EstimatedEnemyPos - pLocal->GetPosition()).VectorNormalize() * 900;

					if (GNavMesh->IsPointWall(EndPosition))
						if (Q->CastOnTarget(enemy, kHitChanceLow)) { return; }
				}
			}
		}
	}
}

void Graves::OnGameUpdate()
{
	try
	{
		if (StackIndex >= 0)
		{

			if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && (StackIndex > 4 || EnemiesInRange(Hero, 1500) == 0))
				ClearStack();
			else 
			{
				if (Stack[StackIndex] == "GravesQLineSpell")
				{
					Q->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 1000), kHitChanceVeryHigh);
				}
				else if (Stack[StackIndex] == "GravesChargeShot")
				{
					if (Hero->GetSpellRemainingCooldown(kSlotR) > 0.1f)
					{
						StackIndex++;
					}
					R->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 1500), kHitChanceVeryHigh);
				}
				else if (Stack[StackIndex] == "GravesMove" && UseECombo->Enabled())
				{
					E->CastOnPosition(GGame->CursorPosition());
					if (Hero->GetSpellRemainingCooldown(kSlotE) > 0.1f)
					{
						GOrbwalking->ResetAA();
						ClearStack();
					}
				}
				else //combo done
				{
					ClearStack();
				}
			}
		}

		if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
			Combo();

		if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
		{
			for (auto Mob : GEntityList->GetAllMinions(false, true, true))
			{
				if (!Mob->IsDead() && Mob->IsValidTarget() && !Mob->IsWard() && (Mob->IsCreep() || Mob->IsJungleCreep()))
				{
					if ((Mob->GetPosition() - Hero->GetPosition()).Length() < 500 && UseQJungle->Enabled() && Q->IsReady() && Hero->ManaPercent() > QJungleMana->GetFloat())
					{
						Q->CastOnUnit(Mob);
					}

					if ((Mob->GetPosition() - Hero->GetPosition()).Length() < W->Range() && UseWJungle->Enabled() && W->IsReady() && Hero->ManaPercent() > WJungleMana->GetFloat())
						W->CastOnTarget(Mob);
				}
			}
		}

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
						R->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 1500), kHitChanceVeryHigh);
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
	catch (const std::exception& e)
	{
		GUtility->CreateDebugConsole();
		GUtility->LogConsole(e.what());
		GUtility->LogFile("UtilityPlusPlusExceptions", "%s", e.what());
		throw;
	}
}

void Graves::OnRender()
{ 
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

void Graves::OnSpellCast(CastedSpell const& Args)
{
	float AttackDamage = Hero->PhysicalDamage() + Hero->BonusDamage();
		
	if (Args.Target_ != nullptr && GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		if ((strstr(Args.Name_, "GravesBasicAttack") || strstr(Args.Name_, "GravesCritAttack")) && Args.Caster_ == Hero && Args.Target_->IsHero()) //listen for autos
		{
			if (StackIndex < 0) // begin a combo
			{
				int StackPushIndex = 0;
				ComboTarget = Args.Target_;

				if (UseRCombo->Enabled() && E->IsReady() && R->IsReady() && (ComboTarget->GetHealth() - (GDamage->CalcPhysicalDamage(Hero, ComboTarget, 2 * AttackDamage) + CalcRDamage(ComboTarget)) < 0))
				{
					Stack[StackPushIndex] = "GravesChargeShot";
					StackPushIndex++;
					Stack[StackPushIndex] = "GravesMove";

					StackIndex = 0; // tell the stack it has items to commense
				}
				else if (E->IsReady())
				{
					if (ComboQTypeOption->GetInteger() == 0 && Q->IsReady()) // add Q to combo array
					{
						Stack[StackPushIndex] = "GravesQLineSpell";
						StackPushIndex++;
					}

					Stack[StackPushIndex] = "GravesMove";

					StackIndex = 0; // tell the stack it has items to commense
				}
			}
		}
		if (strstr(Args.Name_, "GravesChargeShot") && StackIndex >= 0 && Stack[StackIndex] == "GravesChargeShot")
		{
			StackIndex++;
		}
		else if (strstr(Args.Name_, "GravesQLineSpell") && StackIndex >= 0 && Stack[StackIndex] == "GravesQLineSpell")
		{
			StackIndex++;
		}
	}
	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear && (strstr(Args.Name_, "GravesBasicAttack") || strstr(Args.Name_, "GravesCritAttack")))
	{
		if (Args.Target_ != nullptr && Args.Target_->IsJungleCreep() && Args.Target_->IsValidTarget() && !Args.Target_->IsDead())
		{
			if (UseEJungle->Enabled() && E->IsReady() && Hero->ManaPercent() > EJungleMana->GetFloat())
			{
				E->CastOnPosition(GGame->CursorPosition());
				GOrbwalking->ResetAA();
			}
		}
	}
}

