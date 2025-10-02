// ============================================================================
// FILE: src/game/client/hud_inventory_toggle.cpp
// DESC: Creates the inventory panel lazily; toggles visibility.
// ============================================================================
#include "cbase.h"
#include "vgui/IVGui.h"
#include "vgui_controls/Frame.h"
#include "vgui_inventory_grid.h"
#include "iclientmode.h"

class CInventorySystemUI : public CAutoGameSystem
{
public:
	CInventoryGridPanel *m_pPanel = nullptr;

	void PostInit() OVERRIDE
	{
		if ( !m_pPanel )
			m_pPanel = new CInventoryGridPanel();
	}
} g_InvUI;

CON_COMMAND( inv_toggle, "Toggle inventory panel" )
{
	if ( !g_InvUI.m_pPanel ) return;
	g_InvUI.m_pPanel->SetVisible( !g_InvUI.m_pPanel->IsVisible() );
	if ( g_InvUI.m_pPanel->IsVisible() ) g_InvUI.m_pPanel->MoveToFront();
}