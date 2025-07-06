//
// Created by Matq on 13/12/2024.
//

#pragma once

#include "effect/ErrorBulletFrenzy.h"
#include "effect/ErrorCantHear.h"
#include "effect/ErrorConfusingHealth.h"
#include "effect/ErrorDangerousRecoil.h"
#include "effect/ErrorDisoriented.h"
#include "effect/ErrorHealersParadise.h"
#include "effect/ErrorInvincible.h"
#include "effect/ErrorRangedFists.h"
#include "effect/ErrorSlowdown.h"
#include "effect/ErrorSpiky.h"
#include "effect/ErrorTeleport.h"
#include "client/game/ui/Interface.h"

struct ErrorFrame
{
	Vec2i Pos;
	Vec2i Size;
};

struct ErrorFrames
{
	std::vector<ErrorFrame> Frames;

	ErrorFrames() = default;
	void CreateFrames(std::vector<ErrorStatusEffect *>& group);
	void CreateFramesUI(std::vector<ErrorStatusEffect *>& group);

};

class ErrorStatuses : public InterfaceElement
{
private:
	Character *m_Parent;
	Drawing *m_Drawing;
	bool m_Gui; // temp

	ErrorStatusEffect *m_LastActivatedEffect;
	std::vector<ErrorStatusEffect *> m_Effects;
	ErrorFrames m_Frames;

	void DrawIngame(); // temp
	void DrawAsGUI(); // temp

public:
	ErrorBulletFrenzy BulletFrenzy;
	ErrorCantHear CantHear;
	ErrorConfusingHealth ConfusingHealth;
	ErrorDangerousRecoil DangerousRecoil;
	ErrorDisoriented Disoriented;
	ErrorHealersParadise HealersParadise;
	ErrorInvincible Invincible;
	ErrorRangedFists RangedFists;
	ErrorSlowdown Slowdown;
	ErrorSpiky Spiky;
	ErrorTeleport Teleport;

	ErrorStatuses(Interface *interface, Character *parent, bool gui);

	// Getting
	[[nodiscard]] ErrorStatusEffect *GetLastActivated() { return m_LastActivatedEffect; }
	[[nodiscard]] std::vector<ErrorStatusEffect *>& Group() { return m_Effects; }

	// Generating
	bool AnyActive();
	bool AnyActive(double from_seconds_ago);

	// Setting
	void SetLastActivated(ErrorStatusEffect *effect) { m_LastActivatedEffect = effect; }

	// Ticking
	void Tick();
	void Draw() override;

};
