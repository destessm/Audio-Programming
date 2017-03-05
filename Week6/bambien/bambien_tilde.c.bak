#include "m_pd.h"  

static t_class *ring_tilde_class;  

typedef struct _ring_tilde {  
  t_object  x_obj;
  t_sample f;

  t_inlet *x_in2;
  t_outlet*x_out;  
} t_ring_tilde;  

t_int *ring_tilde_perform(t_int *w)  
{  
  t_ring_tilde *x = (t_ring_tilde *)(w[1]);  
  t_sample   *in1 =     (t_sample *)(w[2]);  
  t_sample   *in2 =     (t_sample *)(w[3]);
  t_sample   *out =     (t_sample *)(w[4]);  
  int           n =            (int)(w[5]);  
  
  while (n--)
    {
      *out++ = (*in1++)*(*in2++);
    }

  return (w+6);  
}  

void ring_tilde_dsp(t_ring_tilde *x, t_signal **sp)  
{  
  dsp_add(ring_tilde_perform, 5, x,  
          sp[0]->s_vec, sp[1]->s_vec, sp[2]->s_vec, sp[0]->s_n);  
}  

void ring_tilde_free(t_ring_tilde *x)  
{  
  inlet_free(x->x_in2);  
  outlet_free(x->x_out);  
}  

void *ring_tilde_new(void)  
{  
  t_ring_tilde *x = (t_ring_tilde *)pd_new(ring_tilde_class);  
  
  x->x_in2=inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
  x->x_out=outlet_new(&x->x_obj, &s_signal);  
  
  return (void *)x;  
}  

void ring_tilde_setup(void) {  
  ring_tilde_class = class_new(gensym("ring~"),  
			       (t_newmethod)ring_tilde_new,  
			       (t_newmethod)ring_tilde_free, 
			       sizeof(t_ring_tilde),  
			       CLASS_DEFAULT,  
			       0);  
			       
  class_addmethod(ring_tilde_class,(t_method)ring_tilde_dsp, gensym("dsp"), 0);
  CLASS_MAINSIGNALIN(ring_tilde_class, t_ring_tilde, f);  
}
