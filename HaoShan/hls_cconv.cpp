#include "source_fix.h"
#include <math.h>
void LOAD_WEIGHT_128B(hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
						FPGA_DATA_FIX WEIGHT1[2][Tm][Tn][K][K],
						int custom_k,
						int custom_Tm,
						int custom_Tn
						){


	DMA_DATA_128B_FIX weight_input_dma;
	weight1:for(int i=0;i<divided_Tm_8;i++){
		weight2:for(int j=0;j<custom_Tn;j++){
			weight3:for(int m=0;m<custom_k;m++){
#pragma HLS loop_tripcount min=3 max=11 avg=3
				weight4: for(int l=0;l<custom_k;l++){
#pragma HLS loop_tripcount min=3 max=11 avg=3
#pragma HLS PIPELINE II=1
					weight_input_dma=input_dma_W.read();

					if(i<divided_Tm_8/2){
						if(i*8<custom_Tm)
							WEIGHT1[0][i*8][j][m][l]=weight_input_dma.data.data1;
						if(i*8+1<custom_Tm)
							WEIGHT1[0][i*8+1][j][m][l]=weight_input_dma.data.data2;
						if(i*8+2<custom_Tm)
							WEIGHT1[0][i*8+2][j][m][l]=weight_input_dma.data.data3;
						if(i*8+3<custom_Tm)
							WEIGHT1[0][i*8+3][j][m][l]=weight_input_dma.data.data4;
						if(i*8+4<custom_Tm)
							WEIGHT1[0][i*8+4][j][m][l]=weight_input_dma.data.data5;
						if(i*8+5<custom_Tm)
							WEIGHT1[0][i*8+5][j][m][l]=weight_input_dma.data.data6;
						if(i*8+6<custom_Tm)
							WEIGHT1[0][i*8+6][j][m][l]=weight_input_dma.data.data7;
						if(i*8+7<custom_Tm)
							WEIGHT1[0][i*8+7][j][m][l]=weight_input_dma.data.data8;
					}else{
						if(i*8<custom_Tm)
							WEIGHT1[1][i*8][j][m][l]=weight_input_dma.data.data1;
						if(i*8+1<custom_Tm)
							WEIGHT1[1][i*8+1][j][m][l]=weight_input_dma.data.data2;
						if(i*8+2<custom_Tm)
							WEIGHT1[1][i*8+2][j][m][l]=weight_input_dma.data.data3;
						if(i*8+3<custom_Tm)
							WEIGHT1[1][i*8+3][j][m][l]=weight_input_dma.data.data4;
						if(i*8+4<custom_Tm)
							WEIGHT1[1][i*8+4][j][m][l]=weight_input_dma.data.data5;
						if(i*8+5<custom_Tm)
							WEIGHT1[1][i*8+5][j][m][l]=weight_input_dma.data.data6;
						if(i*8+6<custom_Tm)
							WEIGHT1[1][i*8+6][j][m][l]=weight_input_dma.data.data7;
						if(i*8+7<custom_Tm)
							WEIGHT1[1][i*8+7][j][m][l]=weight_input_dma.data.data8;

					}
				}
			}
		}
	}
}


