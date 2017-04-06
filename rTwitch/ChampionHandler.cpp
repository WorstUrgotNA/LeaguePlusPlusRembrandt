#include "ChampionHandler.h"
// CHAMPION LIST
#include "Graves.h"
#include "Caitlyn.h"
#include "Twitch.h"
#include "Sivir.h"
#include "Tristana.h"
#include "Kogmaw.h"


ChampionHandler::ChampionHandler()
{
	Menu = GPluginSDK->AddMenu("[Rembrandt AIO] Champion Settings");
	LoadChampPlugin = Menu->CheckBox("Use Rembrandt AIO Champion Plugins:", true);

	PluginActive = LoadChampPlugin->Enabled();

	
	if (strstr(GEntityList->Player()->ChampionName(), "Graves"))
		GravesHandler = new Graves(Menu);
	else if (strstr(GEntityList->Player()->ChampionName(), "Caitlyn"))
		CaitlynHandler = new Caitlyn(Menu);
	else if (strstr(GEntityList->Player()->ChampionName(), "Twitch"))
		TwitchHandler = new Twitch(Menu);
	else if (strstr(GEntityList->Player()->ChampionName(), "Sivir"))
		SivirHandler = new Sivir(Menu);
	else if (strstr(GEntityList->Player()->ChampionName(), "Tristana"))
		TristanaHandler = new Tristana(Menu);
	else if (strstr(GEntityList->Player()->ChampionName(), "KogMaw"))
		KogmawHandler = new Kogmaw(Menu);


	LoadEvents();
}

ChampionHandler::~ChampionHandler()
{
	UnloadEvents();
	Menu->Remove();
}

void ChampionHandler::_OnGameUpdate()
{
	PluginActive = LoadChampPlugin->Enabled();


	if (PluginActive)
	{
		if (GravesHandler != nullptr)
			GravesHandler->OnGameUpdate();
		if (CaitlynHandler != nullptr)
			CaitlynHandler->OnGameUpdate();
		if (TwitchHandler != nullptr)
			TwitchHandler->OnGameUpdate();
		if (SivirHandler != nullptr)
			SivirHandler->OnGameUpdate();
		if (TristanaHandler != nullptr)
			TristanaHandler->OnGameUpdate();
		if (KogmawHandler != nullptr)
			KogmawHandler->OnGameUpdate();
	}
	
}

void ChampionHandler::_OnRender()
{
	if (PluginActive)
	{
		if (GravesHandler != nullptr)
			GravesHandler->OnRender();
		if (CaitlynHandler != nullptr)
			CaitlynHandler->OnRender();
		if (TwitchHandler != nullptr)
			TwitchHandler->OnRender();
		if (SivirHandler != nullptr)
			SivirHandler->OnRender();
		if (TristanaHandler != nullptr)
			TristanaHandler->OnRender();
		if (KogmawHandler != nullptr)
			KogmawHandler->OnRender();
	}
	
}

void ChampionHandler::_OnRealSpellCast(CastedSpell const& Args)
{
	if (TwitchHandler != nullptr)
		TwitchHandler->OnRealSpellCast(Args);
}

void ChampionHandler::_OnSpellCast(CastedSpell const& Args)
{
	if (PluginActive)
	{
		if (GravesHandler != nullptr)
			GravesHandler->OnSpellCast(Args);
		if (CaitlynHandler != nullptr)
			CaitlynHandler->OnSpellCast(Args);
		//if (TwitchHandler != nullptr)
			//TwitchHandler->OnSpellCast(Args);
		if (SivirHandler != nullptr)
			SivirHandler->OnSpellCast(Args);
		if (TristanaHandler != nullptr)
			TristanaHandler->OnSpellCast(Args);
		if (KogmawHandler != nullptr)
			KogmawHandler->OnSpellCast(Args);
	}
	
}

bool ChampionHandler::_OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition)
{
	if (TwitchHandler != nullptr && PluginActive)
		return TwitchHandler->OnPreCast(Slot, Target, StartPosition, EndPosition);
	else if (CaitlynHandler != nullptr && PluginActive)
		return CaitlynHandler->OnPreCast(Slot, Target, StartPosition, EndPosition);
	else
		return true;
}

