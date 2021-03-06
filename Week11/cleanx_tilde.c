/*[cleanx~] 
  David Estes-Smargiassi
  modeled after Eric Lyon's [cleaner~]
 */
#import "m_pd.h"
#include "stdlib.h" 
#include "math.h" 

static t_class *cleanx_tilde_class;

static char *version = "[cleanx~] v1.0 David Estes-Smargiassi 2017";

typedef struct _cleanx_tilde {
  t_object x_obj;
  t_sample f;
  float *mag;
  float *phase;
  long vecsize;
} t_cleanx_tilde;


t_int *cleanx_tilde_perform(t_int *w)
{
  t_cleanx_tilde *x = (t_cleanx_tilde *)(w[1]);
  t_sample *real_input = (t_sample *)(w[2]);
  t_sample *imag_input = (t_sample *)(w[3]);
  t_sample *threshmult = (t_sample *)(w[4]);
  t_sample *multiplier = (t_sample *)(w[5]);
  t_sample *real_output = (t_sample *)(w[6]);
  t_sample *imag_output = (t_sample *)(w[7]);
  int n = (w[8]) / 2;

  float *phase = x->phase;
  float *mag = x->mag;
  t_sample maxamp = 0.0;
  float threshold;
  float mult;
  int i;
  float a, b; // variables for Polar/Cartesian conversions

  for (i = 0; i <= n; i++) 
    {
      a = (i == n ? real_input[1] : real_input[i]);
      b = (i == 0 || i == n ? 0. : imag_input[i]);
      mag[i] = hypot(a, b);
      phase[i] = -atan2(b, a);
    }
  
  for (i=0; i<n; i++)
    {
      if(maxamp < mag[i])
	{
	  maxamp = mag[i];
	}
    }
  threshold = *threshmult * maxamp;
  mult = *multiplier;
  for (i=0; i<n; i++)
    {
      if(mag[i] < threshold)
	{
	  mag[i] *= mult;
	}
    }

  /* Convert from polar to a complex spectrum */
  
  for ( i = 0; i <= n; i++ ) 
    {
      real_output[i] = mag[i] * cos(phase[i]);
      if ( i != n )
	{
	  imag_output[i] = -mag[i] * sin(phase[i]);
	} 
      else 
	{
	  imag_output[i] = 0.0;
	}
  }

  return w + 9;
}

void cleanx_tilde_dsp(t_cleanx_tilde *x, t_signal **sp)
{
  long bytes; // bytesize for magnitude and phase vectors

  if(x->vecsize != sp[0]->s_n)
    {
      x->vecsize = sp[0]->s_n;
      bytes = x->vecsize * sizeof(float);
      if(x->mag == NULL)
	{
	  x->phase = (float *) malloc(bytes);
	  x->mag = (float *) malloc(bytes);
	}
    else 
      {
	free(x->phase);
	free(x->mag);
	x->phase = (float *) malloc(bytes);
	x->mag = (float *) malloc(bytes);
      }

    /* Zero out the phase and magnitude vectors */

    memset(x->phase, 0, bytes);
    memset(x->mag, 0, bytes);
  }
  
  dsp_add(cleanx_tilde_perform, 8, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[3]->s_vec, 
	  sp[4]->s_vec, sp[5]->s_vec, sp[0]->s_n);
}

void *cleanx_tilde_new(void)
{
  t_cleanx_tilde *x = (t_cleanx_tilde *)pd_new(cleanx_tilde_class);
  
  inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
  inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
  inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
  outlet_new(&x->x_obj, &s_signal);
  outlet_new(&x->x_obj, &s_signal);
  
  x->vecsize = 0;
  x->mag = NULL;
  
  return (void *)x;
}

void cleanx_tilde_free(t_cleanx_tilde *x)
{
  if(x->mag != NULL)
    {
      free(x->mag);
      free(x->phase);
    }
}

void cleanx_tilde_setup(void)
{
  cleanx_tilde_class = class_new(gensym("cleanx~"),
				 (t_newmethod)cleanx_tilde_new, 
				 (t_newmethod)cleanx_tilde_free,
				 sizeof(t_cleanx_tilde),
				 CLASS_DEFAULT,
				 0);
  class_addmethod(cleanx_tilde_class, (t_method)cleanx_tilde_dsp,
		  gensym("dsp"), 0);
  CLASS_MAINSIGNALIN(cleanx_tilde_class, t_cleanx_tilde, f);
  post(version);
}
