/* [pan3~]
   Variable 'panning' object
   David Estes-Smargiassi
   February 9, 2017
   Adapted from code by IOhannes m zmolnig
 */

#include "m_pd.h"
#include <math.h>

char *version = "Pan3 object v1.0, by David Estes-Smargiassi\n";

static t_class *pan3_tilde_class;

typedef struct _pan3_tilde {
  t_object x_obj;
  
  t_sample f_pan;
  t_sample f;
  
  t_inlet *x_in2;
  t_outlet *x_out1;
  t_outlet *x_out2;

  short mode; // 0 = linear, 1 = equal power, 2 = cosine
} t_pan3_tilde;

t_sample clipSample(t_sample f)
{
  t_sample f_new = f;
  if(f < 0)
    {
      f_new = 0.0;
    }
  else if(f > 1)
    {
      f_new = 1.0;
    }
  return f_new;
}

t_int *pan3_tilde_perform(t_int *w)
{
  t_pan3_tilde *x = (t_pan3_tilde *)(w[1]);
  t_sample *in1 = (t_sample *)(w[2]);
  t_sample *out1 = (t_sample *)(w[3]);
  t_sample *out2 = (t_sample *)(w[4]);
  int n = (int)(w[5]);
  
  t_sample pan = clipSample(x->f_pan);
  float twopi = 6.2832;
  while (n--)
    {
      t_sample in = *in1++;
      if(x->mode == 0)
	{
	  // perform linear pan
	  *out1++ = in*(1-pan);
	  *out2++ = in*pan;
	}
      else if(x->mode == 1)
	{
	  // perform equal power pan
	  *out1++ = in*sqrt(1-pan);
	  *out2++ = in*sqrt(pan);
	}
      else if(x->mode == 2)
	{
	  // perform cosine pan
	  *out1++ = in*cos(twopi*(pan*0.25-0.5));
	  *out2++ = in*cos(twopi*(pan*0.25-0.25));
	}
    }
  return (w+6);
} 

void pan3_tilde_dsp(t_pan3_tilde *x, t_signal **sp)
{
  dsp_add(pan3_tilde_perform, 5, x, sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);
}

void pan3_tilde_free(t_pan3_tilde *x)
{
  inlet_free(x->x_in2);
  outlet_free(x->x_out1);
  outlet_free(x->x_out2);
}

void *pan3_tilde_new(t_atom *s, int argc, t_atom *argv)
{
  poststring(version);
  t_pan3_tilde *x = (t_pan3_tilde *)pd_new(pan3_tilde_class);

  x->x_in2 = floatinlet_new(&x->x_obj, &x->f_pan);
  x->x_out1 = outlet_new(&x->x_obj, &s_signal);
  x->x_out2 = outlet_new(&x->x_obj, &s_signal);
  
  char cmode = 'L';

  if(argc==0)
    {
      x->f_pan = 0.5;
    }
  else if(argc==1)
    {
      x->f_pan = 0.5;

      t_atom *catom = &argv[0];
      char *buff = (char *)malloc(sizeof(char)*16);
      atom_string(catom, buff, sizeof(char)*16);

      cmode = buff[0];

    }
  else if(argc==2)
    {
      t_atom *fatom = &argv[0];
      t_atom *catom = &argv[1];

      x->f_pan = atom_getint(fatom);
      char *buff = (char *)malloc(sizeof(char)*16);
      atom_string(catom, buff, sizeof(char)*16);

      cmode = buff[0];
    }

  x->mode = 0;
  if(cmode == 'e' || cmode == 'E')
    {
      x->mode = 1; // equal power panning
      post("Started using Equal Power Panning");
    }
  else if(cmode == 'c' || cmode == 'C')
    {
      x->mode = 2; // cosine panning
      post("Started using Cosine Panning");
    }
  else
    {  // else, linear panning (default)
      post("Started using Linear Panning");
    }


  return (void *)x;
}

void pan3_tilde_setup(void)
{
  pan3_tilde_class = class_new(gensym("pan3~"),
			      (t_newmethod)pan3_tilde_new,
			      (t_newmethod)pan3_tilde_free,
			      sizeof(t_pan3_tilde),
			      CLASS_DEFAULT,
			      A_GIMME, 0);

  class_addmethod(pan3_tilde_class,
		  (t_method)pan3_tilde_dsp, gensym("dsp"), 0);
  CLASS_MAINSIGNALIN(pan3_tilde_class, t_pan3_tilde, f);
}
