#include "m_pd.h"  

#include <string.h>
#include <math.h>
#include <m_pd.h>
#include <stdio.h>

static char *wodeVersion = "[slambiwode~] (slightly less bad ambisonics worse decoder) v.1 David Estes-Smargiassi 2017";

static t_class *slambiwode_tilde_class;

typedef struct _slambiwode_tilde {  
  t_object  x_obj;
  t_sample f;  
  int dectype;
} t_slambiwode_tilde;  

t_int *slambiwode_tilde_stereo(t_int *w)  
{  
  t_slambiwode_tilde *x = (t_slambiwode_tilde *)(w[1]);  
  t_sample *in1  = (t_sample *)(w[2]); // W
  t_sample *in2  = (t_sample *)(w[3]); // Y
  t_sample *in3  = (t_sample *)(w[4]); // X
  t_sample *out1 = (t_sample *)(w[5]);
  t_sample *out2 = (t_sample *)(w[6]);
  int n = (int)(w[7]);  
  t_sample sample1, sample2, sample3, sample4;

  
  while (n--)
    {
      sample1 = (*in1++); // W
      sample2 = (*in2++); // Y
      sample3 = (*in3++); // X
      
      // basic decoder
      (*out1++) = sample1 * 0.707 + sample3* 0.5;
      (*out2++) = sample1 * 0.707 + sample3* -0.5;
    }

  return (w+8);
}

t_int *slambiwode_tilde_quad(t_int *w)  
{  
  t_slambiwode_tilde *x = (t_slambiwode_tilde *)(w[1]);  
  t_sample *in1  = (t_sample *)(w[2]); // W
  t_sample *in2  = (t_sample *)(w[3]); // Y
  t_sample *in3  = (t_sample *)(w[4]); // X
  t_sample *out1 = (t_sample *)(w[5]);
  t_sample *out2 = (t_sample *)(w[6]);
  t_sample *out3 = (t_sample *)(w[7]);
  t_sample *out4 = (t_sample *)(w[8]);
  int n = (int)(w[9]);  
  t_sample sample1, sample2, sample3, sample4;

  
  while (n--)
    {
      sample1 = (*in1++); // W
      sample2 = (*in2++); // Y
      sample3 = (*in3++); // X
      
      // basic quad decoder [counter-clockwise]
      // front left
      (*out1++) = (sample1 * 0.3536) + (sample2 * 0.1768)  + (sample3 * 0.1768);
      // back left
      (*out2++) = (sample1 * 0.3536) + (sample2 * -0.1768) + (sample3 * 0.1768);
      // back right
      (*out3++) = (sample1 * 0.3536) + (sample2 * -0.1768) + (sample3 * -0.1768);
      // front right
      (*out2++) = (sample1 * 0.3536) + (sample2 * 0.1768)  + (sample3 * -0.1768);
    }

  return (w+10);
}

t_int *slambiwode_tilde_pent(t_int *w)  
{  
  t_slambiwode_tilde *x = (t_slambiwode_tilde *)(w[1]);  
  t_sample *in1  = (t_sample *)(w[2]); // W
  t_sample *in2  = (t_sample *)(w[3]); // X
  t_sample *in3  = (t_sample *)(w[4]); // Y
  t_sample *in4  = (t_sample *)(w[5]); // V
  t_sample *in5  = (t_sample *)(w[6]); // U
  t_sample *out1 = (t_sample *)(w[7]);
  t_sample *out2 = (t_sample *)(w[8]);
  t_sample *out3 = (t_sample *)(w[9]);
  t_sample *out4 = (t_sample *)(w[10]);
  t_sample *out5 = (t_sample *)(w[11]);
  int n = (int)(w[12]);  
  t_sample sample1, sample2, sample3, sample4, sample5;

  
  while (n--)
    {
      sample1 = (*in1++); // W
      sample2 = (*in2++); // Y
      sample3 = (*in3++); // X
      sample4 = (*in4++); // V
      sample5 = (*in5++); // U
      
      // basic pent decoder [counter-clockwise]
      (*out1++) = (sample1 * 0.405) + (sample2 * 0.320)  + (sample3 * 0.310)
	+ (sample4 * 0.085) * (sample5 * 0.125);

      (*out2++) = (sample1 * 0.405) + (sample2 * 0.320)  + (sample3 * -0.310)
	+ (sample4 * 0.085) * (sample5 * -0.125);

      (*out3++) = (sample1 * 0.085) + (sample2 * 0.040)  + (sample3 * 0.000)
	+ (sample4 * 0.045) * (sample5 * 0.000);

      (*out4++) = (sample1 * 0.635) + (sample2 * -0.335)  + (sample3 * 0.380)
	+ (sample4 * 0.080) * (sample5 * 0.080);

      (*out5++) = (sample1 * 0.635) + (sample2 * -0.335)  + (sample3 * -0.280)
	+ (sample4 * -0.080) * (sample5 * -0.080);
    }

  return (w+13);
}

void slambiwode_tilde_dsp(t_slambiwode_tilde *x, t_signal **sp)  
{  
  if(x->dectype <= 1) // Stereo
    {
      dsp_add(slambiwode_tilde_stereo,7, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[0]->s_n);
    }
  else if(x->dectype == 2) // Quad
    {
      dsp_add(slambiwode_tilde_stereo,10, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec,
	      sp[6]->s_vec, sp[7]->s_vec, sp[0]->s_n);
    }
  else // Pent 
    {
      dsp_add(slambiwode_tilde_stereo,12, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec,
	      sp[6]->s_vec, sp[7]->s_vec, sp[8]->s_vec, sp[9]->s_vec,
	      sp[0]->s_n);
    }
}

void *slambiwode_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
  t_slambiwode_tilde *x = (t_slambiwode_tilde *)pd_new(slambiwode_tilde_class);  
  
  char modename[64];
  if(argc > 0)
    {
      atom_string(argv, modename, 64);
    }
  int mode = 1;
  int numInlets = 0;
  int numOutlets= 0;
  if( strcmp(modename,"pent") == 0 )
    {
      mode = 3;
      numInlets = 5;
      numOutlets = 5;
    }
  else if( strcmp(modename,"quad") == 0 )
    {
      mode = 2;
      numInlets = 3;
      numOutlets = 4;
    }
  else // if( strcmp(modename, "stereo") == 0 )
    {
      mode = 1;
      numInlets = 3;
      numOutlets = 2;
    }
  x->dectype = mode;
  
  int i;
  for(i=0; i<numInlets-1; i++)
    {
      inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    }
  for(i=0; i<numOutlets; i++)
    {
      outlet_new(&x->x_obj, &s_signal);
    }
  
  
  return (void *)x;  
}  

void slambiwode_tilde_setup(void) {  
  slambiwode_tilde_class = class_new(gensym("slambiwode~"),  
				   (t_newmethod)slambiwode_tilde_new,  
				   0, 
				   sizeof(t_slambiwode_tilde),  
				   CLASS_DEFAULT,  
				   A_GIMME,
				   0);  
			       
  class_addmethod(slambiwode_tilde_class,(t_method)slambiwode_tilde_dsp, gensym("dsp"), 0);
  CLASS_MAINSIGNALIN(slambiwode_tilde_class, t_slambiwode_tilde, f);
  class_sethelpsymbol(slambiwode_tilde_class, gensym("slambilib-help.pd"));
  post(wodeVersion);
}
