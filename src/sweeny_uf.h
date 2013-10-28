#ifndef SWEENY_UF_H
#define SWEENY_UF_H
char init_sweeny_uf(double _q,unsigned int _l,double _beta,double _coupl,
        unsigned int _cutoff,unsigned _tslength,unsigned int rng_seed,void *ts_0,void *ts_1,
        void * ts_2,void *ts_3, void *ts_4);

void destroy_sweeny_uf(void);
char simulate_sweeny_uf(void);

#endif /*SWEENY_UF_H*/
