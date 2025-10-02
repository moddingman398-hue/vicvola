// src/game/client/vgui_inventory_grid.h
#pragma once
#include "vgui_controls/Frame.h"
#include "utlvector.h"

struct InvItem { int id, w, h; bool rotate; };

class CInventoryGridPanel : public vgui::Frame
{
    DECLARE_CLASS_SIMPLE(CInventoryGridPanel, vgui::Frame);
public:
    CInventoryGridPanel();

    void SetGridSize(int w, int h);
    bool PlaceItem(const InvItem& item, int gx, int gy);
    void RemoveItem(int id);
    void ToggleRotate(int id);

private:
    int m_GridW, m_GridH;
    CUtlVector<int> m_Cells;         // -1 empty or item id
    CUtlVector<InvItem> m_Items;

    void Paint() OVERRIDE;
    bool Fits(const InvItem& it, int gx, int gy) const;
    void ClearCellsTo(int value);
};
