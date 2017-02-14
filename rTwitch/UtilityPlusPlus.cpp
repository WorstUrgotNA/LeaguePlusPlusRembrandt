#include "PluginSDK.h"
#include "Color.h"
#include "SkinHack.h"
#include "string"
#include "windows.h"
#include "Gui.h"
#include "DrawTowerRange.h"

PluginSetup("Utility PRO++ by Rembrandt");

IMenu* MainMenu;
IMenu* AutoSmiteMenu;
IMenu* Defensives;
IMenu* SummonerHealMenu;
IMenu* SummonerBarrierMenu;
IMenu* CleanseMenu;
IMenu* AutoLevelUp;
IMenu* Potions;
IMenu* DefensiveItems;
IMenu* FaceOfTheMountainMenu;
IMenu* RedemptionMenu;
IMenu* ZhonyasMenu;
IMenu* SeraphsMenu;
IMenu* LocketMenu;
IMenu* RanduinsMenu;
IMenu* OffensiveItems;
IMenu* GunbladeMenu;
IMenu* BotrkMenu;
IMenu* CutlassMenu;
IMenu* YoumuusMenu;
IMenu* GLP800Menu;
IMenu* AutoTrinketMenu;
IMenu* MikaelsMenu;
IMenu* CCFilter;

IMenuOption* SmiteActive;
IMenuOption* SmiteKey;
IMenuOption* HealActive;
IMenuOption* HealPercent;
IMenuOption* BarrierActive;
IMenuOption* BarrierPercent;
IMenuOption* CleanseActive;
IMenuOption* EnableAutoLevelUp;
IMenuOption* ALUQ;
IMenuOption* ALUW;
IMenuOption* ALUE;
IMenuOption* ALUR;
IMenuOption* ALUStartLevel;
IMenuOption* PotionsEnabled;
IMenuOption* PotionsPercent;
IMenuOption* FaceOfTheMountainEnabled;
IMenuOption* FaceOfTheMountainPercent;
IMenuOption* RedemptionEnabled;
IMenuOption* RedemptionPercent;
IMenuOption* ZhonyasEnabled;
IMenuOption* SeraphsEnabled;
IMenuOption* SeraphsPercent;
IMenuOption* LocketEnabled;
IMenuOption* LocketPercent;
IMenuOption* RanduinsEnabled;
IMenuOption* GunbladeEnabled;
IMenuOption* BotrkEnabled;
IMenuOption* CutlassEnabled;
IMenuOption* YoumuusEnabled;
IMenuOption* GLP800Enabled;
IMenuOption* AutoUpgradeTrinket;
IMenuOption* CleanseCharm;
IMenuOption* CleanseDisarm;
IMenuOption* CleanseFear;
IMenuOption* CleanseFlee;
IMenuOption* CleansePolymorph;
IMenuOption* CleanseSnare;
IMenuOption* CleanseTaunt;
IMenuOption* CleanseStun;
IMenuOption* CleanseExhaust;
IMenuOption* CleanseSuppression;
IMenuOption* CleanseBlind;
IMenuOption* CleanseTeamate1;
IMenuOption* CleanseTeamate2;
IMenuOption* CleanseTeamate3;
IMenuOption* CleanseTeamate4;
IMenuOption* CleanseTeamate5;

IUnit* CleanseTeamate01;
IUnit* CleanseTeamate02;
IUnit* CleanseTeamate03;
IUnit* CleanseTeamate04;
IUnit* CleanseTeamate05;



ISpell* HEAL;
ISpell* BARRIER;
ISpell* EXHAUST;
ISpell* CLEANSE;
ISpell* SMITE;

IInventoryItem* QSS;
IInventoryItem* Scimitar;
IInventoryItem* Mikaels;

IInventoryItem* HealthPotion;
IInventoryItem* RefillablePotion;
IInventoryItem* HuntersPotion;
IInventoryItem* CorruptingPotion;
IInventoryItem* Biscuit;

IInventoryItem* FaceOfTheMountain;
IInventoryItem* Redemption;
IInventoryItem* Zhonyas;
IInventoryItem* Seraphs;
IInventoryItem* Locket;
IInventoryItem* Randuins;

IInventoryItem* Gunblade;
IInventoryItem* Botrk;
IInventoryItem* Cutlass;
IInventoryItem* Youmuus;
IInventoryItem* GLP800;

IFont* SmiteEnabledFont;

int HumanizeDelayCleanse;

short keystate;
bool smiteKeyWasDown = false;

//  DEBUG CODE
//std::string SmiteDamage = std::to_string(GDamage->GetSummonerSpellDamage(GEntityList->Player(), minion, kSummonerSpellSmite)); // Converting the smite damage to a string
//GGame->PrintChat(SmiteDamage.data());

//std::string minionPredHealth = std::to_string(GHealthPrediction->GetPredictedHealth(minion, kLastHitPrediction, 200, 0));
//GGame->PrintChat(minionPredHealth.data());
//GGame->PrintChat("Smite cast on:");
//GGame->PrintChat(minion->GetObjectName());

