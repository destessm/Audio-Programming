/****************************************************
 *   This code is explicated in Chapter 8 of        *
 *   "Designing Audio Objects for Max/MSP and Pd"   *
 *   by Eric Lyon.                                  *   
 ****************************************************/

/* Required Max/MSP Header Files */

#include "ext.h" 
#include "z_dsp.h" 
#include "ext_obex.h"

/* The object structure */

typedef struct _cleaner {
	t_pxobject obj;
} t_cleaner;

/* The class declaration */

static t_class *cleaner_class;

/* Function prototypes */

void *cleaner_new(void);
void cleaner_dsp(t_cleaner *x, t_signal **sp, short *count);
t_int *cleaner_perform(t_int *w);
void cleaner_assist(t_cleaner *x, void *b, long msg, long arg, char *dst);

/* The main() function */

int main(void)
{	
	cleaner_class = class_new("cleaner~", (method)cleaner_new, (method)dsp_free, sizeof(t_cleaner), 0,0);
	class_addmethod(cleaner_class, (method)cleaner_dsp, "dsp", A_CANT, 0);
	class_addmethod(cleaner_class, (method)cleaner_assist, "assist", A_CANT, 0);
	class_dspinit(cleaner_class);
	class_register(CLASS_BOX, cleaner_class);
	post("cleaner~ from \"Designing Audio Objects\" by Eric Lyon");
	return 0;
}

/* The new instance routine */

void *cleaner_new(void)
{
	t_cleaner *x = (t_cleaner *)object_alloc(cleaner_class);
	dsp_setup((t_pxobject *)x, 3);
	outlet_new((t_object *)x, "signal");
	return x;
}

/* The perform routine */

t_int *cleaner_perform(t_int *w)
{
	t_float *input = (t_float *) (w[1]);
	t_float *threshmult = (t_float *) (w[2]);
	t_float *multiplier = (t_float *) (w[3]);
	t_float *output = (t_float *) (w[4]);
	t_int n = w[5];
	
	/* Initialize the maximum amplitude to 0.0 */
	
	float maxamp = 0.0;
	float threshold; // locally generated threshold
	float mult; // hold first value of *multiplier
	int i;
	
	/* 
	 Extract the maximum amplitude from the input vector. We
	 assume here that all amplitude values will be positive. 
	 */
	
	for(i = 0; i < n; i++){
		if(maxamp < input[i]){
			maxamp = input[i];
		}
	}
	
	/* 
	 Calculate the synthesis threshold relative to the 
	 maximum amplitude for the current FFT frame. 
	 */
	
	threshold = *threshmult * maxamp;
	
	/* Get first value in *multiplier vector */
	
	mult = *multiplier;
	
	/* Rescale any amplitude values that fall below the threshold */

	for(i = 0; i < n; i++){
		
		if(input[i] < threshold){
			input[i] *= mult;
		}		
		output[i] = input[i];
	}
	
	/* Return the next address on the DSP chain */
	
	return w + 6;
}

/* The assist method */

void cleaner_assist(t_cleaner *x, void *b, long msg, long arg, char *dst)
{
	if (msg==ASSIST_INLET) {
		switch (arg) {
			case 0:
				sprintf(dst,"(signal) Input");
				break;
			case 1:
				sprintf(dst,"(signal) Threshold Generating Multiplier");
				break;
			case 2:
				sprintf(dst,"(signal) Noise Multiplier");
				break;
		}
	} else if (msg==ASSIST_OUTLET) {
		sprintf(dst,"(signal) Output");
	}
}

/* The DSP method */

void cleaner_dsp(t_cleaner *x, t_signal **sp, short *count)
{
	dsp_add(cleaner_perform, 5, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, sp[0]->s_n);
}