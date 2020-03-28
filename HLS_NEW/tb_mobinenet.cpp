#include "source_fix.h"
#include <math.h>
#include <stdlib.h>


int do_conv(int M,int N,int R,int C,int c_k,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * weight,FPGA_DATA_FIX * output, FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		);

int do_conv_pad(int M,int N,int R,int C,int outR,int outC,int pad,int c_k,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * weight,FPGA_DATA_FIX * output, FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		);

int do_bn(int M,int N,int R,int C,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * E,FPGA_DATA_FIX * V,FPGA_DATA_FIX *BN_output,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_E,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_V,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		);

int do_dconv(int M,int N,int R,int C,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * weight,FPGA_DATA_FIX * output, FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		);

int do_dconv_pad(int M,int N,int R,int C,int pad,
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

	hls::stream<DMA_DATA_128B_FIX> input_bn_I("input_bn_I");
	hls::stream<DMA_DATA_128B_FIX> running_mean("input_E");
	hls::stream<DMA_DATA_128B_FIX> running_var("input_V");

	// Initialization
	extern FPGA_DATA_FIX input[CH1*(R+2)*(C+2)];
	extern FPGA_DATA_FIX weight_1[CH1*CH2*3*3];
	extern FPGA_DATA_FIX bias_1[CH2];
	extern FPGA_DATA_FIX weight_2[EXP2*CH2*1*1];
	extern FPGA_DATA_FIX bias_2[EXP2];
	extern FPGA_DATA_FIX weight_3[EXP2*1*3*3];
	extern FPGA_DATA_FIX bias_3[EXP2];
	extern FPGA_DATA_FIX weight_4[EXP2*CH3*1*1];
	extern FPGA_DATA_FIX bias_4[CH3];
	extern FPGA_DATA_FIX x_1[CH2*R*C];
	extern FPGA_DATA_FIX x_2[EXP2*(R+2)*(C+2)];
	extern FPGA_DATA_FIX x_3[CH2*R*C];
	extern FPGA_DATA_FIX x_4[CH3*(R+2)*(C+2)];

	FPGA_DATA_FIX y_1[CH2*(R)*(C)]={0};
	FPGA_DATA_FIX y_2[EXP2*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_3[CH2*(R)*(C)]={0};
	FPGA_DATA_FIX y_4[CH3*(R+2)*(C+2)]={0};

	do_conv(CH2,CH1,R+2,C+2,3,input,weight_1,y_1,bias_1,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_conv_pad(EXP2,CH2,R,C,R+2,C+2,1,1,y_1,weight_2,y_2,bias_2,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_dconv(EXP2,EXP2,R+2,C+2,y_2,weight_3,y_3,bias_3,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_conv_pad(CH3,EXP2,R,C,R+2,C+2,1,1,y_3,weight_4,y_4,bias_4,input_dma_W,input_dma_I,input_dma_B,output_dma_O);

	int real_R = R+2;
	int real_C = C+2;

	printf("\n\n================\n");


	for(int m=0;m<CH2;m++){
		for(int r=0;r<real_R;r++){
			for(int c=0;c<real_C;c++){
				int res;
				int g_res;
				res = y_4[m*(real_R)*(real_C) + r*(real_C) + c];
				g_res = x_4[m*(real_R)*(real_C) + (r)*(real_C) + (c)];
				printf("%d ",res-g_res);
			}
			printf("\n");
		}
		printf("========%d==========\n",m);
		printf("\n");
	}

//	// Do dConv.
//	do_dconv_pad(CH2,CH1,R,C,1,input,weight_1,my_output,bias_1,input_dma_W,input_dma_I,input_dma_B,output_dma_O);


//	for(int m=0;m<CH2;m++){
//		for(int r=0;r<R;r++){
//			for(int c=0;c<C;c++){
//				int res;
//				res = my_output[m*(R)*(C) + r*(C) + c];
//				printf("%d ", res);
//			}
//			printf("\n");
//		}
//
//		for(int r=0;r<R-1*K+1;r++){
//			for(int c=0;c<C-1*K+1;c++){
//				int g_res;
//				g_res = output[m*(R-1*K+1)*(C-1*K+1) + r*(C-1*K+1) + c];
//				printf("%d ", g_res);
//			}
//			printf("\n");
//		}
//		printf("========%d==========\n",m);
//		printf("\n");
//	}




	return 0;
}
