#include "m_pd.h"
#include <math.h>
#include <stdlib.h>
#include <time.h>

#define DEFAULT_INC 2

/* Complex Number Functions */
float complexMult_r(float a, float b, float c, float d)
{
  return a*c - b*d;
}

float complexMult_i(float a, float b, float c, float d)
{
  return a*d + b*c;
}

float complexGetPhase(float a, float b)
{
  if(0 == a && 0 == b)
    return 0;
  else
    return atan2(b, a);
}

float complexGetAmp(float a, float b)
{
  return a*a + b*b;
}

char *version = "[squeak~] by David Estes-Smargiassi, 2017";

static t_class *squeak_tilde_class;

typedef struct _squeak_tilde {
  t_object x_obj;
  t_sample f;
  int block_size;
  int numinc; // only integer multiples, just for convenience
  float phase;
} t_squeak_tilde;

t_int *squeak_tilde_perform(t_int *w)
{
  t_squeak_tilde *x = (t_squeak_tilde *)(w[1]);
  t_sample *in1 = (t_sample *)(w[2]);
  t_sample *in2 = (t_sample *)(w[3]);
  t_sample *out1 = (t_sample *)(w[4]);
  t_sample *out2 = (t_sample *)(w[5]);
  int n = (int)(w[6]);
  int N = n;
  int i;
  float conv_r;
  float conv_i;
  float phase;
  t_sample in_i, in_r;


  while (n)
    {
      in_i = (*in2++);
      in_r = (*in1++);
      
      if(n == N)
	{
	  for(i = 0; i < x->numinc; i++)
	    {
	      (*out1++) = 0;
	      (*out2++) = 0;
	      n--;
	    }
	}

      phase = x->phase * M_PI * 0.00555555555;
      conv_r = complexMult_r(in_r, in_i, cos(phase), sin(phase));
      conv_i = complexMult_i(in_r, in_i, cos(phase), sin(phase));
      
      (*out1++) = conv_r;// * x->window[n];
      (*out2++) = conv_i;// * x->window[n];
      n--;
    }
  for(i = 0; i < x->numinc; i++)
    {
      (*in1++);
      (*in2++);
    }
	
  return (w+7);
}

void squeak_tilde_dsp(t_squeak_tilde *x, t_signal **sp)
{
  dsp_add(squeak_tilde_perform, 6, x, sp[0]->s_vec, sp[1]->s_vec,
          sp[2]->s_vec, sp[3]->s_vec, sp[0]->s_n);
}

void squeak_tilde_pitch(t_squeak_tilde *x, t_symbol *s, int argc, t_atom *argv)
{
  if(argc)
    {
      x->numinc = (int) atom_getfloat(&argv[0]);
      post("[squeak~]: uptuning by %d", x->numinc);
    }
}

void squeak_tilde_phase(t_squeak_tilde *x, t_symbol *s, int argc, t_atom *argv)
{
  if(argc)
    {
      x->phase = atom_getfloat(&argv[0]);
      post("[squeak~]: rephasing by %.2f", x->phase);
    }
}

void *squeak_tilde_new(t_atom *s, int argc, t_atom *argv)
{
  post(version);
  t_squeak_tilde *x = (t_squeak_tilde *)pd_new(squeak_tilde_class);
  
  inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
  outlet_new(&x->x_obj, &s_signal);
  outlet_new(&x->x_obj, &s_signal);

  x->phase = 0;
  if(argc)
    {
      x->numinc = (int) atom_getfloat(&argv[0]);
    }
  else
    {
      x->numinc = DEFAULT_INC;
    }

  return (void *)x;
}

void squeak_tilde_setup(void)
{
  squeak_tilde_class = class_new(gensym("squeak~"),
				(t_newmethod)squeak_tilde_new, 0,
				sizeof(t_squeak_tilde),
				CLASS_DEFAULT, A_GIMME, 0);
  class_addmethod(squeak_tilde_class,
                  (t_method)squeak_tilde_dsp, gensym("dsp"), 0);
  class_addmethod(squeak_tilde_class,
                  (t_method)squeak_tilde_pitch, gensym("uptune"), A_GIMME, 0);
  class_addmethod(squeak_tilde_class,
                  (t_method)squeak_tilde_phase, gensym("phase"), A_GIMME, 0);
  CLASS_MAINSIGNALIN(squeak_tilde_class, t_squeak_tilde, f);
}