void LOAD_IFM(hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		FPGA_DATA_FIX IFM[2][Tn_8][Tr+K-1][Tc+K-1],
		int custom_Tr,
		int custom_Tc,
		int custom_k,
		int custom_Tn
		){



	// Weiwen-01-23: Fix bug custom_Tr+K-1 ---> custom_Tr+custom_k-1 for self-defined K
	DMA_DATA_128B_FIX ifm_input_dma;
	ifm1:for(int i=0;i<divided_Tn_8;i++){
		ifm2:for(int j=0;j<custom_Tr+custom_k-1;j++){
			ifm3:for(int m=0;m<custom_Tc+custom_k-1;m++){
#pragma HLS PIPELINE II=1
				ifm_input_dma=input_dma_I.read();
				if(i*8<custom_Tn){
					IFM[0][i*8][j][m]=ifm_input_dma.data.data1;
					IFM[1][i*8][j][m]=ifm_input_dma.data.data1;
				}
				if(i*8+1<custom_Tn){
					IFM[0][i*8+1][j][m]=ifm_input_dma.data.data2;
					IFM[1][i*8+1][j][m]=ifm_input_dma.data.data2;
				}
				if(i*8+2<custom_Tn){
					IFM[0][i*8+2][j][m]=ifm_input_dma.data.data3;
					IFM[1][i*8+2][j][m]=ifm_input_dma.data.data3;
				}
				if(i*8+3<custom_Tn){
					IFM[0][i*8+3][j][m]=ifm_input_dma.data.data4;
					IFM[1][i*8+3][j][m]=ifm_input_dma.data.data4;
				}
				if(i*8+4<custom_Tn){
					IFM[0][i*8+4][j][m]=ifm_input_dma.data.data5;
					IFM[1][i*8+4][j][m]=ifm_input_dma.data.data5;
				}
				if(i*8+5<custom_Tn){
					IFM[0][i*8+5][j][m]=ifm_input_dma.data.data6;
					IFM[1][i*8+5][j][m]=ifm_input_dma.data.data6;
				}
				if(i*8+6<custom_Tn){
					IFM[0][i*8+6][j][m]=ifm_input_dma.data.data7;
					IFM[1][i*8+6][j][m]=ifm_input_dma.data.data7;
				}
				if(i*8+7<custom_Tn){
					IFM[0][i*8+7][j][m]=ifm_input_dma.data.data8;
					IFM[1][i*8+7][j][m]=ifm_input_dma.data.data8;
				}
			}
		}
	}
}


void FIRE(  FPGA_DATA_FIX WEIGHT1[2][Tm][Tn][K][K],
			FPGA_DATA_FIX IFM[2][Tn_8][Tr+K-1][Tc+K-1],
			FPGA_DATA_FIX OFM[Tm][Tr][Tc],
			int row,
			int col,
			int custom_k,
			int custom_Tr,
			int custom_Tc,
			int custom_Tm,
			int custom_Tn
			){
	for(int i=0;i<custom_k;i++){
#pragma HLS loop_tripcount min=3 max=11 avg=3
		for(int j=0;j<custom_k;j++){
#pragma HLS loop_tripcount min=3 max=11 avg=3
			for(int trr=row;(trr<row+custom_Tr);trr++){
#pragma HLS loop_tripcount min=1 max=Tr avg=Tr
				for(int tcc=col;(tcc<col+custom_Tc);tcc++){
#pragma HLS loop_tripcount min=1 max=Tc avg=Tc
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=OFM inter false
					for(int too=0;too<custom_Tm; too++){
						for(int tii=0;tii<custom_Tn;tii++){
							FPGA_DATA_FIX add_res1;
							if(too<Tm/2)
								add_res1 = WEIGHT1[0][too][tii][i][j]*IFM[0][tii][trr+i][tcc+j];
							else
								add_res1 = WEIGHT1[1][too][tii][i][j]*IFM[1][tii][trr+i][tcc+j];
							OFM[too][trr][tcc] = OFM[too][trr][tcc] + add_res1;
						}
					}
				}
			}
		}
	}
}

//FPGA_DATA Batch_Norm(
//		FPGA_DATA input,
//		FPGA_DATA E[Tm],
//		FPGA_DATA V[Tm],
//		int idx,
//		float eps,
//		float gamma,
//		float beta,
//		bool is_batch){
//	if(is_batch==false){
//		return input;
//	}else{
//		return (input-E[idx])/sqrt(V[idx]+eps)*gamma+beta;
//	}
//}

