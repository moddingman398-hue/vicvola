
#pragma once
#include "utlvector.h"
#include "utlstring.h"
struct InvSize{int w,h;}; struct InvPos{int x,y;};
enum ItemType{IT_WEAPON,IT_AMMO,IT_ARMOR,IT_ATTACHMENT,IT_MISC};
struct ItemDef{CUtlString id; ItemType type; InvSize size;};
struct PlacedItem{ItemDef def; InvPos pos; bool rotated;};
class RE4Inventory{
public:
    RE4Inventory(int w,int h):m_W(w),m_H(h){}
    bool CanPlace(const ItemDef& d, InvPos at, bool rot) const;
    bool Place(const ItemDef& d, InvPos at, bool rot);
    bool Remove(const CUtlString& id);
    CUtlVector<PlacedItem> m_Items; int m_W,m_H;
};
