#ifndef SWEENY_DC_H
#define SWEENY_DC_H
char init_sweeny_dc(double q_,unsigned int l_,double beta_,double coupl_,
        unsigned int cutof_f,unsigned tslength_,unsigned int rng_seed,
        void *ts_0,void *ts_1, void * ts_2,void *ts_3,void *ts_4);

void destroy_sweeny_dc(void);
char simulate_sweeny_dc(void);

#endif /*SWEENY_DC_H*/
