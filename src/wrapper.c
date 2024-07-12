/**
 * @file wrapper.c
 * @author Onyx and Iris (code@onyxandiris.online)
 * @brief Provides public functions that wrap the iVMR calls
 * @version 0.9.0
 * @date 2024-07-06
 *
 * @copyright Copyright (c) 2024
 * https://github.com/onyx-and-iris/vmrcli/blob/main/LICENSE
 */

#include <windows.h>
#include <stdio.h>
#include <time.h>
#include "wrapper.h"
#include "log.h"
#include "util.h"

#define KIND_STR_LEN 64
#define VERSION_STR_LEN 32
#define LOGIN_TIMEOUT 2

/**
 * @brief Logs into the API.
 * Tests for valid connection for up to 2 seconds.
 * If successful initializes the dirty parameters.
 *
 * @param vmr Pointer to the iVMR interface
 * @param kind The kind of Voicemeeter Gui to launch.
 * @return long VBVMR_Login return value
 */
long login(PT_VMR vmr, int kind)
{
    long rep;
    long v;

    log_trace("VBVMR_Login()");
    rep = vmr->VBVMR_Login();
    if (rep == 1)
    {
        run_voicemeeter(vmr, kind);
        char kind_s[KIND_STR_LEN];
        log_info(
            "Launching Voicemeeter %s GUI",
            kind_as_string(kind_s, kind, KIND_STR_LEN));
    }

    time_t start = time(NULL);
    do
    {
        if ((rep = version(vmr, &v)) == 0)
        {
            char version_s[VERSION_STR_LEN];
            log_info(
                "Successfully logged into the Voicemeeter API v%s",
                version_as_string(version_s, v, VERSION_STR_LEN));
            break;
        }
        Sleep(50);
    } while (difftime(time(NULL), start) < LOGIN_TIMEOUT);

    if (rep == 0)
    {
        clear(vmr, is_pdirty);
    }
    return rep;
}

/**
 * @brief Logs out of the API giving a short wait to allow a
 * final instruction to complete.
 *
 * @param vmr Pointer to the iVMR interface
 * @return long VBVMR_Logout return value
 */
long logout(PT_VMR vmr)
{
    long rep;

    Sleep(20); /* give time for last command */
    log_trace("VBVMR_Logout()");
    rep = vmr->VBVMR_Logout();
    if (rep == 0)
        log_info("Successfully logged out of the Voicemeeter API");
    return rep;
}

long run_voicemeeter(PT_VMR vmr, int kind)
{
    log_trace("VBVMR_RunVoicemeeter(%d)", kind);
    return vmr->VBVMR_RunVoicemeeter((long)kind);
}

long type(PT_VMR vmr, long *type)
{
    log_trace("VBVMR_GetVoicemeeterType(<long> *t)");
    return vmr->VBVMR_GetVoicemeeterType(type);
}

long version(PT_VMR vmr, long *version)
{
    log_trace("VBVMR_GetVoicemeeterVersion(<long> *v)");
    return vmr->VBVMR_GetVoicemeeterVersion(version);
}

bool is_pdirty(PT_VMR vmr)
{
    log_trace("VBVMR_IsParametersDirty()");
    return vmr->VBVMR_IsParametersDirty() == 1;
}

long get_parameter_float(PT_VMR vmr, char *param, float *f)
{
    log_trace("VBVMR_GetParameterFloat(%s, <float> *f)", param);
    return vmr->VBVMR_GetParameterFloat(param, f);
}

long get_parameter_string(PT_VMR vmr, char *param, unsigned short *s)
{
    log_trace("VBVMR_GetParameterStringW(%s, <unsigned short> *s)", param);
    return vmr->VBVMR_GetParameterStringW(param, s);
}

long set_parameter_float(PT_VMR vmr, char *param, float val)
{
    log_trace("VBVMR_SetParameterFloat(%s, %.1f)", param, val);
    return vmr->VBVMR_SetParameterFloat(param, val);
}

long set_parameter_string(PT_VMR vmr, char *param, char *s)
{
    log_trace("VBVMR_SetParameterStringA(%s, %s)", param, s);
    return vmr->VBVMR_SetParameterStringA(param, s);
}

long set_parameters(PT_VMR vmr, char *command)
{
    log_trace("VBVMR_SetParameters(%s)", command);
    return vmr->VBVMR_SetParameters(command);
}

bool is_mdirty(PT_VMR vmr)
{
    log_trace("VBVMR_MacroButton_IsDirty()");
    return vmr->VBVMR_MacroButton_IsDirty() == 1;
}

long macrobutton_getstatus(PT_VMR vmr, long n, float *val, long mode)
{
    log_trace("VBVMR_MacroButton_GetStatus(%ld, <float> *v, %ld)", n, mode);
    return vmr->VBVMR_MacroButton_GetStatus(n, val, mode);
}

long macrobutton_setstatus(PT_VMR vmr, long n, float val, long mode)
{
    log_trace("VBVMR_MacroButton_SetStatus(%ld, %d, %ld)", n, (int)val, mode);
    return vmr->VBVMR_MacroButton_SetStatus(n, val, mode);
}

void clear(PT_VMR vmr, bool (*f)(PT_VMR))
{
    Sleep(30);
    while (f(vmr))
        Sleep(1);
}