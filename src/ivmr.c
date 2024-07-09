/**
 * @file ivmr.c
 * @author Vincent Burel, Onyx and Iris (code@onyxandiris.online)
 * @brief Functions for initializing the iVMR interface.
 * Defines a single public function that returns a pointer to the interface.
 * @version 0.7.0
 * @date 2024-07-06
 *
 * @copyright Vincent Burel(c)2015-2021 All Rights Reserved
 * https://github.com/vburel2018/Voicemeeter-SDK/blob/main/LICENSE
 *
 * Copyright (c) 2024 Onyx and Iris
 * https://github.com/onyx-and-iris/vmrcli/blob/main/LICENSE
 */

#include <stdbool.h>
#include <stdio.h>
#include <windows.h>
#include "ivmr.h"
#include "util.h"
#include "log.h"

static T_VBVMR_INTERFACE iVMR;

static long initialize_dll_interfaces(PT_VMR vmr);
static bool registry_get_voicemeeter_folder(char *szDir);

/**
 * @brief Create an interface object
 *
 * @return PT_VMR Pointer to the iVMR interface
 */
PT_VMR create_interface()
{
    PT_VMR vmr = &iVMR;
    int rep;

    rep = initialize_dll_interfaces(vmr);
    if (rep < 0)
    {
        if (rep == -100)
        {
            log_fatal("Voicemeeter is not installed");
            exit(EXIT_FAILURE);
        }
        else
        {
            log_fatal("Error loading Voicemeeter dll with code %d\n", rep);
            exit(EXIT_FAILURE);
        }
    }

    return vmr;
}

/*******************************************************************************/
/**                                GET DLL INTERFACE                          **/
/*******************************************************************************/
static long initialize_dll_interfaces(PT_VMR vmr)
{
    HMODULE G_H_Module = NULL;
    char szDllName[1024];
    memset(vmr, 0, sizeof(T_VBVMR_INTERFACE));

    // get Voicemeeter installation directory
    if (!registry_get_voicemeeter_folder(szDllName))
    {
        // Voicemeeter not installed
        return -100;
    }
    // use right dll according to O/S type
    if (sizeof(void *) == 8)
        strcat(szDllName, "\\VoicemeeterRemote64.dll");
    else
        strcat(szDllName, "\\VoicemeeterRemote.dll");

    // Load Dll
    G_H_Module = LoadLibrary(szDllName);
    if (G_H_Module == NULL)
        return -101;

    // Get function pointers
    vmr->VBVMR_Login = (T_VBVMR_Login)GetProcAddress(G_H_Module, "VBVMR_Login");
    vmr->VBVMR_Logout = (T_VBVMR_Logout)GetProcAddress(G_H_Module, "VBVMR_Logout");
    vmr->VBVMR_RunVoicemeeter = (T_VBVMR_RunVoicemeeter)GetProcAddress(G_H_Module, "VBVMR_RunVoicemeeter");
    vmr->VBVMR_GetVoicemeeterType = (T_VBVMR_GetVoicemeeterType)GetProcAddress(G_H_Module, "VBVMR_GetVoicemeeterType");
    vmr->VBVMR_GetVoicemeeterVersion = (T_VBVMR_GetVoicemeeterVersion)GetProcAddress(G_H_Module, "VBVMR_GetVoicemeeterVersion");

    vmr->VBVMR_IsParametersDirty = (T_VBVMR_IsParametersDirty)GetProcAddress(G_H_Module, "VBVMR_IsParametersDirty");
    vmr->VBVMR_GetParameterFloat = (T_VBVMR_GetParameterFloat)GetProcAddress(G_H_Module, "VBVMR_GetParameterFloat");
    vmr->VBVMR_GetParameterStringA = (T_VBVMR_GetParameterStringA)GetProcAddress(G_H_Module, "VBVMR_GetParameterStringA");
    vmr->VBVMR_GetParameterStringW = (T_VBVMR_GetParameterStringW)GetProcAddress(G_H_Module, "VBVMR_GetParameterStringW");
    vmr->VBVMR_GetLevel = (T_VBVMR_GetLevel)GetProcAddress(G_H_Module, "VBVMR_GetLevel");
    vmr->VBVMR_GetMidiMessage = (T_VBVMR_GetMidiMessage)GetProcAddress(G_H_Module, "VBVMR_GetMidiMessage");

    vmr->VBVMR_SetParameterFloat = (T_VBVMR_SetParameterFloat)GetProcAddress(G_H_Module, "VBVMR_SetParameterFloat");
    vmr->VBVMR_SetParameters = (T_VBVMR_SetParameters)GetProcAddress(G_H_Module, "VBVMR_SetParameters");
    vmr->VBVMR_SetParametersW = (T_VBVMR_SetParametersW)GetProcAddress(G_H_Module, "VBVMR_SetParametersW");
    vmr->VBVMR_SetParameterStringA = (T_VBVMR_SetParameterStringA)GetProcAddress(G_H_Module, "VBVMR_SetParameterStringA");
    vmr->VBVMR_SetParameterStringW = (T_VBVMR_SetParameterStringW)GetProcAddress(G_H_Module, "VBVMR_SetParameterStringW");

    vmr->VBVMR_Output_GetDeviceNumber = (T_VBVMR_Output_GetDeviceNumber)GetProcAddress(G_H_Module, "VBVMR_Output_GetDeviceNumber");
    vmr->VBVMR_Output_GetDeviceDescA = (T_VBVMR_Output_GetDeviceDescA)GetProcAddress(G_H_Module, "VBVMR_Output_GetDeviceDescA");
    vmr->VBVMR_Output_GetDeviceDescW = (T_VBVMR_Output_GetDeviceDescW)GetProcAddress(G_H_Module, "VBVMR_Output_GetDeviceDescW");
    vmr->VBVMR_Input_GetDeviceNumber = (T_VBVMR_Input_GetDeviceNumber)GetProcAddress(G_H_Module, "VBVMR_Input_GetDeviceNumber");
    vmr->VBVMR_Input_GetDeviceDescA = (T_VBVMR_Input_GetDeviceDescA)GetProcAddress(G_H_Module, "VBVMR_Input_GetDeviceDescA");
    vmr->VBVMR_Input_GetDeviceDescW = (T_VBVMR_Input_GetDeviceDescW)GetProcAddress(G_H_Module, "VBVMR_Input_GetDeviceDescW");

    vmr->VBVMR_MacroButton_IsDirty = (T_VBVMR_MacroButton_IsDirty)GetProcAddress(G_H_Module, "VBVMR_MacroButton_IsDirty");
    vmr->VBVMR_MacroButton_GetStatus = (T_VBVMR_MacroButton_GetStatus)GetProcAddress(G_H_Module, "VBVMR_MacroButton_GetStatus");
    vmr->VBVMR_MacroButton_SetStatus = (T_VBVMR_MacroButton_SetStatus)GetProcAddress(G_H_Module, "VBVMR_MacroButton_SetStatus");

    // check pointers are valid
    if (vmr->VBVMR_Login == NULL)
        return -1;
    if (vmr->VBVMR_Logout == NULL)
        return -2;
    if (vmr->VBVMR_RunVoicemeeter == NULL)
        return -2;
    if (vmr->VBVMR_GetVoicemeeterType == NULL)
        return -3;
    if (vmr->VBVMR_GetVoicemeeterVersion == NULL)
        return -4;
    if (vmr->VBVMR_IsParametersDirty == NULL)
        return -5;
    if (vmr->VBVMR_GetParameterFloat == NULL)
        return -6;
    if (vmr->VBVMR_GetParameterStringA == NULL)
        return -7;
    if (vmr->VBVMR_GetParameterStringW == NULL)
        return -8;
    if (vmr->VBVMR_GetLevel == NULL)
        return -9;
    if (vmr->VBVMR_SetParameterFloat == NULL)
        return -10;
    if (vmr->VBVMR_SetParameters == NULL)
        return -11;
    if (vmr->VBVMR_SetParametersW == NULL)
        return -12;
    if (vmr->VBVMR_SetParameterStringA == NULL)
        return -13;
    if (vmr->VBVMR_SetParameterStringW == NULL)
        return -14;
    if (vmr->VBVMR_GetMidiMessage == NULL)
        return -15;

    if (vmr->VBVMR_Output_GetDeviceNumber == NULL)
        return -30;
    if (vmr->VBVMR_Output_GetDeviceDescA == NULL)
        return -31;
    if (vmr->VBVMR_Output_GetDeviceDescW == NULL)
        return -32;
    if (vmr->VBVMR_Input_GetDeviceNumber == NULL)
        return -33;
    if (vmr->VBVMR_Input_GetDeviceDescA == NULL)
        return -34;
    if (vmr->VBVMR_Input_GetDeviceDescW == NULL)
        return -35;

    if (vmr->VBVMR_MacroButton_IsDirty == NULL)
        return -36;
    if (vmr->VBVMR_MacroButton_GetStatus == NULL)
        return -37;
    if (vmr->VBVMR_MacroButton_SetStatus == NULL)
        return -38;

    return 0;
}

