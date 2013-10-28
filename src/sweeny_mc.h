char sweeny_setup(unsigned int impl_idx, double q, unsigned int l, double beta, 
        double coupl, unsigned int cutoff, unsigned int tslength,
        unsigned int rng_seed,
        void *ts_0, void *ts_1,void *ts_2, void *ts_3, void *ts_4);

void sweeny_destroy(); 

char sweeny_simulate();
