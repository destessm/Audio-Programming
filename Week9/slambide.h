#include "m_pd.h"  

#include <string.h>
#include <math.h>
#include <m_pd.h>
#include <stdio.h>

static char *deVersion = "[slambide~] v.1 David Estes-Smargiassi 2017";

static t_class *slambide_tilde_class;

typedef struct _slambide_tilde {  
  t_object  x_obj;
  t_sample f;
  int inphase; // 0 = no, 1 = yes
  int dectype;
  int numOutlets;
  float *locCoefs;
} t_slambide_tilde;  

/*
t_int *slambide_tilde_stereo(t_int *w)  
{  
  t_slambide_tilde *x = (t_slambide_tilde *)(w[1]);  
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

t_int *slambide_tilde_quad(t_int *w)  
{  
  t_slambide_tilde *x = (t_slambide_tilde *)(w[1]);  
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

t_int *slambide_tilde_pent(t_int *w)  
{  
  t_slambide_tilde *x = (t_slambide_tilde *)(w[1]);  
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
	+ (sample4 * 0.085) + (sample5 * 0.125);

      (*out2++) = (sample1 * 0.405) + (sample2 * 0.320)  + (sample3 * -0.310)
	+ (sample4 * 0.085) + (sample5 * -0.125);

      (*out3++) = (sample1 * 0.085) + (sample2 * 0.040)  + (sample3 * 0.000)
	+ (sample4 * 0.045) + (sample5 * 0.000);

      (*out4++) = (sample1 * 0.635) + (sample2 * -0.335)  + (sample3 * 0.380)
	+ (sample4 * 0.080) + (sample5 * 0.080);

      (*out5++) = (sample1 * 0.635) + (sample2 * -0.335)  + (sample3 * -0.280)
	+ (sample4 * -0.080) + (sample5 * -0.080);
    }

  return (w+13);
}
*/
t_int slambide_tilde_perform(t_int *w)
{
  /* generalized perform method */
  t_slambide_tilde *x = (t_slambide_tilde *)(w[1]);
  int numIns = 1 + x->dectype * 2;
  int numOuts = x->numOutlets;
  t_sample **ins = (t_sample **)malloc(numIns * sizeof(t_sample));
  t_sample **outs = (t_sample **)malloc(numOuts * sizeof(t_sample));
  int i;
  int j;
  for(i=0; i<numIns; i++)
    {
      ins[i] = (t_sample *)(w[i+2]);
    }
  for(i=0; i<numOuts; i++)
    {
      outs[i] = (t_sample *)(w[numIns+i+2]);
    }
  int n = (int) w[2+numIns+numOuts];
  t_sample *samples = (t_sample *)malloc(numIns*sizeof(t_sample));
  
  /* matrix stuff with x->locCoefs*/
  while (n--)
    {
      for(i=0; i<numIns; i++)
	{
	  samples[i] = (*ins[i]++);
	}
      
      for(i=0; i<numOuts; i++)
	{
	  t_sample temp = 0;
	  for(j=0; j<numIns; i++)
	    {
	      temp += samples[j] * x->locCoefs[j*numIns + i];
	    }
	  (*outs[i]++) = temp;
	}
    }
  return (w+3+numIns+numOuts);
}


void slambide_tilde_dsp_attempt(t_slambide_tilde *x, t_signal **sp)
{
  int dsp_ct = 2 + (1 + x->dectype * 2) + x->numOutlets;
  t_int *lst = (t_signal **)malloc((dsp_ct-2)*sizeof(t_signal));
  int i;
  lst[0] = (t_int)x;
  for(i=1; i<dsp_ct-2; i++)
    {
      lst[i] = (t_int)sp[i]->s_vec;
    }
  lst[dsp_ct-1] = (t_int) sp[0]->s_n;
  dsp_addv(slambide_tilde_perform, dsp_ct, lst);
}