#pragma region Events
void LoadSpells()
{


	auto PlayerSum1  = GPluginSDK->GetEntityList()->Player()->GetSpellName(kSummonerSlot1);
	auto PlayerSum2 = GPluginSDK->GetEntityList()->Player()->GetSpellName(kSummonerSlot2);

	if (strcmp(PlayerSum1, "SummonerSmite") == 0) { SMITE = GPluginSDK->CreateSpell(kSummonerSlot1, 500); }
	else if (strcmp(PlayerSum1, "SummonerHeal") == 0) { HEAL = GPluginSDK->CreateSpell(kSummonerSlot1, 850); }
	else if (strcmp(PlayerSum1, "SummonerBarrier") == 0) { BARRIER = GPluginSDK->CreateSpell(kSummonerSlot1, 0); }
	else if (strcmp(PlayerSum1, "SummonerExhaust") == 0) { EXHAUST = GPluginSDK->CreateSpell(kSummonerSlot1, 650); }
	else if (strcmp(PlayerSum1, "SummonerBoost") == 0) { CLEANSE = GPluginSDK->CreateSpell(kSummonerSlot1, 0); }

	if (strcmp(PlayerSum2, "SummonerSmite") == 0) { SMITE = GPluginSDK->CreateSpell(kSummonerSlot2, 500); }
	else if (strcmp(PlayerSum2, "SummonerHeal") == 0) { HEAL = GPluginSDK->CreateSpell(kSummonerSlot2, 850); }
	else if (strcmp(PlayerSum2, "SummonerBarrier") == 0) { BARRIER = GPluginSDK->CreateSpell(kSummonerSlot2, 0); }
	else if (strcmp(PlayerSum2, "SummonerExhaust") == 0) { EXHAUST = GPluginSDK->CreateSpell(kSummonerSlot2, 650); }
	else if (strcmp(PlayerSum2, "SummonerBoost") == 0) { CLEANSE = GPluginSDK->CreateSpell(kSummonerSlot2, 0); }

	//Cleansers
	QSS = GPluginSDK->CreateItemForId(3140, 0);
	Scimitar = GPluginSDK->CreateItemForId(3139, 0);
	Mikaels = GPluginSDK->CreateItemForId(3222, 600);
	//Potions
	HealthPotion = GPluginSDK->CreateItemForId(2003, 0);
	RefillablePotion = GPluginSDK->CreateItemForId(2031, 0);
	HuntersPotion = GPluginSDK->CreateItemForId(2032, 0);
	CorruptingPotion = GPluginSDK->CreateItemForId(2033, 0);
	Biscuit = GPluginSDK->CreateItemForId(2010, 0);
	//Defensives
	FaceOfTheMountain = GPluginSDK->CreateItemForId(3401, 600);
	Redemption = GPluginSDK->CreateItemForId(3107, 5500);
	Zhonyas = GPluginSDK->CreateItemForId(3157, 0);
	Seraphs = GPluginSDK->CreateItemForId(3040, 0);
	Locket = GPluginSDK->CreateItemForId(3190, 600);
	Randuins = GPluginSDK->CreateItemForId(3143, 400);
	//Offensive
	Gunblade = GPluginSDK->CreateItemForId(3146, 700);
	Botrk = GPluginSDK->CreateItemForId(3153, 550);
	Cutlass = GPluginSDK->CreateItemForId(3144, 550);
	Youmuus = GPluginSDK->CreateItemForId(3142, 0);
	GLP800 = GPluginSDK->CreateItemForId(3030, 800);


	SmiteEnabledFont = GRender->CreateFont("Tahoma", 18.f);

	SmiteEnabledFont->SetLocationFlags(kFontLocationCenter);
	SmiteEnabledFont->SetOutline(true);
}

float GetDistance(IUnit* source, IUnit* target)
{
	auto x1 = source->GetPosition().x;
	auto x2 = target->GetPosition().x;
	auto y1 = source->GetPosition().y;
	auto y2 = target->GetPosition().y;
	auto z1 = source->GetPosition().z;
	auto z2 = target->GetPosition().z;
	return static_cast<float>(sqrt(pow((x2-x1), 2.0) + pow((y2-y1), 2.0) + pow((z2-z1), 2.0)));
}

void AddTeamatesToCleanse()
{
	int indx = 0;
	auto Teamates = GEntityList->GetAllHeros(true, false);
	for (auto teamate : Teamates)
	{
		if (indx == 0)
		{
			indx++;
			CleanseTeamate1 = MikaelsMenu->CheckBox(teamate->ChampionName(), true);
			CleanseTeamate01 = teamate;
		}
		else if (indx == 1)
		{
			indx++;
			CleanseTeamate2 = MikaelsMenu->CheckBox(teamate->ChampionName(), true);
			CleanseTeamate02 = teamate;
		}
		else if (indx == 2)
		{
			indx++;
			CleanseTeamate3 = MikaelsMenu->CheckBox(teamate->ChampionName(), true);
			CleanseTeamate03 = teamate;
		}
		else if (indx == 3)
		{
			indx++;
			CleanseTeamate4 = MikaelsMenu->CheckBox(teamate->ChampionName(), true);
			CleanseTeamate04 = teamate;
		}
		else if (indx == 4)
		{
			CleanseTeamate5 = MikaelsMenu->CheckBox(teamate->ChampionName(), true);
			CleanseTeamate05 = teamate;
			return;
		}
	}
}

