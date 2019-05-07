/*
 * Copyright (c) 2018, Pensando Systems Inc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>

#include "ev.h"
#include "evutils.h"

void
evutil_run(EV_P)
{
#ifdef LIBEV
    ev_run(EV_A_ 0);
#endif
}
