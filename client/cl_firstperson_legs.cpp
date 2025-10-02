#include "cbase.h"
#include "c_baseanimating.h"
#include "model_types.h"
#include "bone_setup.h"
#include "clienteffectprecachesystem.h"

class C_FP_Legs : public C_BaseAnimating
{
public:
	DECLARE_CLASS( C_FP_Legs, C_BaseAnimating );
	DECLARE_CLIENTCLASS();

	C_FP_Legs() {}

	int DrawModel( int flags ) OVERRIDE
	{
		// Why: render in view space so it doesn't z-fight world & respects FOV
		this->SetRenderOrigin( CurrentViewOrigin() + Vector(0,0,-48) ); // tweak for your model
		this->SetRenderAngles( QAngle( 0, CurrentViewAngles().y, 0 ) );
		return BaseClass::DrawModel( flags );
	}

	void ClientThink() OVERRIDE
	{
		C_BasePlayer *pl = C_BasePlayer::GetLocalPlayer();
		if ( !pl ) return;
		// Follow player
		SetAbsOrigin( pl->GetAbsOrigin() );
		SetAbsAngles( pl->GetAbsAngles() );
		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}
};

IMPLEMENT_CLIENTCLASS_DT( C_FP_Legs, DT_FP_Legs, CFP_Legs )
END_RECV_TABLE()

// Create one in HUD init or on spawn and assign model:
// auto legs = new C_FP_Legs(); legs->SetModel("models/player/legs_only.mdl");