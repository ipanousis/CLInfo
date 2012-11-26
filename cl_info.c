#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>

void DisplayPlatformInfo(cl_platform_id id, cl_platform_info name, char* str){

  cl_int errNum;
  size_t paramValueSize;
    
  errNum = clGetPlatformInfo(id,name,0,NULL,&paramValueSize);

  if(errNum != CL_SUCCESS)
    printf("Failed to find OpenCL platform %s\n",str);

  char* info = (char*)alloca(sizeof(char)*paramValueSize);

  errNum = clGetPlatformInfo(id,name,paramValueSize,info,NULL);

  if(errNum != CL_SUCCESS)
    printf("Failed to find OpenCL platform %s\n",str);

  printf("Platform %s : %s\n",str,info);
}

void displayGeneralInfo(cl_device_id device){

  cl_uint compute_units;
  cl_device_info info[] = {CL_DEVICE_TYPE,CL_DEVICE_VENDOR_ID,CL_DEVICE_MAX_COMPUTE_UNITS,
                           CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,CL_DEVICE_MAX_WORK_ITEM_SIZES,
                           CL_DEVICE_MAX_WORK_GROUP_SIZE,CL_DEVICE_MAX_MEM_ALLOC_SIZE,
                           CL_DEVICE_GLOBAL_MEM_SIZE,CL_DEVICE_LOCAL_MEM_SIZE,CL_DEVICE_AVAILABLE
                          };

  cl_device_type infoUlong;
  cl_uint infoUint;

  clGetDeviceInfo(device, CL_DEVICE_TYPE, sizeof(cl_device_type), &infoUlong, NULL);

  printf("\n--------- CLInfo: GPU Device ID %d ---------\n",(int)device);

  char str[16384];

  clGetDeviceInfo(device, 
    CL_DEVICE_NAME, 
    sizeof(str), 
    &str, 
    NULL);

  printf("device name = %s\n",str);

  clGetDeviceInfo(device,
    CL_DEVICE_VERSION,
    sizeof(str),
    &str,
    NULL);

  printf("device version = %s\n",str);

  clGetDeviceInfo(device,
    CL_DEVICE_PROFILE,
    sizeof(str),
    &str,
    NULL);

  printf("device profile = %s\n",str);

//  clGetDeviceInfo(device,
//    CL_DEVICE_OPENCL_C_VERSION,
//    sizeof(str),
//   &str,
//   NULL);

// printf("device opencl cl version = %s\n",str);

  clGetDeviceInfo(device,
    CL_DEVICE_MAX_COMPUTE_UNITS,
    sizeof(cl_uint),
    &compute_units,
    NULL);
    
  printf("compute_units = %d\n",compute_units);

  clGetDeviceInfo(device,
    CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS,
    sizeof(cl_uint),
    &infoUint,
    NULL);
    
  printf("max work item dimensions = %d\n",infoUint);

  size_t wiSizes[infoUint];

  clGetDeviceInfo(device,
    CL_DEVICE_MAX_WORK_ITEM_SIZES,
    sizeof(size_t) * infoUint,
    wiSizes,
    NULL);
    
  int i;
  for(i = 0; i < infoUint; i++)
    printf("max work item sizes (%d) = %d\n",i,wiSizes[i]);

  size_t wgSize;

  clGetDeviceInfo(device,
    CL_DEVICE_MAX_WORK_GROUP_SIZE,
    sizeof(size_t),
    &wgSize,
    NULL);
  
  printf("max work group size = %d\n",wgSize);

  clGetDeviceInfo(device,
    CL_DEVICE_GLOBAL_MEM_SIZE,
    sizeof(cl_ulong),
    &infoUlong,
    NULL);
    
  printf("global mem size = %ld\n",infoUlong);

  clGetDeviceInfo(device,
    CL_DEVICE_LOCAL_MEM_SIZE,
    sizeof(cl_ulong),
    &infoUlong,
    NULL);
    
  printf("local mem size = %ld\n",infoUlong);

  clGetDeviceInfo(device,
    CL_DEVICE_IMAGE2D_MAX_WIDTH,
    sizeof(size_t),
    &wgSize,
    NULL);

  printf("max image2d width = %d\n",wgSize);

  clGetDeviceInfo(device,
    CL_DEVICE_IMAGE2D_MAX_HEIGHT,
    sizeof(size_t),
    &wgSize,
    NULL);

  printf("max image2d height = %d\n",wgSize);

  cl_bool supported;

  clGetDeviceInfo(device,
    CL_DEVICE_IMAGE_SUPPORT,
    sizeof(cl_bool),
    &supported,
    NULL);

  printf("image support = %d\n",supported);

  printf("\n-------------------\n");
}

