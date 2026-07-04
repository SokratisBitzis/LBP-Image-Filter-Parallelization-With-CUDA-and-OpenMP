#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <omp.h>
#include <string.h>

#define length 16384
#define width 16384

int main(){
	FILE *fp;//main file pointer
	FILE *rp;//registry pointer
	int i=0,j=0;
	unsigned char *Linput=malloc((length*width)*sizeof(unsigned char));
	if((fp=fopen("drone01-16384x16384.raw","r"))==NULL){//open input image or exit with 1 if problem appears
		printf("Error in opening the input image\n");
		exit(1);
	}
	for (i=0; i<length; i++) fread(&Linput[i*width], 1, width, fp);//fread and fwrite can only be used for images with width <= 4096 1byte pixels
    fclose(fp);//open input image end
    
    if((rp=fopen("drone01-16384x16384newregister.txt","a"))==NULL){//open register file or exit with 4 if problem appears
		printf("Error in opening the input image\n");
		exit(4);
	}
    
	unsigned char *Loutput=malloc((length*width)*sizeof(unsigned char));
	unsigned char T=0;//target brightness
	double tss=0,tse=0,tps=0,tpe=0;
	short bt=0,np=0;
	
	//serial ver.
	
	tss=omp_get_wtime();//serial starting time
	for(i=0;i<length;i++){//set Loutput
		for(j=0;j<width;j++){
			T=Linput[i*width+j];//our protagonist
			bt=0;//temporary lbp value
			if(j>0&&Linput[i*width+j-1]>T) bt+=128;
			if(i<(length-1)&&j>0&&Linput[(i+1)*width+j-1]>T) bt+=64;
			if(i<(length-1)&&Linput[(i+1)*width+j]>T) bt+=32;
			if(i<(length-1)&&j<(width-1)&&Linput[(i+1)*width+j+1]>T) bt+=16;
			if(j<(width-1)&&Linput[i*width+j+1]>T) bt+=8;
			if(i>0&&j<(width-1)&&Linput[(i-1)*width+j+1]>T) bt+=4;
			if(i>0&&Linput[(i-1)*width+j]>T) bt+=2;
			if(i>0&&j>0&&Linput[(i-1)*width+j-1]>T) bt+=1;
			Loutput[i*width+j]=bt;
		}
	}//end set Loutput
	tse=omp_get_wtime();//serial ending time
	if((fp=fopen("serialout.raw","w"))==NULL){//output LBP image or exit with 2 if problem appears
		printf("Error in creating the output image\n");
		exit(2);
	}
	for (i=0; i<length; i++) fwrite(&Loutput[i*width], 1, width, fp);
    fclose(fp);//output LBP image end
	
	printf("Serial time is %f\n",tse-tss);
	fprintf(rp,"\nSerial time is %f\n\n",tse-tss);
	
	//serial ver. end
	
	/*-------------------------------------------------------------------------------------------------------------*/
	
	//parallel ver.
	
	for(np=1;np<65;np*=2){
		tps=omp_get_wtime();//parallel starting time
		#pragma omp parallel num_threads(np) shared(Loutput) private(i,j,T,bt)
		{
			#pragma omp for //schedule(static)
			for(i=0;i<length;i++){//set Loutput
				for(j=0;j<width;j++){
					T=Linput[i*width+j];
					bt=0;
					if(j>0&&Linput[i*width+j-1]>T) bt+=128;
					if(i<(length-1)&&j>0&&Linput[(i+1)*width+j-1]>T) bt+=64;
					if(i<(length-1)&&Linput[(i+1)*width+j]>T) bt+=32;
					if(i<(length-1)&&j<(width-1)&&Linput[(i+1)*width+j+1]>T) bt+=16;
					if(j<(width-1)&&Linput[i*width+j+1]>T) bt+=8;
					if(i>0&&j<(width-1)&&Linput[(i-1)*width+j+1]>T) bt+=4;
					if(i>0&&Linput[(i-1)*width+j]>T) bt+=2;
					if(i>0&&j>0&&Linput[(i-1)*width+j-1]>T) bt+=1;
					Loutput[i*width+j]=bt;
				}
			}//set Loutput end
		}
		tpe=omp_get_wtime();//parallel ending time
		if(np==1){//produce new parallel images or exit with 3 if problem appears
			if((fp=fopen("parallelout1.raw","w"))==NULL){
				printf("Error in creating the output image\n");
				exit(3);
			}
		}
		else if(np==2){
			if((fp=fopen("parallelout2.raw","w"))==NULL){
				printf("Error in creating the output image\n");
				exit(3);
			}
		}
		else if(np==4){
			if((fp=fopen("parallelout4.raw","w"))==NULL){
				printf("Error in creating the output image\n");
				exit(3);
			}
		}
		else if(np==8){
			if((fp=fopen("parallelout8.raw","w"))==NULL){
				printf("Error in creating the output image\n");
				exit(3);
			}
		}
		else if(np==16){
			if((fp=fopen("parallelout16.raw","w"))==NULL){
				printf("Error in creating the output image\n");
				exit(3);
			}
		}
		else if(np==32){
			if((fp=fopen("parallelout32.raw","w"))==NULL){
				printf("Error in creating the output image\n");
				exit(3);
			}
		}
		else if(np==64){
			if((fp=fopen("parallelout64.raw","w"))==NULL){
				printf("Error in creating the output image\n");
				exit(3);
			}
		}
		for (i=0; i<length; i++) fwrite(&Loutput[i*width], 1, width, fp);
    	fclose(fp);//produce new parallel images end
				
		printf("\nSerial version time is %f\n",(tse-tss));
		printf("Parallel version time for %d threads is %f\n",np,(tpe-tps));
		fprintf(rp,"Parallel version time for %d threads is %f\n",np,(tpe-tps));
		printf("Speedup for %d threads is %f\n",np,(tse-tss)/(tpe-tps));
		fprintf(rp,"Speedup for %d threads is %f\n",np,(tse-tss)/(tpe-tps));
		printf("Efficiency for %d threads is %f\n",np,((tse-tss)/(tpe-tps))/np);
		fprintf(rp,"Efficiency for %d threads is %f\n\n",np,((tse-tss)/(tpe-tps))/np);
	}
	
	//end parallel ver.
	
	free(Linput);
	free(Loutput);
	
	fprintf(rp,"\n------------------------------------------------\n");
	fclose(rp);
	
	return 0;
}

