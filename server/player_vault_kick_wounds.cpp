#include "cbase.h"
#include "movevars_shared.h"
#include "player.h"
#include "physics.h"
#include "IEffects.h"
#include "te_effect_dispatch.h"

ConVar sv_vault_maxheight( "sv_vault_maxheight", "48", FCVAR_ARCHIVE, "Max obstacle height to vault." );
ConVar sv_vault_forward( "sv_vault_forward", "32", FCVAR_ARCHIVE, "Forward clearance needed." );
ConVar sv_kick_force( "sv_kick_force", "600", FCVAR_ARCHIVE, "Kick impulse to physics objects." );
ConVar sv_bleed_dps( "sv_bleed_dps", "3", FCVAR_ARCHIVE, "Bleed damage per second." );

class CPlayerFAExt
{
public:
	static bool TryVault( CBasePlayer *pPlayer )
	{
		if ( !pPlayer ) return false;
		Vector fwd; AngleVectors( pPlayer->EyeAngles(), &fwd );
		Vector start = pPlayer->EyePosition();
		Vector end = start + fwd * 48.0f;

		trace_t tr;
		UTIL_TraceHull( start, end, VEC_HULL_MIN, VEC_HULL_MAX, MASK_PLAYERSOLID_BRUSHONLY, pPlayer, COLLISION_GROUP_PLAYER_MOVEMENT, &tr );
		if ( tr.fraction < 1.0f )
		{
			// Obstacle ahead: check height
			Vector top = tr.endpos + Vector(0,0, sv_vault_maxheight.GetFloat());
			trace_t tr2;
			UTIL_TraceHull( tr.endpos + fwd * sv_vault_forward.GetFloat(), top, VEC_HULL_MIN, VEC_HULL_MAX, MASK_PLAYERSOLID_BRUSHONLY, pPlayer, COLLISION_GROUP_PLAYER_MOVEMENT, &tr2 );
			if ( tr2.startsolid || tr2.fraction < 1.0f )
				return false;

			// Teleport up-and-over with gentle forward velocity
			Vector dest = tr.endpos + fwd * 24.0f + Vector(0,0, sv_vault_maxheight.GetFloat());
			pPlayer->SetAbsOrigin( dest );
			pPlayer->SetAbsVelocity( fwd * 120.0f );
			pPlayer->DoMuzzleFlash(); // temp feedback
			return true;
		}
		return false;
	}

	static void Kick( CBasePlayer *pPlayer )
	{
		Vector fwd; AngleVectors( pPlayer->EyeAngles(), &fwd );
		Vector src = pPlayer->EyePosition();
		Vector dst = src + fwd * 64.0f;

		trace_t tr;
		UTIL_TraceLine( src, dst, MASK_SHOT, pPlayer, COLLISION_GROUP_PLAYER, &tr );
		if ( tr.fraction < 1.0f && tr.m_pEnt )
		{
			IPhysicsObject *phys = tr.m_pEnt->VPhysicsGetObject();
			if ( phys )
				phys->ApplyForceCenter( fwd * sv_kick_force.GetFloat() );
			CTakeDamageInfo info( pPlayer, pPlayer, 15.0f, DMG_CLUB );
			tr.m_pEnt->TakeDamage( info );
			EmitSound_t params; params.m_pSoundName = "Player.Kick"; pPlayer->EmitSound( params );
		}
	}
};

// Simple wounds on player
class CPlayerWounds : public CLogicalEntity
{
public:
	DECLARE_CLASS( CPlayerWounds, CLogicalEntity );
	DECLARE_DATADESC();

	float m_flBleedUntil;

	void InputStartBleeding( inputdata_t &data ) { m_flBleedUntil = gpGlobals->curtime + 10.0f; }
	void InputStopBleeding( inputdata_t &data )  { m_flBleedUntil = gpGlobals->curtime; }
	void InputBandage( inputdata_t &data )       { m_flBleedUntil = gpGlobals->curtime; }

	void Think() OVERRIDE
	{
		CBasePlayer *p = UTIL_GetLocalPlayer();
		if ( p && gpGlobals->curtime < m_flBleedUntil )
		{
			p->TakeDamage( CTakeDamageInfo( this, this, sv_bleed_dps.GetFloat() * gpGlobals->frametime, DMG_GENERIC ) );
		}
		SetNextThink( gpGlobals->curtime + 0.1f );
	}
};

LINK_ENTITY_TO_CLASS( player_wounds, CPlayerWounds );

BEGIN_DATADESC( CPlayerWounds )
	DEFINE_FIELD( m_flBleedUntil, FIELD_TIME ),
	DEFINE_INPUTFUNC( FIELD_VOID, "StartBleeding", InputStartBleeding ),
	DEFINE_INPUTFUNC( FIELD_VOID, "StopBleeding",  InputStopBleeding ),
	DEFINE_INPUTFUNC( FIELD_VOID, "Bandage",       InputBandage ),
END_DATADESC()

// Bind server console commands for vault/kick (you can hook to +use/+jump combos)
CON_COMMAND( fa_vault, "Attempt a vault over a low obstacle." )
{
	CBasePlayer *p = UTIL_GetCommandClient();
	if ( p ) CPlayerFAExt::TryVault( p );
}

CON_COMMAND( fa_kick, "Kick forward (physics push + light damage)." )
{
	CBasePlayer *p = UTIL_GetCommandClient();
	if ( p ) CPlayerFAExt::Kick( p );
}
