/****************************************************
 *   This code is explicated in Chapter 7 of        *
 *   "Designing Audio Objects for Max/MSP and Pd"   *
 *   by Eric Lyon.                                  *   
 ****************************************************/

/* Required header files */

#include "ext.h"			
#include "ext_obex.h"	

/*
 Since bed is not a signal object, we do not include "z_dsp.h". 
 We do include "buffer.h" for access to various buffer-related declarations.
 */

#include "buffer.h"			

/* The class pointer */

void *bed_class;

/* The object structure */

typedef struct _bed 
{
	t_object	obj; // the Max object
	t_buffer	*buffy; // the Buffer
	t_symbol	*b_name; // the name of the buffer
	float		*undo_samples; // contains samples to undo the previous operation
	long		undo_start; // start frame for the undo replace
	long		undo_frames; // how many frames in the undo
	long		cut_frames; // how many frames were cut?
	long		can_undo; // flag that an undo is possible
	long		undo_resize; // flag that the undo process will resize the buffer
	long		undo_cut; // flag that last operation was a cut
} t_bed;

/* Function prototypes */

void *bed_new(t_symbol *s, short argc, t_atom *argv);
int attach_buffer(t_bed *x);
void bed_info(t_bed *x);
void bed_normalize(t_bed *x);
void bed_fadein(t_bed *x, double fadetime);
void bed_cut(t_bed *x, double start, double end);
void bed_undo(t_bed *x);
void bed_dblclick(t_bed *x);
int attach_any_buffer(t_buffer **b, t_symbol *b_name);
void bed_paste(t_bed *x, t_symbol *destname);
void bed_free(t_bed *x);
void bed_bufname(t_bed *x, t_symbol *name);
void bed_assist(t_bed *x, void *b, long m, long a, char *s);

/* The main() function */

int main(void)
{	
	/* 
	 Define and use local variable c to hold the bed class pointer,
	 in order to save typing in the class_addmethod() calls.
	*/
	
	t_class *c; 
	c = class_new("bed", (method)bed_new, (method)bed_free, (long)sizeof(t_bed), 0, A_GIMME, 0);
	class_addmethod(c, (method)bed_bufname, "bufname", A_SYM, 0);
    class_addmethod(c, (method)bed_info, "info", 0);	
	class_addmethod(c, (method)bed_normalize, "normalize", 0);
	class_addmethod(c, (method)bed_fadein, "fadein", A_FLOAT, 0);
	class_addmethod(c, (method)bed_cut, "cut", A_FLOAT, A_FLOAT, 0);
	class_addmethod(c, (method)bed_paste, "paste", A_SYM, 0);
	class_addmethod(c, (method)bed_dblclick, "dblclick", A_CANT, 0);
	class_addmethod(c, (method)bed_assist, "assist", A_CANT, 0);
	class_addmethod(c, (method)bed_undo, "undo", 0);
	
	/* We do not call class_dspinit() since bed is not a signal object */
	
	class_register(CLASS_BOX, c);
	
	/* 
	 Copy the class address from the local pointer c to the 
	 global pointer bed_class */
	
	bed_class = c;
	post("bed from \"Designing Audio Objects\" by Eric Lyon");
	return 0;
}

/* The new instance method */

void *bed_new(t_symbol *s, short argc, t_atom *argv)
{
	t_bed *x = (t_bed *)object_alloc(bed_class); 
	atom_arg_getsym(&x->b_name, 0, argc, argv);
	x->undo_samples = NULL;
	x->can_undo = 0;
	x->undo_cut = 0;
	return x;
}


/* The info method */

void bed_info(t_bed *x)
{
	t_buffer *b;
	
	if(! attach_buffer(x)){
		return;
	} 	
	b = x->buffy;
	post("my name is: %s", b->b_name->s_name);
	post("my frame count is: %d", b->b_frames);
	post("my channel count is: %d", b->b_nchans);
	post("my validity is: %d", b->b_valid);
	post("my in use status is: %d", b->b_inuse);
}

/* The bufname method */

void bed_bufname(t_bed *x, t_symbol *name)
{
	x->b_name = name;
}

