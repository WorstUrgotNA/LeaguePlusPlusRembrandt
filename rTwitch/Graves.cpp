#include "Graves.h"

Graves::Graves(IMenu* Parent)
{
	//Create Spells
	Q = GPluginSDK->CreateSpell2(kSlotQ, kLineCast, true, false, kCollidesWithWalls);
	W = GPluginSDK->CreateSpell2(kSlotW, kTargetCast, true, false, kCollidesWithYasuoWall);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, false, false, kCollidesWithNothing);
	R = GPluginSDK->CreateSpell2(kSlotR, kLineCast, true, false, kCollidesWithYasuoWall);

	SemiManualKey = false;

	W->SetOverrideRadius(150);
	W->SetOverrideSpeed(1650);
	R->SetOverrideSpeed(500);
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

	GravesMenu = Parent->AddMenu("Graveytrain");
	ComboPercent = GravesMenu->AddFloat("R in Combo at HP%:", 0, 100, 30);
}

Graves::~Graves()
{
	//GravesMenu->Remove();
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
			if (flDistance <= range)
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
	//Initial R Damage Fully Calculated - apply resists
	auto Armor = Target->Armor();
	if (Armor > 0) // Arm pen %
	{
		//Hero->ArmorPenetrationPercent();
	}
	//auto FlatArmorPenetration = Hero->ArmorPenetrationFlat ×(0.6 + 0.4 × Target's level ÷ 18)

	auto Multiplier = Armor > 0 ? (100/(100+Armor)) : 2 - (100/(100-Armor));
	auto FinalDamage = InitDamage * Multiplier;

	GRender->Notification(Vec4(255, 255, 255, 255), 0, "R Damage: %i", static_cast<int>(GDamage->CalcPhysicalDamage(Hero, Target, InitDamage))); //
	return FinalDamage;
}

void Graves::Combo()
{
	if (StackIndex < 0)
	{
		if (R->IsReady())
		{
			for (auto enemy : GEntityList->GetAllHeros(false, true))
			{
				if ((enemy->GetPosition() - GEntityList->Player()->GetPosition()).Length() < 1500 && enemy->GetHealth() - CalcRDamage(enemy) < 0)
					R->CastOnTarget(enemy, kHitChanceHigh);
			}
			//auto Target = GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 1500);

			
		}

		if (W->IsReady() && EnemiesInRange(GEntityList->Player(), 950) > 0)
		{
			W->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 950));
		}
	}
}

void Graves::OnGameUpdate()
{
	if (StackIndex >= 0)
	{
		
		if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && (StackIndex > 4 || EnemiesInRange(GEntityList->Player(), 1500) == 0))
			ClearStack();

		if (Stack[StackIndex] == "GravesQLineSpell")
		{
			Q->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 1000), kHitChanceHigh);
		}
		else if (Stack[StackIndex] == "GravesChargeShot")
		{
			R->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 1500), kHitChanceHigh);
		}
		else if (Stack[StackIndex] == "GravesMove")
		{
			E->CastOnPosition(GGame->CursorPosition());
			if (GEntityList->Player()->GetSpellRemainingCooldown(kSlotE) > 0.1f)
			{
				GOrbwalking->ResetAA();
				StackIndex++;
			}
		}
		else //combo done
		{
			ClearStack();
		}
	}

	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		Combo();


	//key press
	keystate = GetAsyncKeyState(84); //smite key

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
					R->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, PhysicalDamage, 1500), kHitChanceHigh);
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

void Graves::OnSpellCast(CastedSpell const& Args)
{
	auto MyHero = GEntityList->Player();
		
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
	{
		if ((strstr(Args.Name_, "GravesBasicAttack") || strstr(Args.Name_, "GravesCritAttack")) && Args.Caster_ == MyHero && Args.Target_->IsHero()) //listen for autos
		{
			//GRender->Notification(Vec4(255, 255, 255, 255), 0, "%s", Args.Target_->GetObjectName());
			if (StackIndex < 0) // begin a combo
			{
				int StackPushIndex = 0;

				if (E->IsReady())
				{
					ComboTarget = Args.Target_;
					if (Q->IsReady()) // add Q to combo array
					{
						Stack[StackPushIndex] = "GravesQLineSpell";
						StackPushIndex++;
					}
					if (R->IsReady()) // add R to combo array
					{
						if ((ComboTarget->GetHealth() - CalcRDamage(ComboTarget)) < ComboTarget->GetMaxHealth() * (ComboPercent->GetFloat() / 100))
						{
							Stack[StackPushIndex] = "GravesChargeShot";
							StackPushIndex++;
						}
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
	if (GOrbwalking->GetOrbwalkingMode() == kModeLaneClear)
	{
		for (auto mob : GEntityList->GetAllMinions(false, false, true))
		{
			if (!mob->IsDead() && mob->IsValidTarget() && (mob->GetPosition() - MyHero->GetPosition()).Length() < 500)
			{
				if (Q->IsReady())
					Q->CastOnUnit(mob);
				if (W->IsReady())
					W->CastOnUnit(mob);
				if (E->IsReady())
				{
					E->CastOnPosition(GGame->CursorPosition());
					GOrbwalking->ResetAA();
				}
			}
		}
	}
}