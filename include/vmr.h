#ifndef __VMR_H__
#define __VMR_H__

#include <stdbool.h>
#include "voicemeeterRemote.h"

enum kind
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
long get_parameter_string(PT_VMR vmr, char *param, unsigned short *s);
long set_parameter_float(PT_VMR vmr, char *param, float val);
long set_parameter_string(PT_VMR vmr, char *param, char *s);
long set_parameters(PT_VMR vmr, char *command);

bool is_mdirty(PT_VMR vmr);
long macrobutton_getstatus(PT_VMR vmr, long n, float *val, long mode);
long macrobutton_setstatus(PT_VMR vmr, long n, float val, long mode);

void clear_dirty(PT_VMR vmr);

#endif /* __VMR_H__ */