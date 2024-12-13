//
// Created by Matq on 13/12/2024.
//

#include "ErrorStatusEffect.h"
#include "../ErrorStatuses.h"

ErrorStatusEffect::ErrorStatusEffect(ErrorType type,
                                     Texture* texture,
                                     ErrorStatuses* parent,
                                     unsigned long long effect_duration) {
    m_Parent = parent;
    m_Type = type;
    m_Texture = texture;
    m_Effectee = nullptr;
    m_EffectDuration = effect_duration;
    m_ActivatedTimestamp = 0;
    m_EndTimestamp = 0;
    m_Active = false;

    m_Parent->Group().push_back(this);
}

void ErrorStatusEffect::Activate() {
    if (m_EffectDuration == 0)
        return;

    m_ActivatedTimestamp = m_Parent->GetInterface()->GameWindow()->World()->GetTick();
    m_EndTimestamp = m_ActivatedTimestamp + m_EffectDuration;
    m_Active = true;
}
void ErrorStatusEffect::Deactivate() {
    m_Active = false;
}

void ErrorStatusEffect::Tick() {
    if (!m_Active)
        return;

    if (m_Parent->GetInterface()->GameWindow()->World()->GetTick() >= m_EndTimestamp)
        m_Active = false;
}
