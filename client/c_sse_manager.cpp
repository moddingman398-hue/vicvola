
#include "cbase.h"
#include "materialsystem/imaterialsystem.h"
#include "iviewrender.h"
#include "screenspaceeffects.h"
#include "convar.h"

static ConVar mb_pp_vignette_on_ads("mb_pp_vignette_on_ads","1", FCVAR_ARCHIVE);
extern bool MB_GetWantsADS();

class CMBVignette : public IScreenSpaceEffect {
public:
    void Init() override {}
    void Shutdown() override {}
    void SetParameters(KeyValues*) override {}
    void Render(int x, int y, int w, int h) override {
        if (!mb_pp_vignette_on_ads.GetBool() || !MB_GetWantsADS()) return;
        IMaterial* mat = materials->FindMaterial("effects/vignette", TEXTURE_GROUP_OTHER);
        if (!mat || IsErrorMaterial(mat)) return;
        CMatRenderContextPtr ctx(materials);
        ctx->DrawScreenSpaceQuad(mat, x, y, w, h);
    }
};

static CMBVignette g_MBVignette;
EXPOSE_INTERFACE(CMBVignette, IScreenSpaceEffect, "MB_VignetteEffect");