void displayImageInfo(cl_context context){

  /*
    cl_int clGetSupportedImageFormats (cl_context context,
    cl_mem_flags flags,
    cl_mem_object_type image_type,
    cl_uint num_entries,
    cl_image_format *image_formats,
    cl_uint *num_image_formats)
  */

  cl_mem_flags flagSets[3] = {CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,CL_MEM_WRITE_ONLY | CL_MEM_COPY_HOST_PTR,CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR};
  cl_mem_object_type imageTypes[2] = {CL_MEM_OBJECT_IMAGE2D,CL_MEM_OBJECT_IMAGE3D};

  cl_channel_order requiredOrders[1] = {CL_RGBA};
  //cl_channel_type  requiredTypes[0]  = {};

  int flagIndex, typeIndex;
  for(typeIndex = 0; typeIndex < 2; typeIndex++){

    for(flagIndex = 0; flagIndex < 3; flagIndex++){

      cl_image_format * image_formats = (cl_image_format*)malloc(sizeof(cl_image_format) * 64);
      cl_uint num_image_formats;

      cl_int errNum = clGetSupportedImageFormats(context, flagSets[flagIndex],
                                                 imageTypes[typeIndex], 64, 
                                                 image_formats, &num_image_formats);

      if(errNum){
        printf("Error getting supported image formats for type/flag %d/%d (%d)\n",typeIndex,flagIndex,errNum);
        continue;
      }

      printf("Type/Flag %d/%d - %d image formats total\n",typeIndex,flagIndex,num_image_formats);
      int i;
      for(i = 0; i < num_image_formats; i++){

        int j;
        for(j = 0; j < 1; j++){
          if(image_formats[i].image_channel_order == requiredOrders[j])
            printf("Format %d ; ChannelOrder = %d, DataType = %d\n", i, 
                   image_formats[i].image_channel_order,image_formats[i].image_channel_data_type);
        }

      }

    }

  }

}

void displayInfo(void){

  cl_int errNum;
  cl_uint numPlatforms;
  cl_platform_id * platformIds;
  cl_device_id device;
  cl_context context = NULL;

  errNum = clGetPlatformIDs(0,NULL,&numPlatforms);

  if(errNum != CL_SUCCESS || numPlatforms <= 0){
    printf("Failed to find any OpenCL platform\n");
    return;
  }

  platformIds = (cl_platform_id*)alloca(sizeof(cl_platform_id)*numPlatforms);

  errNum = clGetPlatformIDs(numPlatforms,platformIds,NULL);

  if(errNum != CL_SUCCESS){
    printf("Failed to find any OpenCL platform\n");
    return;
  }

  printf("Number of platforms:\t%d\n",numPlatforms);

  cl_uint i;
  for(i = 0; i< numPlatforms; i++){

    // Platform info
    DisplayPlatformInfo(platformIds[i],CL_PLATFORM_PROFILE,"CL_PLATFORM_PROFILE");
    DisplayPlatformInfo(platformIds[i],CL_PLATFORM_VERSION,"CL_PLATFORM_VERSION");
    DisplayPlatformInfo(platformIds[i],CL_PLATFORM_VENDOR,"CL_PLATFORM_VENDOR");
    DisplayPlatformInfo(platformIds[i],CL_PLATFORM_EXTENSIONS,"CL_PLATFORM_EXTENSIONS");

    cl_uint num_entries = 4;
    cl_device_id * devices = (cl_device_id*)malloc(sizeof(cl_device_id) * num_entries);
    cl_uint num_devices;

    // GPU device info
    errNum = clGetDeviceIDs(platformIds[i], CL_DEVICE_TYPE_GPU, num_entries, devices, &num_devices);
    
    if(errNum || num_devices == 0){
      printf("Failed to find GPU device on platform %d\n",i);
      continue;
    }
    
    printf("\nCLInfo detected %d GPU device(s)\n",num_devices);

    int j;
    for(j = 0; j < num_devices; j++){
       displayGeneralInfo(devices[j]);
    }

    //context = clCreateContext(NULL,1,&device,NULL,NULL,NULL);
    //displayImageInfo(context);
  }

}

int main(int argc, char** argv){  

  displayInfo();

  return 0;
}
