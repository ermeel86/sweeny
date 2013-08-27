#include "fileio_hdf5.h"
#include <stdlib.h>
#include <string.h>
hid_t openFile_w(const char *filename) {
    
    return H5Fcreate(filename,H5F_ACC_TRUNC,H5P_DEFAULT,H5P_DEFAULT);

}

herr_t closeFile(hid_t file_id) {
    return H5Fclose(file_id);
}

hid_t openFile_r(const char *filename) {
    
    return H5Fopen(filename, H5F_ACC_RDONLY , H5P_DEFAULT);

}
herr_t writeInt1d(hid_t file_id,const char *dsName, void * array, int length) {
    hid_t  dataset_id,dataspace_id;
    hsize_t     dim = length;
    herr_t status;
    /* Create the data space for the dataset. */

    dataspace_id = H5Screate_simple(1, &dim, NULL);
    /* Create the dataset. */
    dataset_id = H5Dcreate(file_id, dsName, H5T_NATIVE_INT, dataspace_id, 
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    status = H5Sclose(dataspace_id);
    /*Write the dataset*/
    status = H5Dwrite(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                     array);
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    return status;
}

herr_t write_u32_1d(hid_t file_id,const char *dsName, void * array, int length) {
    hid_t  dataset_id,dataspace_id;
    hsize_t     dim = length;
    herr_t status;
    /* Create the data space for the dataset. */

    dataspace_id = H5Screate_simple(1, &dim, NULL);
    /* Create the dataset. */
    dataset_id = H5Dcreate(file_id, dsName, H5T_STD_U32LE, dataspace_id, 
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    status = H5Sclose(dataspace_id);
    /*Write the dataset*/
    status = H5Dwrite(dataset_id, H5T_STD_U32LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                     array);
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    return status;
}

herr_t write_u64_1d(hid_t file_id,const char *dsName, void * array, int length) {
    hid_t  dataset_id,dataspace_id;
    hsize_t     dim = length;
    herr_t status;
    /* Create the data space for the dataset. */

    dataspace_id = H5Screate_simple(1, &dim, NULL);
    /* Create the dataset. */
    dataset_id = H5Dcreate(file_id, dsName, H5T_STD_U64LE, dataspace_id, 
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    status = H5Sclose(dataspace_id);
    /*Write the dataset*/
    status = H5Dwrite(dataset_id, H5T_STD_U64LE, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                     array);
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    return status;
}

herr_t writeAttribute_double(hid_t file_id, const char *dsName, const char *attrName,void  *val) {
   hid_t dataset = H5Dopen(file_id,dsName,H5P_DEFAULT);
   hid_t aid = H5Screate(H5S_SCALAR);
   hid_t attr = H5Acreate(dataset,attrName, H5T_NATIVE_DOUBLE, aid, H5P_DEFAULT,H5P_DEFAULT);
   herr_t ret = H5Awrite(attr,H5T_NATIVE_DOUBLE,val);
   ret = H5Sclose(aid);
   ret = H5Aclose(attr);
   return ret;
}
herr_t writeAttribute_float(hid_t file_id, const char *dsName, const char *attrName,void  *val) {
   hid_t dataset = H5Dopen(file_id,dsName,H5P_DEFAULT);
   hid_t aid = H5Screate(H5S_SCALAR);
   hid_t attr = H5Acreate(dataset,attrName, H5T_NATIVE_FLOAT, aid, H5P_DEFAULT,H5P_DEFAULT);
   herr_t ret = H5Awrite(attr,H5T_NATIVE_FLOAT,val);
   ret = H5Sclose(aid);
   ret = H5Aclose(attr);
   return ret;
}
herr_t writeAttribute_int(hid_t file_id, const char *dsName, const char *attrName,void  *val) {
   hid_t dataset = H5Dopen(file_id,dsName,H5P_DEFAULT);
   hid_t aid = H5Screate(H5S_SCALAR);
   hid_t attr = H5Acreate(dataset,attrName, H5T_NATIVE_INT, aid, H5P_DEFAULT,H5P_DEFAULT);
   herr_t ret = H5Awrite(attr,H5T_NATIVE_INT,val);
   ret = H5Sclose(aid);
   ret = H5Aclose(attr);
   return ret;
}
herr_t writeAttribute_char(hid_t file_id, const char *dsName, const char *attrName,void  *val) {
   hid_t dataset = H5Dopen(file_id,dsName,H5P_DEFAULT);
   hid_t aid = H5Screate(H5S_SCALAR);
   hid_t attr = H5Acreate(dataset,attrName, H5T_STD_I8LE, aid, H5P_DEFAULT,H5P_DEFAULT);
   herr_t ret = H5Awrite(attr,H5T_STD_I8LE,val);
   ret = H5Sclose(aid);
   ret = H5Aclose(attr);
   return ret;
}
herr_t writeAttribute_u32(hid_t file_id, const char *dsName, const char *attrName,void  *val) {
   hid_t dataset = H5Dopen(file_id,dsName,H5P_DEFAULT);
   hid_t aid = H5Screate(H5S_SCALAR);
   hid_t attr = H5Acreate(dataset,attrName, H5T_STD_U32LE, aid, H5P_DEFAULT,H5P_DEFAULT);
   herr_t ret = H5Awrite(attr,H5T_STD_U32LE,val);
   ret = H5Sclose(aid);
   ret = H5Aclose(attr);
   return ret;
}

herr_t writeInt2d(hid_t file_id,const char *dsName, void * matrix, int DIM_X, int DIM_Y) {
    hid_t  dataset_id,dataspace_id;
    hsize_t     dims[] = {DIM_Y,DIM_X}; /*DIM_Y corresponds to number of rows, DIM_X to columns*/
    herr_t status;
    /* Create the data space for the dataset. */

    dataspace_id = H5Screate_simple(2, dims, NULL);
    /* Create the dataset. */
    if(sizeof(int) == 4)
        dataset_id = H5Dcreate(file_id, dsName, H5T_STD_I32LE, dataspace_id, 
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    else
        dataset_id = H5Dcreate(file_id, dsName, H5T_STD_I64LE, dataspace_id, 
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
    status = H5Sclose(dataspace_id);
    /*Write the dataset*/
    status = H5Dwrite(dataset_id, H5T_NATIVE_INT, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                     matrix);
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    return status;    
}


herr_t writeFloat2d(hid_t file_id,const char *dsName, void * matrix, int DIM_X, int DIM_Y) {
    hid_t  dataset_id,dataspace_id;
    hsize_t     dims[] = {DIM_Y,DIM_X}; /*DIM_Y corresponds to number of rows, DIM_X to columns*/
    herr_t status;
    /* Create the data space for the dataset. */

    dataspace_id = H5Screate_simple(2, dims, NULL);
    /* Create the dataset. */

    dataset_id = H5Dcreate(file_id, dsName, H5T_IEEE_F32LE, dataspace_id, 
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    status = H5Sclose(dataspace_id);
    /*Write the dataset*/
    status = H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                     matrix);
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    return status;    
}

herr_t writeFloat1d(hid_t file_id,const char *dsName, void * array, int length) {
    hid_t  dataset_id,dataspace_id;
    hsize_t     dim = length;
    herr_t status;
    /* Create the data space for the dataset. */

    dataspace_id = H5Screate_simple(1, &dim, NULL);
    /* Create the dataset. */
    dataset_id = H5Dcreate(file_id, dsName, H5T_IEEE_F32LE, dataspace_id, 
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    status = H5Sclose(dataspace_id);
    /*Write the dataset*/
    status = H5Dwrite(dataset_id, H5T_NATIVE_FLOAT, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                     array);
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    return status;    
}

herr_t writeDouble2d(hid_t file_id,const char *dsName, void * matrix, int DIM_X, int DIM_Y) {
    hid_t  dataset_id,dataspace_id;
    hsize_t     dims[] = {DIM_Y,DIM_X}; /*DIM_Y corresponds to number of rows, DIM_X to columns*/
    herr_t status;
    /* Create the data space for the dataset. */

    dataspace_id = H5Screate_simple(2, dims, NULL);
    /* Create the dataset. */

    dataset_id = H5Dcreate(file_id, dsName,  H5T_IEEE_F64LE, dataspace_id, 
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    status = H5Sclose(dataspace_id);
    /*Write the dataset*/
    status = H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                     matrix);
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    return status;    
}

herr_t writeDouble1d(hid_t file_id,const char *dsName, void * array, int length) {
    hid_t  dataset_id,dataspace_id;
    hsize_t     dim = length;
    herr_t status;
    /* Create the data space for the dataset. */

    dataspace_id = H5Screate_simple(1, &dim, NULL);
    /* Create the dataset. */
    dataset_id = H5Dcreate(file_id, dsName,  H5T_IEEE_F64LE, dataspace_id, 
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    status = H5Sclose(dataspace_id);
    /*Write the dataset*/
    status = H5Dwrite(dataset_id, H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                     array);
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    return status;    
}



hdf5_ext_info * open1dDoubleExt(const char *filename, hid_t file_id ,hsize_t chunkL) {
    hdf5_ext_info *nh = malloc(sizeof(*nh));
    nh->dsetName = malloc(sizeof(char)*(strlen(filename)+1));
    nh->dsetName = strcpy(nh->dsetName,filename);
    nh->cur_offset = nh->cur_dim = 0;
    nh->chunk_len = chunkL;
    nh->file_id = file_id;
    
    
    
    return nh;
    
    
}



void close1dDoubleExt(hdf5_ext_info *hi) {
    if(!hi)
        return;
    free(hi->dsetName);
    if(hi->cur_dim) H5Dclose (hi->dataset);
    free(hi);
}


void extend1dDoubleExt(hdf5_ext_info *hi, void *array, hsize_t len) {
    if(!hi || !array || len <=0)
        return;
    if(hi->cur_dim == 0) {
        hi->cur_dim = len;
        /* Create the data space with unlimited dimensions. */
        hsize_t      maxdim = H5S_UNLIMITED;
        hi->dataspace = H5Screate_simple (1, &(hi->cur_dim),&maxdim); 


        /* Modify dataset creation properties, i.e. enable chunking  */
        hid_t prop = H5Pcreate (H5P_DATASET_CREATE);
        hi->status = H5Pset_chunk (prop, 1, &(hi->chunk_len));

        /* Create a new dataset within the file using chunk 
        creation properties.  */
        hi->dataset = H5Dcreate (hi->file_id, hi->dsetName,   H5T_NATIVE_DOUBLE, hi->dataspace,
                            H5P_DEFAULT, prop, H5P_DEFAULT);


        /* Write data to dataset */
        hi->status = H5Dwrite (hi->dataset,   H5T_NATIVE_DOUBLE, H5S_ALL, H5S_ALL,
                        H5P_DEFAULT, array);
        hi->status = H5Pclose (prop);
        hi->status = H5Sclose (hi->dataspace);

    }
    else {
       /* Extend the dataset  */
        hi->cur_offset = hi->cur_dim ;
        hi->cur_dim += len;
        hi->status = H5Dextend (hi->dataset, &(hi->cur_dim));

        /* Select a hyperslab in extened portion of dataset  */
        hi->filespace = H5Dget_space (hi->dataset);
        hi->status = H5Sselect_hyperslab (hi->filespace, H5S_SELECT_SET, &(hi->cur_offset), NULL,
                                  &len, NULL);  

        /* Define memory space */
        hi->memspace = H5Screate_simple (1, &len, NULL); 

        /* Write the data to the extended portion of dataset  */
        hi->status = H5Dwrite (hi->dataset,   H5T_NATIVE_DOUBLE, hi->memspace, hi->filespace,
                       H5P_DEFAULT, array);    
        hi->status = H5Sclose (hi->memspace);
        hi->status = H5Sclose (hi->filespace);
        
    }
    
    
    
    
}
hid_t init2dCharTimeFrames(int nx, int ny, int ntimes, char *dsName, hid_t file_id) {
    hid_t  dataset_id,dataspace_id;
    hsize_t     dims[] = {ntimes,ny,nx}; 
    /* Create the data space for the dataset. */

    dataspace_id = H5Screate_simple(3, dims, NULL);
    /* Create the dataset. */

    dataset_id = H5Dcreate(file_id, dsName,  H5T_NATIVE_CHAR , dataspace_id, 
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

     H5Sclose(dataspace_id);

    return dataset_id; 

}

herr_t writeNext2dCharTimeFrame(void *tframe,hid_t dataset_id,int nx, int ny, int ntimes) {
  static int cur_frame_t = 0; // current frame
  static hsize_t start[] = {0,0,0}; // offset
  static hsize_t count[3]; // array to hold number of elements to be written 
  static hid_t mem_dataspace_id; // dataspace for memory 
  static hsize_t mem_start[] = {0,0}; // offset in memory
  static hsize_t mem_count[2]; // dimensions of memory
  static herr_t ret;
  static hsize_t dataspace_id;
  if(cur_frame_t == ntimes)
    return -1;
  
  if(!cur_frame_t) {
    dataspace_id =  H5Dget_space(dataset_id); // get the dataspace of the given dataset
    count[0] = 1;
    count[1] = ny;
    count[2] = nx;  // specify number of elements to be selected
    mem_count[0] = ny;
    mem_count[1] = nx; 
    mem_dataspace_id = H5Screate_simple(2, mem_count, NULL); // create dataspace for memory
  }
  else
    start[0] += 1;

  ret = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start, NULL, count, NULL);
  ret = H5Sselect_hyperslab(mem_dataspace_id, H5S_SELECT_SET, mem_start, NULL, mem_count, NULL);  
  ret = H5Dwrite(dataset_id, H5T_NATIVE_CHAR , mem_dataspace_id, dataspace_id, H5P_DEFAULT, tframe);
  ret = H5Sselect_none(dataspace_id); // forget selection for dataset
  ++cur_frame_t;

  if(cur_frame_t == ntimes) { // final frame? close dataspaces
        ret = H5Sclose(dataspace_id); 
	ret = H5Sclose(mem_dataspace_id);
  }
  return ret;
}

herr_t close2dCharTimeFrames(hid_t dataset_id){ 
 return H5Dclose(dataset_id); 
}


herr_t writeU81d(hid_t file_id,const char *dsName, void * array, int length) {
    hid_t  dataset_id,dataspace_id;
    hsize_t     dim = length;
    herr_t status;
    /* Create the data space for the dataset. */

    dataspace_id = H5Screate_simple(1, &dim, NULL);
    /* Create the dataset. */
    dataset_id = H5Dcreate(file_id, dsName, H5T_NATIVE_UCHAR, dataspace_id, 
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);

    status = H5Sclose(dataspace_id);
    /*Write the dataset*/
    status = H5Dwrite(dataset_id, H5T_NATIVE_UCHAR, H5S_ALL, H5S_ALL, H5P_DEFAULT, 
                     array);
    /* End access to the dataset and release resources used by it. */
    status = H5Dclose(dataset_id);
    return status;
}

herr_t readNext2dCharTimeFrame(char *tframe,hid_t dataset_id,int nx, int ny, int ntimes) {
  static int cur_frame_t = 0; // current frame
  static hsize_t start[] = {0,0,0}; // offset
  static hsize_t count[3]; // array to hold number of elements to be written 
  static hid_t mem_dataspace_id; // dataspace for memory 
  static hsize_t mem_start[] = {0,0}; // offset in memory
  static hsize_t mem_count[2]; // dimensions of memory
  static herr_t ret;
  static hsize_t dataspace_id;
  if(cur_frame_t == ntimes)
    return -1;
  
  if(!cur_frame_t) {
    dataspace_id =  H5Dget_space(dataset_id); // get the dataspace of the given dataset
    count[0] = 1;
    count[1] = ny;
    count[2] = nx;  // specify number of elements to be selected
    mem_count[0] = ny;
    mem_count[1] = nx; 
    mem_dataspace_id = H5Screate_simple(2, mem_count, NULL); // create dataspace for memory
  }
  else
    start[0] += 1;

  ret = H5Sselect_hyperslab(dataspace_id, H5S_SELECT_SET, start, NULL, count, NULL);
  ret = H5Sselect_hyperslab(mem_dataspace_id, H5S_SELECT_SET, mem_start, NULL, mem_count, NULL);  
  ret = H5Dread(dataset_id, H5T_NATIVE_CHAR , mem_dataspace_id, dataspace_id, H5P_DEFAULT, tframe);
  ret = H5Sselect_none(dataspace_id); // forget selection for dataset
  ++cur_frame_t;

  if(cur_frame_t == ntimes) { // final frame? close dataspaces
        ret = H5Sclose(dataspace_id); 
	ret = H5Sclose(mem_dataspace_id);
  }
  return ret;
}

hid_t init2dCharTimeFrames_r(char *dsName, hid_t file_id) {
    return H5Dopen(file_id,dsName,H5P_DEFAULT);
}

herr_t close2dCharTimeFrames_r(hid_t dataset_id){ 
 return H5Dclose(dataset_id); 
}


