#include "source_fix.h"
#include <math.h>



void avgpool_LOAD_IFM(hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		FPGA_DATA_FIX IFM[2][Tn_8][K_avgpool*Tr_avgpool][K_avgpool*Tc_avgpool],
		int custom_Tr,
		int custom_Tc,
		int custom_k,
		int custom_Tn
		){

	// Weiwen-01-23: Fix bug custom_Tr+K-1 ---> custom_Tr+custom_k-1 for self-defined K
	DMA_DATA_128B_FIX ifm_input_dma;
	ifm1:for(int i=0;i<divided_Tn_8;i++){
		ifm2:for(int j=0;j<K_avgpool*custom_Tr;j++){
			ifm3:for(int m=0;m<K_avgpool*custom_Tc;m++){
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


void avgpool_FIRE(FPGA_DATA_FIX IFM[2][Tn_8][K_avgpool*Tr_avgpool][K_avgpool*Tc_avgpool],
			FPGA_DATA_FIX OFM[Tm][Tr_avgpool][Tc_avgpool],
			int row,
			int col,
			int custom_Tr,
			int custom_Tc,
			int custom_Tm,
			int custom_Tn
			){
	for(int trr=row;(trr<(row+custom_Tr));trr++){
#pragma HLS loop_tripcount min=1 max=Tr avg=Tr
		for(int tcc=col;(tcc<(col+custom_Tc));tcc++){
#pragma HLS loop_tripcount min=1 max=Tc avg=Tc
			for(int i=0;i<K_avgpool;i++){
				for(int j=0;j<K_avgpool;j++){
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=OFM inter false
					for(int tii=0;tii<custom_Tn;tii++){
						if(tii<Tn/2){
							if(i==0 && j==0)
								OFM[tii][trr][tcc] = IFM[0][tii][K_avgpool*trr+i][K_avgpool*tcc+j];
							else{
								OFM[tii][trr][tcc] = IFM[0][tii][K_avgpool*trr+i][K_avgpool*tcc+j]+OFM[tii][trr][tcc];
								if(i==K_avgpool-1 && j==K_avgpool-1)
									OFM[tii][trr][tcc] = OFM[tii][trr][tcc]/K_avgpool/K_avgpool;
							}
						}
						else{
							if(i==0 && j==0)
								OFM[tii][trr][tcc] = IFM[1][tii][K_avgpool*trr+i][K_avgpool*tcc+j];
							else{
								OFM[tii][trr][tcc] = IFM[1][tii][K_avgpool*trr+i][K_avgpool*tcc+j]+OFM[tii][trr][tcc];
								if(i==K_avgpool-1 && j==K_avgpool-1)
									OFM[tii][trr][tcc] = OFM[tii][trr][tcc]/K_avgpool/K_avgpool;
							}
						}
					}
				}
			}
		}
	}
}



void avgpool_OFM_STORE_NONLINEAR(hls::stream<DMA_DATA_128B_FIX> &output_dma_O,
		FPGA_DATA_FIX OFM[Tm][Tr_avgpool][Tc_avgpool],

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
						output_dma_O_data.data.data1=OFM[i*8][j][k];
					if(i*8+1<custom_Tm)
						output_dma_O_data.data.data2=OFM[i*8+1][j][k];
					if(i*8+2<custom_Tm)
						output_dma_O_data.data.data3=OFM[i*8+2][j][k];
					if(i*8+3<custom_Tm)
						output_dma_O_data.data.data4=OFM[i*8+3][j][k];
					if(i*8+4<custom_Tm)
						output_dma_O_data.data.data5=OFM[i*8+4][j][k];
					if(i*8+5<custom_Tm)
						output_dma_O_data.data.data6=OFM[i*8+5][j][k];
					if(i*8+6<custom_Tm)
						output_dma_O_data.data.data7=OFM[i*8+6][j][k];
					if(i*8+7<custom_Tm)
						output_dma_O_data.data.data8=OFM[i*8+7][j][k];
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
						T = OFM[i*8+0][j][k];
						output_dma_O_data.data.data1=T>=0?T:FPGA_DATA_FIX(0);
					}
						//output_dma_O_data.data.data1=OFM[i*8+0][j][k]+BIAS[i*8+0]>=0?OFM[i*8+0][j][k]+BIAS[i*8+0]:0;
					if(i*8+1<custom_Tm){
						T = OFM[i*8+1][j][k];
						output_dma_O_data.data.data2=T>=0?T:FPGA_DATA_FIX(0);
					}
						//output_dma_O_data.data.data2=OFM[i*8+1][j][k]+BIAS[i*8+1]>=0?OFM[i*8+1][j][k]+BIAS[i*8+1]:Z;
					if(i*8+2<custom_Tm){
						T = OFM[i*8+2][j][k];
						output_dma_O_data.data.data3=T>=0?T:FPGA_DATA_FIX(0);
					}
						//output_dma_O_data.data.data3=OFM[i*8+2][j][k]+BIAS[i*8+2]>=0?OFM[i*8+2][j][k]+BIAS[i*8+2]:Z;
					if(i*8+3<custom_Tm){
						T = OFM[i*8+3][j][k];
						output_dma_O_data.data.data4=T>=0?T:FPGA_DATA_FIX(0);
					}
						//output_dma_O_data.data.data4=OFM[i*8+3][j][k]+BIAS[i*8+3]>=0?OFM[i*8+3][j][k]+BIAS[i*8+3]:Z;
					if(i*8+4<custom_Tm){
						T = OFM[i*8+4][j][k];
						output_dma_O_data.data.data5=T>=0?T:FPGA_DATA_FIX(0);
					}
						//output_dma_O_data.data.data5=OFM[i*8+4][j][k]+BIAS[i*8+4]>=0?OFM[i*8+4][j][k]+BIAS[i*8+4]:Z;
					if(i*8+5<custom_Tm){
						T = OFM[i*8+5][j][k];
						output_dma_O_data.data.data6=T>=0?T:FPGA_DATA_FIX(0);
					}
						//output_dma_O_data.data.data6=OFM[i*8+5][j][k]+BIAS[i*8+5]>=0?OFM[i*8+5][j][k]+BIAS[i*8+5]:Z;
					if(i*8+6<custom_Tm){
						T = OFM[i*8+6][j][k];
						output_dma_O_data.data.data7=T>=0?T:FPGA_DATA_FIX(0);
					}
						//output_dma_O_data.data.data7=OFM[i*8+6][j][k]+BIAS[i*8+6]>=0?OFM[i*8+6][j][k]+BIAS[i*8+6]:Z;
					if(i*8+7<custom_Tm){
						T = OFM[i*8+7][j][k];
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
						T=OFM[i*8+0][j][k]+3;
						FPGA_DATA_FIX d1 = T>=0?T:FPGA_DATA_FIX(0);
						d1 = (d1<=6?d1:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+0][j][k]);
						output_dma_O_data.data.data1=d1;
					}
					if(i*8+1<custom_Tm){
						T=OFM[i*8+1][j][k]+3;
						FPGA_DATA_FIX d2 = T>=0?T:FPGA_DATA_FIX(0);
						//FPGA_DATA_FIX d2 = OFM[i*8+1][j][k]+BIAS[i*8+1]+3>=Z?OFM[i*8+1][j][k]+BIAS[i*8+1]+3:Z;
						d2 = (d2<=6?d2:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+1][j][k]);
						output_dma_O_data.data.data2=d2;
					}
					if(i*8+2<custom_Tm){
						T=OFM[i*8+2][j][k]+3;
						FPGA_DATA_FIX d3 = T>=0?T:FPGA_DATA_FIX(0);
						//FPGA_DATA_FIX d3 = OFM[i*8+2][j][k]+BIAS[i*8+2]+3>=Z?OFM[i*8+2][j][k]+BIAS[i*8+2]+3:Z;
						d3 = (d3<=6?d3:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+2][j][k]);
						output_dma_O_data.data.data3=d3;
					}
					if(i*8+3<custom_Tm){
						T=OFM[i*8+3][j][k]+3;
						FPGA_DATA_FIX d4 = T>=0?T:FPGA_DATA_FIX(0);
						//FPGA_DATA_FIX d4 = OFM[i*8+3][j][k]+BIAS[i*8+3]+3>=Z?OFM[i*8+3][j][k]+BIAS[i*8+3]+3:Z;
						d4 = (d4<=6?d4:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+3][j][k]);
						output_dma_O_data.data.data4=d4;
					}
					if(i*8+4<custom_Tm){
						T=OFM[i*8+4][j][k]+3;
						FPGA_DATA_FIX d5 = T>=0?T:FPGA_DATA_FIX(0);
						//FPGA_DATA_FIX d5 = OFM[i*8+4][j][k]+BIAS[i*8+4]+3>=Z?OFM[i*8+4][j][k]+BIAS[i*8+4]+3:Z;
						d5 = (d5<=6?d5:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+4][j][k]);
						output_dma_O_data.data.data5=d5;
					}
					if(i*8+5<custom_Tm){
						T=OFM[i*8+5][j][k]+3;
						FPGA_DATA_FIX d6 = T>=0?T:FPGA_DATA_FIX(0);
						//FPGA_DATA_FIX d6 = OFM[i*8+5][j][k]+BIAS[i*8+5]+3>=Z?OFM[i*8+5][j][k]+BIAS[i*8+5]+3:Z;
						d6 = (d6<=6?d6:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+5][j][k]);
						output_dma_O_data.data.data6=d6;
					}
					if(i*8+6<custom_Tm){
						T=OFM[i*8+6][j][k]+3;
						FPGA_DATA_FIX d7 = T>=0?T:FPGA_DATA_FIX(0);
						//FPGA_DATA_FIX d7 = OFM[i*8+6][j][k]+BIAS[i*8+6]+3>=Z?OFM[i*8+6][j][k]+BIAS[i*8+6]+3:Z;
						d7 = (d7<=6?d7:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+6][j][k]);
						output_dma_O_data.data.data7=d7;
					}
					if(i*8+7<custom_Tm){
						T=OFM[i*8+7][j][k]+3;
						FPGA_DATA_FIX d8 = T>=0?T:FPGA_DATA_FIX(0);
						//FPGA_DATA_FIX d8 = OFM[i*8+7][j][k]+BIAS[i*8+7]+3>=Z?OFM[i*8+7][j][k]+BIAS[i*8+7]+3:Z;
						d8 = (d8<=6?d8:FPGA_DATA_FIX(6))/FPGA_DATA_FIX(6)*(OFM[i*8+7][j][k]);
						output_dma_O_data.data.data8=d8;
					}
					output_dma_O.write(output_dma_O_data);
				}
			}
		}
	}

}


