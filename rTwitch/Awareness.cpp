#include "Awareness.h"
#include "Gui.h"
#include "Tracker.h"
#include "GankDetection.h"

Awareness::Awareness(IMenu* Parent)
{
	GUtility->LogConsole("Adding Menu");
	Menu		= Parent->AddMenu("Tracker PRO");
	GUtility->LogConsole("Adding GUI");
	Ui			= new Gui(Menu);
	GUtility->LogConsole("Adding Tracking");
	Tracking	= new Tracker(Menu);
	GUtility->LogConsole("Adding Ganks");
	Ganks		= new GankDetection(Menu);

	LoadEvents();
}

Awareness::~Awareness()
{

	UnloadEvents();
	Menu->Remove();

	delete Ui;
}

void Awareness::_OnGameUpdate()
{
	Tracking->OnGameUpdate();
	Ganks->OnGameUpdate();
}

void Awareness::_OnJungleNotify(JungleNotifyData* Args)
{
	Ganks->OnJungleNotify(Args);
}

void Awareness::_OnRender()
{
	
	Ui->OnRender();
	
	Tracking->OnRender();
	
	Ganks->OnRender();
	
}

void Awareness::_OnCreateObject(IUnit* Args)
{
	Ui->OnCreateObject(Args);
	Tracking->OnCreateObject(Args);
}

void Awareness::_OnDestroyObject(IUnit* Args)
{
	Ui->OnDestroyObject(Args);
	Tracking->OnDestroyObject(Args);
}

void Awareness::_OnUnitDeath(IUnit* Args)
{
	Tracking->OnUnitDeath(Args);
}

void Awareness::_OnProcessSpell(CastedSpell const& Args)
{
	Tracking->OnProcessSpell(Args);
}

void Awareness::_OnTeleport(OnTeleportArgs* Args)
{
	Ui->OnTeleport(Args);
	Ganks->OnTeleport(Args);
}


void Awareness::LoadEvents()
{
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnCreateObject, OnCreateObject);
	GEventManager->AddEventHandler(kEventOnDestroyObject, OnDestroyObject);
	GEventManager->AddEventHandler(kEventOnUnitDeath, OnUnitDeath);
	GEventManager->AddEventHandler(kEventOnSpellCast, OnProcessSpell);
	GEventManager->AddEventHandler(kEventOnJungleNotification, OnJungleNotify);
	GEventManager->AddEventHandler(kEventOnTeleport, OnTeleport);
}

void Awareness::UnloadEvents()
{
	GEventManager->RemoveEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnCreateObject, OnCreateObject);
	GEventManager->RemoveEventHandler(kEventOnDestroyObject, OnDestroyObject);
	GEventManager->RemoveEventHandler(kEventOnUnitDeath, OnUnitDeath);
	GEventManager->RemoveEventHandler(kEventOnSpellCast, OnProcessSpell);
	GEventManager->RemoveEventHandler(kEventOnJungleNotification, OnJungleNotify);
	GEventManager->RemoveEventHandler(kEventOnTeleport, OnTeleport);
}

PLUGIN_EVENTD(void) Awareness::OnTeleport(OnTeleportArgs* Args)
{
	GPluginInstance->_OnTeleport(Args);
}

PLUGIN_EVENTD(void) Awareness::OnGameUpdate()
{
	GPluginInstance->_OnGameUpdate();
}

PLUGIN_EVENTD(void) Awareness::OnJungleNotify(JungleNotifyData* Args)
{
	GPluginInstance->_OnJungleNotify(Args);
}
PLUGIN_EVENTD(void) Awareness::OnRender()
{
	GPluginInstance->_OnRender();
}

PLUGIN_EVENTD(void) Awareness::OnCreateObject(IUnit* Args)
{
	GPluginInstance->_OnCreateObject(Args);
}

PLUGIN_EVENTD(void) Awareness::OnDestroyObject(IUnit* Args)
{
	GPluginInstance->_OnDestroyObject(Args);
}

PLUGIN_EVENTD(void) Awareness::OnUnitDeath(IUnit* Args)
{
	GPluginInstance->_OnUnitDeath(Args);
}

PLUGIN_EVENTD(void) Awareness::OnProcessSpell(CastedSpell const& Args)
{
	GPluginInstance->_OnProcessSpell(Args);
}