/* The undo method */

void bed_undo(t_bed *x)
{
	t_buffer *b;
	float *local_samples; // for storing undo samples
	long local_frames; // framesize of the buffer
	long chunksize; // size of memory alloc in bytes
	long offset; // skip time into the buffer
	t_atom rv; // needed for message call
	
	if(! x->can_undo ){
		object_post((t_object *)x,"nothing to undo");
		return;
	}	
	
	if(! attach_buffer(x)){
		
		return;
	} 

	b = x->buffy;
	ATOMIC_INCREMENT(&b->b_inuse);
    if (!b->b_valid) {
		ATOMIC_DECREMENT(&b->b_inuse);
    	return;
	}
	
	/* Take care of the special case for undoing a cut */
	
	if(x->undo_cut){
		/* Copy all samples from the main buffer to local buffer */
		
		local_frames = b->b_frames;
		chunksize = local_frames * b->b_nchans * sizeof(float);
		local_samples = (float *) sysmem_newptr(chunksize);
		sysmem_copyptr(b->b_samples, local_samples,  chunksize);
		
		
		/* 
		 Release the main buffer so that it can be accessed through the subsequent
		 call to "sizeinsamps." 
		 */
		
		ATOMIC_DECREMENT(&b->b_inuse);
		
		/* Enlarge the main buffer to incorporate the previously cut segment */
		
		object_method_long(&b->b_obj, gensym("sizeinsamps"), x->undo_frames + local_frames, &rv);
		
		/* Re-acquire the main buffer */
		
		ATOMIC_INCREMENT(&b->b_inuse);
		
		/* Copy the first part back to the main buffer */
		
		
		chunksize = x->undo_start * b->b_nchans * sizeof(float);
		sysmem_copyptr(local_samples, b->b_samples, chunksize);
		
		
		/* Insert the cut piece back to the main buffer */
		
		chunksize = x->undo_frames * b->b_nchans * sizeof(float);
		offset = x->undo_start * b->b_nchans;
		sysmem_copyptr(x->undo_samples, b->b_samples + offset, chunksize);
		
		
		/* Add the last piece back into the main buffer */
		
		chunksize = (local_frames - x->undo_start) * b->b_nchans * sizeof(float);
		offset = (x->undo_start + x->undo_frames) * b->b_nchans;
		sysmem_copyptr(local_samples + (x->undo_start * b->b_nchans), b->b_samples + offset, chunksize);
		
		/* Release the buffer */
		 
		ATOMIC_DECREMENT(&b->b_inuse);
		
		/* Turn off the undo_cut flag */
		
		x->undo_cut = 0;
		
		/* Free the local sample memory */
		
		sysmem_freeptr(local_samples);
		
		/* Instruct any waveform~ objects to redraw the buffer */
		
		object_method((t_object *)b, gensym("dirty"));
		return;
	}
	/* This is the block for undoing in-place processing */
	
	/* Calculate the bytesize of the frames to be undone */
	
	chunksize = x->undo_frames * b->b_nchans * sizeof(float);
	
	/* The block for undoing a resize */
	
	if(x->undo_resize){
		
		/*
		 The buffer must be resized using "sizeinsamps." Note that "sizeinsamps" 
		 actually refers to sample frames, not individual samples. So if the buffer
		 is stereo, the call "sizeinsamps 1000" generates space for 2000 samples.
		 */
		
		/* Release the buffer */
			
		ATOMIC_DECREMENT(&b->b_inuse);
		
		/* Resize the buffer */
		
		object_method_long(&b->b_obj, gensym("sizeinsamps"), x->undo_frames, &rv);
		
		/* Reacquire the buffer */
		
		ATOMIC_INCREMENT(&b->b_inuse);
	}
	/* Copy the saved samples back into the (possibly resized) buffer */
	
	sysmem_copyptr(x->undo_samples,  b->b_samples + x->undo_start, chunksize);

	/* Reset undo flag, since there is nothing left to undo */
	
	x->can_undo = 0;
	
	/* Force a waveform~ redraw if necessary */
	
	object_method((t_object *)b, gensym("dirty"));
	
	/* Release the buffer */
	
	ATOMIC_DECREMENT(&b->b_inuse);	
}

