#include "m_pd.h"  

#define DEGTORAD 0.01745329;

static t_class *bambien_tilde_class;  

typedef struct _bambien_tilde {  
  t_object  x_obj;
  t_sample f;

  t_inlet  *x_in2;
  t_inlet  *x_in3;
  t_outlet *x_out1;
  t_outlet *x_out2;
  t_outlet *x_out3;
  t_outlet *x_out4;
} t_bambien_tilde;  

t_int *bambien_tilde_perform(t_int *w)  
{  
  t_bambien_tilde *x = (t_bambien_tilde *)(w[1]);  
  t_sample *in1  = (t_sample *)(w[2]); // audio
  t_sample *in2  = (t_sample *)(w[3]); // azimuth
  t_sample *in3  = (t_sample *)(w[4]); // elevation
  t_sample *out1 = (t_sample *)(w[5]);
  t_sample *out2 = (t_sample *)(w[6]);
  t_sample *out3 = (t_sample *)(w[7]);
  t_sample *out4 = (t_sample *)(w[8]);
  int n = (int)(w[9]);  
  t_sample sample1, sample2, sample3;

  float kPl = 2; /* ?? */
  
  while (n--)
    {
      sample1 = (*in1++); // audio in 1
      sample2 = (*in2++) * DEGTORAD; // audio in 2
      sample3 = (*in3++ - 0.5)*kPl; // audio in 3
      
      (*out1++) = sample1 * 0.707; // W
      (*out2++) = sample1 * cosf(sample2); // X
      (*out3++) = sample1 * sinf(sample2); // Y
      (*out4++) = sample1 * sinf(sample3); // Z
    }

  return (w+10);  
}  

void bambien_tilde_dsp(t_bambien_tilde *x, t_signal **sp)  
{  
  dsp_add(bambien_tilde_perform, 9, x, sp[0]->s_vec, sp[1]->s_vec, 
	  sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, 
	  sp[6]->s_vec, sp[0]->s_n);  
}  

void bambien_tilde_free(t_bambien_tilde *x)  
{  
  inlet_free(x->x_in2);  
  inlet_free(x->x_in3);  
  outlet_free(x->x_out1); 
  outlet_free(x->x_out2);
  outlet_free(x->x_out3);
  outlet_free(x->x_out4);
}  

void *bambien_tilde_new(void)  
{  
  t_bambien_tilde *x = (t_bambien_tilde *)pd_new(bambien_tilde_class);  
  
  x->x_in2=inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
  x->x_in3=inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);

  x->x_out1=outlet_new(&x->x_obj, &s_signal);  
  x->x_out2=outlet_new(&x->x_obj, &s_signal);  
  x->x_out3=outlet_new(&x->x_obj, &s_signal);  
  x->x_out4=outlet_new(&x->x_obj, &s_signal);  
  
  return (void *)x;  
}  

void bambien_tilde_setup(void) {  
  bambien_tilde_class = class_new(gensym("bambien~"),  
			       (t_newmethod)bambien_tilde_new,  
			       (t_newmethod)bambien_tilde_free, 
			       sizeof(t_bambien_tilde),  
			       CLASS_DEFAULT,  
			       0);  
			       
  class_addmethod(bambien_tilde_class,(t_method)bambien_tilde_dsp, gensym("dsp"), 0);
  CLASS_MAINSIGNALIN(bambien_tilde_class, t_bambien_tilde, f);  
}
