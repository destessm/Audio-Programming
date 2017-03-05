/****************************************************
 *   This code is used in Chapter 1 of              *
 *   "Designing Audio Objects for Max/MSP and Pd"   *
 *   by Eric Lyon.                                  *   
 ****************************************************/

/* Header files required by Max/MSP */

#include "ext.h"
#include "z_dsp.h"
#include "ext_obex.h"

/* The class pointer */

static t_class *mirror_class;

/* The object structure */

typedef struct _mirror
{
	t_pxobject obj; // The Max/MSP object
} t_mirror;

/* Function prototypes */

void *mirror_new(void);
t_int *mirror_perform(t_int *w);
void mirror_dsp(t_mirror *x, t_signal **sp, short *count);

/* 
 The main() function, where the class is defined, and messages are bound to it. 
 This function is run just once when the external is loaded to Max/MSP. Even if multiple
 copies of the external are instantiated, main() is called only once. So this is a good
 place to print information about the external to the Max window.
 */

int main(void)
{
	/* Initialize the class */
	
	mirror_class = class_new("mirror~", (method)mirror_new, (method)dsp_free, sizeof(t_mirror), 0, 0);

	/* Bind the DSP method, which is called when the DACs are turned on */
    
	class_addmethod(mirror_class,(method)mirror_dsp, "dsp", A_CANT, 0);
	
	/* Add standard Max/MSP methods to your class */

	class_dspinit(mirror_class);
	
	/* Register the class with Max */
	
	class_register(CLASS_BOX, mirror_class);
	
	/* Print authorship message to the Max window */
	
	post("mirror~ from \"Designing Audio Objects\" by Eric Lyon");
	
	/* Return with no error */

	return 0;
}

/* The new instance routine */

void *mirror_new(void)
{
	/* Instantiate a new mirror~ object */
	
    t_mirror *x = (t_mirror *)object_alloc(mirror_class);
	
	/* Create one signal inlet */

    dsp_setup((t_pxobject *)x,1);	
	
	/* Create one signal outlet */
	
    outlet_new((t_pxobject *)x, "signal");
	
	/* Return a pointer to the new object */

    return x;
}

/* The perform routine */

t_int *mirror_perform(t_int *w)
{
	/* Copy the signal inlet pointer */
	
	float *in = (t_float *) (w[1]);
	
	/* Copy the signal outlet pointer */
	
	float *out = (t_float *) (w[2]);
	
	/* Copy the signal vector size */
	
	int n = w[3];
	
	/* Copy 'n' samples from the signal inlet to the signal outlet */
	
	while (n--) { 
		*out++ = *in++;
	}
	
	/* Return the next address on the signal chain */
	
	return w + 4;
}

/* The DSP method */

void mirror_dsp(t_mirror *x, t_signal **sp, short *count)
{
	/* Call the dsp_add() function, passing the DSP routine to
	 be used, which is mirror_perform() in this case; the number of remaining 
	 arguments; a pointer to the signal inlet; a pointer to the signal outlet; 
	 and finally, the signal vector size in samples.
	 */
	
	dsp_add(mirror_perform, 3, sp[0]->s_vec, sp[1]->s_vec, sp[0]->s_n);
}