void OFM_STORE_NONLINEAR(hls::stream<DMA_DATA_128B_FIX> &output_dma_O,
		FPGA_DATA_FIX OFM[Tm][Tr][Tc],
		FPGA_DATA_FIX BIAS[Tm],
		int custom_Tr,
		int custom_Tc,
		int custom_Tm,
		int NL_Opt){

	DMA_DATA_128B_FIX output_dma_O_data;

	if(NL_Opt==0){
		// No Non-Linear Applied
		n_ofm1:for(int i=0;i<divided_Tm_8;i++){
			n_ofm2:for(int j=0;j<custom_Tr;j++){
				n_ofm3:for(int k=0;k<custom_Tc;k++){
	#pragma HLS PIPELINE II=1
					output_dma_O_data.last=0;
					if (i==divided_Tm_8-1 && j==custom_Tr-1 && k==custom_Tc-1){
						output_dma_O_data.last=1;
						}
					if(i*8<custom_Tm)
						output_dma_O_data.data.data1=OFM[i*8][j][k]+BIAS[i*8];
					if(i*8+1<custom_Tm)
						output_dma_O_data.data.data2=OFM[i*8+1][j][k]+BIAS[i*8+1];
					if(i*8+2<custom_Tm)
						output_dma_O_data.data.data3=OFM[i*8+2][j][k]+BIAS[i*8+2];
					if(i*8+3<custom_Tm)
						output_dma_O_data.data.data4=OFM[i*8+3][j][k]+BIAS[i*8+3];
					if(i*8+4<custom_Tm)
						output_dma_O_data.data.data5=OFM[i*8+4][j][k]+BIAS[i*8+4];
					if(i*8+5<custom_Tm)
						output_dma_O_data.data.data6=OFM[i*8+5][j][k]+BIAS[i*8+5];
					if(i*8+6<custom_Tm)
						output_dma_O_data.data.data7=OFM[i*8+6][j][k]+BIAS[i*8+6];
					if(i*8+7<custom_Tm)
						output_dma_O_data.data.data8=OFM[i*8+7][j][k]+BIAS[i*8+7];
					output_dma_O.write(output_dma_O_data);
				}
			}
		}

	}else if(NL_Opt==1){
		// ReLU
		r_ofm1:for(int i=0;i<divided_Tm_8;i++){
			r_ofm2:for(int j=0;j<custom_Tr;j++){
				r_ofm3:for(int k=0;k<custom_Tc;k++){
	#pragma HLS PIPELINE II=1
					FPGA_DATA_FIX T;
					output_dma_O_data.last=0;
					if (i==divided_Tm_8-1 && j==custom_Tr-1 && k==custom_Tc-1){
						output_dma_O_data.last=1;
						}
					if(i*8+0<custom_Tm){
						T = OFM[i*8+0][j][k]+BIAS[i*8+0];
						output_dma_O_data.data.data1=T>=0?T:FPGA_DATA_FIX(0);
					}
						//output_dma_O_data.data.data1=OFM[i*8+0][j][k]+BIAS[i*8+0]>=0?OFM[i*8+0][j][k]+BIAS[i*8+0]:0;
					if(i*8+1<custom_Tm){
						T = OFM[i*8+1][j][k]+BIAS[i*8+1];
						output_dma_O_data.data.data2=T>=0?T:FPGA_DATA_FIX(0);
					}
						//output_dma_O_data.data.data2=OFM[i*8+1][j][k]+BIAS[i*8+1]>=0?OFM[i*8+1][j][k]+BIAS[i*8+1]:Z;
					if(i*8+2<custom_Tm){
						T = OFM[i*8+2][j][k]+BIAS[i*8+2];
						output_dma_O_data.data.data3=T>=0?T:FPGA_DATA_FIX(0);
					}
						//output_dma_O_data.data.data3=OFM[i*8+2][j][k]+BIAS[i*8+2]>=0?OFM[i*8+2][j][k]+BIAS[i*8+2]:Z;
					if(i*8+3<custom_Tm){
						T = OFM[i*8+3][j][k]+BIAS[i*8+3];
						output_dma_O_data.data.data4=T>=0?T:FPGA_DATA_FIX(0);
					}
						//output_dma_O_data.data.data4=OFM[i*8+3][j][k]+BIAS[i*8+3]>=0?OFM[i*8+3][j][k]+BIAS[i*8+3]:Z;
					if(i*8+4<custom_Tm){
						T = OFM[i*8+4][j][k]+BIAS[i*8+4];
						output_dma_O_data.data.data5=T>=0?T:FPGA_DATA_FIX(0);
					}
						//output_dma_O_data.data.data5=OFM[i*8+4][j][k]+BIAS[i*8+4]>=0?OFM[i*8+4][j][k]+BIAS[i*8+4]:Z;
					if(i*8+5<custom_Tm){
						T = OFM[i*8+5][j][k]+BIAS[i*8+5];
						output_dma_O_data.data.data6=T>=0?T:FPGA_DATA_FIX(0);
					}
						//output_dma_O_data.data.data6=OFM[i*8+5][j][k]+BIAS[i*8+5]>=0?OFM[i*8+5][j][k]+BIAS[i*8+5]:Z;
					if(i*8+6<custom_Tm){
						T = OFM[i*8+6][j][k]+BIAS[i*8+6];
						output_dma_O_data.data.data7=T>=0?T:FPGA_DATA_FIX(0);
					}
						//output_dma_O_data.data.data7=OFM[i*8+6][j][k]+BIAS[i*8+6]>=0?OFM[i*8+6][j][k]+BIAS[i*8+6]:Z;
					if(i*8+7<custom_Tm){
						T = OFM[i*8+7][j][k]+BIAS[i*8+7];
						output_dma_O_data.data.data8=T>=0?T:FPGA_DATA_FIX(0);
					}
						//output_dma_O_data.data.data8=OFM[i*8+7][j][k]+BIAS[i*8+7]>=0?OFM[i*8+7][j][k]+BIAS[i*8+7]:Z;
					output_dma_O.write(output_dma_O_data);
				}
			}
		}
	}else if(NL_Opt==2){
		// h-switch
		hs_ofm1:for(int i=0;i<divided_Tm_8;i++){
			hs_ofm2:for(int j=0;j<custom_Tr;j++){
				hs_ofm3:for(int k=0;k<custom_Tc;k++){
	#pragma HLS PIPELINE II=1
					output_dma_O_data.last=0;
					FPGA_DATA_FIX T;
					if (i==divided_Tm_8-1 && j==custom_Tr-1 && k==custom_Tc-1){
						output_dma_O_data.last=1;
					}
					if(i*8+0<custom_Tm){
						T=OFM[i*8+0][j][k]+BIAS[i*8+0]+3;
						FPGA_DATA_FIX d1 = T>=0?T:FPGA_DATA_FIX(0);
						d1 = (d1<=6?d1:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+0][j][k]+BIAS[i*8+0]);
						output_dma_O_data.data.data1=d1;
					}
					if(i*8+1<custom_Tm){
						T=OFM[i*8+1][j][k]+BIAS[i*8+1]+3;
						FPGA_DATA_FIX d2 = T>=0?T:FPGA_DATA_FIX(0);
						//FPGA_DATA_FIX d2 = OFM[i*8+1][j][k]+BIAS[i*8+1]+3>=Z?OFM[i*8+1][j][k]+BIAS[i*8+1]+3:Z;
						d2 = (d2<=6?d2:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+1][j][k]+BIAS[i*8+1]);
						output_dma_O_data.data.data2=d2;
					}
					if(i*8+2<custom_Tm){
						T=OFM[i*8+2][j][k]+BIAS[i*8+2]+3;
						FPGA_DATA_FIX d3 = T>=0?T:FPGA_DATA_FIX(0);
						//FPGA_DATA_FIX d3 = OFM[i*8+2][j][k]+BIAS[i*8+2]+3>=Z?OFM[i*8+2][j][k]+BIAS[i*8+2]+3:Z;
						d3 = (d3<=6?d3:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+2][j][k]+BIAS[i*8+2]);
						output_dma_O_data.data.data3=d3;
					}
					if(i*8+3<custom_Tm){
						T=OFM[i*8+3][j][k]+BIAS[i*8+3]+3;
						FPGA_DATA_FIX d4 = T>=0?T:FPGA_DATA_FIX(0);
						//FPGA_DATA_FIX d4 = OFM[i*8+3][j][k]+BIAS[i*8+3]+3>=Z?OFM[i*8+3][j][k]+BIAS[i*8+3]+3:Z;
						d4 = (d4<=6?d4:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+3][j][k]+BIAS[i*8+3]);
						output_dma_O_data.data.data4=d4;
					}
					if(i*8+4<custom_Tm){
						T=OFM[i*8+4][j][k]+BIAS[i*8+4]+3;
						FPGA_DATA_FIX d5 = T>=0?T:FPGA_DATA_FIX(0);
						//FPGA_DATA_FIX d5 = OFM[i*8+4][j][k]+BIAS[i*8+4]+3>=Z?OFM[i*8+4][j][k]+BIAS[i*8+4]+3:Z;
						d5 = (d5<=6?d5:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+4][j][k]+BIAS[i*8+4]);
						output_dma_O_data.data.data5=d5;
					}
					if(i*8+5<custom_Tm){
						T=OFM[i*8+5][j][k]+BIAS[i*8+5]+3;
						FPGA_DATA_FIX d6 = T>=0?T:FPGA_DATA_FIX(0);
						//FPGA_DATA_FIX d6 = OFM[i*8+5][j][k]+BIAS[i*8+5]+3>=Z?OFM[i*8+5][j][k]+BIAS[i*8+5]+3:Z;
						d6 = (d6<=6?d6:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+5][j][k]+BIAS[i*8+5]);
						output_dma_O_data.data.data6=d6;
					}
					if(i*8+6<custom_Tm){
						T=OFM[i*8+6][j][k]+BIAS[i*8+6]+3;
						FPGA_DATA_FIX d7 = T>=0?T:FPGA_DATA_FIX(0);
						//FPGA_DATA_FIX d7 = OFM[i*8+6][j][k]+BIAS[i*8+6]+3>=Z?OFM[i*8+6][j][k]+BIAS[i*8+6]+3:Z;
						d7 = (d7<=6?d7:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+6][j][k]+BIAS[i*8+6]);
						output_dma_O_data.data.data7=d7;
					}
					if(i*8+7<custom_Tm){
						T=OFM[i*8+7][j][k]+BIAS[i*8+7]+3;
						FPGA_DATA_FIX d8 = T>=0?T:FPGA_DATA_FIX(0);
						//FPGA_DATA_FIX d8 = OFM[i*8+7][j][k]+BIAS[i*8+7]+3>=Z?OFM[i*8+7][j][k]+BIAS[i*8+7]+3:Z;
						d8 = (d8<=6?d8:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+7][j][k]+BIAS[i*8+7]);
						output_dma_O_data.data.data8=d8;
					}
					output_dma_O.write(output_dma_O_data);
				}
			}
		}
	}

}


