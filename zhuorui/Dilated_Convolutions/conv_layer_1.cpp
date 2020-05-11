#include "source_fix.h"
#include <math.h>
#include <stdlib.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int cconv_main();
int dconv_main();
int bn_main();
int bilinear_main();

int do_conv(int M,int N,int R,int C, int K1,int dilation,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * weight,FPGA_DATA_FIX * output, FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		);

int main(){
	hls::stream<DMA_DATA_128B_FIX> input_dma_W("input_dma_W");
	hls::stream<DMA_DATA_128B_FIX> input_dma_I("input_dma_I");
	hls::stream<DMA_DATA_128B_FIX> input_dma_B("input_dma_B");
	hls::stream<DMA_DATA_128B_FIX> output_dma_O("output_dma_O");

	// Initialization
	const int K1= K+(K-1)*(r-1);
	extern FPGA_DATA_FIX input[N*R*C];
	extern FPGA_DATA_FIX weight[M*N*K*K];
	extern FPGA_DATA_FIX bias[M];
	extern FPGA_DATA_FIX output[M*(R-K1+1)*(C-K1+1)];
	FPGA_DATA_FIX my_output[M*(R-K1+1)*(C-K1+1)];


//	FPGA_DATA_FIX * input = (FPGA_DATA_FIX *) malloc(N*R*C*sizeof(FPGA_DATA_FIX));
//	FPGA_DATA_FIX * weight = (FPGA_DATA_FIX *) malloc(M*N*K*K*sizeof(FPGA_DATA_FIX));
//	FPGA_DATA_FIX * bias= (FPGA_DATA_FIX *) malloc(M*sizeof(FPGA_DATA_FIX));
//	FPGA_DATA_FIX * output = (FPGA_DATA_FIX *) malloc(M*R*C*sizeof(FPGA_DATA_FIX));


	// Do Conv.
	do_conv(M,N,R,C,K1,r,input,weight,my_output,bias,input_dma_W,input_dma_I,input_dma_B,output_dma_O);

	int OR=R-K1+1,OC=C-K1+1;
/*	for(int m=0;m<M;m++){
		for(int r=0;r<R-K1+1;r++){
			for(int c=0;c<C-K1+1;c++){
				printf("%d ", my_output[m*OR*OC + r*OC + c]);
			}
			printf("\n");
		}
		printf("========%d==========\n",m);
		printf("\n");
	}*/

	printf("Difference\n");

	for(int m=0;m<M;m++){
		for(int r=0;r<R-K1+1;r++){
			for(int c=0;c<C-K1+1;c++){
				printf("%d ", my_output[m*OR*OC + r*OC + c]-output[m*OR*OC+r*OC+c]);
			}
			printf("\n");
		}
		printf("===================\n");
		printf("\n");
	}

	return 0;
}
