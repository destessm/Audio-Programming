/* slightly less (bad) ambisonics library */

#include "slambien.h"
#include "slambide.h"
#include "slambiwode.h"

void slambilib_setup(void)
{
  slambien_tilde_setup();
  slambide_tilde_setup();
  slambiwode_tilde_setup();

  post("slambilib, the Slightly Less (bad) AMBIsonics LIBrary");
  post("v1.0 Copyright 2017 David Estes-Smargiassi");
}
