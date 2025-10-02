// ----------------------------------------------------------------------------
// game/server/player_legs.cpp  (SERVER)
// ----------------------------------------------------------------------------
#include "cbase.h"
#include "player_legs.h"

BEGIN_DATADESC(CPlayerLegs)
// DEFINE_FIELD( m_hPlayer, FIELD_EHANDLE ); // add if you persist it
END_DATADESC()

CPlayerLegs::CPlayerLegs() = default;

void CPlayerLegs::SetOwner(CBasePlayer * owner) {
    m_hPlayer = owner;
}
