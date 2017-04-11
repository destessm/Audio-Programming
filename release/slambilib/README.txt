[slambilib], the slightly less bad ambisonics library (v1)

[slambien~] Encoder
usage: [slambien~ <order>]
<order> = 1, 2, or 3
EX: [slambien~ 2]


[slambiwode~] Worse Decoder
usage: [slambiwode~ <speaker conf> <in-phase>]
<speaker conf> = stereo, quad, pent, hex, or oct
<in-phase> = y or n
EX: [slambiwode~ pent y]

*** NOT IN USE YET ***
[slambide~] Generalized Decoder
usage: [slambide~ <in-phase> <order> <list of speaker angles>]
<in-phase> = y or n
<order> = 1, 2, or 3
<list of speaker angles> = speaker angles in degrees, each in range [0,360)
EX: [slambide~ y 2 315 225 135 45]
**********************

Either copy the slambide.pd_darwin in this directory to your personal PD
external folder that is already linked to from PD or link this folder to PD by
going to PD -> preferences -> Add, and navigate to this folder.

(c) David Estes-Smargiassi 2017
