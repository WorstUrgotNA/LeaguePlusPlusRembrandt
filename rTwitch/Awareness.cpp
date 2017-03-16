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
	GUtility->LogConsole("Attempting Awareness OnRender...");
	Ui->OnRender();
	GUtility->LogConsole("Ui OnRender Complete");
	Tracking->OnRender();
	GUtility->LogConsole("Tracker OnRender Complete");
	Ganks->OnRender();
	GUtility->LogConsole("GankDetection OnRender Complete");
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


void Awareness::LoadEvents()
{
	GEventManager->AddEventHandler(kEventOnGameUpdate, OnGameUpdate);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnCreateObject, OnCreateObject);
	GEventManager->AddEventHandler(kEventOnDestroyObject, OnDestroyObject);
	GEventManager->AddEventHandler(kEventOnUnitDeath, OnUnitDeath);
	GEventManager->AddEventHandler(kEventOnSpellCast, OnProcessSpell);
	GEventManager->AddEventHandler(kEventOnJungleNotification, OnJungleNotify);
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