/* The normalize method */

void bed_normalize(t_bed *x)
{
	t_buffer *b;
	float maxamp = 0.0;
	float rescale;
	long chunksize;
	int i;
	
	/* Attach the buffer and check that it is valid */
	
	if( ! attach_buffer(x) ){
		return;
	} 
	
	b = x->buffy;
	ATOMIC_INCREMENT(&b->b_inuse);
    if (!b->b_valid) {
		ATOMIC_DECREMENT(&b->b_inuse);
		object_post((t_object *)x, "bed normalize: not a valid buffer!");
    	return;
	}
	
	/* Calculate the maximum amplitude */
	
	for(i = 0; i < b->b_frames * b->b_nchans; i++){
		if(maxamp < fabs(b->b_samples[i]) ){
			maxamp = fabs(b->b_samples[i]);
		}
	}
	
	/* Generate the rescale factor */

	if(maxamp > 0.000001){
		rescale = 1.0 / maxamp;
	} 
	else {
		object_post((t_object *)x,"amplitude is too low to rescale: %f", maxamp);
		ATOMIC_DECREMENT(&b->b_inuse);
		return;
	}
	
	/* Calculate the byte size for the undo sample block */
	
	chunksize = b->b_frames * b->b_nchans * sizeof(float);
	
	/* Store samples for undo */
	
	if( x->undo_samples == NULL ){
		x->undo_samples = (float *) sysmem_newptr(chunksize);
	} else {
		x->undo_samples = (float *) sysmem_resizeptr(x->undo_samples, chunksize);
	}
	if(x->undo_samples == NULL){
		object_post((t_object *)x,"cannot allocate memory for undo");
		x->can_undo = 0;
		ATOMIC_DECREMENT(&b->b_inuse);
		return;
	} 
	else {
		x->can_undo = 1;
		x->undo_cut = 0;
		x->undo_start = 0;
		x->undo_frames = b->b_frames;
		x->undo_resize = 0;

		sysmem_copyptr( b->b_samples, x->undo_samples,chunksize);
		
	}
	
	/* Perform the normalization */
	
	for(i = 0; i < b->b_frames * b->b_nchans; i++){
		b->b_samples[i] *= rescale;
	}	
	/* Force waveform~ to redraw the buffer */
	
	object_method(&b->b_obj, gensym("dirty"));
	
	/* Release the buffer */
	
	ATOMIC_DECREMENT(&b->b_inuse);
}


/* The cut method */

