#include "source_fix.h"
#include <math.h>
#include <stdlib.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

int cconv_main();
int dconv_main();
int bn_main();
int bilinear_main();

void maxpool(hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O,
		int row,int col,int num,int N,int custom_k,int custom_Tr,int custom_Tc,
		int custom_Tm,int custom_Tn,int NL_Opt);

void do_maxpool(int M,int N,int R,int C,int outR,int outC,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * output,
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
	extern FPGA_DATA_FIX input[N*R*C];
	extern FPGA_DATA_FIX output[N*(R/2)*(C/2)];
	FPGA_DATA_FIX stage1_pool_output[N*(R/2)*(C/2)];
	int real_R = R/2;
	int real_C = C/2;


	do_maxpool(N,N,R,C,R/2,C/2,input,stage1_pool_output,input_dma_W,input_dma_I,input_dma_B,output_dma_O);

	printf("Difference\n");
	for(int m=0;m<N;m++){
		for(int r=0;r<real_R;r++){
			for(int c=0;c<real_C;c++){
				FPGA_DATA_FIX res;
				FPGA_DATA_FIX g_res;
				res = stage1_pool_output[m*(real_R)*(real_C) + r*(real_C) + c];
				g_res = output[m*(real_R)*(real_C) + (r)*(real_C) + (c)];
				std::cout << res-g_res <<" ";
			}
			printf("\n");
		}
		printf("========%d==========\n",m);
		printf("\n");
	}

	return 0;
}
