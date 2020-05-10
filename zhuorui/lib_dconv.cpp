#include "source_fix.h"
#include <math.h>
#include <stdlib.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))
void dconv(hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O,
		int row,
		int col,
		int num,
		int N,
		int custom_k,
		int custom_s,
		int custom_Tr,
		int custom_Tc,
		int NL_Opt
		);


int stream_IFM_in_dconv(int M,int N,int R,int C,int custom_s,
		int row, int col,
		int ich,
		int ich_max, int row_max, int col_max,
		int custom_Tr,int custom_Tc,
		FPGA_DATA_FIX * input,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I){

	DMA_DATA_128B_FIX ifm;

//	for(int ich=0; ich<ich_max+1;ich++){
		for(int i=0;i<divided_Tn_8_dconv;i++){
			for(int j=0;j<(custom_Tr-1)*custom_s+K_dconv;j++){
				for(int m=0;m<(custom_Tr-1)*custom_s+K_dconv;m++){
					ifm.data.data1 = ich*Tn_dconv+i*8+0<N?input[(ich*Tn_dconv+i*8+0)*R*C + (row*custom_s*Tr_dconv + j)*C + (col*custom_s*Tc_dconv + m)]:0;
					ifm.data.data2 = ich*Tn_dconv+i*8+1<N?input[(ich*Tn_dconv+i*8+1)*R*C + (row*custom_s*Tr_dconv + j)*C + (col*custom_s*Tc_dconv + m)]:0;
					ifm.data.data3 = ich*Tn_dconv+i*8+2<N?input[(ich*Tn_dconv+i*8+2)*R*C + (row*custom_s*Tr_dconv + j)*C + (col*custom_s*Tc_dconv + m)]:0;
					ifm.data.data4 = ich*Tn_dconv+i*8+3<N?input[(ich*Tn_dconv+i*8+3)*R*C + (row*custom_s*Tr_dconv + j)*C + (col*custom_s*Tc_dconv + m)]:0;
					ifm.data.data5 = ich*Tn_dconv+i*8+4<N?input[(ich*Tn_dconv+i*8+4)*R*C + (row*custom_s*Tr_dconv + j)*C + (col*custom_s*Tc_dconv + m)]:0;
					ifm.data.data6 = ich*Tn_dconv+i*8+5<N?input[(ich*Tn_dconv+i*8+5)*R*C + (row*custom_s*Tr_dconv + j)*C + (col*custom_s*Tc_dconv + m)]:0;
					ifm.data.data7 = ich*Tn_dconv+i*8+6<N?input[(ich*Tn_dconv+i*8+6)*R*C + (row*custom_s*Tr_dconv + j)*C + (col*custom_s*Tc_dconv + m)]:0;
					ifm.data.data8 = ich*Tn_dconv+i*8+7<N?input[(ich*Tn_dconv+i*8+7)*R*C + (row*custom_s*Tr_dconv + j)*C + (col*custom_s*Tc_dconv + m)]:0;
					if(i==divided_Tn_8_dconv-1 && j==(custom_Tr-1)*custom_s+K_dconv-1 && m==(custom_Tc-1)*custom_s+K_dconv-1)
						ifm.last = true;
					else
						ifm.last = false;
					input_dma_I.write(ifm);
				}
			}
		}
//	}
	return 0;
}