void bed_cut(t_bed *x, double start, double end)
{
	t_buffer *b;
	long chunksize; // size of cut chunk in bytes
	long cutframes; // frames to cut
	long startframe, endframe;
	t_atom rv; // return value, needed for message call
	long offset1, offset2; // memory offsets
	float *local_samples; // storage for undo samples
	long local_frames; // buffer frame count
	
	if(! attach_buffer(x)){
		return;
	} 
	
	b = x->buffy;
	ATOMIC_INCREMENT(&b->b_inuse);
    if (!b->b_valid) {
		ATOMIC_DECREMENT(&b->b_inuse);
    	return;
	}
	
	/* Calculate frame values in samples */
	
	startframe = start * 0.001 * b->b_sr;
	endframe = end * 0.001 * b->b_sr;
	cutframes = endframe - startframe;
	
	/* Check for invalid frame data */
	
	if(cutframes <= 0 || cutframes > b->b_frames){
		object_post((t_object *)x,"bad cut data: %f %f", start, end);
		ATOMIC_DECREMENT(&b->b_inuse);
		return;
	}	
	
	/* Store undo size */
	
	x->undo_frames = cutframes;
	
	/* Store samples for undo  */
	
	local_frames = b->b_frames;
	chunksize = local_frames * b->b_nchans * sizeof(float);
	local_samples = (float *) sysmem_newptr(chunksize);
	sysmem_copyptr(b->b_samples, local_samples,  chunksize);
	
	/* Exit if memory allocation fails */
	
	if(local_samples == NULL){
		object_post((t_object *)x,"cannot allocate memory for undo");
		x->can_undo = 0;
		ATOMIC_DECREMENT(&b->b_inuse);
		return;
	}
	
	/* Allocate memory for just the cut segment */
	
	chunksize = cutframes * b->b_nchans * sizeof(float);	
	if( x->undo_samples == NULL ){
		x->undo_samples = (float *) sysmem_newptr(chunksize);
	} 
	else {
		x->undo_samples = (float *) sysmem_resizeptr(x->undo_samples, chunksize);
	}
	if(x->undo_samples == NULL){
		object_post((t_object *)x,"cannot allocate memory for cut segment");
		x->can_undo = 0;
		ATOMIC_DECREMENT(&b->b_inuse);
		return;
	} 
	else {
		x->can_undo = 1;
		x->undo_start = startframe; 
		x->undo_frames = cutframes;
		x->undo_resize = 1;
		sysmem_copyptr(b->b_samples + (startframe * b->b_nchans), x->undo_samples,  chunksize);
	}
	
	/* Release and resize the main buffer */
	
	ATOMIC_DECREMENT(&b->b_inuse);
	// reduce size of the buffer
	object_method_long(&b->b_obj, gensym("sizeinsamps"), (b->b_frames - cutframes), &rv);
	ATOMIC_INCREMENT(&b->b_inuse);
	
	
	/* Copy samples up to the start of the cut */
	
	chunksize = startframe * b->b_nchans * sizeof(float);
	sysmem_copyptr(local_samples, b->b_samples,  chunksize);
	
	/* Copy from the end of the cut to the end of the buffer */
	
	chunksize = (local_frames - endframe) * b->b_nchans * sizeof(float);
	offset1 = startframe * b->b_nchans;
	offset2 = endframe * b->b_nchans;
	sysmem_copyptr(local_samples + offset2, b->b_samples + offset1,  chunksize);
		
	/* Redraw the buffer */
	
	object_method(&b->b_obj, gensym("dirty"));
	ATOMIC_DECREMENT(&b->b_inuse);
	
	/* Free local memory */

	sysmem_freeptr(local_samples);
	
	/* Set the undo cut flag */
	
	x->undo_cut = 1;
}

/* The paste method */

void bed_paste(t_bed *x, t_symbol *destname)
{
	t_buffer *destbuf = NULL; // destination buffer
	t_atom rv; // return value from "sizeinsamps" message
	long chunksize; // bytesize of samples to be copied
	if(x->can_undo){
		if(! attach_buffer(x)){
			return;
		} 
		if(attach_any_buffer(&destbuf, destname)){
			
			/* Return if there is a channel mismatch */
			
			if(destbuf->b_nchans != x->buffy->b_nchans){
				object_post((t_object *)x, "bed: channel mismatch between %s and %s", destname->s_name, x->b_name->s_name);
				return;
			}			
			
			/* Resize the destination buffer */
			
			object_method_long(&destbuf->b_obj, gensym("sizeinsamps"), x->undo_frames, &rv);
			
			/* Acquire exclusive access to the destination buffer */
			
			ATOMIC_INCREMENT(&destbuf->b_inuse);
			
			/* Copy samples to the destination buffer */
			
			chunksize = x->undo_frames * destbuf->b_nchans * sizeof(float);
			sysmem_copyptr(x->undo_samples,  destbuf->b_samples, chunksize);
			
			/* Release the destination buffer */
			
			ATOMIC_DECREMENT(&destbuf->b_inuse);
		}
		else{
			/* Error message for an invalid destination object */
			
			object_post((t_object *)x,"%s is not a valid destination buffer", destname->s_name);
		}
	} else {
		
		/* Error message for when there is nothing in the undo buffer */
		
		object_post((t_object *)x,"nothing to paste");
	}
}


/* The fade in method */

