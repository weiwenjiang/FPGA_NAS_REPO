#include "source_fix.h"
#include <math.h>
#include <stdlib.h>

void batch_norm(hls::stream<DMA_DATA_128B_FIX> &input,
		hls::stream<DMA_DATA_128B_FIX> &running_mean,
		hls::stream<DMA_DATA_128B_FIX> &running_var,
		hls::stream<DMA_DATA_128B_FIX> &output,
		float eps,int gamma,int beta,int custom_Tr,int custom_Tc,int op);

int stream_bn_IFM_in(int M,int N,int R,int C,
		int row, int col,
		int ich_max, int row_max, int col_max,
		int custom_Tr,int custom_Tc,
		FPGA_DATA_FIX * input,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I){

	DMA_DATA_128B_FIX ifm;

	for(int ich=0; ich<ich_max;ich++){
		for(int i=0;i<divided_Tn_8_bn;i++){
			for(int j=0;j<custom_Tr;j++){
				for(int m=0;m<custom_Tc;m++){
					ifm.data.data1 = ich*Tn_bn+i*8+0<N?input[(ich*Tn_bn+i*8+0)*R*C + (row*Tr_bn + j)*C + (col*Tc_bn + m)]:0;
					ifm.data.data2 = ich*Tn_bn+i*8+1<N?input[(ich*Tn_bn+i*8+1)*R*C + (row*Tr_bn + j)*C + (col*Tc_bn + m)]:0;
					ifm.data.data3 = ich*Tn_bn+i*8+2<N?input[(ich*Tn_bn+i*8+2)*R*C + (row*Tr_bn + j)*C + (col*Tc_bn + m)]:0;
					ifm.data.data4 = ich*Tn_bn+i*8+3<N?input[(ich*Tn_bn+i*8+3)*R*C + (row*Tr_bn + j)*C + (col*Tc_bn + m)]:0;
					ifm.data.data5 = ich*Tn_bn+i*8+4<N?input[(ich*Tn_bn+i*8+4)*R*C + (row*Tr_bn + j)*C + (col*Tc_bn + m)]:0;
					ifm.data.data6 = ich*Tn_bn+i*8+5<N?input[(ich*Tn_bn+i*8+5)*R*C + (row*Tr_bn + j)*C + (col*Tc_bn + m)]:0;
					ifm.data.data7 = ich*Tn_bn+i*8+6<N?input[(ich*Tn_bn+i*8+6)*R*C + (row*Tr_bn + j)*C + (col*Tc_bn + m)]:0;
					ifm.data.data8 = ich*Tn_bn+i*8+7<N?input[(ich*Tn_bn+i*8+7)*R*C + (row*Tr_bn + j)*C + (col*Tc_bn + m)]:0;
					if(i==divided_Tn_8_bn-1 && j==custom_Tr-1 && m==custom_Tc-1)
						ifm.last = true;
					else
						ifm.last = false;
					input_dma_I.write(ifm);
				}
			}
		}
	}
	return 0;
}


int stream_bn_EV_in(int M,int N,int R,int C,
		int mch,
		FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B){

	DMA_DATA_128B_FIX bia;
	for(int i=0;i<divided_Tn_8_bn;i++){
		bia.data.data1 = mch*Tn_bn + i*8+0 < M ? bias[mch*Tn_bn + i*8+0] : 0;
		bia.data.data2 = mch*Tn_bn + i*8+1 < M ? bias[mch*Tn_bn + i*8+1] : 0;
		bia.data.data3 = mch*Tn_bn + i*8+2 < M ? bias[mch*Tn_bn + i*8+2] : 0;
		bia.data.data4 = mch*Tn_bn + i*8+3 < M ? bias[mch*Tn_bn + i*8+3] : 0;
		bia.data.data5 = mch*Tn_bn + i*8+4 < M ? bias[mch*Tn_bn + i*8+4] : 0;
		bia.data.data6 = mch*Tn_bn + i*8+5 < M ? bias[mch*Tn_bn + i*8+5] : 0;
		bia.data.data7 = mch*Tn_bn + i*8+6 < M ? bias[mch*Tn_bn + i*8+6] : 0;
		bia.data.data8 = mch*Tn_bn + i*8+7 < M ? bias[mch*Tn_bn + i*8+7] : 0;
		if(i==divided_Tn_8_bn-1)
			bia.last = true;
		else
			bia.last = false;
		input_dma_B.write(bia);
	}
	return 0;
}