void OFM_STORE( hls::stream<DMA_DATA_128B_FIX> &output_dma_O,
				hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
				FPGA_DATA_FIX OFM[Tm][Tr][Tc],
				FPGA_DATA_FIX BIAS[Tm],
				int custom_Tr,
				int custom_Tc,
				int custom_Tm,
				int NL_Opt){



	DMA_DATA_128B_FIX bias_input_dma;
	bias1:for(int i=0;i<divided_Tm_8;i++){
#pragma HLS PIPELINE II=1
		bias_input_dma=input_dma_B.read();
		BIAS[i*8]=bias_input_dma.data.data1;
		BIAS[i*8+1]=bias_input_dma.data.data2;
		BIAS[i*8+2]=bias_input_dma.data.data3;
		BIAS[i*8+3]=bias_input_dma.data.data4;
		BIAS[i*8+4]=bias_input_dma.data.data5;
		BIAS[i*8+5]=bias_input_dma.data.data6;
		BIAS[i*8+6]=bias_input_dma.data.data7;
		BIAS[i*8+7]=bias_input_dma.data.data8;
	}

	OFM_STORE_NONLINEAR(output_dma_O,OFM,BIAS,custom_Tr,custom_Tc,custom_Tm,NL_Opt);

	bias2:for(int j=0;j<custom_Tr;j++){
		for(int k=0;k<custom_Tc;k++){
#pragma HLS PIPELINE II=1
			for(int i=0;i<Tm;i++){
				OFM[i][j][k] = 0;
			}
		}
	}

}