void slambide_tilde_dsp(t_slambide_tilde *x, t_signal **sp)
{
  int dsp_ct = 2 + (1 + x->dectype * 2) + x->numOutlets;
  switch(dsp_ct)
    {
    case 7:
      dsp_add(slambide_tilde_perform, 7, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[0]->s_n);
    case 8: 
      dsp_add(slambide_tilde_perform, 8, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[0]->s_n);
      break;
    case 9:
      dsp_add(slambide_tilde_perform, 9, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec,
	      sp[0]->s_n);
      break;
    case 10:
      dsp_add(slambide_tilde_perform, 10, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec,
	      sp[7]->s_vec, sp[0]->s_n);
      break;
    case 11:
      dsp_add(slambide_tilde_perform, 11, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec,
	      sp[7]->s_vec, sp[8]->s_vec, sp[0]->s_n);
      break;
    case 12:
      dsp_add(slambide_tilde_perform, 12, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec,
	      sp[7]->s_vec, sp[8]->s_vec, sp[9]->s_vec, sp[0]->s_n);
      break;
    case 13:
      dsp_add(slambide_tilde_perform, 13, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec,
	      sp[7]->s_vec, sp[8]->s_vec, sp[9]->s_vec, sp[10]->s_vec, sp[0]->s_n);
      break;
    case 14:
      dsp_add(slambide_tilde_perform, 14, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec,
	      sp[7]->s_vec, sp[8]->s_vec, sp[9]->s_vec, sp[10]->s_vec, sp[11]->s_vec,
	      sp[0]->s_n);
      break;
    case 15:
      dsp_add(slambide_tilde_perform, 15, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec,
	      sp[7]->s_vec, sp[8]->s_vec, sp[9]->s_vec, sp[10]->s_vec, sp[11]->s_vec,
	      sp[12]->s_vec, sp[0]->s_n);
      break;
    case 16:
      dsp_add(slambide_tilde_perform, 16, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec,
	      sp[7]->s_vec, sp[8]->s_vec, sp[9]->s_vec, sp[10]->s_vec, sp[11]->s_vec,
	      sp[12]->s_vec, sp[13]->s_vec, sp[0]->s_n);
      break;
    case 17:
      dsp_add(slambide_tilde_perform, 17, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec, sp[6]->s_vec,
	      sp[7]->s_vec, sp[8]->s_vec, sp[9]->s_vec, sp[10]->s_vec, sp[11]->s_vec,
	      sp[12]->s_vec, sp[13]->s_vec, sp[14]->s_vec, sp[0]->s_n);
      break;
    default: // case 6
      dsp_add(slambide_tilde_perform, 6, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[0]->s_n);
    }
}

void slambide_tilde_free(t_slambide_tilde *x)
{
  free(x->locCoefs);
}

void *slambide_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
  t_slambide_tilde *x = (t_slambide_tilde *)pd_new(slambide_tilde_class);  
  
  // Usage:
  // [slambide~ <y/n in-phase><order> <list of angles for outputs>]
  
  if(argc <= 2)
    {
      error("Error! Incorrect usage! Please follow the following guidelines:");
      error("[slambide~ <y/n in-phase> <order number> <list of angles for outputs>]");
      x->dectype = 0;
      x->locCoefs = (float *) malloc(1*sizeof(float)); 
      // just so that free() doesn't complain
    }
  else{
    x->inphase = 1;
    char *buff = (char *)malloc(64*sizeof(char));
    atom_string(&argv[0], buff, 64);
    if( buff[0] == 'n' )
      {
	x->inphase = 0;
      }

    int tmp = atom_getfloat(&argv[1]); // not sure if 0 or 1
    int mode = (tmp <= 1) ? 1 : (tmp == 2) ? 2 : 3;
    x->dectype = mode;
    int numInlets = (mode == 1) ? 3 : (mode == 2) ? 5 : 7;
    int i;
    for(i=0; i<numInlets-1; i++)
      {
	inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
      }
    
    int numOutlets = argc-1;
    if( numOutlets > 8 )
      {
	error("Error! Only up to 8 outlets are supported by this version of [slambide~]!");
	numOutlets = 8;
      }
    else if (numOutlets < 1)
      {
	error("Error! Only between 1 and 8 outlets are supported by this version of [slambide~]!");
	// this will crash
      }
    int width = 1 + mode*2;
    x->locCoefs = (float *) malloc(numOutlets*width*sizeof(float));
    
    // do math here for coeficients
    float uni = 1/(mode+1);
    int j;
    int curAngle;
    for(i=0; i < numOutlets; i++)
      {
	curAngle = atom_getfloat(&argv[i+2]);
	x->locCoefs[i*width] = uni * 0.707; 
	for(j=1; j < width; j++)
	  {
	    if( j%2 ==0 )
	      { // sin
		x->locCoefs[i*width + j] = uni * sinf( (j/2) * curAngle);
	      }
	    else
	      { // cos
		x->locCoefs[i*width + j] = uni * cosf( (j/2 + 1) * curAngle);
	      }
	  }
      }
    
    for(i=1; i<numOutlets; i++)
      {
	outlet_new(&x->x_obj, &s_signal);
      }
  
  }
  return (void *)x;  
}  

void slambide_tilde_setup(void) {  
  slambide_tilde_class = class_new(gensym("slambide~"),  
				   (t_newmethod)slambide_tilde_new,  
				   0, 
				   sizeof(t_slambide_tilde),  
				   CLASS_DEFAULT,  
				   A_GIMME,
				   0);  
			       
  class_addmethod(slambide_tilde_class,(t_method)slambide_tilde_dsp, gensym("dsp"), 0);
  CLASS_MAINSIGNALIN(slambide_tilde_class, t_slambide_tilde, f);
  class_sethelpsymbol(slambide_tilde_class, gensym("slambilib-help.pd"));
  post(deVersion);
}