int EnemiesInRange(IUnit* Source, float range)
{
	auto Targets = GEntityList->GetAllHeros(false, true);
	auto enemiesInRange = 0;

	for (auto target : Targets)
	{
		if (target != nullptr)
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

bool InFountain(IUnit* Source)
{
	if (Source->GetPosition().x < 1276 && Source->GetPosition().z < 1344) { return true; }
	if (Source->GetPosition().x > 13540 && Source->GetPosition().z > 13470) { return true; }

	return false;
}

void UseDefensives()
{
	/*auto ActiveMissiles = GPluginSDK->GetEntityList()->GetAllMissiles(false, true);
	for (IUnit* ActiveMissile : ActiveMissiles)
	{			
		auto AttackerUnit = GMissileData->GetCaster(ActiveMissile);
		if (!(strstr(AttackerUnit->GetObjectName(), "Minion")) && GMissileData->GetTarget(ActiveMissile) == GPluginSDK->GetEntityList()->Player()) {
			if (strstr(AttackerUnit->GetObjectName(), "Turret"))
			{
				if ((GPluginSDK->GetEntityList()->Player()->GetHealth() - GDamage->GetAutoAttackDamage(AttackerUnit, GPluginSDK->GetEntityList()->Player(), false)) <= HealPercent->GetInteger())
				{
					if (HEAL != nullptr && HEAL->IsReady() && !(GPluginSDK->GetEntityList()->Player()->IsDead())) { HEAL->CastOnPlayer(); }
					GGame->PrintChat(GMissileData->GetName(ActiveMissile)); // debug lines
					GGame->PrintChat(std::to_string(GDamage->GetSpellDamage(AttackerUnit, GPluginSDK->GetEntityList()->Player(), GMissileData->GetSpellSlot(ActiveMissile))).data());
					GGame->PrintChat(std::to_string(GDamage->GetAutoAttackDamage(AttackerUnit, GPluginSDK->GetEntityList()->Player(), false)).data());
				}
				if ((GPluginSDK->GetEntityList()->Player()->GetHealth() - GDamage->GetAutoAttackDamage(AttackerUnit, GPluginSDK->GetEntityList()->Player(), false)) <= BarrierPercent->GetInteger())
				{
					if (BARRIER != nullptr && BARRIER->IsReady() && !(GPluginSDK->GetEntityList()->Player()->IsDead())) { BARRIER->CastOnPlayer(); }
					GGame->PrintChat(GMissileData->GetName(ActiveMissile)); // debug lines
					GGame->PrintChat(std::to_string(GDamage->GetSpellDamage(AttackerUnit, GPluginSDK->GetEntityList()->Player(), GMissileData->GetSpellSlot(ActiveMissile))).data());
					GGame->PrintChat(std::to_string(GDamage->GetAutoAttackDamage(AttackerUnit, GPluginSDK->GetEntityList()->Player(), false)).data());
				}
			}
		}
	}*/
	auto Hero = GEntityList->Player();

	//ITEMS
	if (FaceOfTheMountain->IsOwned() && FaceOfTheMountain->IsReady() && FaceOfTheMountainEnabled->Enabled() && !(Hero->IsDead()))
	{
		auto Teamates = GEntityList->GetAllHeros(true, false);
		for (IUnit* Teamate : Teamates)
		{
			if (!(Teamate->IsDead()) && FaceOfTheMountain->IsTargetInRange(Teamate) && Teamate->HealthPercent() <= FaceOfTheMountainPercent->GetFloat() && EnemiesInRange(Teamate, 800) > 0) { FaceOfTheMountain->CastOnTarget(Teamate); } //Cast on injured teamate
		}
	}

	if (Redemption->IsOwned() && Redemption->IsReady() && RedemptionEnabled->Enabled() && !(Hero->IsDead()))
	{
		auto Teamates = GEntityList->GetAllHeros(true, false);
		for (IUnit* Teamate : Teamates)
		{
			if (!(Teamate->IsDead()) && Redemption->IsTargetInRange(Teamate) && Teamate->HealthPercent() <= RedemptionPercent->GetFloat() && EnemiesInRange(Teamate, 800) > 0) { Redemption->CastOnPosition(Teamate->GetPosition()); } //Cast on injured teamate
		}
	}

	if (Seraphs->IsOwned() && Seraphs->IsReady() && SeraphsEnabled->Enabled() && !(Hero->IsDead()))
	{
		if (Hero->HealthPercent() <= SeraphsPercent->GetFloat() && EnemiesInRange(Hero, 800) > 0) { Seraphs->CastOnPlayer(); }

	}

	if (Locket->IsOwned() && Locket->IsReady() && LocketEnabled->Enabled() && !(Hero->IsDead()))
	{
		auto Teamates = GEntityList->GetAllHeros(true, false);
		for (IUnit* Teamate : Teamates)
		{
			if (!(Teamate->IsDead()) && Locket->IsTargetInRange(Teamate) && Teamate->HealthPercent() <= LocketPercent->GetFloat() && EnemiesInRange(Teamate, 600) > 0) { Locket->CastOnPlayer(); } //Cast on injured teamate
		}
	}


	//HEAL
	if (HEAL != nullptr && HEAL->IsReady() && HealActive->Enabled() && !(GPluginSDK->GetEntityList()->Player()->IsDead()))
	{
		if (GPluginSDK->GetEntityList()->Player()->HealthPercent() <= HealPercent->GetInteger()) { HEAL->CastOnPlayer(); } //Cast on self

		auto Teamates = GEntityList->GetAllHeros(true, false);
		for (IUnit* Teamate : Teamates)
		{
			if (!(Teamate->IsDead()) && GetDistance(Hero, Teamate) <= HEAL->GetSpellRange() && Teamate->HealthPercent() <= HealPercent->GetInteger() && EnemiesInRange(Teamate, 700) > 0) { HEAL->CastOnUnit(Teamate); } //Cast on injured teamate
		}
	}
	

	//BARRIER
	if (BARRIER != nullptr && BARRIER->IsReady() && BarrierActive->Enabled() && !(Hero->IsDead()) && Hero->HealthPercent() <= BarrierPercent->GetInteger() && EnemiesInRange(Hero, 700) > 0)
	{
		BARRIER->CastOnPlayer();
	}

	//Potions
	if (!(GEntityList->Player()->IsDead()) && PotionsEnabled->Enabled() && GEntityList->Player()->HealthPercent() <= PotionsPercent->GetFloat() && 
		!(GEntityList->Player()->HasBuff("RegenerationPotion") || GEntityList->Player()->HasBuff("ItemMiniRegenPotion") || GEntityList->Player()->HasBuff("ItemCrystalFlaskJungle") ||
			GEntityList->Player()->HasBuff("ItemCrystalFlask") || GEntityList->Player()->HasBuff("ItemDarkCrystalFlask")) )
	{
		if (HealthPotion->IsOwned() && HealthPotion->IsReady() && !InFountain(Hero)) {
			HealthPotion->CastOnPlayer();
		}
		if (RefillablePotion->IsOwned() && RefillablePotion->IsReady()) {
			RefillablePotion->CastOnPlayer();
		}
		if (HuntersPotion->IsOwned() && HuntersPotion->IsReady()) {
			HuntersPotion->CastOnPlayer();
		}
		if (CorruptingPotion->IsOwned() && CorruptingPotion->IsReady()) {
			CorruptingPotion->CastOnPlayer();
		}
		if (Biscuit->IsOwned() && Biscuit->IsReady() && !InFountain(Hero)) {
			Biscuit->CastOnPlayer();
		}
	}
	
}

void CheckKeyPresses()
{
	keystate = GetAsyncKeyState(SmiteKey->GetInteger());

	if (keystate < 0) // If most-significant bit is set...
	{
		// key is down . . .
		if (smiteKeyWasDown == false)
		{
			//toggle smite
			if (SmiteActive->GetInteger() == 0) { SmiteActive->UpdateInteger(1); }
			else { SmiteActive->UpdateInteger(0); }
			smiteKeyWasDown = true;
		}
	}
	else
	{
		// key is up . . .
		smiteKeyWasDown = false;
	}
}

void AutoSmite() // AUTO SMITE PRO BY REMBRANDT
{	
	if (SMITE != nullptr && SMITE->IsReady() && SmiteActive->Enabled()) {
		auto minions = GEntityList->GetAllMinions(false, false, true);
		for (IUnit* minion : minions)
		{
			if (strstr(minion->GetObjectName(), "Red") || strstr(minion->GetObjectName(), "Blue") || strstr(minion->GetObjectName(), "Dragon") || strstr(minion->GetObjectName(), "Rift") || strstr(minion->GetObjectName(), "Baron"))
			{
				if (minion != nullptr && !minion->IsDead() && minion->GetHealth() <= GDamage->GetSummonerSpellDamage(GEntityList->Player(), minion, kSummonerSpellSmite))
				{
					SMITE->CastOnUnit(minion);
				}
			}
		}
	}
}

bool TargetValidForMikaels(IUnit* Source)
{
	if (CleanseTeamate1 != nullptr && CleanseTeamate1->Enabled() && CleanseTeamate01->ChampionName() == Source->ChampionName())
	{
		return true;
	}
	if (CleanseTeamate2 != nullptr && CleanseTeamate2->Enabled() && CleanseTeamate02->ChampionName() == Source->ChampionName())
	{
		return true;
	}
	if (CleanseTeamate3 != nullptr && CleanseTeamate3->Enabled() && CleanseTeamate03->ChampionName() == Source->ChampionName())
	{
		return true;
	}
	if (CleanseTeamate4 != nullptr && CleanseTeamate4->Enabled() && CleanseTeamate04->ChampionName() == Source->ChampionName())
	{
		return true;
	}
	if (CleanseTeamate5 != nullptr && CleanseTeamate5->Enabled() && CleanseTeamate05->ChampionName() == Source->ChampionName())
	{
		return true;
	}
	return false;
}

void Combo()
{
	auto Hero = GEntityList->Player();

	//RANDUINS
	if (Randuins->IsOwned() && Randuins->IsReady() && RanduinsEnabled->Enabled() && !(Hero->IsDead()))
	{
		if (EnemiesInRange(Hero, 400) > 0) { Randuins->CastOnPlayer(); }
	}
	//Gunblade
	if (Gunblade->IsOwned() && Gunblade->IsReady() && GunbladeEnabled->Enabled() && !(Hero->IsDead()))
	{
		if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Gunblade->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
		{
			Gunblade->CastOnTarget(GTargetSelector->GetFocusedTarget());
		}
		else
		{
			Gunblade->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 700));
		}
	}
	//Botrk
	if (Botrk->IsOwned() && Botrk->IsReady() && BotrkEnabled->Enabled()  && !(Hero->IsDead()))
	{
		if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Botrk->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
		{
			Botrk->CastOnTarget(GTargetSelector->GetFocusedTarget());
		}
		else
		{
			Botrk->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 550));
		}
	}
	//Cutlass
	if (Cutlass->IsOwned() && Cutlass->IsReady() && CutlassEnabled->Enabled() && !(Hero->IsDead()))
	{
		if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && Cutlass->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
		{
			Cutlass->CastOnTarget(GTargetSelector->GetFocusedTarget());
		}
		else
		{
			Cutlass->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 550));
		}
	}
	//Youmuus
	if (Youmuus->IsOwned() && Youmuus->IsReady() && YoumuusEnabled->Enabled() && !(Hero->IsDead()))
	{
		if (EnemiesInRange(Hero, 600) > 0)
		{
			Youmuus->CastOnPlayer();
		}
	}
	//GLP800
	if (GLP800->IsOwned() && GLP800->IsReady() && GLP800Enabled->Enabled() && !(Hero->IsDead()))
	{
		if (GTargetSelector->GetFocusedTarget() != nullptr && GTargetSelector->GetFocusedTarget()->IsValidTarget() && !(GTargetSelector->GetFocusedTarget()->IsDead()) && GLP800->IsTargetInRange(GTargetSelector->GetFocusedTarget()))
		{
			GLP800->CastOnTarget(GTargetSelector->GetFocusedTarget());
		}
		else
		{
			GLP800->CastOnTarget(GTargetSelector->FindTarget(QuickestKill, SpellDamage, 800));
		}
	}

}

