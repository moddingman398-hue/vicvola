
#include "cbase.h"
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include "ienginevgui.h"
#include "convar.h"
using namespace vgui;

ConVar mb_inv_grid_w("mb_inv_grid_w","10", FCVAR_ARCHIVE);
ConVar mb_inv_grid_h("mb_inv_grid_h","6",  FCVAR_ARCHIVE);

class CInventoryPanel : public Frame {
    DECLARE_CLASS_SIMPLE(CInventoryPanel, Frame);
public:
    CInventoryPanel(VPANEL parent): Frame(NULL,"InventoryPanel") {
        SetParent(parent); SetTitle("Inventory", true);
        SetSize(800, 520); SetVisible(false); SetSizeable(false);
        SetDeleteSelfOnClose(false);
    }
    void Toggle(){ SetVisible(!IsVisible()); MoveToFront(); RequestFocus(); }
};
static CInventoryPanel* g_pInv = nullptr;
class CInventoryUISys : public CAutoGameSystem {
public: const char* Name() override { return "InventoryUI"; }
    bool Init() override { VPANEL root=enginevgui->GetPanel(PANEL_CLIENTDLL); g_pInv=new CInventoryPanel(root); return true; }
} g_InventoryUISys;
CON_COMMAND(mb_inv_toggle,"Toggle inventory UI"){ if(g_pInv) g_pInv->Toggle(); }
