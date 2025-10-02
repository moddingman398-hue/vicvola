// ============================================================================
// FILE: src/game/client/fa_kick_input.cpp  (MOVEMENT FIX)
// DESC: Defines +fa_kick / -fa_kick as ConCommands with string names.
//       FIXED: No longer interferes with jump/movement
//       Usage: bind mouse3 "+fa_kick"
// ============================================================================
#include "cbase.h"

// Global kick state tracking
static bool g_bKickPressed = false;

// Down handler - FIXED: Don't interfere with other inputs
static void FA_Kick_Down(const CCommand& args)
{
    g_bKickPressed = true;

    // Send to server but don't modify any client-side input
    engine->ClientCmd("fa_kick_start\n");

    // Debug output
    Msg("FA_Kick: Started\n");
}

// Up handler - FIXED: Clean state management
static void FA_Kick_Up(const CCommand& args)
{
    g_bKickPressed = false;

    // Send to server
    engine->ClientCmd("fa_kick_release\n");

    // Debug output  
    Msg("FA_Kick: Released\n");
}

// Getter for other systems to check kick state
bool FA_IsKickPressed()
{
    return g_bKickPressed;
}

// Create the commands with string names that include '+' and '-'.
static ConCommand cc_fa_kick_down("+fa_kick", FA_Kick_Down, "Start charging kick", 0);
static ConCommand cc_fa_kick_up("-fa_kick", FA_Kick_Up, "Release kick", 0);