void AutoTrinket()
{
	auto HeroDad = GEntityList->Player();
	if (GEntityList->Player()->GetLevel() < 9 || !(InFountain(GEntityList->Player()))) { return; }
	if (AutoUpgradeTrinket->Enabled())
	{
		if (HeroDad->HasItemId(3341)) //sweeping lense
		{
			GGame->BuyItem(3364); //oracle alteration
		}
		if (HeroDad->HasItemId(3340)) //ward
		{
			GGame->BuyItem(3363); //oracle alteration
		}
	}
}


PLUGIN_EVENT(void) OnOrbwalkBeforeAttack(IUnit* Target)
{

}

PLUGIN_EVENT(void) OnOrbwalkAttack(IUnit* Source, IUnit* Target)
{

}

PLUGIN_EVENT(void) OnOrbwalkAfterAttack(IUnit* Source, IUnit* Target)
{

}

// Return an IUnit object here to force the orbwalker to select it for this tick
PLUGIN_EVENT(IUnit*) OnOrbwalkingFindTarget()
{
	return nullptr;
}

PLUGIN_EVENT(void) OnOrbwalkTargetChange(IUnit* OldTarget, IUnit* NewTarget)
{

}

PLUGIN_EVENT(void) OnOrbwalkNonKillableMinion(IUnit* NonKillableMinion)
{

}

