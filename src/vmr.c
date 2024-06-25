#include <windows.h>
#include <stdio.h>
#include "vmr.h"

long login(T_VBVMR_INTERFACE *iVMR, int kind)
{
    int rep;

    rep = iVMR->VBVMR_Login();
    Sleep(20);
    if (rep == 1)
    {
        rep = run_voicemeeter(iVMR, kind);
        switch (kind)
        {
        case BASIC:
            puts("Launching Voicemeeter Basic GUI");
            break;
        case BANANA:
            puts("Launching Voicemeeter Banana GUI");
            break;
        case POTATO:
            puts("Launching Voicemeeter Potato GUI");
            break;
        case POTATOX64:
            puts("Launching Voicemeeter Potato x64 GUI");
            break;
        }

        Sleep(1200);
    }
    if (rep == 0)
    {
        puts("Successfully logged into the Voicemeeter API");
        clear_dirty(iVMR);
    }
    return rep;
}

long logout(T_VBVMR_INTERFACE *iVMR)
{
    Sleep(20); /* give time for last command */
    return iVMR->VBVMR_Logout();
}

long run_voicemeeter(T_VBVMR_INTERFACE *iVMR, int kind)
{
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