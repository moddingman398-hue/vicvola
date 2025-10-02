
#pragma once
// Simple declarations so any file can call these without externs sprinkled around.
void MB_SetWantsADS(bool b);
bool MB_GetWantsADS();
void MB_UpdateADS(float frametime);
float MB_ComputeADSFOV(float baseFov);
