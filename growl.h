typedef struct {
    SPFLOAT x;
    SPFLOAT y;
    sp_reson *filt[4];
    sp_bal *bal;
    sp_dcblock *dcblk;
} growl_d;

void growl_create(growl_d **growl);
void growl_init(sp_data *sp, growl_d *growl);
void growl_compute(sp_data *sp, growl_d *growl, SPFLOAT *in, SPFLOAT *out);
void growl_destroy(growl_d **growl);
