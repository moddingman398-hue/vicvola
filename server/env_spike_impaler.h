
#pragma once
#include "baseentity.h"
class CEnvSpikeImpaler: public CBaseEntity{
public: DECLARE_CLASS(CEnvSpikeImpaler,CBaseEntity); DECLARE_DATADESC();
    void Spawn() override; void Touch(CBaseEntity* pOther) override; float m_flRequiredSpeed;
};
