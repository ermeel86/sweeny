#include "sweeny_mc.h"
#include "sweeny_dc.h"
#include "sweeny_bfs.h"
#include "sweeny_uf.h"
static char cur_impl;
static char sy_init=0;

char sweeny_setup(unsigned int impl_idx, double q, unsigned int l, double beta, 
        double coupl, unsigned int cutoff, unsigned int tslength,
        unsigned int rng_seed,
        void *ts_0, void *ts_1, void *ts_2, void *ts_3) {
    cur_impl = impl_idx;
    switch(cur_impl) {
        case 0:
            return sy_init = init_sweeny_ibfs(q,l,beta,coupl,cutoff,tslength,
                    rng_seed,ts_0,ts_1,ts_2,ts_3);
        case 1:
            return sy_init = init_sweeny_sbfs(q,l,beta,coupl,cutoff,tslength,
                    rng_seed,ts_0,ts_1,ts_2,ts_3);
        case 2:
            return sy_init = init_sweeny_dc(q,l,beta,coupl,cutoff,tslength,
                    rng_seed,ts_0,ts_1,ts_2,ts_3);
        case 3:
            return sy_init=init_sweeny_uf(q,l,beta,coupl,cutoff,tslength,
                    rng_seed,ts_0,ts_1,ts_2,ts_3);
        default:
            return sy_init = 0;
    }

}
void sweeny_destroy() {
    if(!sy_init)
        return;
    switch(cur_impl) {
        case 0:
            destroy_sweeny_ibfs();
            break;
        case 1:
            destroy_sweeny_sbfs();
            break;
        case 2:
            destroy_sweeny_dc();
            break;
        case 3:
            destroy_sweeny_uf();
            break;
    }
}

char sweeny_simulate() {
    if(!sy_init)
        return 0;
    switch(cur_impl) {
        case 0:
            return simulate_sweeny_ibfs();
        case 1:
            return simulate_sweeny_sbfs();
        case 2:
            return simulate_sweeny_dc();
        case 3:
            return simulate_sweeny_uf();
    }
    return 0;
} 