void bed_fadein(t_bed *x, double fadetime)
{
	t_buffer *b; // local pointer to the buffer
	long chunksize; // size of memory alloc in bytes
	long fadeframes; // frames to fade over
	int i,j;
	
	if(! attach_buffer(x)){
		return;
	} 
	
	b = x->buffy;
	ATOMIC_INCREMENT(&b->b_inuse);
    if (!b->b_valid) {
		ATOMIC_DECREMENT(&b->b_inuse);
    	return;
	}
	
	/* Calculate the fade time in sample frames */
	
	fadeframes = fadetime * 0.001 * b->b_sr;
	if(fadetime <= 0 || fadeframes > b->b_frames){
		object_post((t_object *)x,"bad fade time: %f", fadetime);
		ATOMIC_DECREMENT(&b->b_inuse);
		return;
	}	

	/* Store samples for undo */
	
	chunksize = fadeframes * b->b_nchans * sizeof(float);
	if( x->undo_samples == NULL ){
		x->undo_samples = (float *) sysmem_newptr(chunksize);
	} else {
		x->undo_samples = (float *) sysmem_resizeptr(x->undo_samples, chunksize);
	}
	if(x->undo_samples == NULL){
		object_post((t_object *)x,"cannot allocate memory for undo");
		x->can_undo = 0;
		ATOMIC_DECREMENT(&b->b_inuse);
		return;
	} else {
		x->can_undo = 1;
		x->undo_cut = 0;
		x->undo_start = 0;
		x->undo_frames = fadeframes;
		x->undo_resize = 0;
		sysmem_copyptr(b->b_samples, x->undo_samples,  chunksize);
	}
	
	/* Perform a linear fadein */
	
	for(i = 0; i < fadeframes; i++){
		for(j = 0; j < b->b_nchans; j++){
			b->b_samples[(i * b->b_nchans) + j] *= (float)i / (float) fadeframes;
		}
	}	
	
	/* Redraw the buffer */
	
	object_method(&b->b_obj, gensym("dirty"));
	ATOMIC_DECREMENT(&b->b_inuse);
}

/* The attach buffer utility function */

int attach_buffer(t_bed *x)
{	
	t_object *o; // generic object pointer
	
	/* Set the object pointer to the object named by symbol x->b_name */
	
	o = x->b_name->s_thing;
	
	/* If the object does not exist, post a message and return */
	
	if(o == NULL){
		object_post((t_object *)x, "\"%s\" is not a valid buffer", x->b_name->s_name);
		return 0;
	}
	
	/* If the object is a buffer, then set the pointer x->buffy to point to the buffer */
	
	if (ob_sym(o) == gensym("buffer~")) {
		x->buffy = (t_buffer *) o;
		return 1;
	} 
	
	/* 
	 Otherwise do nothing. This code could be improved with an error message stating that
	 the named object is not a buffer. 
	 */
	
	else {
		return 0;
	}
}

/* The attach any buffer utility function */

int attach_any_buffer(t_buffer **b, t_symbol *b_name)
{
	t_object *o; // generic object pointer 
	
	/* Point to the object named by symbol x->b_name */
	
	o = b_name->s_thing;
	
	/* bad object, return 0 (false) */
	
	if(o == NULL){
		return 0;
	}
	
	/* If the buffer is valid, return 1, otherwise 0 */
	
	if (ob_sym(o) == gensym("buffer~")) {
		*b = (t_buffer *) o;
		if(*b == NULL){
			post("bed: invalid buffer: \"%s\"",b_name->s_name);
			return 0;
		}
		return 1;
	} 
	
	/* Object is not a buffer - return 0 */
	
	else {
		post("bed: \"%s\" is not a buffer",b_name->s_name);
		return 0;
	}
}


/* the free routine */

void bed_free(t_bed *x)
{
	/* bed is not an MSP object so there is no need to free it from the DSP chain */
	
	sysmem_freeptr(x->undo_samples);
}

/* The double click method */

void bed_dblclick(t_bed *x)
{
	t_buffer *b;
	if(! attach_buffer(x)){
		return;
	} 
	b = x->buffy;
	object_method((t_object *)b, gensym("dblclick"));	
}

/* The assist method */

void bed_assist(t_bed *x, void *b, long m, long a, char *s)
{
	/* There is only an inlet on this object, so no branching is required */
	
	sprintf(s, "messages");
}

