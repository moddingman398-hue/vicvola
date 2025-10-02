// game/client/c_player_legs.cpp
#include "cbase.h"
#include "c_player_legs.h"
#include "input.h"
#include "view.h"

C_BasePlayerLegs::C_BasePlayerLegs() = default;

void C_BasePlayerLegs::Initialize(C_BaseEntity * ownerEntity) {
    C_BasePlayer* owner = ToBasePlayer(ownerEntity);
    if (!owner) { Release(); return; }
    m_hPlayer = owner;
}

void C_BasePlayerLegs::Spawn() {}

bool C_BasePlayerLegs::ShouldDraw() {
    C_BasePlayer* owner = m_hPlayer.Get();
    if (!owner) return false;
    return !::input->CAM_IsThirdPerson() && owner->IsAlive();
}

C_BasePlayerLegs* C_BasePlayerLegs::CreateLegs(C_BaseEntity * ownerEntity,
    const char* modelOverride) {
    C_BasePlayer* owner = ToBasePlayer(ownerEntity);
    if (!owner) return nullptr;

    const char* mdl = modelOverride ? modelOverride : owner->GetModelName();
    auto* legs = new C_BasePlayerLegs();

    if (!legs->InitializeAsClientEntity(mdl, RENDER_GROUP_VIEW_MODEL_OPAQUE)) {
        legs->Release();
        return nullptr;
    }

    legs->SetOwnerEntity(owner);

    if (C_BaseViewModel* vm = owner->GetViewModel(0)) {
        legs->SetParent(vm);
        // legs->SetParentAttachment("attach_origin");
    }
    else {
        legs->FollowEntity(owner, true);
    }

    legs->AddEffects(EF_BONEMERGE | EF_BONEMERGE_FASTCULL | EF_NOSHADOW | EF_NORECEIVESHADOW);
    legs->SetRenderMode(kRenderNormal);

    legs->Initialize(ownerEntity);
    legs->Spawn();
    return legs;
}
