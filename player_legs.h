// ----------------------------------------------------------------------------
// game/server/player_legs.h  (SERVER)
// ----------------------------------------------------------------------------
#pragma once
#include "baseanimating.h"
#include "player.h"
#include "ehandle.h"
#include "datamap.h"

class CPlayerLegs : public CBaseAnimating {
public:
    DECLARE_CLASS(CPlayerLegs, CBaseAnimating);
    DECLARE_DATADESC();

    CPlayerLegs();

    void SetOwner(CBasePlayer* owner);

private:
    EHANDLE m_hPlayer; // why: avoid lifetime issues across deletes
};
