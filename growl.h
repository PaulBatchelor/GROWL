typedef struct {
    SPFLOAT x;
    SPFLOAT y;
    sp_reson *filt[4];
    sp_bal *bal;
    sp_dcblock *dcblk;
} growl_d;

void formant_create(growl_d **form);
void formant_init(sp_data *sp, growl_d *form);
void formant_compute(sp_data *sp, growl_d *form, SPFLOAT *in, SPFLOAT *out);
void growl_destroy(growl_d **form);
