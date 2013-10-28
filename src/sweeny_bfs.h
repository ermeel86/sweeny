#ifndef SWEENY_BFS_H
#define SWEENY_BFS_H
char init_sweeny_ibfs(double _q,unsigned int _l,double _beta,double _coupl,
        unsigned int _cutoff,unsigned _tslength,unsigned int rng_seed,
        void *ts_0,void *ts_1,void * ts_2,void *ts_3, void *ts_4);

void destroy_sweeny_ibfs(void);
char simulate_sweeny_ibfs(void);

char init_sweeny_sbfs(double _q,unsigned int _l,double _beta,double _coupl,
        unsigned int _cutoff,unsigned _tslength,unsigned int rng_seed,
        void *ts_0,void *ts_1, void * ts_2,void *ts_3,void *ts_4);

void destroy_sweeny_sbfs(void);
char simulate_sweeny_sbfs(void);

#endif /*SWEENY_BFS_H*/
