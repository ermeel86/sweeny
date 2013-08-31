cdef extern from "./sweeny_mc.h":
    char sweeny_setup()
    #char sweeny_setup(char impl_idx, double q, unsigned int l, double beta, 
    #        double coupl, unsigned int cutoff, unsigned int tslength,
    #        unsigned int rng_seed, char *ts_0, char *ts_1, char *ts_2, char *ts_3)
    void sweeny_destroy()
    char sweeny_simulate()
