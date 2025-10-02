
#include "cbase.h"
#include "re4_inventory.h"
static inline void dims(const ItemDef& d,bool rot,int& W,int& H){ W=rot?d.size.h:d.size.w; H=rot?d.size.w:d.size.h; }
bool RE4Inventory::CanPlace(const ItemDef& d, InvPos at, bool rot) const{
    int W,H; dims(d,rot,W,H);
    if(at.x<0||at.y<0||at.x+W>m_W||at.y+H>m_H) return false;
    for(auto& it: m_Items){
        int iw,ih; dims(it.def,it.rotated,iw,ih);
        for(int y=0;y<H;++y) for(int x=0;x<W;++x){
            int ax=at.x+x, ay=at.y+y;
            for(int yy=0;yy<ih;++yy) for(int xx=0;xx<iw;++xx){
                if(ax==it.pos.x+xx && ay==it.pos.y+yy) return false;
            }
        }
    }
    return true;
}
bool RE4Inventory::Place(const ItemDef& d, InvPos at, bool rot){
    if(!CanPlace(d,at,rot)) return false; m_Items.AddToTail({d,at,rot}); return true;
}
bool RE4Inventory::Remove(const CUtlString& id){
    FOR_EACH_VEC(m_Items,i) if(m_Items[i].def.id==id){ m_Items.Remove(i); return true; } return false;
}
