#include <string.h>
#include <math.h>
#include <m_pd.h>
#include <stdio.h>

#define DEGTORAD 0.01745329;

static char *enVersion = "[slambien~] v.1 David Estes-Smargiassi 2017";

static t_class *slambien_tilde_class;  

typedef struct _slambien_tilde {  
  t_object  x_obj;
  t_sample f;

  int order;

  t_inlet  *x_in2;
  t_outlet *x_out1;
  t_outlet *x_out2;
  t_outlet *x_out3;
  t_outlet *x_out4;
  t_outlet *x_out5;
  t_outlet *x_out6;
  t_outlet *x_out7;
} t_slambien_tilde;  

/* First order perform method */
t_int *slambien_tilde_order1(t_int *w)
{
  t_slambien_tilde *x = (t_slambien_tilde *)(w[1]);  
  t_sample *in1  = (t_sample *)(w[2]); // audio
  t_sample *in2  = (t_sample *)(w[3]); // azimuth
  t_sample *out1 = (t_sample *)(w[4]);
  t_sample *out2 = (t_sample *)(w[5]);
  t_sample *out3 = (t_sample *)(w[6]);
  int n = (int)(w[7]);  
  t_sample sample1, sample2;
  
  while (n--)
    {
      sample1 = (*in1++);
      sample2 = (*in2++)*DEGTORAD;

      (*out1++) = sample1 * 0.707; // W      
      (*out2++) = sample1 * cosf(sample2); // Y
      (*out3++) = sample1 * sinf(sample2); // X
    }

  return (w+8);
}

/* Second Order perform method */
t_int *slambien_tilde_order2(t_int *w)
{
t_slambien_tilde *x = (t_slambien_tilde *)(w[1]);  
  t_sample *in1  = (t_sample *)(w[2]); // audio
  t_sample *in2  = (t_sample *)(w[3]); // azimuth
  t_sample *out1 = (t_sample *)(w[4]);
  t_sample *out2 = (t_sample *)(w[5]);
  t_sample *out3 = (t_sample *)(w[6]);
  t_sample *out4 = (t_sample *)(w[7]);
  t_sample *out5 = (t_sample *)(w[8]);
  int n = (int)(w[9]);  
  t_sample sample1, sample2;
  
  while (n--)
    {
      sample1 = (*in1++);
      sample2 = (*in2++)*DEGTORAD;

      (*out1++) = sample1 * 0.707; // W      
      (*out2++) = sample1 * cosf(sample2); // Y
      (*out3++) = sample1 * sinf(sample2); // X
      (*out4++) = sample1 * cosf(2 * sample2); // V
      (*out5++) = sample1 * sinf(2 * sample2); // U
    }

  return (w+10);
}

/* Third Order perform method */
t_int *slambien_tilde_order3(t_int *w)  
{  
  t_slambien_tilde *x = (t_slambien_tilde *)(w[1]);  
  t_sample *in1  = (t_sample *)(w[2]); // audio
  t_sample *in2  = (t_sample *)(w[3]); // azimuth
  t_sample *out1 = (t_sample *)(w[4]);
  t_sample *out2 = (t_sample *)(w[5]);
  t_sample *out3 = (t_sample *)(w[6]);
  t_sample *out4 = (t_sample *)(w[7]);
  t_sample *out5 = (t_sample *)(w[8]);
  t_sample *out6 = (t_sample *)(w[9]);
  t_sample *out7 = (t_sample *)(w[10]);
  int n = (int)(w[11]);  
  t_sample sample1, sample2;
  
  while (n--)
    {
      sample1 = (*in1++);
      sample2 = (*in2++)*DEGTORAD;

      (*out1++) = sample1 * 0.707; // W      
      (*out2++) = sample1 * cosf(sample2); // Y
      (*out3++) = sample1 * sinf(sample2); // X
      (*out4++) = sample1 * cosf(2 * sample2); // V
      (*out5++) = sample1 * sinf(2 * sample2); // U
      (*out6++) = sample1 * cosf(3 * sample2); // Q
      (*out7++) = sample1 * sinf(3 * sample2); // P
    }

  return (w+12);  
}  

/* DSP Method */
void slambien_tilde_dsp(t_slambien_tilde *x, t_signal **sp)  
{  
  if(x->order <= 1)
    {
      dsp_add(slambien_tilde_order1, 7, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[0]->s_n);
    }
  else if(x->order == 2)
    {
      dsp_add(slambien_tilde_order2, 9, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, 
	      sp[6]->s_vec, sp[0]->s_n);
    }
  else if(x->order >= 3)
    {
      dsp_add(slambien_tilde_order3, 11, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, 
	      sp[6]->s_vec, sp[7]->s_vec, sp[8]->s_vec, sp[0]->s_n);
    }
}  

/* Free Method */
void slambien_tilde_free(t_slambien_tilde *x)  
{  
  inlet_free(x->x_in2);  
  outlet_free(x->x_out1); 
  outlet_free(x->x_out2);
  outlet_free(x->x_out3);
  if( x->order >= 2 ) // test if has more outlets
    {
      outlet_free(x->x_out4);
      outlet_free(x->x_out5);
    }
  if( x->order == 3 ) // test if has even more outlets
    {
      outlet_free(x->x_out6);
      outlet_free(x->x_out7);
    }
}  

/* Constructor Method */
void *slambien_tilde_new(t_floatarg o)  
{  
  t_slambien_tilde *x = (t_slambien_tilde *)pd_new(slambien_tilde_class);  
  
  x->order = (o <= 1) ? 1 : (o == 2) ? 2 : 3;

  x->x_in2=inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);

  x->x_out1=outlet_new(&x->x_obj, &s_signal);  
  x->x_out2=outlet_new(&x->x_obj, &s_signal);  
  x->x_out3=outlet_new(&x->x_obj, &s_signal);  
  if( x->order >= 2 )
    {
      x->x_out4=outlet_new(&x->x_obj, &s_signal);  
      x->x_out5=outlet_new(&x->x_obj, &s_signal);  
    }
  if( x->order == 3 )
    {
      x->x_out6=outlet_new(&x->x_obj, &s_signal);  
      x->x_out7=outlet_new(&x->x_obj, &s_signal);  
    }

  return (void *)x;  
}  

/* External Setup Method */
void slambien_tilde_setup(void) {  
  slambien_tilde_class = class_new(gensym("slambien~"),  
				   (t_newmethod)slambien_tilde_new,  
				   (t_newmethod)slambien_tilde_free, 
				   sizeof(t_slambien_tilde),  
				   CLASS_DEFAULT,  
				   A_DEFFLOAT,
				   0);  
			       
  class_addmethod(slambien_tilde_class,(t_method)slambien_tilde_dsp, gensym("dsp"), 0);
  CLASS_MAINSIGNALIN(slambien_tilde_class, t_slambien_tilde, f);  
  class_sethelpsymbol(slambien_tilde_class, gensym("slambilib-help.pd"));
  post(enVersion);
}