void Load_Fire(hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		FPGA_DATA_FIX WEIGHT[2][2][Tm][Tn][K][K],
		FPGA_DATA_FIX IFM1[2][Tn_8][Tr+K-1][Tc+K-1],
		FPGA_DATA_FIX IFM2[2][Tn_8][Tr+K-1][Tc+K-1],
		FPGA_DATA_FIX OFM[Tm][Tr][Tc],
		int row,
		int col,
		int N,
		int custom_k,
		int custom_Tr,
		int custom_Tc,
		int custom_Tm,
		int custom_Tn
		){
	static int idx = 0;
	// N+1 since each inner pipeline needs an additional iteration.
	// Bug Fix: Weiwen-01-23: N+1 -> N+Tn because we need to guarantee one more execution
	// This is because the first execution we do not have data, thus fire cannot store data
	// We transmit 0 for IFM and WEI in the last iteration
	for(int i=0;i<N+Tn;i+=Tn){
#pragma HLS loop_tripcount min=192 max=192 avg=192
#pragma HLS dependence variable=WEIGHT intra false
		if(idx%2==0){
			LOAD_WEIGHT_128B(input_dma_W,WEIGHT[0],custom_k,custom_Tm,custom_Tn);
			LOAD_IFM(input_dma_I,IFM1,custom_Tr,custom_Tc,custom_k,custom_Tn);
			FIRE(WEIGHT[1],IFM2,OFM, row, col, custom_k,custom_Tr,custom_Tc,custom_Tm,custom_Tn);
		}else{
			LOAD_WEIGHT_128B(input_dma_W,WEIGHT[1], custom_k,custom_Tm,custom_Tn);
			LOAD_IFM(input_dma_I,IFM2,custom_Tr,custom_Tc,custom_k,custom_Tn);
			FIRE(WEIGHT[0],IFM1,OFM, row, col, custom_k,custom_Tr,custom_Tc,custom_Tm,custom_Tn);
		}
		idx+=1;
	}
}

