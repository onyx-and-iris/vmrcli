/**
 * Copyright (c) 2024 Onyx and Iris
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the MIT license. See `ivmr.c` for details.
 */

#ifndef __IVMR_H__
#define __IVMR_H__

#include "VoicemeeterRemote.h"

#define IS_64_BIT sizeof(void *) == 8

PT_VMR create_interface();

#endif /* __IVMR_H__ */