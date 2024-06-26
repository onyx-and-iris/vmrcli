#include <windows.h>
#include <stdio.h>
#include "vmr.h"
#include "log.h"

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
            log_info("Launching Voicemeeter Basic GUI");
            break;
        case BANANA:
            log_info("Launching Voicemeeter Banana GUI");
            break;
        case POTATO:
            log_info("Launching Voicemeeter Potato GUI");
            break;
        case POTATOX64:
            log_info("Launching Voicemeeter Potato x64 GUI");
            break;
        }

        Sleep(1200);
    }
    if (rep == 0)
    {
        log_info("Successfully logged into the Voicemeeter API");
        clear_dirty(iVMR);
    }
    return rep;
}

long logout(T_VBVMR_INTERFACE *iVMR)
{
    int rep;

    Sleep(20); /* give time for last command */
    rep = iVMR->VBVMR_Logout();
    if (rep == 0)
        log_info("Successfully logged out of the Voicemeeter API");
    return rep;
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

bool is_pdirty(T_VBVMR_INTERFACE *iVMR)
{
    return iVMR->VBVMR_IsParametersDirty() == 1;
}

long get_parameter_float(T_VBVMR_INTERFACE *iVMR, char *param, float *f)
{
    return iVMR->VBVMR_GetParameterFloat(param, f);
}

long get_parameter_string(T_VBVMR_INTERFACE *iVMR, char *param, char *s)
{
    return iVMR->VBVMR_GetParameterStringA(param, s);
}

long set_parameter_float(T_VBVMR_INTERFACE *iVMR, char *param, float val)
{
    return iVMR->VBVMR_SetParameterFloat(param, val);
}

long set_parameter_string(T_VBVMR_INTERFACE *iVMR, char *param, char *s)
{
    return iVMR->VBVMR_SetParameterStringA(param, s);
}

long set_parameters(T_VBVMR_INTERFACE *iVMR, char *command)
{
    return iVMR->VBVMR_SetParameters(command);
}

bool is_mdirty(T_VBVMR_INTERFACE *iVMR)
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
    while (is_pdirty(iVMR))
        Sleep(1);
}