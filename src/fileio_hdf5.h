#ifndef FILE_IO_HDF5
#define FILE_IO_HDF5
#undef H5_HAVE_PARALLEL // avoid MPI
#include <hdf5.h>
typedef struct hdf5_ext_info {
    hsize_t cur_dim;
    hsize_t cur_offset;
    hsize_t chunk_len;
    hid_t file_id;
    hid_t dataspace,dataset,filespace,memspace;
    char *dsetName;
    herr_t status;
    
} hdf5_ext_info;

hid_t openFile_w(const char *filename);
hid_t openFile_r(const char *filename);

herr_t closeFile(hid_t file_id);
herr_t writeInt1d(hid_t file_id,const char *dsName, void * array, int length);
herr_t write_u32_1d(hid_t file_id,const char *dsName, void * array, int length);
herr_t write_u64_1d(hid_t file_id,const char *dsName, void * array, int length);


herr_t writeInt2d(hid_t file_id,const char *dsName, void * matrix, int DIM_X, int DIM_Y);

herr_t writeFloat2d(hid_t file_id,const char *dsName, void * matrix, int DIM_X, int DIM_Y) ;

herr_t writeFloat1d(hid_t file_id,const char *dsName, void * array, int length);

herr_t writeDouble2d(hid_t file_id,const char *dsName, void * matrix, int DIM_X, int DIM_Y);

herr_t writeDouble1d(hid_t file_id,const char *dsName, void * array, int length) ;

hdf5_ext_info * open1dDoubleExt(const char *filename, hid_t file_id ,hsize_t chunkL);
herr_t writeAttribute_double(hid_t file_id, const char *dsName, const char *attrName,void  *val);
herr_t writeAttribute_float(hid_t file_id, const char *dsName, const char *attrName,void  *val);
herr_t writeAttribute_int(hid_t file_id, const char *dsName, const char *attrName,void  *val);
herr_t writeAttribute_u32(hid_t file_id, const char *dsName, const char *attrName,void *val);
herr_t writeAttribute_char(hid_t file_id, const char *dsName, const char *attrName,void *val);

void close1dDoubleExt(hdf5_ext_info *hi);


void extend1dDoubleExt(hdf5_ext_info *hi, void *array, hsize_t len);

herr_t writeNext2dCharTimeFrame(void *tframe,hid_t dataset_id,int nx, int ny, int ntimes);
hid_t init2dCharTimeFrames(int nx, int ny, int ntimes, char *dsName, hid_t file_id);
herr_t close2dCharTimeFrames(hid_t dataset_id);

herr_t writeU81d(hid_t file_id,const char *dsName, void * array, int length);

herr_t readNext2dCharTimeFrame(char *tframe,hid_t dataset_id,int nx, int ny, int ntimes);
hid_t init2dCharTimeFrames_r(char *dsName, hid_t file_id);
herr_t close2dCharTimeFrames_r(hid_t dataset_id);
#endif
