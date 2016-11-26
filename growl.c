#include <stdlib.h>
#include <soundpipe.h>
#include "growl.h"

static const SPFLOAT growlants[] = {
/* ae: top right  */
844.0, 1656.0, 2437.0, 3704.0,
/* a: top left */
768.0, 1333.0, 2522.0, 3687.0,
/* i: bottom left */
324.0, 2985.0, 3329.0, 3807.0,
/* u: bottom right */
378.0, 997.0, 2343.0, 3357.0,
};

void growl_create(growl_d **growl)
{
    int i;
    *growl = malloc(sizeof(growl_d));
    growl_d *fp = *growl;
    for(i = 0; i < 4; i++) {
        sp_reson_create(&fp->filt[i]);
    }

    sp_bal_create(&fp->bal);
    sp_dcblock_create(&fp->dcblk);
}

void growl_init(sp_data *sp, growl_d *growl) 
{
    int i;
    for(i = 0; i < 4; i++) {
        sp_reson_init(sp, growl->filt[i]);
        growl->filt[i]->freq = growlants[i];
        growl->filt[i]->bw = 
            (growlants[i] * 0.02) + 50;
    }
    sp_bal_init(sp, growl->bal);
    sp_dcblock_init(sp, growl->dcblk);
    growl->x = 0;
    growl->y = 0;
}

void growl_compute(sp_data *sp, growl_d *growl, SPFLOAT *in, SPFLOAT *out)
{
    int i;
    SPFLOAT tmp_in = *in;
    SPFLOAT tmp_out = 0.0;
    SPFLOAT tf = 0.0;
    SPFLOAT bf = 0.0;
    SPFLOAT freq = 0.0;
    SPFLOAT *x = &growl->x;
    SPFLOAT *y = &growl->y;
    *out = 0.0;

    for(i = 0; i < 4; i++) {
        tf = (*x) * 
            (growlants[i + 3] - growlants[i]) + 
            growlants[i];
        bf = (*x) * 
            (growlants[i + 11] - growlants[i + 7]) + 
            growlants[i + 7];
        freq = (*y) * (bf - tf) + tf;
        growl->filt[i]->freq = freq; 
        growl->filt[i]->bw = 
            ((freq * 0.02) + 50);
        sp_reson_compute(sp, growl->filt[i], &tmp_in, &tmp_out);
        tmp_in = tmp_out;
    }
    
    sp_bal_compute(sp, growl->bal, &tmp_out, in, out);
    tmp_out = *out;
    sp_dcblock_compute(sp, growl->dcblk, &tmp_out, out);
}

void growl_destroy(growl_d **growl)
{
    int i;
    growl_d *fd = *growl;
    for(i = 0; i < 4; i++) {
        sp_reson_destroy(&fd->filt[i]);
    }
    sp_bal_destroy(&fd->bal);
    sp_dcblock_destroy(&fd->dcblk);
    free(*growl);
}

#ifdef BUILD_PLUGIN
#include <sporth.h>

static int growl(plumber_data *pd, sporth_stack *stack, void **ud)
{
    SPFLOAT in;
    SPFLOAT x = 0;
    SPFLOAT y = 0;
    SPFLOAT out = 0;
    growl_d *growl;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            growl_create(&growl);
            *ud = (void *)growl; 
            y = sporth_stack_pop_float(stack);
            x = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:
            growl = *ud;
            growl_init(pd->sp, growl);
            y = sporth_stack_pop_float(stack);
            x = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            growl = *ud;
            y = sporth_stack_pop_float(stack);
            x = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);
            growl->x = x;
            growl->y = y;
            growl_compute(pd->sp, growl, &in, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            growl = *ud;
            growl_destroy(&growl);
            break;
        default: break;
    }
    return PLUMBER_OK;
}

plumber_dyn_func sporth_return_ugen()
{
    return growl;
}

#endif