void cconv(hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
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
		){

#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=row bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=col bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=num bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=N   bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=custom_k   bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=custom_Tr   bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=custom_Tc  bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=custom_Tm   bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=custom_Tn  bundle=CRTL_BUS


#pragma HLS INTERFACE axis port=input_dma_W
#pragma HLS INTERFACE axis port=input_dma_I
#pragma HLS INTERFACE axis port=input_dma_B
#pragma HLS INTERFACE axis port=output_dma_O

	static FPGA_DATA_FIX WEIGHT1[2][2][Tm][Tn][K][K];
#pragma HLS ARRAY_PARTITION variable=WEIGHT1 complete dim=3
#pragma HLS ARRAY_PARTITION variable=WEIGHT1 complete dim=4


	static FPGA_DATA_FIX IFM[2][Tn_8][Tr+K-1][Tc+K-1];
#pragma HLS ARRAY_PARTITION variable=IFM complete dim=1
#pragma HLS ARRAY_PARTITION variable=IFM complete dim=2

	static FPGA_DATA_FIX IFM_DB[2][Tn_8][Tr+K-1][Tc+K-1];
#pragma HLS ARRAY_PARTITION variable=IFM_DB complete dim=1
#pragma HLS ARRAY_PARTITION variable=IFM_DB complete dim=2

	static FPGA_DATA_FIX OFM[Tm][Tr][Tc];
#pragma HLS RESOURCE variable=OFM core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=OFM complete dim=1

	static FPGA_DATA_FIX OFM_DB[Tm][Tr][Tc];
#pragma HLS RESOURCE variable=OFM_DB core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=OFM_DB complete dim=1

	static FPGA_DATA_FIX BIAS[Tm];
#pragma HLS ARRAY_PARTITION variable=BIAS complete dim=1
	static FPGA_DATA_FIX BIAS_DB[Tm];
#pragma HLS ARRAY_PARTITION variable=BIAS_DB complete dim=1

	if(num%2==0){
		Load_Fire(input_dma_W,input_dma_I,WEIGHT1,IFM,IFM_DB,OFM_DB,row,col,N,custom_k,custom_Tr,custom_Tc,custom_Tm,custom_Tn);
		OFM_STORE(output_dma_O,input_dma_B, OFM, BIAS,custom_Tr,custom_Tc,custom_Tm,NL_Opt);
	}else{
		Load_Fire(input_dma_W,input_dma_I,WEIGHT1,IFM,IFM_DB,OFM,row,col,N,custom_k,custom_Tr,custom_Tc,custom_Tm,custom_Tn);
		OFM_STORE(output_dma_O,input_dma_B, OFM_DB, BIAS_DB,custom_Tr,custom_Tc,custom_Tm,NL_Opt);
	}

}








