/**
 * @file fdtd2d_sources.c
 * @brief (File brief)
 *
 * (File explanation)
 *
 * @author Takashi Shimokawabe
 * @date 2017/11/10 Created
 * @version 0.1.0
 *
 * $Header$ 
 */

#include "fdtd2d_sources.h"
#include <math.h>

void plane_wave_incidence(const struct Range *whole, const struct Range *inside, 
                          FLOAT time, int jpos, FLOAT wavelength, FLOAT *ex, FLOAT *ey)
{
    const FLOAT pi = constant.pi;
    const FLOAT c  = constant.c;

    const int inside_end[] = { inside->begin[0] + inside->length[0],
                               inside->begin[1] + inside->length[1] };
    
    const FLOAT freq = c / wavelength; // Hz
    const FLOAT a = 80.0;

    const FLOAT e = a*sin(2.0*pi*freq*time);

    
    if (jpos < inside->begin[1] || jpos >= inside_end[1]) {
        return;
    }    
    
    const int lnx = whole->length[0];
    const int beginx = whole->begin[0];

    const int jj = jpos - whole->begin[1];
    const int inside_begin0 = inside->begin[0];
    const int inside_end0 = inside_end[0];
    
#pragma acc kernels present(ex)
#pragma acc loop independent
    for (int i=inside_begin0; i<inside_end0; i++) {
        const int ii = i    - beginx;
        const int ix = jj*lnx + ii;
        ex[ix] = e;
    }
}

