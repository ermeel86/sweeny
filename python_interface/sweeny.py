#!/usr/bin/env python
import sweeny_c as syc
import numpy as np
class Sweeny(object):
    """
    Provides 4 different implementations of Sweeny's algorithm for the RCM
    on a two-dimensional square lattice with periodic boundary conditions.
    """ 
    def init_sim(self,q,l,beta,coupl,cutoff,tslength,rngseed,impl='ibfs'):
        """
        Resets/Prepares to/for simulation with specified parameters.

        Parameters
        ----------
        q: float
            Cluster-weight, (>0).
        l: int
            Number of vertices per dimension, (>2)
        beta: float
            Inverse temperature, (>0)
        coupl: float
            Coupling constant, (>0)/Ferromagnetic
        cutoff: int
            Number of sweeps for equilibration, (>0)
        tslength: int
            Number of sweeps of actual simulation. This also corresponds
            to the number of observable samples, (>0)
        rngseed: int
            Seed for the pseudo-random-number generator, (>0)
        impl: str
            Type of implementation. 'ibfs', 'sbfs', 'uf' and 'dc' 
            correspond to Interleaved/Sequential BFS, Union-Find and
            Dynamic Connectivity,respectively

        Raises
        ------
        ValueError if the requirements for the simulation parameters are not fullfilled.
        KeyError if the impl identifier is not choosen among the 4 possibilities.
        """
        if self.__sim_i:
            syc.sy_destroy()
            if self.__arr_av and tslength != self.tslength:
                del self.__ts_u32, self.__ts_u64
                self.__arr_av = False

        self.q,self.l,self.beta,self.coupl,self.cutoff,\
        self.tslength,self.rngseed = float(q),int(l),float(beta),\
        float(coupl),int(cutoff),int(tslength),int(rngseed)
        try:
            assert self.q >0
            assert self.l > 2
            assert self.beta > 0
            assert self.coupl > 0
            assert self.cutoff > 0
            assert self.tslength > 0
        except:
            raise ValueError
        self.supp_impl = ('ibfs','sbfs','dc','uf')
        self.impl = impl.lower()
        self.impl_idx = self.supp_impl.index(self.impl)

        assert(self.impl in self.supp_impl)

        if not self.__arr_av:
            # num_bonds, num_cluster, size_giant
            self.__ts_u32 = np.empty((3,tslength),dtype=np.uint32)
            # sec_cs_moment and four_cs_moment
            self.__ts_u64 = np.empty((2,tslength),dtype=np.uint64)
            self.__arr_av = True
            
        assert syc.sy_setup(self.impl_idx,self.q,self.l,self.beta,self.coupl,
                self.cutoff,self.rngseed,self.__ts_u32[0],self.__ts_u32[1],self.__ts_u32[2]
                ,self.__ts_u64[0],self.__ts_u64[1])
        self.__sim_i=True

    def __init__(self,q,l,beta,coupl,cutoff,tslength,rngseed,impl='ibfs'):
        """
        Prepares for simulation with specified parameters.

        Parameters
        ----------
        q: float
            Cluster-weight, (>0).
        l: int
            Number of vertices per dimension, (>2)
        beta: float
            Inverse temperature, (>0)
        coupl: float
            Coupling constant, (>0)/Ferromagnetic
        cutoff: int
            Number of sweeps for equilibration, (>0)
        tslength: int
            Number of sweeps of actual simulation. This also corresponds
            to the number of observable samples, (>0)
        rngseed: int
            Seed for the pseudo-random-number generator, (>0)
        impl: str
            Type of implementation. 'ibfs', 'sbfs', 'uf' and 'dc' 
            correspond to Interleaved/Sequential BFS, Union-Find and
            Dynamic Connectivity,respectively

        Raises
        ------
        ValueError if the requirements for the simulation parameters are not fullfilled.
        KeyError if the impl identifier is not choosen among the 4 possibilities.
        """
        self.__ts_av = False
        self.__sim_i = False
        self.__arr_av = False
        self.init_sim(q,l,beta,coupl,cutoff,tslength,rngseed,impl)

    def simulate(self):
        """
        Start the prepared/configured simulation. Note if run multiple times
        the previous state is used as the "initial" configuration of the 
        simulation (markov process). 
        """
        if self.__sim_i:
            if syc.sy_simulate():
                self.__ts_av = True
    
    @property
    def ts_num_bonds(self):
        """
        Returns
        -------
        ts_num_bonds: ndarray
            Time-series of number of active bonds. If simulation has not been initialized yet then None.
        """
        return self.__ts_u32[0] if self.__arr_av else None
    
    @property
    def ts_num_cluster(self):
        """
        Returns
        -------
        ts_num_cluster: ndarray
            Time-series of number of clusters/components. If simulation has not been initialized yet then None.
        """
        return self.__ts_u32[1] if self.__arr_av else None
    
    @property
    def ts_size_giant(self):
        """
        Returns
        -------
        ts_size_giant: ndarray
            Time-series of the size of largest component. If simulation has not been initialized yet then None.
        """
        return self.__ts_u32[2] if self.__arr_av else None
    
    @property
    def ts_sec_cs_moment(self):
        """
        Returns
        -------
        ts_sec_cs_moment: ndarray
            Time-series of the second moment of the cluster size distribution. If simulation has not been initialized yet then None.
        """
        return self.__ts_u64[0] if self.__arr_av else None

    @property
    def ts_four_cs_moment(self):
        """
        Returns
        -------
        ts_four_cs_moment: ndarray
            Time-series of the fourth moment of the cluster size distribution. If simulation has not been initialized yet then None.
        """
        return self.__ts_u64[1] if self.__arr_av else None
    
    def __destroy__(self):
        if self.__arr_av:
            del self.__ts_u32,self.__ts_u64
        if self.__sim_i:
            syc.sy_destroy()
    def __write_attributes(self,dset):
        ds.attrs['q'] = self.q
        ds.attrs['l'] = self.l
        ds.attrs['beta'] = self.beta
        ds.attrs['coupl'] = self.coupl
        ds.attrs['cutoff'] = self.cutoff
        ds.attrs['tslength'] =self.tslength
        ds.attrs['rngseed'] = self.rngseed

    def export_to_hdf5(self,file_name):
        if self.__arr_av:
            f = h5py.File(file_name,"w")
            
            dset = corr_av_h5py['/'].create_dataset("num_bonds",
                self.tslength,dtype=self.__ts_u32.dtype,
                compression='gzip')
            self.__write_attributes(dset)
 
            dset[...] = self.__ts_u32[0]
            
            dset = corr_av_h5py['/'].create_dataset("num_cluster",
                self.tslength,dtype=self.__ts_u32.dtype,
                compression='gzip')
            dset[...] = self.__ts_u32[1]

            dset = corr_av_h5py['/'].create_dataset("size_giant",
                self.tslength,dtype=self.__ts_u32.dtype,
                compression='gzip')
            dset[...] = self.__ts_u32[2]
            dset = corr_av_h5py['/'].create_dataset("sec_cs_moment",
                self.tslength,dtype=self.__ts_u64.dtype,
                compression='gzip')
            dset[...] = self.__ts_u64[0]
            dset = corr_av_h5py['/'].create_dataset("four_cs_moment",
                self.tslength,dtype=self.__ts_u64.dtype,
                compression='gzip')
            dset[...] = self.__ts_u64[1]
            f.close()