int stream_BN_OFM_out(int M,int N,int R,int C,
		int row, int col, int mch,
		int row_max, int col_max,
		int custom_Tr,int custom_Tc,
		FPGA_DATA_FIX * output,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O){



	for(int i=0;i<divided_Tn_8_bn;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				DMA_DATA_128B_FIX output_dma_O_data = output_dma_O.read();
				if(mch!=-1){
					if(mch*Tm + i*8+0<M)
						output[(mch*Tn_bn + i*8+0)*(R)*(C) + (row*Tr_bn + j)*(C) + (col*Tc_bn + m)] = output_dma_O_data.data.data1;
					if(mch*Tm + i*8+1<M)
						output[(mch*Tn_bn + i*8+1)*(R)*(C) + (row*Tr_bn + j)*(C) + (col*Tc_bn + m)] = output_dma_O_data.data.data2;
					if(mch*Tm + i*8+2<M)
						output[(mch*Tn_bn + i*8+2)*(R)*(C) + (row*Tr_bn + j)*(C) + (col*Tc_bn + m)] = output_dma_O_data.data.data3;
					if(mch*Tm + i*8+3<M)
						output[(mch*Tn_bn + i*8+3)*(R)*(C) + (row*Tr_bn + j)*(C) + (col*Tc_bn + m)] = output_dma_O_data.data.data4;
					if(mch*Tm + i*8+4<M)
						output[(mch*Tn_bn + i*8+4)*(R)*(C) + (row*Tr_bn + j)*(C) + (col*Tc_bn + m)] = output_dma_O_data.data.data5;
					if(mch*Tm + i*8+5<M)
						output[(mch*Tn_bn + i*8+5)*(R)*(C) + (row*Tr_bn + j)*(C) + (col*Tc_bn + m)] = output_dma_O_data.data.data6;
					if(mch*Tm + i*8+6<M)
						output[(mch*Tn_bn + i*8+6)*(R)*(C) + (row*Tr_bn + j)*(C) + (col*Tc_bn + m)] = output_dma_O_data.data.data7;
					if(mch*Tm + i*8+7<M)
						output[(mch*Tn_bn + i*8+7)*(R)*(C) + (row*Tr_bn + j)*(C) + (col*Tc_bn + m)] = output_dma_O_data.data.data8;
				}
			}
		}
	}

	return 0;
}


int do_bn(int M,int N,int R,int C,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * E,FPGA_DATA_FIX * V,FPGA_DATA_FIX *BN_output,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_E,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_V,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		){

	float eps = 1e-5;
	int gamma = 1;
	int beta = 0;

	int ich = 0;
	int mch = 0;
	int row = 0;
	int col = 0;
	int ich_max = ceil(float(N)/Tn_bn);
	int mch_max = ceil(float(M)/Tn_bn);
	int row_max = ceil(float(R)/Tr_bn);
	int col_max = ceil(float(C)/Tc_bn);

	int custom_Tr;
	int custom_Tc;


	int db_flag = 0;

	printf("BN Iterations: %d %d %d %d\n",ich_max,mch_max,row_max,col_max);

	for(row=0; row<row_max;row++){
		for(col=0; col<col_max;col++){

			custom_Tr = Tr_bn;
			custom_Tc = Tc_bn;

			if(row==row_max-1)
				custom_Tr = R  - row*Tr_bn;
			if(col==col_max-1)
				custom_Tc = C  - col*Tc_bn;


			stream_bn_IFM_in(M,N,R,C,row, col, ich_max, row_max, col_max, custom_Tr, custom_Tc, input, input_dma_I);

			for(ich=0;ich<ich_max;ich++){

				stream_bn_EV_in(M,N,R,C,ich,E,input_dma_E);
				stream_bn_EV_in(M,N,R,C,ich,V,input_dma_V);
				batch_norm(input_dma_I,input_dma_E,input_dma_V,output_dma_O,
						eps,gamma,beta,custom_Tr,custom_Tc,1);

				batch_norm(input_dma_I,input_dma_E,input_dma_V,output_dma_O,
						eps,gamma,beta,custom_Tr,custom_Tc,0);
				batch_norm(input_dma_I,input_dma_E,input_dma_V,output_dma_O,
						eps,gamma,beta,custom_Tr,custom_Tc,2);

				batch_norm(input_dma_I,input_dma_E,input_dma_V,output_dma_O,
						eps,gamma,beta,custom_Tr,custom_Tc,3);
				stream_BN_OFM_out(M,N,R,C,row,col,ich,row_max,col_max, custom_Tr,
						custom_Tc, BN_output,output_dma_O);
			}
		}
	}



	return 0;
}
