#include "ObjectTracker.h"
#include <Shlwapi.h>


ObjectTracker::ObjectTracker(IMenu* Parent)
{
	Menu = Parent->AddMenu("Timer PRO");
	Enabled = Menu->CheckBox("Track Important Timers:", true);

	TrackerFont = GRender->CreateFont("Impact", 30.f, kFontWeightNormal);
	TrackerFont->SetLocationFlags(kFontLocationCenter);
	TrackerFont->SetOutline(true);

	LoadEvents();
}

ObjectTracker::~ObjectTracker()
{
	UnloadEvents();
	Menu->Remove();
}

void ObjectTracker::_OnBuffAdd(IUnit* Source, void* BuffData)
{
	/*if (Source == GEntityList->Player())
	{
		for (int i = 0; i < Source->GetNumberOfBuffs(); i++)
		{
			auto BuffPtr = Source->GetBuffByIndex(i);
			GRender->Notification(Vec4(255, 255, 255, 255), 0, "%s", GBuffData->GetBuffName(BuffData));
		}
	}*/
	
	struct TimedObjectsDefinition
	{
		std::string ObjectName;
		float Duration;
		bool IsMoving;
	};


	static std::vector<TimedObjectsDefinition> Definitions =
	{
		{ "SummonerTeleport", 4.5, true },
		{ "teleport_target", 4.5, true },
		{ "bardrstasis", 2.5, false },
		{ "teleport_turret", 4.5, false },
		{ "rebirth", 6, true },
	};

	TimedObject object;

	for (auto obj : Definitions)
	{
		if (StrStrIA(GBuffData->GetBuffName(BuffData), obj.ObjectName.c_str()) != nullptr)
		{
			object.EndTime = GGame->Time() + obj.Duration;

			object.IsMoving = obj.IsMoving;
			if (object.IsMoving)
				object.ObjectUnit = Source;
			else
				object.StagnantPosition = Source->GetPosition();

			TimedObjects.push_back(object);
		}
	}
}

void ObjectTracker::_OnProcessSpell(CastedSpell const& Args)
{
	struct TimedObjectsDefinition
	{
		std::string ObjectName;
		float Duration;
		bool IsMoving;
	};
	//if (!strstr(Args.Name_, "BasicAttack"))
		//GRender->Notification(Vec4(255, 255, 255, 255), 0, "%s", Args.Name_);
	static std::vector<TimedObjectsDefinition> Definitions =
	{
		{ "JudicatorIntervention", 0, true },
		{ "ZhonyasHourglass", 2.5, false },
	};
	
	TimedObject object;

	for (auto obj : Definitions)
	{
		if (StrStrIA(Args.Name_, obj.ObjectName.c_str()) != nullptr)
		{
			if (StrStrIA(Args.Name_, "JudicatorIntervention") != nullptr)
			{
				object.EndTime = GGame->Time() + ((Args.Caster_->GetSpellLevel(kSlotR) - 1) * 0.5f + 2.f);
			}
			else
				object.EndTime = GGame->Time() + obj.Duration;

			object.IsMoving = obj.IsMoving;
			if (object.IsMoving)
				object.ObjectUnit = Args.Caster_;
			else
				object.StagnantPosition = Args.Position_;

			TimedObjects.push_back(object);
		}
	}
}

void ObjectTracker::_OnCreateObject(IUnit* Source)
{
	struct TimedObjectsDefinition
	{
		std::string ObjectName;
		float Duration;
		bool IsMoving;
	};

	//if (!strstr(Source->GetObjectName(), "missile"))
		//GRender->Notification(Vec4(255, 255, 255, 255), 0, "%s", Source->GetObjectName());


	static std::vector<TimedObjectsDefinition> Definitions =
	{
		{ "LifeAura", 4, false },
		{ "ZileanBase_R_Buf.troy", 3, false },
	};

	TimedObject object;

	for (auto obj : Definitions)
	{
		if (StrStrIA(Source->GetObjectName(), obj.ObjectName.c_str()) != nullptr)
		{
			object.EndTime = GGame->Time() + obj.Duration;

			object.IsMoving = obj.IsMoving;
			if (object.IsMoving)
				object.ObjectUnit = Source;
			else
				object.StagnantPosition = Source->GetPosition();

			TimedObjects.push_back(object);
		}
	}
}

void ObjectTracker::_OnRender()
{
	for (auto obj : TimedObjects)
	{
		Vec2 vecScreen;
		if (obj.IsMoving)
		{
			if (GGame->Projection(obj.ObjectUnit->GetPosition(), &vecScreen) && obj.EndTime - GGame->Time() > 0 && !obj.ObjectUnit->IsDead())
				TrackerFont->Render(vecScreen.x, vecScreen.y, "%.1f", obj.EndTime - GGame->Time());
		}
		else
		{
			if (GGame->Projection(obj.StagnantPosition, &vecScreen) && obj.EndTime - GGame->Time() > 0)
				TrackerFont->Render(vecScreen.x, vecScreen.y, "%.1f", obj.EndTime - GGame->Time());
		}
	}
	
}

void ObjectTracker::LoadEvents()
{
	GEventManager->AddEventHandler(kEventOnSpellCast, OnProcessSpell);
	GEventManager->AddEventHandler(kEventOnBuffAdd, OnBuffAdd);
	GEventManager->AddEventHandler(kEventOnRender, OnRender);
	GEventManager->AddEventHandler(kEventOnCreateObject, OnCreateObject);
}

void ObjectTracker::UnloadEvents()
{
	GEventManager->RemoveEventHandler(kEventOnSpellCast, OnProcessSpell);
	GEventManager->RemoveEventHandler(kEventOnBuffAdd, OnBuffAdd);
	GEventManager->RemoveEventHandler(kEventOnRender, OnRender);
	GEventManager->RemoveEventHandler(kEventOnCreateObject, OnCreateObject);
}

PLUGIN_EVENTD(void) ObjectTracker::OnRender()
{
	GPluginInstance2->_OnRender();
}

PLUGIN_EVENTD(void) ObjectTracker::OnCreateObject(IUnit* Source)
{
	GPluginInstance2->_OnCreateObject(Source);
}

PLUGIN_EVENTD(void) ObjectTracker::OnBuffAdd(IUnit* Source, void* BuffData)
{
	GPluginInstance2->_OnBuffAdd(Source, BuffData);
}

PLUGIN_EVENTD(void) ObjectTracker::OnProcessSpell(CastedSpell const& Args)
{
	GPluginInstance2->_OnProcessSpell(Args);
}