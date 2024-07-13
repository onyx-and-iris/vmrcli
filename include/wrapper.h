/**
 * Copyright (c) 2024 Onyx and Iris
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `wrapper.c` for details.
 */

#ifndef __WRAPPER_H__
#define __WRAPPER_H__

#include <stdbool.h>
#include "voicemeeterRemote.h"

enum kind : int
{
    UNKNOWN = -1,
    BASIC = 1,
    BANANA,
    POTATO,
    BASICX64,
    BANANAX64,
    POTATOX64,
    MACROBUTTONS = 11,
    STREAMERVIEW
};

long login(PT_VMR vmr, int kind);
long logout(PT_VMR vmr);
long run_voicemeeter(PT_VMR vmr, int kind);
long type(PT_VMR vmr, long *type);
long version(PT_VMR vmr, long *version);

bool is_pdirty(PT_VMR vmr);
long get_parameter_float(PT_VMR vmr, char *param, float *f);
long get_parameter_string(PT_VMR vmr, char *param, wchar_t *s);
long set_parameter_float(PT_VMR vmr, char *param, float val);
long set_parameter_string(PT_VMR vmr, char *param, char *s);
long set_parameters(PT_VMR vmr, char *command);

bool is_mdirty(PT_VMR vmr);
long macrobutton_getstatus(PT_VMR vmr, long n, float *val, long mode);
long macrobutton_setstatus(PT_VMR vmr, long n, float val, long mode);

void clear(PT_VMR vmr, bool (*f)(PT_VMR));

#endif /* __WRAPPER_H__ */