// src/game/client/vgui_inventory_grid.cpp
#include "cbase.h"
#include "vgui_inventory_grid.h"
#include "vgui/ISurface.h"
#include "iclientmode.h"

using namespace vgui;

CInventoryGridPanel::CInventoryGridPanel()
    : BaseClass(g_pClientMode->GetViewport(), "InventoryGrid")
{
    m_GridW = 10;
    m_GridH = 6;
    m_Cells.SetSize(m_GridW * m_GridH);
    ClearCellsTo(-1);
    SetSize(480, 320);
    SetTitle("Inventory", true);
    SetVisible(false);
}

void CInventoryGridPanel::SetGridSize(int w, int h)
{
    m_GridW = w;
    m_GridH = h;
    m_Cells.SetSize(w * h);
    ClearCellsTo(-1);
}

void CInventoryGridPanel::ClearCellsTo(int value)
{
    for (int i = 0; i < m_Cells.Count(); ++i)
        m_Cells[i] = value;
}

bool CInventoryGridPanel::Fits(const InvItem& it, int gx, int gy) const
{
    const int w = it.rotate ? it.h : it.w;
    const int h = it.rotate ? it.w : it.h;
    if (gx < 0 || gy < 0 || gx + w > m_GridW || gy + h > m_GridH) return false;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
        {
            int idx = (gy + y) * m_GridW + (gx + x);
            if (m_Cells[idx] != -1) return false;
        }
    return true;
}

bool CInventoryGridPanel::PlaceItem(const InvItem& it, int gx, int gy)
{
    if (!Fits(it, gx, gy)) return false;
    m_Items.AddToTail(it);
    const int id = it.id;
    const int w = it.rotate ? it.h : it.w;
    const int h = it.rotate ? it.w : it.h;
    for (int y = 0; y < h; ++y)
        for (int x = 0; x < w; ++x)
            m_Cells[(gy + y) * m_GridW + (gx + x)] = id;
    Repaint();
    return true;
}

void CInventoryGridPanel::RemoveItem(int id)
{
    for (int i = 0; i < m_Cells.Count(); ++i)
        if (m_Cells[i] == id) m_Cells[i] = -1;
    for (int k = 0; k < m_Items.Count(); ++k)
        if (m_Items[k].id == id) { m_Items.Remove(k); break; }
    Repaint();
}

void CInventoryGridPanel::ToggleRotate(int id)
{
    for (int k = 0; k < m_Items.Count(); ++k)
        if (m_Items[k].id == id) { m_Items[k].rotate = !m_Items[k].rotate; break; }
}

void CInventoryGridPanel::Paint()
{
    BaseClass::Paint();
    const int cell = 24, pad = 2;
    for (int y = 0; y < m_GridH; ++y)
        for (int x = 0; x < m_GridW; ++x)
        {
            int l = x * (cell + pad) + 8;
            int t = y * (cell + pad) + 28;
            surface()->DrawSetColor(30, 30, 30, 255);
            surface()->DrawFilledRect(l, t, l + cell, t + cell);
            int id = m_Cells[y * m_GridW + x];
            if (id != -1)
            {
                surface()->DrawSetColor(80, 180, 80, 180);
                surface()->DrawFilledRect(l + 2, t + 2, l + cell - 2, t + cell - 2);
            }
        }
}
