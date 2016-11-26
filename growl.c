#include <stdlib.h>
#include <soundpipe.h>
#include "growl.h"

static const SPFLOAT formants[] = {
/* ae: top right  */
844.0, 1656.0, 2437.0, 3704.0,
/* a: top left */
768.0, 1333.0, 2522.0, 3687.0,
/* i: bottom left */
324.0, 2985.0, 3329.0, 3807.0,
/* u: bottom right */
378.0, 997.0, 2343.0, 3357.0,
};

void formant_create(growl_d **form)
{
    int i;
    *form = malloc(sizeof(growl_d));
    growl_d *fp = *form;
    for(i = 0; i < 4; i++) {
        sp_reson_create(&fp->filt[i]);
    }

    sp_bal_create(&fp->bal);
    sp_dcblock_create(&fp->dcblk);
}

void formant_init(sp_data *sp, growl_d *form) 
{
    int i;
    for(i = 0; i < 4; i++) {
        sp_reson_init(sp, form->filt[i]);
        form->filt[i]->freq = formants[i];
        form->filt[i]->bw = 
            (formants[i] * 0.02) + 50;
    }
    sp_bal_init(sp, form->bal);
    sp_dcblock_init(sp, form->dcblk);
    form->x = 0;
    form->y = 0;
}

void formant_compute(sp_data *sp, growl_d *form, SPFLOAT *in, SPFLOAT *out)
{
    int i;
    SPFLOAT tmp_in = *in;
    SPFLOAT tmp_out = 0.0;
    SPFLOAT tf = 0.0;
    SPFLOAT bf = 0.0;
    SPFLOAT freq = 0.0;
    SPFLOAT *x = &form->x;
    SPFLOAT *y = &form->y;
    *out = 0.0;

    for(i = 0; i < 4; i++) {
        tf = (*x) * 
            (formants[i + 3] - formants[i]) + 
            formants[i];
        bf = (*x) * 
            (formants[i + 11] - formants[i + 7]) + 
            formants[i + 7];
        freq = (*y) * (bf - tf) + tf;
        form->filt[i]->freq = freq; 
        form->filt[i]->bw = 
            ((freq * 0.02) + 50);
        sp_reson_compute(sp, form->filt[i], &tmp_in, &tmp_out);
        tmp_in = tmp_out;
    }
    
    sp_bal_compute(sp, form->bal, &tmp_out, in, out);
    tmp_out = *out;
    sp_dcblock_compute(sp, form->dcblk, &tmp_out, out);
}

void growl_destroy(growl_d **form)
{
    int i;
    growl_d *fd = *form;
    for(i = 0; i < 4; i++) {
        sp_reson_destroy(&fd->filt[i]);
    }
    sp_bal_destroy(&fd->bal);
    sp_dcblock_destroy(&fd->dcblk);
    free(*form);
}

#ifdef BUILD_PLUGIN
#include <sporth.h>

static int growl(plumber_data *pd, sporth_stack *stack, void **ud)
{
    SPFLOAT in;
    SPFLOAT x = 0;
    SPFLOAT y = 0;
    SPFLOAT out = 0;
    growl_d *form;
    switch(pd->mode) {
        case PLUMBER_CREATE:
            formant_create(&form);
            *ud = (void *)form; 
            y = sporth_stack_pop_float(stack);
            x = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_INIT:
            form = *ud;
            formant_init(pd->sp, form);
            y = sporth_stack_pop_float(stack);
            x = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);
            sporth_stack_push_float(stack, 0);
            break;
        case PLUMBER_COMPUTE:
            form = *ud;
            y = sporth_stack_pop_float(stack);
            x = sporth_stack_pop_float(stack);
            in = sporth_stack_pop_float(stack);
            form->x = x;
            form->y = y;
            formant_compute(pd->sp, form, &in, &out);
            sporth_stack_push_float(stack, out);
            break;
        case PLUMBER_DESTROY:
            form = *ud;
            growl_destroy(&form);
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