void avgpool_OFM_STORE( hls::stream<DMA_DATA_128B_FIX> &output_dma_O,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		FPGA_DATA_FIX OFM[Tm][Tr_avgpool][Tc_avgpool],
		int custom_Tr,
		int custom_Tc,
		int custom_Tm,
		int NL_Opt){


	avgpool_OFM_STORE_NONLINEAR(output_dma_O,OFM,custom_Tr,custom_Tc,custom_Tm,NL_Opt);

	bias2:for(int j=0;j<custom_Tr;j++){
		for(int k=0;k<custom_Tc;k++){
#pragma HLS PIPELINE II=1
			for(int i=0;i<Tm;i++){
				OFM[i][j][k] = 0;
			}
		}
	}

}

void avgpool_Load_Fire(hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		FPGA_DATA_FIX IFM1[2][Tn_8][K_avgpool*Tr_avgpool][K_avgpool*Tc_avgpool],
		FPGA_DATA_FIX IFM2[2][Tn_8][K_avgpool*Tr_avgpool][K_avgpool*Tc_avgpool],
		FPGA_DATA_FIX OFM[Tm][Tr_avgpool][Tc_avgpool],
		int row,
		int col,
		int N,
		int custom_k,
		int custom_Tr,
		int custom_Tc,
		int custom_Tm,
		int custom_Tn
		){
	avgpool_LOAD_IFM(input_dma_I, IFM1, custom_Tr, custom_Tc, custom_k, custom_Tn);
	avgpool_FIRE(IFM1, OFM, row, col, custom_Tr, custom_Tc, custom_Tm, custom_Tn);
	}


