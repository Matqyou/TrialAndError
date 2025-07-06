//
// Created by Matq on 13/12/2024.
//

#include "../ErrorStatuses.h"
#include "client/game/GameReference.h"
#include "ErrorStatusEffect.h"

ErrorStatusEffect::ErrorStatusEffect(ErrorStatuses *parent,
									 ErrorType type,
									 const char *name,
									 Texture *texture,
									 unsigned long long effect_duration)
{
	m_Parent = parent;
	m_Type = type;
	m_Name = name;
	m_Texture = texture;
	m_Effectee = nullptr;
	m_EffectDuration = effect_duration;
	m_ActivatedTimestamp = 0;
	m_EndTimestamp = 0;
	m_Active = false;

	m_Parent->Group().push_back(this);
}

void ErrorStatusEffect::Activate()
{
	m_Parent->SetLastActivated(this);
	if (m_EffectDuration == 0)
		return;

	m_ActivatedTimestamp = GameReference.World()->GetTick();
	m_EndTimestamp = m_ActivatedTimestamp + m_EffectDuration;
	m_Active = true;
}
void ErrorStatusEffect::Deactivate()
{
	m_Active = false;
}

void ErrorStatusEffect::Tick()
{
	if (!m_Active)
		return;

	if (GameReference.World()->GetTick() >= m_EndTimestamp)
		m_Active = false;
}
