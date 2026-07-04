#include <stdio.h>
#include <stdlib.h>
#include <cuda.h>

#define h 4096//image height
#define w 4096//image widht
#define size (h*w)//image size

__global__ void CalcLBP(unsigned char *Inimg,unsigned char *Outimg){
	int id=threadIdx.x+blockIdx.x*blockDim.x;
	if(id<size){
		unsigned char T=Inimg[id];//targeted pixed
		unsigned char bt=0;//temporary lbp value
		if( (id%w)>0  &&  Inimg[id-1]>T ) bt+=128;
		if( (id%w)>0  &&  id<(w*(h-1))  &&  Inimg[id+w-1]>T ) bt+=64;
		if( id<(w*(h-1))  &&  Inimg[id+w]>T ) bt+=32;
		if( (id%w)<(w-1)  &&  id<(w*(h-1))  &&  Inimg[id+w+1]>T ) bt+=16;
		if( (id%w)<(w-1)  &&  Inimg[id+1]>T ) bt+=8;
		if( (id%w)<(w-1)  &&  id>=w  &&  Inimg[id-w+1]>T ) bt+=4;
		if( id>=w  &&  Inimg[id-w]>T ) bt+=2;
		if( (id%w)>0  &&  id>=w  &&  Inimg[id-w-1]>T ) bt+=1;
		Outimg[id]=bt;
	}
}

int main(){
	FILE *fp;//main file pointer
	FILE *rp;//registry pointer
	int i=0;
	unsigned char *HostInimg=(unsigned char*)malloc(size);//host input image array
	unsigned char *HostOutimg=(unsigned char*)malloc(size);//host output image array
	unsigned char *DevInimg;//device input image array
	unsigned char *DevOutimg;//device output image array
	cudaMalloc((void**)&DevInimg, size);//allocate space on device for input image
	cudaMalloc((void**)&DevOutimg, size);//allocate space on device for output image
	
	if((fp=fopen("drone01-4096x4096.raw","r"))==NULL){//open input image or exit with 1 if problem appears
		printf("Error in opening the input image\n");
		exit(1);
	}
	for (i=0; i<h; i++) fread(&HostInimg[i*w], 1, w, fp);//fread and fwrite can only be used for images with width <= 4096 1byte pixels
    fclose(fp);//open input image end
    cudaMemcpy(DevInimg, HostInimg, size, cudaMemcpyHostToDevice);//copy host input image to device

    if((rp=fopen("4096x4096cudaregister.txt","a"))==NULL){//open register file or exit with 3 if problem appears
		printf("Error in opening the input image\n");
		exit(3);
	}
    
	cudaEvent_t  start,stop;//time pins
    cudaEventCreate(&start);//create starting pin
    cudaEventCreate(&stop);//create ending pin
    cudaEventRecord(start,0);//find starting time
    
	int blocks=size/1024+(size%1024==0?0:1);
	CalcLBP<<<blocks,1024>>>(DevInimg,DevOutimg);
	
	cudaError_t err = cudaGetLastError();//get error code
	if ( err != cudaSuccess ){//check if any errors occured
		printf("CUDA Error: %s\n", cudaGetErrorString(err));
		exit(-1);
	}
	
	cudaEventRecord(stop,0);//find ending time
    cudaEventSynchronize(stop);//no idea what this does
    float elapsedTime=0;//create elapsed time variable
    cudaEventElapsedTime(&elapsedTime,start,stop);//calculate elapsed time
    printf("Time for %dx%d image LBP calculation:  %3.3f ms\n",w,h,elapsedTime);//print time elapsed between start and finish of LBP calculations
	fprintf(rp,"Time for %dx%d image LBP calculation:  %3.3f ms\n",w,h,elapsedTime);
	
	cudaMemcpy(HostOutimg, DevOutimg, size, cudaMemcpyDeviceToHost);//copy device output to host
	cudaFree(DevInimg);//free device input image array
    cudaFree(DevOutimg);//free device output image array
	free(HostInimg);//free host input image array
	
	if((fp=fopen("cudaout.raw","w"))==NULL){//output LBP image or exit with 2 if problem appears
		printf("Error in creating the output image\n");
		exit(2);
	}
	for (i=0; i<h; i++) fwrite(&HostOutimg[i*w], 1, w, fp);
    fclose(fp);//output LBP image end
	free(HostOutimg);//free host output image array
	
	fprintf(rp,"\n------------------------------------------------\n");
	fclose(rp);
	
	return 0;
}
