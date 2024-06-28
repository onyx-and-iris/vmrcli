#include <stdbool.h>
#include <stdio.h>
#include "cdll.h"
#include "util.h"

/*******************************************************************************/
/**                           GET VOICEMEETER DIRECTORY                       **/
/*******************************************************************************/

#define INSTALLER_UNINST_KEY "VB:Voicemeeter {17359A74-1236-5467}"

#ifndef KEY_WOW64_32KEY
#define KEY_WOW64_32KEY 0x0200
#endif

bool __cdecl registry_get_voicemeeter_folder(char *szDir)
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
    remove_name_in_path(sss);
    if (nnsize > 512)
        nnsize = 512;
    strncpy(szDir, sss, nnsize);

    return true;
}

/*******************************************************************************/
/**                                GET DLL INTERFACE                          **/
/*******************************************************************************/
long initialize_dll_interfaces(T_VBVMR_INTERFACE *iVMR)
{
    HMODULE G_H_Module = NULL;
    char szDllName[1024];
    memset(iVMR, 0, sizeof(T_VBVMR_INTERFACE));

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
    iVMR->VBVMR_Login = (T_VBVMR_Login)GetProcAddress(G_H_Module, "VBVMR_Login");
    iVMR->VBVMR_Logout = (T_VBVMR_Logout)GetProcAddress(G_H_Module, "VBVMR_Logout");
    iVMR->VBVMR_RunVoicemeeter = (T_VBVMR_RunVoicemeeter)GetProcAddress(G_H_Module, "VBVMR_RunVoicemeeter");
    iVMR->VBVMR_GetVoicemeeterType = (T_VBVMR_GetVoicemeeterType)GetProcAddress(G_H_Module, "VBVMR_GetVoicemeeterType");
    iVMR->VBVMR_GetVoicemeeterVersion = (T_VBVMR_GetVoicemeeterVersion)GetProcAddress(G_H_Module, "VBVMR_GetVoicemeeterVersion");

    iVMR->VBVMR_IsParametersDirty = (T_VBVMR_IsParametersDirty)GetProcAddress(G_H_Module, "VBVMR_IsParametersDirty");
    iVMR->VBVMR_GetParameterFloat = (T_VBVMR_GetParameterFloat)GetProcAddress(G_H_Module, "VBVMR_GetParameterFloat");
    iVMR->VBVMR_GetParameterStringA = (T_VBVMR_GetParameterStringA)GetProcAddress(G_H_Module, "VBVMR_GetParameterStringA");
    iVMR->VBVMR_GetParameterStringW = (T_VBVMR_GetParameterStringW)GetProcAddress(G_H_Module, "VBVMR_GetParameterStringW");
    iVMR->VBVMR_GetLevel = (T_VBVMR_GetLevel)GetProcAddress(G_H_Module, "VBVMR_GetLevel");
    iVMR->VBVMR_GetMidiMessage = (T_VBVMR_GetMidiMessage)GetProcAddress(G_H_Module, "VBVMR_GetMidiMessage");

    iVMR->VBVMR_SetParameterFloat = (T_VBVMR_SetParameterFloat)GetProcAddress(G_H_Module, "VBVMR_SetParameterFloat");
    iVMR->VBVMR_SetParameters = (T_VBVMR_SetParameters)GetProcAddress(G_H_Module, "VBVMR_SetParameters");
    iVMR->VBVMR_SetParametersW = (T_VBVMR_SetParametersW)GetProcAddress(G_H_Module, "VBVMR_SetParametersW");
    iVMR->VBVMR_SetParameterStringA = (T_VBVMR_SetParameterStringA)GetProcAddress(G_H_Module, "VBVMR_SetParameterStringA");
    iVMR->VBVMR_SetParameterStringW = (T_VBVMR_SetParameterStringW)GetProcAddress(G_H_Module, "VBVMR_SetParameterStringW");

    iVMR->VBVMR_Output_GetDeviceNumber = (T_VBVMR_Output_GetDeviceNumber)GetProcAddress(G_H_Module, "VBVMR_Output_GetDeviceNumber");
    iVMR->VBVMR_Output_GetDeviceDescA = (T_VBVMR_Output_GetDeviceDescA)GetProcAddress(G_H_Module, "VBVMR_Output_GetDeviceDescA");
    iVMR->VBVMR_Output_GetDeviceDescW = (T_VBVMR_Output_GetDeviceDescW)GetProcAddress(G_H_Module, "VBVMR_Output_GetDeviceDescW");
    iVMR->VBVMR_Input_GetDeviceNumber = (T_VBVMR_Input_GetDeviceNumber)GetProcAddress(G_H_Module, "VBVMR_Input_GetDeviceNumber");
    iVMR->VBVMR_Input_GetDeviceDescA = (T_VBVMR_Input_GetDeviceDescA)GetProcAddress(G_H_Module, "VBVMR_Input_GetDeviceDescA");
    iVMR->VBVMR_Input_GetDeviceDescW = (T_VBVMR_Input_GetDeviceDescW)GetProcAddress(G_H_Module, "VBVMR_Input_GetDeviceDescW");

    // check pointers are valid
    if (iVMR->VBVMR_Login == NULL)
        return -1;
    if (iVMR->VBVMR_Logout == NULL)
        return -2;
    if (iVMR->VBVMR_RunVoicemeeter == NULL)
        return -2;
    if (iVMR->VBVMR_GetVoicemeeterType == NULL)
        return -3;
    if (iVMR->VBVMR_GetVoicemeeterVersion == NULL)
        return -4;
    if (iVMR->VBVMR_IsParametersDirty == NULL)
        return -5;
    if (iVMR->VBVMR_GetParameterFloat == NULL)
        return -6;
    if (iVMR->VBVMR_GetParameterStringA == NULL)
        return -7;
    if (iVMR->VBVMR_GetParameterStringW == NULL)
        return -8;
    if (iVMR->VBVMR_GetLevel == NULL)
        return -9;
    if (iVMR->VBVMR_SetParameterFloat == NULL)
        return -10;
    if (iVMR->VBVMR_SetParameters == NULL)
        return -11;
    if (iVMR->VBVMR_SetParametersW == NULL)
        return -12;
    if (iVMR->VBVMR_SetParameterStringA == NULL)
        return -13;
    if (iVMR->VBVMR_SetParameterStringW == NULL)
        return -14;
    if (iVMR->VBVMR_GetMidiMessage == NULL)
        return -15;

    if (iVMR->VBVMR_Output_GetDeviceNumber == NULL)
        return -30;
    if (iVMR->VBVMR_Output_GetDeviceDescA == NULL)
        return -31;
    if (iVMR->VBVMR_Output_GetDeviceDescW == NULL)
        return -32;
    if (iVMR->VBVMR_Input_GetDeviceNumber == NULL)
        return -33;
    if (iVMR->VBVMR_Input_GetDeviceDescA == NULL)
        return -34;
    if (iVMR->VBVMR_Input_GetDeviceDescW == NULL)
        return -35;

    return 0;
}