PLUGIN_EVENT(void) OnGameUpdate()
{
	UseDefensives();
	SkinHack().UpdateSkin();
	CheckKeyPresses();
	AutoSmite();
	AutoTrinket();

	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo) { Combo();}
}

PLUGIN_EVENT(void) OnRender()
{
	DrawTowerRange().UpdateTowerRanges();

	Vec2 vecScreen;

	vecScreen.y = 300.f;
	vecScreen.x = 500.f;

	//if (GGame->Projection(camp.Position, &vecScreen))
	//SmiteEnabledFont->Render(vecScreen.x, vecScreen.y, "AUTO-SMITE ACTIVE");
	//GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), Q->Range());
	//GRender->

}

PLUGIN_EVENT(void) OnSpellCast(CastedSpell const& Args)
{

}

PLUGIN_EVENT(void) OnUnitDeath(IUnit* Source)
{

}

PLUGIN_EVENT(void) OnCreateObject(IUnit* Source)
{

}

PLUGIN_EVENT(void) OnDestroyObject(IUnit* Source)
{

}

PLUGIN_EVENT(void) OnDoCast(CastedSpell const& Args)
{
}

PLUGIN_EVENT(void) OnInterruptible(InterruptibleSpell const& Args)
{

}

PLUGIN_EVENT(void) OnGapCloser(GapCloserSpell const& Args)
{

}

// Called when issuing an order (e.g move, attack, etc.)
// Return false to stop order from executing
PLUGIN_EVENT(bool) OnIssueOrder(IUnit* Source, DWORD OrderIdx, Vec3* Position, IUnit* Target)
{
	return true;
}