int stream_WEI_in_dconv(int M,int N,
		int ich,
		int ich_max,
		FPGA_DATA_FIX * weight,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W){

	DMA_DATA_128B_FIX wei;
	int custom_k =K_dconv;

//	for(int ich=0; ich<ich_max+1;ich++){
		for(int o=0;o<divided_Tn_8_dconv;o++){
				for(int j=0;j<custom_k;j++){
					for(int m=0;m<custom_k;m++){
						wei.data.data1 = ich*Tn_dconv+o*8+0 < N ?weight[(ich*Tn_dconv+o*8+0)*K_dconv*K_dconv + (j)*K_dconv + (m)]:0;
						wei.data.data2 = ich*Tn_dconv+o*8+1 < N ?weight[(ich*Tn_dconv+o*8+1)*K_dconv*K_dconv + (j)*K_dconv + (m)]:0;
						wei.data.data3 = ich*Tn_dconv+o*8+2 < N ?weight[(ich*Tn_dconv+o*8+2)*K_dconv*K_dconv + (j)*K_dconv + (m)]:0;
						wei.data.data4 = ich*Tn_dconv+o*8+3 < N ?weight[(ich*Tn_dconv+o*8+3)*K_dconv*K_dconv + (j)*K_dconv + (m)]:0;
						wei.data.data5 = ich*Tn_dconv+o*8+4 < N ?weight[(ich*Tn_dconv+o*8+4)*K_dconv*K_dconv + (j)*K_dconv + (m)]:0;
						wei.data.data6 = ich*Tn_dconv+o*8+5 < N ?weight[(ich*Tn_dconv+o*8+5)*K_dconv*K_dconv + (j)*K_dconv + (m)]:0;
						wei.data.data7 = ich*Tn_dconv+o*8+6 < N ?weight[(ich*Tn_dconv+o*8+6)*K_dconv*K_dconv + (j)*K_dconv + (m)]:0;
						wei.data.data8 = ich*Tn_dconv+o*8+7 < N ?weight[(ich*Tn_dconv+o*8+7)*K_dconv*K_dconv + (j)*K_dconv + (m)]:0;
						if(o==divided_Tn_8_dconv-1 && j==custom_k-1 && m==custom_k-1)
							wei.last = true;
						else
							wei.last = false;
						input_dma_W.write(wei);
					}
				}
		}
//	}
	return 0;
}

int stream_BIAs_in_dconv(int M,int N,int R,int C,
		int mch,
		FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B){

	DMA_DATA_128B_FIX bia;
	for(int i=0;i<divided_Tn_8_dconv;i++){
		bia.data.data1 = mch*Tn_dconv + i*8+0 < M ? bias[mch*Tn_dconv + i*8+0] : 0;
		bia.data.data2 = mch*Tn_dconv + i*8+1 < M ? bias[mch*Tn_dconv + i*8+1] : 0;
		bia.data.data3 = mch*Tn_dconv + i*8+2 < M ? bias[mch*Tn_dconv + i*8+2] : 0;
		bia.data.data4 = mch*Tn_dconv + i*8+3 < M ? bias[mch*Tn_dconv + i*8+3] : 0;
		bia.data.data5 = mch*Tn_dconv + i*8+4 < M ? bias[mch*Tn_dconv + i*8+4] : 0;
		bia.data.data6 = mch*Tn_dconv + i*8+5 < M ? bias[mch*Tn_dconv + i*8+5] : 0;
		bia.data.data7 = mch*Tn_dconv + i*8+6 < M ? bias[mch*Tn_dconv + i*8+6] : 0;
		bia.data.data8 = mch*Tn_dconv + i*8+7 < M ? bias[mch*Tn_dconv + i*8+7] : 0;
		if(i==divided_Tn_8_dconv-1)
			bia.last = true;
		else
			bia.last = false;
		input_dma_B.write(bia);
	}
	return 0;
}

