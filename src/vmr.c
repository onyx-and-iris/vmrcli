#include <windows.h>
#include <stdio.h>
#include "vmr.h"

enum kind
{
    BASIC = 1,
    BANANA,
    POTATO,
    POTATOX64 = 6
} kind;

long login(T_VBVMR_INTERFACE *iVMR)
{
    int rep;

    rep = iVMR->VBVMR_Login();
    if (rep == 1)
    {
        rep = run_voicemeeter(iVMR);
        puts("Launching Voicemeeter GUI");
        Sleep(1200);
    }
    return rep;
}

long logout(T_VBVMR_INTERFACE *iVMR)
{
    Sleep(20); /* give time for last command */
    return iVMR->VBVMR_Logout();
}

long run_voicemeeter(T_VBVMR_INTERFACE *iVMR)
{
    int kind = POTATO;
    if (sizeof(void *) == 8)
        kind = POTATOX64;
    return iVMR->VBVMR_RunVoicemeeter((long)kind);
}

long type(T_VBVMR_INTERFACE *iVMR, long *type)
{
    return iVMR->VBVMR_GetVoicemeeterType(type);
}

long version(T_VBVMR_INTERFACE *iVMR, long *version)
{
    return iVMR->VBVMR_GetVoicemeeterType(version);
}

bool pdirty(T_VBVMR_INTERFACE *iVMR)
{
    return iVMR->VBVMR_IsParametersDirty() == 1;
}

long set_parameters(T_VBVMR_INTERFACE *iVMR, char *command)
{
    return iVMR->VBVMR_SetParameters(command);
}

bool mdirty(T_VBVMR_INTERFACE *iVMR)
{
    return iVMR->VBVMR_MacroButton_IsDirty() == 1;
}

long macrobutton_getstatus(T_VBVMR_INTERFACE *iVMR, long n, float *val, long mode)
{
    return iVMR->VBVMR_MacroButton_GetStatus(n, val, mode);
}

long macrobutton_setstatus(T_VBVMR_INTERFACE *iVMR, long n, float val, long mode)
{
    return iVMR->VBVMR_MacroButton_SetStatus(n, val, mode);
}

void clear_dirty(T_VBVMR_INTERFACE *iVMR)
{
    Sleep(30);
    while (pdirty(iVMR))
        Sleep(1);
}