PLUGIN_EVENT(void) OnBuffAdd(IUnit* Source, void* BuffData)
{
	//CLEANSE
	if (CLEANSE != nullptr && CLEANSE->IsReady() && !(GPluginSDK->GetEntityList()->Player()->IsDead()))
	{
		if (Source == GEntityList->Player() && CleanseActive->Enabled() && !Source->IsDead())
		{
			if (Source->HasBuffOfType(BUFF_Charm) && CleanseCharm->Enabled())
			{
				CLEANSE->CastOnPlayer();
			}
			else if (Source->HasBuffOfType(BUFF_Blind) && CleanseBlind->Enabled())
			{
				CLEANSE->CastOnPlayer();
			}
			else if(Source->HasBuffOfType(BUFF_Disarm) && CleanseDisarm->Enabled())
			{
				CLEANSE->CastOnPlayer();
			}
			else if(Source->HasBuffOfType(BUFF_Fear) && CleanseFear->Enabled())
			{
				CLEANSE->CastOnPlayer();
			}
			else if(Source->HasBuffOfType(BUFF_Flee) && CleanseFlee->Enabled())
			{
				CLEANSE->CastOnPlayer();
			}
			else if(Source->HasBuffOfType(BUFF_Polymorph) && CleansePolymorph->Enabled())
			{
				CLEANSE->CastOnPlayer();
			}
			else if(Source->HasBuffOfType(BUFF_Snare) && CleanseSnare->Enabled())
			{
				CLEANSE->CastOnPlayer();
			}
			else if(Source->HasBuffOfType(BUFF_Taunt) && CleanseTaunt->Enabled())
			{
				CLEANSE->CastOnPlayer();
			}
			else if(Source->HasBuff("SummonerExhaust") && CleanseExhaust->Enabled())
			{
				CLEANSE->CastOnPlayer();
			}
			else if(Source->HasBuffOfType(BUFF_Stun) && CleanseStun->Enabled())
			{
				CLEANSE->CastOnPlayer();
			}
		}
	}

	//QSS
	if (Source == GEntityList->Player() && !Source->IsDead() && CleanseActive->Enabled() && (QSS->IsOwned() || Scimitar->IsOwned()))
	{
		if (Source->HasBuffOfType(BUFF_Charm) && CleanseCharm->Enabled())
		{
			if (QSS->IsOwned() && QSS->IsReady()) { QSS->CastOnPlayer(); }
			else if (Scimitar->IsOwned() && Scimitar->IsReady()) { Scimitar->CastOnPlayer(); }
		}
		else if (Source->HasBuffOfType(BUFF_Blind) && CleanseBlind->Enabled())
		{
			if (QSS->IsOwned() && QSS->IsReady()) { QSS->CastOnPlayer(); }
			else if (Scimitar->IsOwned() && Scimitar->IsReady()) { Scimitar->CastOnPlayer(); }
		}
		else if (Source->HasBuffOfType(BUFF_Disarm) && CleanseDisarm->Enabled())
		{
			if (QSS->IsOwned() && QSS->IsReady()) { QSS->CastOnPlayer(); }
			else if (Scimitar->IsOwned() && Scimitar->IsReady()) { Scimitar->CastOnPlayer(); }
		}
		else if (Source->HasBuffOfType(BUFF_Fear) && CleanseFear->Enabled())
		{
			if (QSS->IsOwned() && QSS->IsReady()) { QSS->CastOnPlayer(); }
			else if (Scimitar->IsOwned() && Scimitar->IsReady()) { Scimitar->CastOnPlayer(); }
		}
		else if (Source->HasBuffOfType(BUFF_Flee) && CleanseFlee->Enabled())
		{
			if (QSS->IsOwned() && QSS->IsReady()) { QSS->CastOnPlayer(); }
			else if (Scimitar->IsOwned() && Scimitar->IsReady()) { Scimitar->CastOnPlayer(); }
		}
		else if (Source->HasBuffOfType(BUFF_Polymorph) && CleansePolymorph->Enabled())
		{
			if (QSS->IsOwned() && QSS->IsReady()) { QSS->CastOnPlayer(); }
			else if (Scimitar->IsOwned() && Scimitar->IsReady()) { Scimitar->CastOnPlayer(); }
		}
		else if (Source->HasBuffOfType(BUFF_Snare) && CleanseSnare->Enabled())
		{
			if (QSS->IsOwned() && QSS->IsReady()) { QSS->CastOnPlayer(); }
			else if (Scimitar->IsOwned() && Scimitar->IsReady()) { Scimitar->CastOnPlayer(); }
		}
		else if (Source->HasBuffOfType(BUFF_Taunt) && CleanseTaunt->Enabled())
		{
			if (QSS->IsOwned() && QSS->IsReady()) { QSS->CastOnPlayer(); }
			else if (Scimitar->IsOwned() && Scimitar->IsReady()) { Scimitar->CastOnPlayer(); }
		}
		else if (Source->HasBuff("SummonerExhaust") && CleanseExhaust->Enabled())
		{
			if (QSS->IsOwned() && QSS->IsReady()) { QSS->CastOnPlayer(); }
			else if (Scimitar->IsOwned() && Scimitar->IsReady()) { Scimitar->CastOnPlayer(); }
		}
		else if (Source->HasBuffOfType(BUFF_Stun) && CleanseStun->Enabled())
		{
			if (QSS->IsOwned() && QSS->IsReady()) { QSS->CastOnPlayer(); }
			else if (Scimitar->IsOwned() && Scimitar->IsReady()) { Scimitar->CastOnPlayer(); }
		}
		else if (Source->HasBuffOfType(BUFF_Suppression) && CleanseSuppression->Enabled())
		{
			if (QSS->IsOwned() && QSS->IsReady()) { QSS->CastOnPlayer(); }
			else if (Scimitar->IsOwned() && Scimitar->IsReady()) { Scimitar->CastOnPlayer(); }
		}
	}

	//Mikael's
	if (Mikaels->IsOwned() && Mikaels->IsReady() && !(GEntityList->Player()->IsDead()) && TargetValidForMikaels(Source) && GetDistance(GEntityList->Player(), Source) <= 600 && !Source->IsEnemy(Source) && CleanseActive->Enabled() && !Source->IsDead())
	{
		if (Source->HasBuffOfType(BUFF_Charm) && CleanseCharm->Enabled())
		{
			Mikaels->CastOnTarget(Source);
		}
		else if (Source->HasBuffOfType(BUFF_Blind) && CleanseBlind->Enabled())
		{
			Mikaels->CastOnTarget(Source);
		}
		else if (Source->HasBuffOfType(BUFF_Disarm) && CleanseDisarm->Enabled())
		{
			Mikaels->CastOnTarget(Source);
		}
		else if (Source->HasBuffOfType(BUFF_Fear) && CleanseFear->Enabled())
		{
			Mikaels->CastOnTarget(Source);
		}
		else if (Source->HasBuffOfType(BUFF_Flee) && CleanseFlee->Enabled())
		{
			Mikaels->CastOnTarget(Source);
		}
		else if (Source->HasBuffOfType(BUFF_Polymorph) && CleansePolymorph->Enabled())
		{
			Mikaels->CastOnTarget(Source);
		}
		else if (Source->HasBuffOfType(BUFF_Snare) && CleanseSnare->Enabled())
		{
			Mikaels->CastOnTarget(Source);
		}
		else if (Source->HasBuffOfType(BUFF_Taunt) && CleanseTaunt->Enabled())
		{
			Mikaels->CastOnTarget(Source);
		}
		else if (Source->HasBuff("SummonerExhaust") && CleanseExhaust->Enabled())
		{
			Mikaels->CastOnTarget(Source);
		}
		else if (Source->HasBuffOfType(BUFF_Stun) && CleanseStun->Enabled())
		{
			Mikaels->CastOnTarget(Source);
		}
	}
}

PLUGIN_EVENT(void) OnBuffRemove(IUnit* Source, void* BuffData)
{

}

PLUGIN_EVENT(void) OnGameEnd()
{

}

PLUGIN_EVENT(void) OnLevelUp(IUnit* Source, int NewLevel)
{
	if (EnableAutoLevelUp->Enabled() && Source == GEntityList->Player() && NewLevel >= ALUStartLevel->GetInteger()) //auto level
	{
		for (int i = 1; i <= 4; i++)
		{
			if (ALUR->GetInteger() == i) {
				Source->LevelUpSpell(kSlotR); 
			}
			if (ALUQ->GetInteger() == i) {
				Source->LevelUpSpell(kSlotQ);
			}
			if (ALUW->GetInteger() == i) {
				Source->LevelUpSpell(kSlotW);
			}
			if (ALUE->GetInteger() == i) {
				Source->LevelUpSpell(kSlotE);
			}
			
		}
	}
}

// Only called for local player, before the spell packet is sent
PLUGIN_EVENT(void) OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition)
{
}

PLUGIN_EVENT(void) OnDash(UnitDash* Args)
{

}

PLUGIN_EVENT(void) OnD3DPresent(void* Direct3D9DevicePtr)
{

}

PLUGIN_EVENT(void) OnD3DPreReset(void* Direct3D9DevicePtr)
{

}

PLUGIN_EVENT(void) OnD3DPostReset(void* Direct3D9DevicePtr)
{

}

PLUGIN_EVENT(void) OnRenderBehindHUD()
{

}

