#include "Graves.h"

Graves::Graves(IMenu* Parent)
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
	SemiManualMenuKey = GravesMenu->AddKey("Semi-Manual Ult Key:", 84);

	ComboQTypeOption = GravesMenu->AddSelection("Q Combo Usage:", 0, ComboQType);

	UseWCombo = GravesMenu->CheckBox("Use W in Combo:", true);

	UseQJungle = GravesMenu->CheckBox("Use Q in Jungle Clear:", true);
	QJungleMana = GravesMenu->AddFloat("Minimum Mana Percent:", 0, 100, 40);

	UseWJungle = GravesMenu->CheckBox("Use W in Jungle Clear:", true);
	WJungleMana = GravesMenu->AddFloat("Minimum Mana Percent:", 0, 100, 75);

	UseEJungle = GravesMenu->CheckBox("Use E in Jungle Clear:", true);
	EJungleMana = GravesMenu->AddFloat("Minimum Mana Percent:", 0, 100, 10);

	DrawReady = GravesMenu->CheckBox("Draw Only Ready Spells:", true);
	DrawQ = GravesMenu->CheckBox("Draw Q Range:", true);
	DrawW = GravesMenu->CheckBox("Draw W Range:", true);
	DrawE = GravesMenu->CheckBox("Draw E Range:", true);
	DrawR = GravesMenu->CheckBox("Draw R Range:", true);
}

Graves::~Graves()
{
	GravesMenu->Remove();
}

int Graves::EnemiesInRange(IUnit* Source, float range)
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
	auto Hero = GEntityList->Player();
	float InitDamage = 1.5 * Hero->BonusDamage();

	if (Hero->GetSpellLevel(kSlotR) == 1)
		InitDamage += 250;
	else if (Hero->GetSpellLevel(kSlotR) == 2)
		InitDamage += 400;
	else if (Hero->GetSpellLevel(kSlotR) == 3)
		InitDamage += 550;

	if ((Target->GetPosition() - Hero->GetPosition()).Length() > 1000)
		InitDamage *= 0.8f;
	
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


	return FinalDamage;
}

void Graves::Combo()
{
	if (R->IsReady())
	{
		for (auto enemy : GEntityList->GetAllHeros(false, true))
		{
			if (!enemy->IsClone() && !enemy->IsDead() && enemy->IsValidTarget() && (enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length() < 1500 && enemy->GetHealth() - CalcRDamage(enemy) < 0)
				R->CastOnTarget(enemy, kHitChanceVeryHigh);
		}
	}

	if (StackIndex < 0)
	{
		if (W->IsReady() && UseWCombo->Enabled() && EnemiesInRange(GEntityList->Player(), 950) > 0)
		{
			W->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 950), kHitChanceVeryHigh);
		}

		if (ComboQTypeOption->GetInteger() != 2 && Q->IsReady()) //cast if wall to bang
		{
			auto pLocal = GEntityList->Player();

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

			if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && (StackIndex > 4 || EnemiesInRange(GEntityList->Player(), 1500) == 0))
				ClearStack();
			else 
			{
				if (Stack[StackIndex] == "GravesQLineSpell")
				{
					Q->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 1000), kHitChanceVeryHigh);
				}
				else if (Stack[StackIndex] == "GravesChargeShot")
				{
					if (GEntityList->Player()->GetSpellRemainingCooldown(kSlotR) > 0.1f)
					{
						StackIndex++;
					}
					R->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 1500), kHitChanceVeryHigh);
				}
				else if (Stack[StackIndex] == "GravesMove")
				{
					E->CastOnPosition(GGame->CursorPosition());
					if (GEntityList->Player()->GetSpellRemainingCooldown(kSlotE) > 0.1f)
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
					if ((Mob->GetPosition() - GEntityList->Player()->GetPosition()).Length() < 500 && UseQJungle->Enabled() && Q->IsReady() && GEntityList->Player()->ManaPercent() > QJungleMana->GetFloat())
					{
						Q->CastOnUnit(Mob);
					}

					if ((Mob->GetPosition() - GEntityList->Player()->GetPosition()).Length() < W->Range() && UseWJungle->Enabled() && W->IsReady() && GEntityList->Player()->ManaPercent() > WJungleMana->GetFloat())
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

	if (DrawReady->Enabled())
	{
		if (Q->IsReady() && DrawQ->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }

		if (W->IsReady() && DrawW->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }

		if (R->IsReady() && DrawR->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	}
	else
	{
		if (DrawQ->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range()); }

		if (DrawW->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }

		if (DrawR->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), R->Range()); }
	}
}

void Graves::OnSpellCast(CastedSpell const& Args)
{
	auto MyHero = GEntityList->Player();
	float AttackDamage = GEntityList->Player()->PhysicalDamage() + GEntityList->Player()->BonusDamage();
		
	if (Args.Target_ != nullptr && GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		if ((strstr(Args.Name_, "GravesBasicAttack") || strstr(Args.Name_, "GravesCritAttack")) && Args.Caster_ == MyHero && Args.Target_->IsHero()) //listen for autos
		{
			//GRender->Notification(Vec4(255, 255, 255, 255), 0, "%s", Args.Target_->GetObjectName());
			if (StackIndex < 0) // begin a combo
			{
				int StackPushIndex = 0;
				ComboTarget = Args.Target_;

				if (E->IsReady() && R->IsReady() && (ComboTarget->GetHealth() - (GDamage->CalcPhysicalDamage(MyHero, ComboTarget, 2 * AttackDamage) + CalcRDamage(ComboTarget)) < 0))
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
						//GGame->PrintChat("Pushing Q to Stack");
						Stack[StackPushIndex] = "GravesQLineSpell";
						StackPushIndex++;
						//Q->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 1000), kHitChanceLow);
						//Q->CastOnTarget(ComboTarget);
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
			if (UseEJungle->Enabled() && E->IsReady() && MyHero->ManaPercent() > EJungleMana->GetFloat())
			{
				E->CastOnPosition(GGame->CursorPosition());
				GOrbwalking->ResetAA();
			}
		}
	}
}

