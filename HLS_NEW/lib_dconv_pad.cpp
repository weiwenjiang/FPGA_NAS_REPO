#include "source_fix.h"
#include <math.h>
#include <stdlib.h>

void dconv(hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O,
		int row,
		int col,
		int num,
		int N,
		int custom_k,
		int custom_Tr,
		int custom_Tc,
		int NL_Opt
		);


int stream_IFM_in_dconv(int M,int N,int R,int C,
		int row, int col,
		int ich,
		int ich_max, int row_max, int col_max,
		int custom_Tr,int custom_Tc,
		FPGA_DATA_FIX * input,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I);


int stream_WEI_in_dconv(int M,int N,
		int ich,
		int ich_max,
		FPGA_DATA_FIX * weight,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W);

int stream_BIAs_in_dconv(int M,int N,int R,int C,
		int mch,
		FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B);


int stream_OFM_out_dconv_pad(int M,int N,int R,int C,int pad,
		int row, int col, int mch,
		int row_max, int col_max,
		int custom_Tr,int custom_Tc,
		FPGA_DATA_FIX * output,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O){



	for(int i=0;i<divided_Tn_8_dconv;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				DMA_DATA_128B_FIX output_dma_O_data = output_dma_O.read();
				if(mch!=-1){
					if(mch*Tm + i*8+0<M)
						output[(mch*Tn_dconv + i*8+0)*(R)*(C) + (row*Tr_dconv + j + pad)*(C) + (col*Tc_dconv + m + pad)] = output_dma_O_data.data.data1;
					if(mch*Tm + i*8+1<M)
						output[(mch*Tn_dconv + i*8+1)*(R)*(C) + (row*Tr_dconv + j + pad)*(C) + (col*Tc_dconv + m + pad)] = output_dma_O_data.data.data2;
					if(mch*Tm + i*8+2<M)
						output[(mch*Tn_dconv + i*8+2)*(R)*(C) + (row*Tr_dconv + j + pad)*(C) + (col*Tc_dconv + m + pad)] = output_dma_O_data.data.data3;
					if(mch*Tm + i*8+3<M)
						output[(mch*Tn_dconv + i*8+3)*(R)*(C) + (row*Tr_dconv + j + pad)*(C) + (col*Tc_dconv + m + pad)] = output_dma_O_data.data.data4;
					if(mch*Tm + i*8+4<M)
						output[(mch*Tn_dconv + i*8+4)*(R)*(C) + (row*Tr_dconv + j + pad)*(C) + (col*Tc_dconv + m + pad)] = output_dma_O_data.data.data5;
					if(mch*Tm + i*8+5<M)
						output[(mch*Tn_dconv + i*8+5)*(R)*(C) + (row*Tr_dconv + j + pad)*(C) + (col*Tc_dconv + m + pad)] = output_dma_O_data.data.data6;
					if(mch*Tm + i*8+6<M)
						output[(mch*Tn_dconv + i*8+6)*(R)*(C) + (row*Tr_dconv + j + pad)*(C) + (col*Tc_dconv + m + pad)] = output_dma_O_data.data.data7;
					if(mch*Tm + i*8+7<M)
						output[(mch*Tn_dconv + i*8+7)*(R)*(C) + (row*Tr_dconv + j + pad)*(C) + (col*Tc_dconv + m + pad)] = output_dma_O_data.data.data8;
				}
			}
		}
	}

	return 0;
}

int do_dconv_pad(int M,int N,int R,int C,int pad,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * weight,FPGA_DATA_FIX * output, FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		){

	int ich = 0;
	int mch = 0;
	int row = 0;
	int col = 0;
	int ich_max = ceil(float(N)/Tn_dconv);
	int mch_max = ceil(float(M)/Tn_dconv);
	int row_max = ceil(float(R)/Tr_dconv);
	int col_max = ceil(float(C)/Tc_dconv);

	int custom_Tr;
	int custom_Tc;
	int custom_k = K_dconv;

	int db_flag = 0;



//	printf("Iterations: %d %d %d %d\n",ich_max,mch_max,row_max,col_max);

	for(row=0; row<row_max;row++){
		for(col=0; col<col_max;col++){
			// We need an additional output, since the outer pipeline needing to be filled up
			custom_Tr = Tr_dconv;
			custom_Tc = Tc_dconv;

			if(row==row_max-1)
				custom_Tr = R -K_dconv+1 - row*Tr_dconv;
			if(col==col_max-1)
				custom_Tc = C -K_dconv+1 - col*Tc_dconv;

			for(mch=0; mch<mch_max+1;mch++){

				stream_IFM_in_dconv(M,N,R,C,row, col, mch, ich_max, row_max, col_max, custom_Tr, custom_Tc, input, input_dma_I);
				stream_WEI_in_dconv(M,N,mch,ich_max,weight,input_dma_W);
				stream_BIAs_in_dconv(M,N,R,C,mch-1,bias,input_dma_B);

//				printf("%d %d %d: %d,%d\n",mch, row, col, custom_Tr,custom_Tc);
				dconv(input_dma_W,input_dma_I,input_dma_B,output_dma_O,\
							0,0,db_flag,N,custom_k,custom_Tr,custom_Tc,0);

				db_flag+=1;
				stream_OFM_out_dconv_pad(M,N,R,C,pad, row,col,mch-1,row_max,col_max, custom_Tr, custom_Tc, output,output_dma_O);
			}
		}
	}

	return 0;
}