void avgpool(hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
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



	static FPGA_DATA_FIX IFM[2][Tn_8][K_avgpool*Tr_avgpool][K_avgpool*Tc_avgpool];
#pragma HLS ARRAY_PARTITION variable=IFM complete dim=1
#pragma HLS ARRAY_PARTITION variable=IFM complete dim=2

	static FPGA_DATA_FIX IFM_DB[2][Tn_8][K_avgpool*Tr_avgpool][K_avgpool*Tc_avgpool];
#pragma HLS ARRAY_PARTITION variable=IFM_DB complete dim=1
#pragma HLS ARRAY_PARTITION variable=IFM_DB complete dim=2

	static FPGA_DATA_FIX OFM[Tm][Tr_avgpool][Tc_avgpool];
#pragma HLS RESOURCE variable=OFM core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=OFM complete dim=1

	static FPGA_DATA_FIX OFM_DB[Tm][Tr_avgpool][Tc_avgpool];
#pragma HLS RESOURCE variable=OFM_DB core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=OFM_DB complete dim=1


	if(num%2==0){
		avgpool_Load_Fire(input_dma_W,input_dma_I,IFM,IFM_DB,OFM_DB,row,col,N,custom_k,custom_Tr,custom_Tc,custom_Tm,custom_Tn);
		avgpool_OFM_STORE(output_dma_O,input_dma_B, OFM, custom_Tr,custom_Tc,custom_Tm,NL_Opt);
	}else{
		avgpool_Load_Fire(input_dma_W,input_dma_I,IFM,IFM_DB,OFM,row,col,N,custom_k,custom_Tr,custom_Tc,custom_Tm,custom_Tn);
		avgpool_OFM_STORE(output_dma_O,input_dma_B, OFM_DB,custom_Tr,custom_Tc,custom_Tm,NL_Opt);
	}

}







