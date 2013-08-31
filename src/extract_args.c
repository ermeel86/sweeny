#include <linux/types.h>
#include "extract_args.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <unistd.h>

/*********************************************************************
**********************************************************************/

extern __u32 DX;
extern __u32 seed;
extern __u32 cutoff;
extern __u32 steps;
extern double beta;
extern double coupling;
extern double q;
extern double rnd_num;
extern char verbose;

int extractArgs(int argc, char **argv,char *impl_title) {
    seed = SEED_D;
    DX = DX_D;
    beta = BETA_D;
    steps = STEPS_D;
    q = Q_D;
    coupling = COUPLING_D;
    cutoff = CUTOFF_D;
    int c;
    opterr = 0;
    while((c = getopt(argc,argv,"s:l:q:b:m:j:c:vh")) != -1) {
      switch(c) {
	case 's':
	  seed = atoi(optarg);
	  break;
	case 'l':
	  DX = atoi(optarg);
	  break;
	case 'q':
	  q = atof(optarg);
	  break;
	case 'b':
	  beta = atof(optarg);
	  break;
	case 'm':
	  steps = atoi(optarg);
	  break;
	case 'j':
	  coupling = atof(optarg);
	  break;
	case 'c':
	  cutoff = atoi(optarg);
	  break;
	case 'v':
	  verbose = 1;
	  break;
	case 'h':
	    printf("-s\t Pseudo random number generator seed, default value: %d\n",SEED_D);
	    printf("-l\t Number of vertices per dimension, default value: %d\n",DX_D);
	    printf("-q\t Number of states (also non integer allowed), default value: %f\n",Q_D);
	    printf("-b\t Inverse temperature (In natural units), default value: %f\n",BETA_D);
	    printf("-m\t Maximal (physical) simulation time, default value: %d\n",STEPS_D);
	    printf("-j\t Coupling constant, default value %f\n",COUPLING_D);
  	    printf("-c\t Cutoff for equilibration (only in runtime mode), default value %d\n",CUTOFF_D);
	    printf("-v\t Print summary of all parameters, flag\n");
	    printf("-h\t Show this help, flag\n");
	    return 0;
        case '?':
	default:
	  fprintf(stderr,"Invalid argument(s)\n");
	  abort();
      }
    }

    if(verbose) {
        printf("%s\n",impl_title);
        printf("SEED = %u\n",seed);
        printf("DX = %u\n",DX);
        printf("Q = %f\n",q);
        printf("beta = %f\n",beta);
        printf("steps = %u\n",steps);
        printf("coupling = %f\n",coupling);
        printf("cutoff = %u\n",cutoff);
    }
    return 1;
}

