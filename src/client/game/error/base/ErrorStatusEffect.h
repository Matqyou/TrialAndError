//
// Created by Matq on 13/12/2024.
//

#pragma once

#include <client/game/entities/cartesian/Error.h>

class ErrorStatuses;
class ErrorStatusEffect
{
private:
	ErrorStatuses *m_Parent;
	ErrorType m_Type;
	const char *m_Name;
	Texture *m_Texture;
	Entity *m_Effectee;
	unsigned long long m_EffectDuration;
	unsigned long long m_ActivatedTimestamp;
	unsigned long long m_EndTimestamp;
	bool m_Active;

public:
	ErrorStatusEffect(ErrorStatuses *parent,
					  ErrorType type,
					  const char *name,
					  Texture *texture,
					  unsigned long long effect_duration);

	// Getting
	[[nodiscard]] ErrorType GetType() const { return m_Type; }
	[[nodiscard]] const char *Name() const { return m_Name; }
	[[nodiscard]] Texture *GetTexture() const { return m_Texture; }
	[[nodiscard]] Entity *GetEffectee() const { return m_Effectee; }
	[[nodiscard]] unsigned long long GetActivatedTimestamp() const { return m_ActivatedTimestamp; }
	[[nodiscard]] unsigned long long GetEffectDuration() const { return m_EffectDuration; }
	[[nodiscard]] unsigned long long GetEndTimestamp() const { return m_EndTimestamp; }
	[[nodiscard]] bool IsActive() const { return m_Active; }

	// Manipulation
	void SetEffectee(Entity *effectee) { m_Effectee = effectee; }
	void Activate();
	void Deactivate();

	// Ticking
	void Tick();

};

