#include "source_fix.h"
#include <math.h>
#include <stdlib.h>

void cconv(hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O,
		int row,int col,int num,int N,int custom_k,int custom_Tr,int custom_Tc,
		int custom_Tm,int custom_Tn,int NL_Opt);


int stream_IFM_in(int M,int N,int R,int C,int custom_k,
		int row, int col,
		int ich_max, int row_max, int col_max,
		int custom_Tr,int custom_Tc,
		FPGA_DATA_FIX * input,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I){

	DMA_DATA_128B_FIX ifm;

	for(int ich=0; ich<ich_max+1;ich++){
		for(int i=0;i<divided_Tn_8;i++){
			for(int j=0;j<custom_Tr+custom_k-1;j++){
				for(int m=0;m<custom_Tc+custom_k-1;m++){
					ifm.data.data1 = ich*Tn+i*8+0<N?input[(ich*Tn+i*8+0)*R*C + (row*Tr + j)*C + (col*Tc + m)]:0;
					ifm.data.data2 = ich*Tn+i*8+1<N?input[(ich*Tn+i*8+1)*R*C + (row*Tr + j)*C + (col*Tc + m)]:0;
					ifm.data.data3 = ich*Tn+i*8+2<N?input[(ich*Tn+i*8+2)*R*C + (row*Tr + j)*C + (col*Tc + m)]:0;
					ifm.data.data4 = ich*Tn+i*8+3<N?input[(ich*Tn+i*8+3)*R*C + (row*Tr + j)*C + (col*Tc + m)]:0;
					ifm.data.data5 = ich*Tn+i*8+4<N?input[(ich*Tn+i*8+4)*R*C + (row*Tr + j)*C + (col*Tc + m)]:0;
					ifm.data.data6 = ich*Tn+i*8+5<N?input[(ich*Tn+i*8+5)*R*C + (row*Tr + j)*C + (col*Tc + m)]:0;
					ifm.data.data7 = ich*Tn+i*8+6<N?input[(ich*Tn+i*8+6)*R*C + (row*Tr + j)*C + (col*Tc + m)]:0;
					ifm.data.data8 = ich*Tn+i*8+7<N?input[(ich*Tn+i*8+7)*R*C + (row*Tr + j)*C + (col*Tc + m)]:0;
					if(i==divided_Tn_8-1 && j==custom_Tr-1+custom_k-1 && m==custom_Tc-1+custom_k-1)
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



int stream_WEI_in(int M,int N,int custom_k,
		int mch,
		int ich_max,
		FPGA_DATA_FIX * weight,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W){

	DMA_DATA_128B_FIX wei;


	for(int ich=0; ich<ich_max+1;ich++){
		for(int o=0;o<divided_Tm_8;o++){
			for(int i=0;i<Tn;i++){
				for(int j=0;j<custom_k;j++){
					for(int m=0;m<custom_k;m++){
						wei.data.data1 = mch*Tm + o*8+0 < M && ich*Tn+i < N ?weight[(mch*Tm + o*8+0)*N*custom_k*custom_k + (ich*Tn+i)*custom_k*custom_k + (j)*custom_k + (m)]:0;
						wei.data.data2 = mch*Tm + o*8+1 < M && ich*Tn+i < N ?weight[(mch*Tm + o*8+1)*N*custom_k*custom_k + (ich*Tn+i)*custom_k*custom_k + (j)*custom_k + (m)]:0;
						wei.data.data3 = mch*Tm + o*8+2 < M && ich*Tn+i < N ?weight[(mch*Tm + o*8+2)*N*custom_k*custom_k + (ich*Tn+i)*custom_k*custom_k + (j)*custom_k + (m)]:0;
						wei.data.data4 = mch*Tm + o*8+3 < M && ich*Tn+i < N ?weight[(mch*Tm + o*8+3)*N*custom_k*custom_k + (ich*Tn+i)*custom_k*custom_k + (j)*custom_k + (m)]:0;
						wei.data.data5 = mch*Tm + o*8+4 < M && ich*Tn+i < N ?weight[(mch*Tm + o*8+4)*N*custom_k*custom_k + (ich*Tn+i)*custom_k*custom_k + (j)*custom_k + (m)]:0;
						wei.data.data6 = mch*Tm + o*8+5 < M && ich*Tn+i < N ?weight[(mch*Tm + o*8+5)*N*custom_k*custom_k + (ich*Tn+i)*custom_k*custom_k + (j)*custom_k + (m)]:0;
						wei.data.data7 = mch*Tm + o*8+6 < M && ich*Tn+i < N ?weight[(mch*Tm + o*8+6)*N*custom_k*custom_k + (ich*Tn+i)*custom_k*custom_k + (j)*custom_k + (m)]:0;
						wei.data.data8 = mch*Tm + o*8+7 < M && ich*Tn+i < N ?weight[(mch*Tm + o*8+7)*N*custom_k*custom_k + (ich*Tn+i)*custom_k*custom_k + (j)*custom_k + (m)]:0;
						if(o==divided_Tm_8-1 && i==Tn-1 && j==custom_k-1 && m==custom_k-1)
							wei.last = true;
						else
							wei.last = false;
						input_dma_W.write(wei);
					}
				}
			}
		}
	}
	return 0;
}

int stream_BIAs_in(int M,int N,int R,int C,
		int mch,
		FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B){

	DMA_DATA_128B_FIX bia;
	for(int i=0;i<divided_Tm_8;i++){
		bia.data.data1 = mch*Tm + i*8+0 < M ? bias[mch*Tm + i*8+0] : 0;
		bia.data.data2 = mch*Tm + i*8+1 < M ? bias[mch*Tm + i*8+1] : 0;
		bia.data.data3 = mch*Tm + i*8+2 < M ? bias[mch*Tm + i*8+2] : 0;
		bia.data.data4 = mch*Tm + i*8+3 < M ? bias[mch*Tm + i*8+3] : 0;
		bia.data.data5 = mch*Tm + i*8+4 < M ? bias[mch*Tm + i*8+4] : 0;
		bia.data.data6 = mch*Tm + i*8+5 < M ? bias[mch*Tm + i*8+5] : 0;
		bia.data.data7 = mch*Tm + i*8+6 < M ? bias[mch*Tm + i*8+6] : 0;
		bia.data.data8 = mch*Tm + i*8+7 < M ? bias[mch*Tm + i*8+7] : 0;
		if(i==divided_Tm_8-1)
			bia.last = true;
		else
			bia.last = false;
		input_dma_B.write(bia);
	}
	return 0;
}

int stream_OFM_out(int M,int N,int R,int C,int custom_k,
		int row, int col, int mch,
		int row_max, int col_max,
		int custom_Tr,int custom_Tc,
		FPGA_DATA_FIX * output,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O){



	for(int i=0;i<divided_Tm_8;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				DMA_DATA_128B_FIX output_dma_O_data = output_dma_O.read();
				if(mch!=-1){
					// Weiwen-01-23: Condition here to make sure mch*Tm + offset is larger than M
					if(mch*Tm + i*8+0<M)
						output[(mch*Tm + i*8+0)*(R-custom_k+1)*(C-custom_k+1) + (row*Tr + j)*(C-custom_k+1) + (col*Tc + m)] = output_dma_O_data.data.data1;
					if(mch*Tm + i*8+1<M)
						output[(mch*Tm + i*8+1)*(R-custom_k+1)*(C-custom_k+1) + (row*Tr + j)*(C-custom_k+1) + (col*Tc + m)] = output_dma_O_data.data.data2;
					if(mch*Tm + i*8+2<M)
						output[(mch*Tm + i*8+2)*(R-custom_k+1)*(C-custom_k+1) + (row*Tr + j)*(C-custom_k+1) + (col*Tc + m)] = output_dma_O_data.data.data3;
					if(mch*Tm + i*8+3<M)
						output[(mch*Tm + i*8+3)*(R-custom_k+1)*(C-custom_k+1) + (row*Tr + j)*(C-custom_k+1) + (col*Tc + m)] = output_dma_O_data.data.data4;
					if(mch*Tm + i*8+4<M)
						output[(mch*Tm + i*8+4)*(R-custom_k+1)*(C-custom_k+1) + (row*Tr + j)*(C-custom_k+1) + (col*Tc + m)] = output_dma_O_data.data.data5;
					if(mch*Tm + i*8+5<M)
						output[(mch*Tm + i*8+5)*(R-custom_k+1)*(C-custom_k+1) + (row*Tr + j)*(C-custom_k+1) + (col*Tc + m)] = output_dma_O_data.data.data6;
					if(mch*Tm + i*8+6<M)
						output[(mch*Tm + i*8+6)*(R-custom_k+1)*(C-custom_k+1) + (row*Tr + j)*(C-custom_k+1) + (col*Tc + m)] = output_dma_O_data.data.data7;
					if(mch*Tm + i*8+7<M)
						output[(mch*Tm + i*8+7)*(R-custom_k+1)*(C-custom_k+1) + (row*Tr + j)*(C-custom_k+1) + (col*Tc + m)] = output_dma_O_data.data.data8;
				}
			}
		}
	}

	return 0;
}

int do_conv(int M,int N,int R,int C,int c_k,
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
	int ich_max = ceil(float(N)/Tn);
	int mch_max = ceil(float(M)/Tm);
	int row_max = ceil(float(R)/Tr);
	int col_max = ceil(float(C)/Tc);

	int custom_Tr;
	int custom_Tc;
	int custom_k = c_k;

	int db_flag = 0;



	printf("Iterations: %d %d %d %d\n",ich_max,mch_max,row_max,col_max);

	for(row=0; row<row_max;row++){
		for(col=0; col<col_max;col++){
			// We need an additional output, since the outer pipeline needing to be filled up
			for(mch=0; mch<mch_max+1;mch++){

				custom_Tr = Tr;
				custom_Tc = Tc;

				if(row==row_max-1)
					custom_Tr = R -custom_k+1 - row*Tr;
				if(col==col_max-1)
					custom_Tc = C -custom_k+1 - col*Tc;


				stream_IFM_in(M,N,R,C,custom_k, row, col, ich_max, row_max, col_max, custom_Tr, custom_Tc, input, input_dma_I);
				stream_WEI_in(M,N,custom_k,mch,ich_max,weight,input_dma_W);

				stream_BIAs_in(M,N,R,C,mch-1,bias,input_dma_B);

				printf("%d %d %d: %d,%d\n",mch, row, col, custom_Tr,custom_Tc);
				cconv(input_dma_W,input_dma_I,input_dma_B,output_dma_O,\
							0,0,db_flag,N,custom_k,custom_Tr,custom_Tc,Tm,Tn,0);

				db_flag+=1;
				// mch-1 for double buffer
				stream_OFM_out(M,N,R,C,custom_k, row,col,mch-1,row_max,col_max, custom_Tr, custom_Tc, output,output_dma_O);


			}
		}
	}



	return 0;
}