/*******************************************************************************/
/**                           GET VOICEMEETER DIRECTORY                       **/
/*******************************************************************************/

#define INSTALLER_UNINST_KEY "VB:Voicemeeter {17359A74-1236-5467}"

#ifndef KEY_WOW64_32KEY
#define KEY_WOW64_32KEY 0x0200
#endif

static bool registry_get_voicemeeter_folder(char *szDir)
{
    char szKey[256];
    char sss[1024];
    DWORD nnsize = 1024;
    HKEY hkResult;
    LONG rep;
    DWORD pptype = REG_SZ;
    sss[0] = 0;
    const char uninstDirKey[] = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall";

    // build Voicemeeter uninstallation key
    strcpy(szKey, uninstDirKey);
    strcat(szKey, "\\");
    strcat(szKey, INSTALLER_UNINST_KEY);

    // open key
    rep = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, KEY_READ, &hkResult);
    if (rep != ERROR_SUCCESS)
    {
        // if not present we consider running in 64bit mode and force to read 32bit registry
        rep = RegOpenKeyEx(HKEY_LOCAL_MACHINE, szKey, 0, KEY_READ | KEY_WOW64_32KEY, &hkResult);
    }
    if (rep != ERROR_SUCCESS)
        return false;
    // read uninstall path from registry
    rep = RegQueryValueEx(hkResult, "UninstallString", 0, &pptype, (unsigned char *)sss, &nnsize);
    RegCloseKey(hkResult);

    if (pptype != REG_SZ)
        return false;
    if (rep != ERROR_SUCCESS)
        return false;
    // remove name to get the path only
    remove_last_part_of_path(sss);
    if (nnsize > 512)
        nnsize = 512;
    strncpy(szDir, sss, nnsize);

    return true;
}