// Return false to set this message as handled
PLUGIN_EVENT(bool) OnWndProc(HWND Wnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	return true;
}

PLUGIN_EVENT(void) OnEnterVisible(IUnit* Source)
{

}

PLUGIN_EVENT(void) OnExitVisible(IUnit* Source)
{

}
#pragma endregion

// Called when plugin is first loaded
PLUGIN_API void OnLoad(IPluginSDK* PluginSDK)
{
	// Initializes global interfaces for core access
	PluginSDKSetup(PluginSDK);

	//Initialize Menus
	MainMenu = GPluginSDK->AddMenu("Utility PRO++ [AIO] by Rembrandt");

	Defensives = MainMenu->AddMenu("Activator");
	
	CleanseMenu = Defensives->AddMenu("Cleanse / QSS / Mikaels");
	CleanseActive = CleanseMenu->CheckBox("Enabled: ", true);
	MikaelsMenu = CleanseMenu->AddMenu("Mikaels Filter");
	AddTeamatesToCleanse();
	CCFilter = CleanseMenu->AddMenu("CC Filter");
	CleanseCharm = CCFilter->CheckBox("Use on Charm ", true);
	CleanseDisarm = CCFilter->CheckBox("Use on Disarm ", true);
	CleanseFear = CCFilter->CheckBox("Use on Fear ", true);
	CleanseFlee = CCFilter->CheckBox("Use on Flee ", true);
	CleansePolymorph = CCFilter->CheckBox("Use on Polymorph ", true);
	CleanseSnare = CCFilter->CheckBox("Use on Snare", true);
	CleanseStun = CCFilter->CheckBox("Use on Stun ", true);
	CleanseExhaust = CCFilter->CheckBox("Use on Exhaust ", true);
	CleanseSuppression = CCFilter->CheckBox("Use on Suppression ", true);
	CleanseBlind = CCFilter->CheckBox("Use on Blind ", true);

	DefensiveItems = Defensives->AddMenu("Defensive Items"); // DEFENSIVE ITEMS MENU
	FaceOfTheMountainMenu = DefensiveItems->AddMenu("Face of The Mountain");
	FaceOfTheMountainEnabled = FaceOfTheMountainMenu->CheckBox("Enabled:", true);
	FaceOfTheMountainPercent = FaceOfTheMountainMenu->AddFloat("Use at Health %:", 1, 99, 50);

	RedemptionMenu = DefensiveItems->AddMenu("Redemption");
	RedemptionEnabled = RedemptionMenu->CheckBox("Enabled:", true);
	RedemptionPercent = RedemptionMenu->AddFloat("Use at Health %:", 1, 99, 40);

	ZhonyasMenu = DefensiveItems->AddMenu("Zhonyas Hourglass");
	ZhonyasEnabled = ZhonyasMenu->CheckBox("COMING SOON WITH DAMAGE PREDICTION:", true);

	SeraphsMenu = DefensiveItems->AddMenu("Seraphs Embrace");
	SeraphsEnabled = SeraphsMenu->CheckBox("Enabled:", true);
	SeraphsPercent = SeraphsMenu->AddFloat("Use at Health %:", 1, 99, 50);

	LocketMenu = DefensiveItems->AddMenu("Locket of the Iron Solari");
	LocketEnabled = LocketMenu->CheckBox("Enabled:", true);
	LocketPercent = LocketMenu->AddFloat("Use at Health %:", 1, 99, 45);

	RanduinsMenu = DefensiveItems->AddMenu("Randuins Omen");
	RanduinsEnabled = RanduinsMenu->CheckBox("Enabled:", true);

	OffensiveItems = Defensives->AddMenu("Offensive Items");//OFFENSIVE ITEMS MENU

	GunbladeMenu = OffensiveItems->AddMenu("Hextech Gunblade");
	GunbladeEnabled = GunbladeMenu->CheckBox("Enabled in Combo:", true);
	BotrkMenu = OffensiveItems->AddMenu("Blade of the Ruined King");
	BotrkEnabled = BotrkMenu->CheckBox("Enabled in Combo:", true);
	CutlassMenu = OffensiveItems->AddMenu("Bilgewater Cutlass");
	CutlassEnabled = CutlassMenu->CheckBox("Enabled in Combo:", true);
	YoumuusMenu = OffensiveItems->AddMenu("Youmuus Ghostblade");
	YoumuusEnabled = YoumuusMenu->CheckBox("Enabled in Combo:", true);

	GLP800Menu = OffensiveItems->AddMenu("Hextech GLP-800");
	GLP800Enabled = GLP800Menu->CheckBox("Enabled in Combo:", true);

	SummonerHealMenu = Defensives->AddMenu("Summoner: Heal");
	HealActive = SummonerHealMenu->CheckBox("Enabled: ", true);
	HealPercent = SummonerHealMenu->AddInteger("Use at Health %: ", 1, 99, 30);

	SummonerBarrierMenu = Defensives->AddMenu("Summoner: Barrier");
	BarrierActive = SummonerBarrierMenu->CheckBox("Enabled: ", true);
	BarrierPercent = SummonerBarrierMenu->AddInteger("Use at Health %: ", 1, 99, 30);

	AutoSmiteMenu = Defensives->AddMenu("Summoner: Smite");
	SmiteActive = AutoSmiteMenu->CheckBox("Smite Buffs/Epic Monsters: ", true);
	SmiteKey = AutoSmiteMenu->AddKey("Toggle Key:", 77);

	Potions = Defensives->AddMenu("Potions");
	PotionsEnabled = Potions->CheckBox("Use Potions: ", true);
	PotionsPercent = Potions->AddFloat("Drink at Health %: ", 1, 99, 60);

	AutoLevelUp = MainMenu->AddMenu("Auto Level");
	EnableAutoLevelUp = AutoLevelUp->CheckBox("Enable: ", false);
	ALUR = AutoLevelUp->AddInteger("R: ", 1, 4, 1);
	ALUQ = AutoLevelUp->AddInteger("Q: ", 1, 4, 2);
	ALUW = AutoLevelUp->AddInteger("W: ", 1, 4, 3);
	ALUE = AutoLevelUp->AddInteger("E: ", 1, 4, 4);
	ALUStartLevel = AutoLevelUp->AddInteger("Start at level: ", 1, 16, 4);
	
	AutoTrinketMenu = MainMenu->AddMenu("Auto Trinket");
	AutoUpgradeTrinket = AutoTrinketMenu->CheckBox("Auto Upgrade Trinket:", false);

	SkinHack().InitMenu(MainMenu);
	SkinHack().UpdateSkin();
	DrawTowerRange().InitMenu(MainMenu);
	LoadSpells();

	

	GEventManager->AddEventHandler(kEventOrbwalkBeforeAttack, OnOrbwalkBeforeAttack);
	GEventManager->AddEventHandler(kEventOrbwalkOnAttack, OnOrbwalkAttack);
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAfterAttack);
	GEventManager->AddEventHandler(kEventOrbwalkFindTarget, OnOrbwalkingFindTarget);
	GEventManager->AddEventHandler(kEventOrbwalkTargetChange, OnOrbwalkTargetChange);
	GEventManager->AddEventHandler(kEventOrbwalkNonKillableMinion, OnOrbwalkNonKillableMinion);
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnSpellCast, OnSpellCast);
	GEventManager->AddEventHandler(kEventOnUnitDeath, OnUnitDeath);
	GEventManager->AddEventHandler(kEventOnCreateObject, OnCreateObject);
	GEventManager->AddEventHandler(kEventOnDestroyObject, OnDestroyObject);
	GEventManager->AddEventHandler(kEventOnDoCast, OnDoCast);
	GEventManager->AddEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->AddEventHandler(kEventOnGapCloser, OnGapCloser);
	GEventManager->AddEventHandler(kEventOnIssueOrder, OnIssueOrder);
	GEventManager->AddEventHandler(kEventOnBuffAdd, OnBuffAdd);
	GEventManager->AddEventHandler(kEventOnBuffRemove, OnBuffRemove);
	GEventManager->AddEventHandler(kEventOnGameEnd, OnGameEnd);
	GEventManager->AddEventHandler(kEventOnLevelUp, OnLevelUp);
	GEventManager->AddEventHandler(kEventOnPreCast, OnPreCast);
	GEventManager->AddEventHandler(kEventOnDash, OnDash);
	GEventManager->AddEventHandler(kEventOnD3DPresent, OnD3DPresent);
	GEventManager->AddEventHandler(kEventOnD3DPreReset, OnD3DPreReset);
	GEventManager->AddEventHandler(kEventOnD3DPostReset, OnD3DPostReset);
	GEventManager->AddEventHandler(kEventOnRenderBehindHud, OnRenderBehindHUD);
	GEventManager->AddEventHandler(kEventOnWndProc, OnWndProc);
	GEventManager->AddEventHandler(kEventOnEnterVisible, OnEnterVisible);
	GEventManager->AddEventHandler(kEventOnExitVisible, OnExitVisible);

	GRender->NotificationEx(Color::Orange().Get(), 2, true, true, "Utility PRO++ loaded - Updated for Patch 7.3 by Rembrandt");
}

