/* BangGen external
   by David Estes-Smargiassi
   February 2, 2017
   
   Bang Generator. Takes in three floats of unknown range and outputs bangs if
   certain parameters are met (not supposed to be known). Essentially acts like
   a beat generator for random inputs. Note that it does its operations on four
   integers, not three; it uses the three previously stored integers AND the new
   integer from inlet. This provides some variation so that the same input
   values will not necessarily result in the same set of outputted bangs (unless
   previously stored numbers were also the same).
 */

#include "m_pd.h"

static t_class *banggen_class;

char *version = "BangGen object v1.0, by David Estes-Smargiassi\n";

// Struct: contains x_obj, array of outlets, and three currently-held integers
typedef struct t_banggen
{
  t_object x_obj;
  t_outlet *outs[4];
  int a,b,c;
} t_banggen;

// Constructor
// Initializes inlets and outlets
void *banggen_new(void)
{
  poststring(version);
  t_banggen *x = (t_banggen *)pd_new(banggen_class);

  // Initialize inlets
  inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("in_b"));
  inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("in_c"));

  // Initialize outlets
  for(int i=0; i<4; i++)
    {
      x->outs[i] = outlet_new(&x->x_obj, &s_bang);
    }

  return (void *)x;
}

// General function; takes in 4 integers and outputs bangs to various outlets
void triggerbangs(t_banggen *x, int a, int b, int c, int d)
{
  // Outlet 0:
  if( (d|a) > (c|a) && d > a )
    {
      outlet_bang(x->outs[0]);
    }
  // Outlet 1:
  if( (c>>b > d>>a) || (c <= ~a) )
    {
      outlet_bang(x->outs[1]);
    }
  // Outlet 2:
  if( ((~c)+(~a) < (b^c)-(a|d)) )
    {
      outlet_bang(x->outs[2]);
    }
  // Outlet 3:
  if( (a&b)<<a > (c&b)<<d )
    {
      outlet_bang(x->outs[3]);
    } 
}

/* Functions for Float Inputs (from each inlet) */
void banggen_float_a(t_banggen *x, t_floatarg f)
{
  int d = (int)f;  
  triggerbangs(x, x->a, x->b, x->c, d);
  x->a = d;
}

void banggen_float_b(t_banggen *x, t_floatarg f)
{
  int d = (int)f;
  triggerbangs(x, x->a, x->b, x->c, d);
  x->b = d;
}

void banggen_float_c(t_banggen *x, t_floatarg f)
{
  int d = (int)f;
  triggerbangs(x, x->a, x->b, x->c, d);
  x->c = d;
}

// Setup
void banggen_setup(void)
{
  banggen_class = class_new(gensym("banggen"),
			    (t_newmethod)banggen_new, 0,
			    sizeof(t_banggen),
			    CLASS_DEFAULT, 0);

  // Inlet Methods

  // first inlet (a)
  class_addfloat(banggen_class, banggen_float_a);
  
  // second inlet (b)
  class_addmethod(banggen_class, (t_method)banggen_float_b, gensym("in_b"), A_FLOAT, 0);
  
  // third inlet (c)
  class_addmethod(banggen_class, (t_method)banggen_float_c, gensym("in_c"), A_FLOAT, 0);
}


