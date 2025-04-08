/**
 * @file fdtd2d.c
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

#include "fdtd2d.h"

void calc_ex_ey(const struct Range *whole, const struct Range *inside,
                const FLOAT *hz, const FLOAT *cexly, const FLOAT *ceylx, FLOAT *ex, FLOAT *ey)
{
    const int nx    = inside->length[0];
    const int ny    = inside->length[1];
    const int mgn0  = inside->begin[0] - whole->begin[0];
    const int mgn1  = inside->begin[1] - whole->begin[1] ;
    const int lnx   = whole->length[0];

#pragma acc kernels present(ex, cexly, hz)
#pragma acc loop independent
    for (int j=0; j<ny+1; j++) {
#pragma acc loop independent
        for (int i=0; i<nx; i++) {
            const int ix = (j+mgn1)*lnx + i+mgn0;
            const int jm = ix - lnx;
            ex[ix] += cexly[ix]*(hz[ix]-hz[jm]);
        }
    }
    
#pragma acc kernels present(ey, ceylx, hz)
#pragma acc loop independent
    for (int j=0; j<ny; j++) {
#pragma acc loop independent
        for (int i=0; i<nx+1; i++) {
            const int ix = (j+mgn1)*lnx + i+mgn0;
            const int im = ix - 1;
            ey[ix] += - ceylx[ix]*(hz[ix]-hz[im]);
        }
    }
}

void calc_hz(const struct Range *whole, const struct Range *inside,
             const FLOAT *ey, const FLOAT *ex, const FLOAT *chzlx, const FLOAT *chzly, FLOAT *hz)
{
    const int nx    = inside->length[0];
    const int ny    = inside->length[1];
    const int mgn0  = inside->begin[0] - whole->begin[0];
    const int mgn1  = inside->begin[1] - whole->begin[1];
    const int lnx   = whole->length[0];

#pragma acc kernels present(hz, chzlx, ey, chzly, ex)
#pragma acc loop independent
    for (int j=0; j<ny; j++) {
#pragma acc loop independent
        for (int i=0; i<nx; i++) {
            const int ix = (j+mgn1)*lnx + i+mgn0;
            const int ip = ix + 1;
            const int jp = ix + lnx;
            hz[ix] += - chzlx[ix]*(ey[ip]-ey[ix]) + chzly[ix]*(ex[jp]-ex[ix]);
        }
    }
}

void pml_boundary_ex(const struct Range *whole, const struct Range *inside,
                     const FLOAT *hz, const FLOAT *cexy, const FLOAT *cexyl, const FLOAT *rer_ex,
                     FLOAT *ex, FLOAT *exy)
{
    const int bi[] = { inside->begin[0], inside->begin[1] };
    const int ei[] = { inside->begin[0] + inside->length[0], inside->begin[1] + inside->length[1] };
    const int bw[] = { whole->begin[0], whole->begin[1] };
    const int ew[] = { whole->begin[0] + whole->length[0], whole->begin[1] + whole->length[1] };

    static int r[4][4];
    
    static int first = 1;
    
    const int lnx = whole->length[0];
    const int bw0 = bw[0];
    const int bw1 = bw[1];
    
    if(first){
        const int ei0 = ei[0];
        const int ei1 = ei[1];
        const int ew0 = ew[0];
        const int ew1 = ew[1];
        const int bi0 = bi[0];
        const int bi1 = bi[1];
        
        r[0][0] = bw0+1; r[0][1] = ew0; r[0][2] = bw1; r[0][3] = bi1;
        r[1][0] = bw0+1; r[1][1] = ew0; r[1][2] = ei1; r[1][3] = ew1;
        r[2][0] = bw0+1; r[2][1] = bi0; r[2][2] = bi1; r[2][3] = ei1;
        r[3][0] = ei0+1; r[3][1] = ew0; r[3][2] = bi1; r[3][3] = ei1;
#pragma acc enter data copyin(r)
        first = 0;
    }
    

#pragma acc parallel present(exy, cexy, exy, rer_ex, cexyl, hz, ex)
#pragma acc loop seq
    for (int l=0; l<4; l++) {
#pragma acc loop independent
        for (int j=r[l][2]; j<r[l][3]; j++) {
#pragma acc loop independent
            for (int i=r[l][0]; i<r[l][1]; i++) {

                const int jj = j - bw1;
                const int ii = i - bw0;
                
                const int ix  = jj*lnx + ii;
                const int jm  = ix - lnx;
                exy[ix] = cexy[jj]*exy[ix] + rer_ex[ix]*cexyl[jj]*(hz[ix] - hz[jm]);
                ex [ix]  = exy[ix];
            }
        }
        
    }
    
}


void pml_boundary_ey(const struct Range *whole, const struct Range *inside,
                     const FLOAT *hz, const FLOAT *ceyx, const FLOAT *ceyxl, const FLOAT *rer_ey,
                     FLOAT *ey, FLOAT *eyx)
{
    const int bi[] = { inside->begin[0], inside->begin[1] };
    const int ei[] = { inside->begin[0] + inside->length[0], inside->begin[1] + inside->length[1] };
    const int bw[] = { whole->begin[0], whole->begin[1] };
    const int ew[] = { whole->begin[0] + whole->length[0], whole->begin[1] + whole->length[1] };

    static int r[4][4];
    
    static int first = 1;
    
    const int lnx = whole->length[0];
    const int bw0 = bw[0];
    const int bw1 = bw[1];
    
    if(first){
        const int ei0 = ei[0];
        const int ei1 = ei[1];
        const int ew0 = ew[0];
        const int ew1 = ew[1];
        const int bi0 = bi[0];
        const int bi1 = bi[1];
        
        r[0][0] = bw0+1; r[0][1] = ew0; r[0][2] = bw1; r[0][3] = bi1;
        r[1][0] = bw0+1; r[1][1] = ew0; r[1][2] = ei1; r[1][3] = ew1;
        r[2][0] = bw0+1; r[2][1] = bi0; r[2][2] = bi1; r[2][3] = ei1;
        r[3][0] = ei0+1; r[3][1] = ew0; r[3][2] = bi1; r[3][3] = ei1;
#pragma acc enter data copyin(r)
        first = 0;
    }
    
#pragma acc parallel present(eyx, ceyx, rer_ey, ceyxl, hz, ey)
#pragma acc loop seq
    for (int l=0; l<4; l++) {
#pragma acc loop independent
        for (int j=r[l][2]; j<r[l][3]; j++) {
#pragma acc loop independent
            for (int i=r[l][0]; i<r[l][1]; i++) {

                const int jj = j - bw1;
                const int ii = i - bw0;
                
                const int ix  = jj*lnx + ii;
                const int im  = ix - 1;
                eyx[ix] = ceyx[ii]*eyx[ix] - rer_ey[ix]*ceyxl[ii]*(hz[ix]-hz[im]);
                ey [ix]  = eyx[ix];
            }
        }
        
    }    
}


void pml_boundary_hz(const struct Range *whole, const struct Range *inside,
                     const FLOAT *ey, const FLOAT *ex,
                     const FLOAT *chzx, const FLOAT *chzxl, const FLOAT *chzy, const FLOAT *chzyl,
                     FLOAT *hz, FLOAT *hzx, FLOAT *hzy)
{
    const int bi[] = { inside->begin[0], inside->begin[1] };
    const int ei[] = { inside->begin[0] + inside->length[0], inside->begin[1] + inside->length[1] };
    const int bw[] = { whole->begin[0], whole->begin[1] };
    const int ew[] = { whole->begin[0] + whole->length[0], whole->begin[1] + whole->length[1] };

    static int r[4][4];
    
    static int first = 1;
    
    const int lnx = whole->length[0];
    const int bw0 = bw[0];
    const int bw1 = bw[1];
    
    if(first){
        const int ei0 = ei[0];
        const int ei1 = ei[1];
        const int ew0 = ew[0];
        const int ew1 = ew[1];
        const int bi0 = bi[0];
        const int bi1 = bi[1];
        r[0][0] = bw0; r[0][1] = ew0-1; r[0][2] = bw1; r[0][3] = bi1;
        r[1][0] = bw0; r[1][1] = ew0-1; r[1][2] = ei1; r[1][3] = ew1-1;
        r[2][0] = bw0; r[2][1] = bi0  ; r[2][2] = bi1; r[2][3] = ei1;
        r[3][0] = ei0; r[3][1] = ew0-1; r[3][2] = bi1; r[3][3] = ei1;
#pragma acc enter data copyin(r)
        first = 0;
    }
    
#pragma acc parallel present(hzx, chzx, hzx, chzxl, ey, hzy, chzy, hzy, chzyl, ex, hz)
#pragma acc loop seq
    for (int l=0; l<4; l++) {
#pragma acc loop independent
        for (int j=r[l][2]; j<r[l][3]; j++) {
#pragma acc loop independent
            for (int i=r[l][0]; i<r[l][1]; i++) {
                
                const int jj = j - bw1;
                const int ii = i - bw0;
                
                const int ix  = jj*lnx + ii;
                const int ip  = ix + 1;
                const int jp  = ix + lnx;
                hzx[ix] = chzx[ii]*hzx[ix] - chzxl[ii]*(ey[ip]-ey[ix]);
                hzy[ix] = chzy[jj]*hzy[ix] + chzyl[jj]*(ex[jp]-ex[ix]);
                hz [ix]  = hzx[ix] + hzy[ix];
            }
        }
        
    }
    
}