// Called when plugin is unloaded
PLUGIN_API void OnUnload()
{
	MainMenu->Remove();

	GEventManager->RemoveEventHandler(kEventOrbwalkBeforeAttack, OnOrbwalkBeforeAttack);
	GEventManager->RemoveEventHandler(kEventOrbwalkOnAttack, OnOrbwalkAttack);
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAfterAttack);
	GEventManager->RemoveEventHandler(kEventOrbwalkFindTarget, OnOrbwalkingFindTarget);
	GEventManager->RemoveEventHandler(kEventOrbwalkTargetChange, OnOrbwalkTargetChange);
	GEventManager->RemoveEventHandler(kEventOrbwalkNonKillableMinion, OnOrbwalkNonKillableMinion);
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnSpellCast, OnSpellCast);
	GEventManager->RemoveEventHandler(kEventOnUnitDeath, OnUnitDeath);
	GEventManager->RemoveEventHandler(kEventOnCreateObject, OnCreateObject);
	GEventManager->RemoveEventHandler(kEventOnDestroyObject, OnDestroyObject);
	GEventManager->RemoveEventHandler(kEventOnDoCast, OnDoCast);
	GEventManager->RemoveEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->RemoveEventHandler(kEventOnGapCloser, OnGapCloser);
	GEventManager->RemoveEventHandler(kEventOnIssueOrder, OnIssueOrder);
	GEventManager->RemoveEventHandler(kEventOnBuffAdd, OnBuffAdd);
	GEventManager->RemoveEventHandler(kEventOnBuffRemove, OnBuffRemove);
	GEventManager->RemoveEventHandler(kEventOnGameEnd, OnGameEnd);
	GEventManager->RemoveEventHandler(kEventOnLevelUp, OnLevelUp);
	GEventManager->RemoveEventHandler(kEventOnPreCast, OnPreCast);
	GEventManager->RemoveEventHandler(kEventOnDash, OnDash);
	GEventManager->RemoveEventHandler(kEventOnD3DPresent, OnD3DPresent);
	GEventManager->RemoveEventHandler(kEventOnD3DPreReset, OnD3DPreReset);
	GEventManager->RemoveEventHandler(kEventOnD3DPostReset, OnD3DPostReset);
	GEventManager->RemoveEventHandler(kEventOnRenderBehindHud, OnRenderBehindHUD);
	GEventManager->RemoveEventHandler(kEventOnWndProc, OnWndProc);
	GEventManager->RemoveEventHandler(kEventOnEnterVisible, OnEnterVisible);
	GEventManager->RemoveEventHandler(kEventOnExitVisible, OnExitVisible);
}