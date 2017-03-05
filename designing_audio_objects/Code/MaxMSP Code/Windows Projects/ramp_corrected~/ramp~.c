/****************************************************
 *   This code is a solution to the debugging       *
 *   problem posed in chapter 11 of                 *
 *   "Designing Audio Objects for Max/MSP and Pd"   *
 *   by Eric Lyon.                                  *   
 ****************************************************/

#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"

static t_class *ramp_class;

typedef struct _ramp
{
	t_pxobject x_obj;
	long counter;
	long maximum;
} t_ramp;

void *ramp_new(void);
t_int *ramp_perform(t_int *w);
void ramp_dsp(t_ramp *x, t_signal **sp, short *count);
void ramp_assist(t_ramp *x, void *b, long msg, long arg, char *dst);

int main(void)
{
	t_class *c = ramp_class = 
		class_new("ramp~", (method)ramp_new, (method)dsp_free, sizeof(t_ramp), 0,0);
	class_addmethod(c, (method)ramp_dsp, "dsp", A_CANT, 0);
	class_addmethod(c, (method)ramp_assist, "assist", A_CANT, 0);
	class_dspinit(c);
	class_register(CLASS_BOX, c);
	post("ramp~ from \"Designing Audio Objects\" By Eric Lyon");
	return 0;
}

void *ramp_new(void)
{
    t_ramp *x = (t_ramp *)object_alloc(ramp_class);
	
    dsp_setup((t_pxobject *)x,2);	
    outlet_new((t_pxobject *)x, "signal");	
	x->maximum = 44100;
	x->counter = 0;
    return x;
}

void ramp_assist(t_ramp *x, void *b, long msg, long arg, char *dst)
{
	if (msg == ASSIST_INLET) {
		switch(arg){
			case 0: sprintf(dst,"(signal) Trigger"); break;
			case 1: sprintf(dst,"(signal) Ramp Time in Samples"); break;
				
		}
	} else if (msg == ASSIST_OUTLET) {
		sprintf(dst,"(signal) Ramp");
	}
}

t_int *ramp_perform(t_int *w)
{
	t_ramp *x = (t_ramp *) (w[1]);
	float *trigger = (t_float *)(w[2]);
	float *maxcount = (t_float *)(w[3]);
	float *out = (t_float *)(w[4]);
	int n = w[5];
	long maximum = x->maximum;
	long counter = x->counter;
	float invmax; 
	int i;
	invmax = maximum ? 1.0 / (float)maximum : 0.0;
	for(i = 0; i < n; i++){
		if(trigger[i]){
			counter = 0;
			maximum = maxcount[i];
		}
		out[i] = counter * invmax;
		if(counter < maximum){
			counter++;
		}
	}
	x->counter = counter;
	x->maximum = maximum;
	return w + 6;
}

void ramp_dsp(t_ramp *x, t_signal **sp, short *count)
{
	dsp_add(ramp_perform, 5, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

