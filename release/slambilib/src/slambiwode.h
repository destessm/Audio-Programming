#include "m_pd.h"  

#include <string.h>
#include <math.h>
#include <m_pd.h>
#include <stdio.h>
#define DEGTORAD 0.0174532925

static char *wodeVersion = "[slambiwode~] (slightly less bad ambisonics worse decoder) v.1 David Estes-Smargiassi 2017";

static t_class *slambiwode_tilde_class;

typedef struct _slambiwode_tilde {  
  t_object  x_obj;
  t_sample f;  
  int inphase;
  int dectype;
  int numOutlets;
  int numInlets;
  float *locCoefs;
  float *optCoefs;
} t_slambiwode_tilde;  

/* *****************************************************
   PERFORM METHODS 
   *****************************************************/
t_int *slambiwode_tilde_stereo(t_int *w)  
{ 
  t_slambiwode_tilde *x = (t_slambiwode_tilde *)(w[1]);  
  t_sample *in1  = (t_sample *)(w[2]); // W
  t_sample *in2  = (t_sample *)(w[3]); // Y
  t_sample *in3  = (t_sample *)(w[4]); // X
  t_sample *out1 = (t_sample *)(w[5]);
  t_sample *out2 = (t_sample *)(w[6]);
  int n = (int)(w[7]);  

  int i,j;
  t_sample samples[3];
  t_sample outsamples[2];
  int nIns  = x->numInlets;
  int nOuts = x->numOutlets;
  
  while (n--)
    { 
      samples[0] = (*in1++) * x->optCoefs[0]; // W
      samples[1] = (*in2++) * x->optCoefs[1]; // Y
      samples[2] = (*in3++) * x->optCoefs[2]; // X
      
      for(i=0; i<nOuts; i++)
	{
	  outsamples[i] = 0;
	}
      
      for(i=0; i<nOuts; i++) // row
	{
	  for(j=0; j<nIns; j++) // col
	    {
	      outsamples[i] += (samples[j] * x->locCoefs[i*nIns + j]);
	    }
	}
      (*out1++) = outsamples[0];
      (*out2++) = outsamples[1];
      
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
  t_sample sample1, sample2, sample3;

  int i,j;
  t_sample samples[3];
  t_sample outsamples[4];
  int nIns  = x->numInlets;
  int nOuts = x->numOutlets;
  
  while (n--)
    { 
      samples[0] = (*in1++) * x->optCoefs[0]; // W
      samples[1] = (*in2++) * x->optCoefs[1]; // Y
      samples[2] = (*in3++) * x->optCoefs[2]; // X

      for(i=0; i<nOuts; i++)
        {
          outsamples[i] = 0;
        }

      for(i=0; i<nOuts; i++) // row
        {
          for(j=0; j<nIns; j++) // col
            {
              outsamples[i] += (samples[j] * x->locCoefs[i*nIns + j]);
	    }
        }
      (*out1++) = outsamples[0];
      (*out2++) = outsamples[1];
      (*out3++) = outsamples[2];
      (*out4++) = outsamples[3];
      //post("Outsample1: %.2f",outsamples[1]);
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

  int i,j;
  t_sample samples[5];
  t_sample outsamples[5];
  int nIns  = x->numInlets;
  int nOuts = x->numOutlets;

   while (n--)
    {
      samples[0] = (*in1++) * x->optCoefs[0]; // W
      samples[1] = (*in2++) * x->optCoefs[1]; // Y
      samples[2] = (*in3++) * x->optCoefs[2]; // X
      samples[3] = (*in4++) * x->optCoefs[3]; // V
      samples[4] = (*in5++) * x->optCoefs[4]; // U

      for(i=0; i<nOuts; i++)
        {
          outsamples[i] = 0;
        }

      for(i=0; i<nOuts; i++) // row
        {
          for(j=0; j<nIns; j++) // col
            {
              outsamples[i] += (samples[j] * x->locCoefs[i*nIns + j]);
            }
        }
      (*out1++) = outsamples[0];
      (*out2++) = outsamples[1];
      (*out3++) = outsamples[2];
      (*out4++) = outsamples[3];
      (*out5++) = outsamples[4];
    }

  return (w+13);
}

t_int *slambiwode_tilde_hex(t_int *w)  
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
  t_sample *out6 = (t_sample *)(w[12]);
  int n = (int)(w[13]);  

  int i,j;
  t_sample samples[5];
  t_sample outsamples[6];
  int nIns  = x->numInlets;
  int nOuts = x->numOutlets;

 
  while (n--)
    {
      samples[0] = (*in1++) * x->optCoefs[0]; // W
      samples[1] = (*in2++) * x->optCoefs[1]; // Y
      samples[2] = (*in3++) * x->optCoefs[2]; // X
      samples[3] = (*in4++) * x->optCoefs[3]; // V
      samples[4] = (*in5++) * x->optCoefs[4]; // U

      for(i=0; i<nOuts; i++)
        {
          outsamples[i] = 0;
        }

      for(i=0; i<nOuts; i++) // row
        {
          for(j=0; j<nIns; j++) // col
            {
              outsamples[i] += (samples[j] * x->locCoefs[i*nIns + j]);
            }
        }
      (*out1++) = outsamples[0];
      (*out2++) = outsamples[1];
      (*out3++) = outsamples[2];
      (*out4++) = outsamples[3];
      (*out5++) = outsamples[4];
      (*out6++) = outsamples[5];
    }

  return (w+14);
}

t_int *slambiwode_tilde_oct(t_int *w)  
{  
  t_slambiwode_tilde *x = (t_slambiwode_tilde *)(w[1]);  
  t_sample *in1  = (t_sample *)(w[2]); // W
  t_sample *in2  = (t_sample *)(w[3]); // X
  t_sample *in3  = (t_sample *)(w[4]); // Y
  t_sample *in4  = (t_sample *)(w[5]); // V
  t_sample *in5  = (t_sample *)(w[6]); // U
  t_sample *in6  = (t_sample *)(w[7]); // Q
  t_sample *in7  = (t_sample *)(w[8]); // P
  t_sample *out1 = (t_sample *)(w[9]);
  t_sample *out2 = (t_sample *)(w[10]);
  t_sample *out3 = (t_sample *)(w[11]);
  t_sample *out4 = (t_sample *)(w[12]);
  t_sample *out5 = (t_sample *)(w[13]);
  t_sample *out6 = (t_sample *)(w[14]);
  t_sample *out7 = (t_sample *)(w[15]);
  t_sample *out8 = (t_sample *)(w[16]);
  int n = (int)(w[17]);  

  int i,j;
  t_sample samples[7];
  t_sample outsamples[8];
  int nIns  = x->numInlets;
  int nOuts = x->numOutlets;

 
  while (n--)
    {
      samples[0] = (*in1++) * x->optCoefs[0]; // W
      samples[1] = (*in2++) * x->optCoefs[1]; // Y
      samples[2] = (*in3++) * x->optCoefs[2]; // X
      samples[3] = (*in4++) * x->optCoefs[3]; // V
      samples[4] = (*in5++) * x->optCoefs[4]; // U
      samples[5] = (*in6++) * x->optCoefs[5]; // Q
      samples[6] = (*in7++) * x->optCoefs[6]; // P

      for(i=0; i<nOuts; i++)
        {
          outsamples[i] = 0;
        }

      for(i=0; i<nOuts; i++) // row
        {
          for(j=0; j<nIns; j++) // col
            {
              outsamples[i] += (samples[j] * x->locCoefs[i*nIns + j]);
            }
        }
      (*out1++) = outsamples[0];
      (*out2++) = outsamples[1];
      (*out3++) = outsamples[2];
      (*out4++) = outsamples[3];
      (*out5++) = outsamples[4];
      (*out6++) = outsamples[5];
      (*out7++) = outsamples[6];
      (*out8++) = outsamples[7];
    }

  return (w+18); // end of oct
}

/* *****************************************************
   DSP METHOD 
   *****************************************************/
void slambiwode_tilde_dsp(t_slambiwode_tilde *x, t_signal **sp)  
{  
  if(x->dectype <= 1) // Stereo
    {
      dsp_add(slambiwode_tilde_stereo, 7, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[0]->s_n);
    }
  else if(x->dectype == 2) // Quad
    {
      dsp_add(slambiwode_tilde_quad, 9, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec,
	      sp[6]->s_vec, sp[0]->s_n);
    }
  else if(x->dectype == 3) // Pent
    {
      dsp_add(slambiwode_tilde_pent, 12, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec,
	      sp[6]->s_vec, sp[7]->s_vec, sp[8]->s_vec, sp[9]->s_vec,
	      sp[0]->s_n);
    }
  else if(x->dectype == 5)
    {
      dsp_add(slambiwode_tilde_oct, 17, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec,
	      sp[6]->s_vec, sp[7]->s_vec, sp[8]->s_vec, sp[9]->s_vec,
	      sp[10]->s_vec, sp[11]->s_vec, sp[12]->s_vec, sp[13]->s_vec, 
	      sp[14]->s_vec, sp[0]->s_n);
    }
  else // Hex
    {
      dsp_add(slambiwode_tilde_hex, 13, x, sp[0]->s_vec, sp[1]->s_vec,
	      sp[2]->s_vec, sp[3]->s_vec, sp[4]->s_vec, sp[5]->s_vec,
	      sp[6]->s_vec, sp[7]->s_vec, sp[8]->s_vec, sp[9]->s_vec,
	      sp[10]->s_vec, sp[0]->s_n);
    }
}

/* *****************************************************
   CONSTRUCTOR AND DESTRUCTOR METHODS 
   *****************************************************/
void slambiwode_tilde_free(t_slambiwode_tilde *x)
{
  free(x->locCoefs);
  free(x->optCoefs);
}

void *slambiwode_tilde_new(t_symbol *s, int argc, t_atom *argv)
{
  t_slambiwode_tilde *x = (t_slambiwode_tilde *)pd_new(slambiwode_tilde_class);  
  
  char modename[64];
  if(argc > 0)
    {
      atom_string(&argv[0], modename, 64);
    }
  x->inphase = 1;
  if(argc > 1)
    {
      char phname[64];
      atom_string(&argv[1], phname, 64);
      if(phname[0] == 'n')
	{
	  x->inphase = 0;
	}
    }
 
  int mode = 1;
  int numInlets = 0;
  int numOutlets= 0;
  if( strcmp(modename, "oct") == 0)
    {
      mode = 5;
      numInlets = 7;
      numOutlets = 8;
    }
  else if( strcmp(modename,"hex") == 0)
    {
      mode = 4;
      numInlets = 5;
      numOutlets = 6;
    }
  else if( strcmp(modename,"pent") == 0 )
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
      if( strcmp(modename, "stereo") != 0 )
	{
	  post("Invalid or no speaker configuration given. Using Stereo instead.");
	}
      mode = 1;
      numInlets = 3;
      numOutlets = 2;
    }
  x->dectype = mode;
  x->numInlets = numInlets;
  x->numOutlets = numOutlets;
  
  // generate inlets and outlets
  int i;
  for(i=0; i<numInlets-1; i++)
    {
      inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_signal, &s_signal);
    }
  for(i=0; i<numOutlets; i++)
    {
      outlet_new(&x->x_obj, &s_signal);
    }
  
  // generate location coeficients
  int width = 1 + mode*2;
  x->locCoefs = (float *) malloc(numOutlets*width*sizeof(float));    
  float uni = 1.0/((float)mode+1.0);
  int j;
  int curAngle;
  float *angles = (float *)malloc(numOutlets*sizeof(float));
  // generate angles, starting front left and going counter-clockwise
  switch(mode)
    {
    case 5:
      angles[0] = 337.5;
      angles[1] = 292.5;
      angles[2] = 245.5;
      angles[3] = 202.5;
      angles[4] = 157.5;
      angles[5] = 112.5;
      angles[6] = 67.5;
      angles[7] = 22.5;
    case 4:
      angles[0] = 330;
      angles[1] = 270;
      angles[2] = 210;
      angles[3] = 150;
      angles[4] = 90;
      angles[5] = 30;
    case 3: 
      angles[0] = 324;
      angles[1] = 252;
      angles[2] = 180;
      angles[3] = 108;
      angles[4] = 36;
      break;
    case 2:
      angles[0] = 315;
      angles[1] = 225;
      angles[2] = 135;
      angles[3] = 45;
    case 1:
      angles[0] = 270;
      angles[1] = 90;
    defaut:
      angles[0] = 270;
      angles[1] = 90;
    } 
  for(i=0; i < numOutlets; i++)
    {
      curAngle = angles[i] * DEGTORAD;
      x->locCoefs[i*width] = uni * 0.707; 
      for(j=1; j < width; j++)
	{
	  if( j%2 ==0 )
	    { // sin
	      x->locCoefs[i*width + j] = uni * sinf( ((float)j/2.0) * curAngle);
	    }
	  else
	    { // cos
	      x->locCoefs[i*width + j] = uni * cosf( ((float)j/2.0 + 1) * curAngle);
	    }
	}
    }
  
  // generate optimization
  x->optCoefs = (float *) malloc(numInlets*sizeof(float));
  x->optCoefs[0] = 1;
  int l = 1;
  if(x->inphase)
    {
      for(i=1; i < numInlets; i+=2)
	{
	  float t = pow(fact(mode), 2);
	  float o = t / (fact(mode+l) * fact(mode-l));
	  x->optCoefs[i] = o;
	  x->optCoefs[i+1] = o;
	  l++;
	}
    }
  else
    { // coefs are all 1 if optimization is off
      for(i=1; i < numInlets; i++)
	{
	  x->optCoefs[i] = 1;
	}
    }  

  return (void *)x;  
}  


/* *****************************************************
   SETUP METHOD 
   *****************************************************/
void slambiwode_tilde_setup(void) {  
  slambiwode_tilde_class = class_new(gensym("slambiwode~"),  
				     (t_newmethod)slambiwode_tilde_new,  
				     (t_newmethod) slambiwode_tilde_free, 
				     sizeof(t_slambiwode_tilde),  
				     CLASS_DEFAULT,  
				     A_GIMME,
				     0);  
			       
  class_addmethod(slambiwode_tilde_class,(t_method)slambiwode_tilde_dsp, gensym("dsp"), 0);
  CLASS_MAINSIGNALIN(slambiwode_tilde_class, t_slambiwode_tilde, f);
  class_sethelpsymbol(slambiwode_tilde_class, gensym("slambilib-help.pd"));
  post(wodeVersion);
}
