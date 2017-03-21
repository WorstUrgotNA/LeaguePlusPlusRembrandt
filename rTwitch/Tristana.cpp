#include "Tristana.h"

Tristana::Tristana(IMenu* Parent)
{
	Hero = GEntityList->Player();

	//Create Spells
	Q = GPluginSDK->CreateSpell2(kSlotQ, kTargetCast, false, false, (kCollidesWithNothing));
	Q->SetSkillshot(0.25, 0, 1000, FLT_MAX);
	W = GPluginSDK->CreateSpell2(kSlotW, kCircleCast, false, false, (kCollidesWithNothing));
	W->SetSkillshot(0.5, 270, 1500, 900);
	E = GPluginSDK->CreateSpell2(kSlotE, kTargetCast, true, false, (kCollidesWithYasuoWall));
	E->SetSkillshot(0.25, 0, 1000, 550);
	R = GPluginSDK->CreateSpell2(kSlotR, kTargetCast, true, false, (kCollidesWithYasuoWall));
	R->SetSkillshot(0.25, 0, 1000, 650);

	//Menu
	TristanaMenu = Parent->AddMenu("Tristana PRO++");

	QinCombo = TristanaMenu->CheckBox("Use Q in Combo:", true);
	EinCombo = TristanaMenu->CheckBox("Use E in Combo:", true);
	RinCombo = TristanaMenu->CheckBox("Use R in Combo:", true);
	InteruptSpellCasts = TristanaMenu->CheckBox("Use R to Interupt:", true);
	RGapCloser = TristanaMenu->CheckBox("Use R on Gap Closer:", true);

	DrawReady = TristanaMenu->CheckBox("Draw Only Ready Spells:", true);
	DrawW = TristanaMenu->CheckBox("Draw W Range:", true);

}

Tristana::~Tristana()
{

}

float Tristana::CalcRDamage(IUnit* Target)
{
	float InitDamage = Hero->TotalMagicDamage();

	if (Hero->GetSpellLevel(kSlotR) == 1)
		InitDamage += 300;
	else if (Hero->GetSpellLevel(kSlotR) == 2)
		InitDamage += 400;
	else if (Hero->GetSpellLevel(kSlotR) == 3)
		InitDamage += 500;

	return GDamage->CalcMagicDamage(Hero, Target, InitDamage);
}

float Tristana::CalcEDamage(IUnit* Target)
{
	if (!Target->HasBuff("tristanaechargesound"))
		return 0;

	auto StackCount = Target->GetBuffCount("tristanaecharge") + 2;
	if (StackCount > 4) { StackCount = 4; }
	auto DamageModifier = 1 + 0.3 * StackCount;

	auto index = Hero->GetSpellLevel(kSlotE) - 1;
	auto InitDamage = std::vector<double>({ 60, 70, 80, 90, 100 }).at(index);
	auto BonusPhys = (std::vector<double>({ 0.5, 0.65, 0.8, 0.95, 1.1 }).at(index)) * Hero->BonusDamage();
	auto BonusMagic = 0.5 * Hero->TotalMagicDamage();

	auto FinalDamage = (InitDamage + BonusPhys + BonusMagic) * DamageModifier;

	/*PHYSICAL DAMAGE :
	60 / 70 / 80 / 90 / 100 
	(+50 / 65 / 80 / 95 / 110 % bonus AD) 
	(+50 % AP) */

	return GDamage->CalcPhysicalDamage(Hero, Target, FinalDamage);
}

int Tristana::EnemiesInRange(IUnit* Source, float range)
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


void Tristana::Combo()
{
	auto Target = GTargetSelector->FindTarget(QuickestKill, SpellDamage, Hero->AttackRange());
	if (Target != nullptr)
	{
		if (R->IsReady() && RinCombo->Enabled() && CalcRDamage(Target) > Target->GetHealth())
		{

			R->CastOnTarget(Target);
		}
	}
}

void Tristana::OnGameUpdate()
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo)
		Combo();
}

void Tristana::OnRender()
{
	if (DrawReady->Enabled())
	{
		if (W->IsReady() && DrawW->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
	}
	else
	{
		if (DrawW->Enabled()) { GRender->DrawOutlinedCircle(GEntityList->Player()->GetPosition(), Vec4(255, 255, 0, 255), W->Range()); }
	}
}

void Tristana::OnSpellCast(CastedSpell const& Args)
{
	if (Args.Target_ == Hero && Args.Caster_->IsDashing() && Args.Caster_->IsHero() && Args.Caster_->IsEnemy(Hero))
		R->CastOnTarget(Args.Caster_);

}

void Tristana::OnOrbwalkAttack(IUnit* Source, IUnit* Target)
{
	//GRender->Notification(Vec4(255, 255, 255, 255), 0, "%.f", CalcEDamage(Target) + GDamage->GetAutoAttackDamage(Hero, Target, true));
	if (Target->IsHero() && RinCombo->Enabled() && GOrbwalking->GetOrbwalkingMode() == kModeCombo && R->IsReady() && CalcEDamage(Target) + CalcRDamage(Target) + GDamage->GetAutoAttackDamage(Hero, Target, true) > Target->GetHealth() && Target->IsValidTarget() && !Target->IsDead())
		R->CastOnTarget(Target);
}

void Tristana::BeforeAttack(IUnit* Target)
{
	if (Target->IsHero() && GOrbwalking->GetOrbwalkingMode() == kModeCombo && Target->IsValidTarget() && !Target->IsDead())
	{
		if (Q->IsReady() && QinCombo->Enabled()) { Q->CastOnPlayer(); }
		if (E->IsReady() && EinCombo->Enabled()) { E->CastOnTarget(Target); }
	}
}

void Tristana::OnInterruptible(InterruptibleSpell const& Args)
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && Args.Target != nullptr && Args.Target->IsEnemy(Hero) && InteruptSpellCasts->Enabled() && R->IsReady() && Args.DangerLevel >= kHighDanger)
		R->CastOnTarget(Args.Target);
}

void Tristana::OnGapCloser(GapCloserSpell const& Args)
{
	if (GOrbwalking->GetOrbwalkingMode() == kModeCombo && RGapCloser->Enabled() && Args.Sender->IsEnemy(Hero) && Args.Sender->IsValidTarget() && !Args.Sender->IsDead() && (Hero->GetPosition() - Args.EndPosition).Length() < 200)
		R->CastOnTarget(Args.Sender);
}
