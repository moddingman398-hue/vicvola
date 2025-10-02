// game/client/c_player_legs.h
#pragma once
#include "cbase.h"
#include "c_baseanimating.h"
#include "c_baseplayer.h"
#include "ehandle.h"

/**
 * Client-only first-person legs.
 * Created on the client; follows the local player's viewmodel or player.
 */
class C_BasePlayerLegs : public C_BaseAnimating {
public:
    DECLARE_CLASS(C_BasePlayerLegs, C_BaseAnimating);

    C_BasePlayerLegs();

    void Initialize(C_BaseEntity* ownerEntity);
    void Spawn();

    static C_BasePlayerLegs* CreateLegs(C_BaseEntity* ownerEntity,
        const char* modelOverride = nullptr);

    bool ShouldDraw() override;

private:
    CHandle<C_BasePlayer> m_hPlayer;  // typed handle fixes C_BaseEntity* → C_BasePlayer*
};