void ChampionHandler::_OnOrbwalkAttack(IUnit* Source, IUnit* Target)
{
	if (PluginActive)
	{
		if (CaitlynHandler != nullptr)
			CaitlynHandler->OnOrbwalkAttack(Source, Target);
		if (TwitchHandler != nullptr)
			TwitchHandler->OnOrbwalkAttack(Source, Target);
		if (SivirHandler != nullptr)
			SivirHandler->OnOrbwalkAttack(Source, Target);
		if (TristanaHandler != nullptr)
			TristanaHandler->OnOrbwalkAttack(Source, Target);
		if (KogmawHandler != nullptr)
			KogmawHandler->OnOrbwalkAttack(Source, Target);
	}
	
}

void ChampionHandler::_BeforeAttack(IUnit* Target)
{
	if (PluginActive)
	{
		if (TristanaHandler != nullptr)
			TristanaHandler->BeforeAttack(Target);
		if (KogmawHandler != nullptr)
			KogmawHandler->BeforeAttack(Target);
	}
	
}

void ChampionHandler::_OnInterruptible(InterruptibleSpell const& Args)
{
	if (PluginActive)
	{
		if (TristanaHandler != nullptr)
			TristanaHandler->OnInterruptible(Args);
	}
	
}

void ChampionHandler::_OnGapCloser(GapCloserSpell const& Args)
{
	if (PluginActive)
	{
		if (TristanaHandler != nullptr)
			TristanaHandler->OnGapCloser(Args);
	}
	
}


void ChampionHandler::LoadEvents()
{
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnSpellCast, OnRealSpellCast);
	GEventManager->AddEventHandler(kEventOnDoCast, OnSpellCast);
	GEventManager->AddEventHandler(kEventOnPreCast, OnPreCast);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAttack);
	GEventManager->AddEventHandler(kEventOrbwalkBeforeAttack, BeforeAttack);
	GEventManager->AddEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->AddEventHandler(kEventOnGapCloser, OnGapCloser);
}

void ChampionHandler::UnloadEvents()
{
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnSpellCast, OnRealSpellCast);
	GEventManager->RemoveEventHandler(kEventOnDoCast, OnSpellCast);
	GEventManager->RemoveEventHandler(kEventOnPreCast, OnPreCast);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOrbwalkAfterAttack, OnOrbwalkAttack);
	GEventManager->RemoveEventHandler(kEventOrbwalkBeforeAttack, BeforeAttack);
	GEventManager->RemoveEventHandler(kEventOnInterruptible, OnInterruptible);
	GEventManager->RemoveEventHandler(kEventOnGapCloser, OnGapCloser);
}

PLUGIN_EVENTD(void) ChampionHandler::OnGameUpdate()
{
	GPluginInstance3->_OnGameUpdate();
}

PLUGIN_EVENTD(void) ChampionHandler::OnRender()
{
	GPluginInstance3->_OnRender();
}

PLUGIN_EVENTD(void) ChampionHandler::OnRealSpellCast(CastedSpell const& Args)
{
	GPluginInstance3->_OnRealSpellCast(Args);
}

PLUGIN_EVENTD(void) ChampionHandler::OnSpellCast(CastedSpell const& Args)
{
	GPluginInstance3->_OnSpellCast(Args);
}
PLUGIN_EVENTD(void) ChampionHandler::OnPreCast(int Slot, IUnit* Target, Vec3* StartPosition, Vec3* EndPosition)
{
	GPluginInstance3->_OnPreCast(Slot, Target, StartPosition, EndPosition);
}

PLUGIN_EVENTD(void) ChampionHandler::OnOrbwalkAttack(IUnit* Source, IUnit* Target)
{
	GPluginInstance3->_OnOrbwalkAttack(Source, Target);
}

PLUGIN_EVENTD(void) ChampionHandler::BeforeAttack(IUnit* Target)
{
	GPluginInstance3->_BeforeAttack(Target);
}

PLUGIN_EVENTD(void) ChampionHandler::OnInterruptible(InterruptibleSpell const& Args)
{
	GPluginInstance3->_OnInterruptible(Args);
}

PLUGIN_EVENTD(void) ChampionHandler::OnGapCloser(GapCloserSpell const& Args)
{
	GPluginInstance3->_OnGapCloser(Args);
}
