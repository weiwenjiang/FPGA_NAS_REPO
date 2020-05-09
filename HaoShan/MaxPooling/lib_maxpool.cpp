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
		int row,
		int col,
		int num,
		int N,
		int custom_k,
		int custom_Tr,
		int custom_Tc,
		int custom_Tm,
		int custom_Tn,
		int NL_Opt
		);


int maxpool_stream_IFM_in(int M,int N,int R,int C,
		int row, int col,
		int ich,
		int ich_max, int row_max, int col_max,
		int custom_Tr,int custom_Tc,
		FPGA_DATA_FIX * input,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I){

	DMA_DATA_128B_FIX ifm;

		for(int i=0;i<divided_Tn_8;i++){
			for(int j=0;j<2*custom_Tr;j++){
				for(int m=0;m<2*custom_Tc;m++){
					ifm.data.data1 = ich*Tn+i*8+0<N?input[(ich*Tn+i*8+0)*R*C + (row*2*Tr_maxpool + j)*C + (col*2*Tc_maxpool + m)]:FPGA_DATA_FIX(0);
					ifm.data.data2 = ich*Tn+i*8+1<N?input[(ich*Tn+i*8+1)*R*C + (row*2*Tr_maxpool + j)*C + (col*2*Tc_maxpool + m)]:FPGA_DATA_FIX(0);
					ifm.data.data3 = ich*Tn+i*8+2<N?input[(ich*Tn+i*8+2)*R*C + (row*2*Tr_maxpool + j)*C + (col*2*Tc_maxpool + m)]:FPGA_DATA_FIX(0);
					ifm.data.data4 = ich*Tn+i*8+3<N?input[(ich*Tn+i*8+3)*R*C + (row*2*Tr_maxpool + j)*C + (col*2*Tc_maxpool + m)]:FPGA_DATA_FIX(0);
					ifm.data.data5 = ich*Tn+i*8+4<N?input[(ich*Tn+i*8+4)*R*C + (row*2*Tr_maxpool + j)*C + (col*2*Tc_maxpool + m)]:FPGA_DATA_FIX(0);
					ifm.data.data6 = ich*Tn+i*8+5<N?input[(ich*Tn+i*8+5)*R*C + (row*2*Tr_maxpool + j)*C + (col*2*Tc_maxpool + m)]:FPGA_DATA_FIX(0);
					ifm.data.data7 = ich*Tn+i*8+6<N?input[(ich*Tn+i*8+6)*R*C + (row*2*Tr_maxpool + j)*C + (col*2*Tc_maxpool + m)]:FPGA_DATA_FIX(0);
					ifm.data.data8 = ich*Tn+i*8+7<N?input[(ich*Tn+i*8+7)*R*C + (row*2*Tr_maxpool + j)*C + (col*2*Tc_maxpool + m)]:FPGA_DATA_FIX(0);
					if(i==divided_Tn_8-1 && j==2*custom_Tr-1 && m==2*custom_Tc-1)
						ifm.last = true;
					else
						ifm.last = false;
					input_dma_I.write(ifm);
				}
			}
		}



	return 0;
}


int maxpool_stream_OFM_out(int M,int N,int R,int C,int outR,int outC,
		int row, int col, int mch,
		int row_max, int col_max,
		int custom_Tr,int custom_Tc,
		FPGA_DATA_FIX * output,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O){
	int flag=0;
	for(int i=0;i<divided_Tm_8;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				DMA_DATA_128B_FIX output_dma_O_data = output_dma_O.read();

				if(mch!=-1){
					flag =1;
					if(mch*Tm + i*8+0<M)
						output[(mch*Tm + i*8+0)*(outR)*(outC) + (row*Tr_maxpool + j )*(outC) + (col*Tc_maxpool + m )] = output_dma_O_data.data.data1;
					if(mch*Tm + i*8+1<M)
						output[(mch*Tm + i*8+1)*(outR)*(outC) + (row*Tr_maxpool + j )*(outC) + (col*Tc_maxpool + m )] = output_dma_O_data.data.data2;
					if(mch*Tm + i*8+2<M)
						output[(mch*Tm + i*8+2)*(outR)*(outC) + (row*Tr_maxpool + j )*(outC) + (col*Tc_maxpool + m )] = output_dma_O_data.data.data3;
					if(mch*Tm + i*8+3<M)
						output[(mch*Tm + i*8+3)*(outR)*(outC) + (row*Tr_maxpool + j )*(outC) + (col*Tc_maxpool + m )] = output_dma_O_data.data.data4;
					if(mch*Tm + i*8+4<M)
						output[(mch*Tm + i*8+4)*(outR)*(outC) + (row*Tr_maxpool + j )*(outC) + (col*Tc_maxpool + m )] = output_dma_O_data.data.data5;
					if(mch*Tm + i*8+5<M)
						output[(mch*Tm + i*8+5)*(outR)*(outC) + (row*Tr_maxpool + j )*(outC) + (col*Tc_maxpool + m )] = output_dma_O_data.data.data6;
					if(mch*Tm + i*8+6<M)
						output[(mch*Tm + i*8+6)*(outR)*(outC) + (row*Tr_maxpool + j )*(outC) + (col*Tc_maxpool + m )] = output_dma_O_data.data.data7;
					if(mch*Tm + i*8+7<M)
						output[(mch*Tm + i*8+7)*(outR)*(outC) + (row*Tr_maxpool + j )*(outC) + (col*Tc_maxpool + m )] = output_dma_O_data.data.data8;
				}
			}
		}
	}
	if(flag==1)
			printf("enter output\n");
	return 0;
}

int do_maxpool(int M,int N,int R,int C,int outR,int outC,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * output,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		){

	int ich = 0;
	int mch = 0;
	int row = 0;
	int col = 0;
	int ich_max = ceil(float(N)/Tn);
	int mch_max = ceil(float(M)/Tm);
	int row_max = ceil(float(outR)/Tr_maxpool);
	int col_max = ceil(float(outC)/Tc_maxpool);

	int custom_Tr;
	int custom_Tc;
	int custom_k = K_maxpool;

	int db_flag = 0;


	printf("Iterations: %d %d %d %d\n",ich_max,mch_max,row_max,col_max);

	for(row=0; row<row_max;row++){
		for(col=0; col<col_max;col++){
			// We need an additional output, since the outer pipeline needing to be filled up

			for(mch=0; mch<mch_max+1;mch++){

				custom_Tr = Tr_maxpool;
				custom_Tc = Tc_maxpool;

				if(row==row_max-1)
					custom_Tr = outR  - row*Tr_maxpool;
				if(col==col_max-1)
					custom_Tc = outC  - col*Tc_maxpool;

				printf("%d %d %d: %d,%d\n", row, col, mch,custom_Tr,custom_Tc);
				maxpool_stream_IFM_in(M,N,R,C,row, col,mch, ich_max, row_max, col_max, custom_Tr, custom_Tc, input, input_dma_I);

//				printf("%d %d %d: %d,%d\n",row, col,mch,custom_Tr,custom_Tc);
				maxpool(input_dma_W,input_dma_I,input_dma_B,output_dma_O,\
						0,0,db_flag,N,custom_k,custom_Tr,custom_Tc,Tm,Tn,0);

				db_flag+=1;
				maxpool_stream_OFM_out(M,N,R,C,outR,outC,row,col,mch-1,row_max,col_max, custom_Tr, custom_Tc, output,output_dma_O);
			}

		}
	}


	return 0;
}