int stream_OFM_out_dconv(int M,int N,int R,int C,int custom_s,
		int row, int col, int mch,
		int row_max, int col_max,
		int custom_Tr,int custom_Tc,
		FPGA_DATA_FIX * output,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O){

	int OR = (R-K_dconv)/custom_s+1,OC = (C-K_dconv)/custom_s+1;//changed

	for(int i=0;i<divided_Tn_8_dconv;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				DMA_DATA_128B_FIX output_dma_O_data = output_dma_O.read();
				if(mch!=-1){
					if(mch*Tm + i*8+0<M)
						output[(mch*Tn_dconv + i*8+0)*OR*OC + (row*Tr_dconv + j)*OC + (col*Tc_dconv + m)] = output_dma_O_data.data.data1;
					if(mch*Tm + i*8+1<M)
						output[(mch*Tn_dconv + i*8+1)*OR*OC + (row*Tr_dconv + j)*OC + (col*Tc_dconv + m)] = output_dma_O_data.data.data2;
					if(mch*Tm + i*8+2<M)
						output[(mch*Tn_dconv + i*8+2)*OR*OC + (row*Tr_dconv + j)*OC + (col*Tc_dconv + m)] = output_dma_O_data.data.data3;
					if(mch*Tm + i*8+3<M)
						output[(mch*Tn_dconv + i*8+3)*OR*OC + (row*Tr_dconv + j)*OC + (col*Tc_dconv + m)] = output_dma_O_data.data.data4;
					if(mch*Tm + i*8+4<M)
						output[(mch*Tn_dconv + i*8+4)*OR*OC + (row*Tr_dconv + j)*OC + (col*Tc_dconv + m)] = output_dma_O_data.data.data5;
					if(mch*Tm + i*8+5<M)
						output[(mch*Tn_dconv + i*8+5)*OR*OC + (row*Tr_dconv + j)*OC + (col*Tc_dconv + m)] = output_dma_O_data.data.data6;
					if(mch*Tm + i*8+6<M)
						output[(mch*Tn_dconv + i*8+6)*OR*OC + (row*Tr_dconv + j)*OC + (col*Tc_dconv + m)] = output_dma_O_data.data.data7;
					if(mch*Tm + i*8+7<M)
						output[(mch*Tn_dconv + i*8+7)*OR*OC + (row*Tr_dconv + j)*OC + (col*Tc_dconv + m)] = output_dma_O_data.data.data8;
				}
			}
		}
	}

	return 0;
}

int do_dconv(int M,int N,int R,int C,int c_s,
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

	int custom_Tr;
	int custom_Tc;
	int custom_k = K_dconv;
    int custom_s = c_s;

    int ich_max = ceil(float(N)/Tn_dconv);
	int mch_max = ceil(float(M)/Tn_dconv);
	int row_max = ceil(float(R)/(custom_s*Tr_dconv));
	int col_max = ceil(float(C)/(custom_s*Tc_dconv));

	int db_flag = 0;



//	printf("Iterations: %d %d %d %d\n",ich_max,mch_max,row_max,col_max);

	for(row=0; row<row_max;row++){
		for(col=0; col<col_max;col++){
			// We need an additional output, since the outer pipeline needing to be filled up
			custom_Tr = Tr_dconv;
			custom_Tc = Tc_dconv;

			if(row==row_max-1)
				custom_Tr = (R -K_dconv)/custom_s+1 - row*Tr_dconv;//change
			if(col==col_max-1)
				custom_Tc = (C -K_dconv)/custom_s+1 - col*Tc_dconv;//change

			for(mch=0; mch<mch_max+1;mch++){

				stream_IFM_in_dconv(M,N,R,C,row, col,custom_s, mch, ich_max, row_max, col_max, custom_Tr, custom_Tc, input, input_dma_I);
				stream_WEI_in_dconv(M,N,mch,ich_max,weight,input_dma_W);
				stream_BIAs_in_dconv(M,N,R,C,mch-1,bias,input_dma_B);

//				printf("%d %d %d: %d,%d\n",mch, row, col, custom_Tr,custom_Tc);
				dconv(input_dma_W,input_dma_I,input_dma_B,output_dma_O,\
							0,0,db_flag,N,custom_k,custom_s,custom_Tr,custom_Tc,0);

				db_flag+=1;
				stream_OFM_out_dconv(M,N,R,C,custom_s,row,col,mch-1,row_max,col_max, custom_Tr, custom_Tc, output,output_dma_O);
			}
		}
	}

	return 0;
}
