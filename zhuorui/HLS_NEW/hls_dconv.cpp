#include "source_fix.h"

void dconv_LOAD_WEIGHT_128B(hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
						FPGA_DATA_FIX WEIGHT1[2][Tn_dconv][K_dconv][K_dconv],
						int custom_k,
						int custom_s){


	DMA_DATA_128B_FIX weight_input_dma;
	weight1:for(int i=0;i<divided_Tn_8_dconv;i++){
		weight3:for(int m=0;m<custom_k;m++){
#pragma HLS loop_tripcount min=3 max=11 avg=3
			weight4: for(int l=0;l<custom_k;l++){
#pragma HLS loop_tripcount min=3 max=11 avg=3
#pragma HLS PIPELINE II=1
				weight_input_dma=input_dma_W.read();

				if(i<divided_Tn_8_dconv/2){
					if(i*8<Tn_dconv)
						WEIGHT1[0][i*8][m][l]=weight_input_dma.data.data1;
					if(i*8+1<Tn_dconv)
						WEIGHT1[0][i*8+1][m][l]=weight_input_dma.data.data2;
					if(i*8+2<Tn_dconv)
						WEIGHT1[0][i*8+2][m][l]=weight_input_dma.data.data3;
					if(i*8+3<Tn_dconv)
						WEIGHT1[0][i*8+3][m][l]=weight_input_dma.data.data4;
					if(i*8+4<Tn_dconv)
						WEIGHT1[0][i*8+4][m][l]=weight_input_dma.data.data5;
					if(i*8+5<Tn_dconv)
						WEIGHT1[0][i*8+5][m][l]=weight_input_dma.data.data6;
					if(i*8+6<Tn_dconv)
						WEIGHT1[0][i*8+6][m][l]=weight_input_dma.data.data7;
					if(i*8+7<Tn_dconv)
						WEIGHT1[0][i*8+7][m][l]=weight_input_dma.data.data8;

				}else{
					if(i*8<Tn_dconv)
						WEIGHT1[1][i*8][m][l]=weight_input_dma.data.data1;
					if(i*8+1<Tn_dconv)
						WEIGHT1[1][i*8+1][m][l]=weight_input_dma.data.data2;
					if(i*8+2<Tn_dconv)
						WEIGHT1[1][i*8+2][m][l]=weight_input_dma.data.data3;
					if(i*8+3<Tn_dconv)
						WEIGHT1[1][i*8+3][m][l]=weight_input_dma.data.data4;
					if(i*8+4<Tn_dconv)
						WEIGHT1[1][i*8+4][m][l]=weight_input_dma.data.data5;
					if(i*8+5<Tn_dconv)
						WEIGHT1[1][i*8+5][m][l]=weight_input_dma.data.data6;
					if(i*8+6<Tn_dconv)
						WEIGHT1[1][i*8+6][m][l]=weight_input_dma.data.data7;
					if(i*8+7<Tn_dconv)
						WEIGHT1[1][i*8+7][m][l]=weight_input_dma.data.data8;

				}
			}
		}
	}
}


void dconv_LOAD_IFM(hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		FPGA_DATA_FIX IFM[2][Tn_dconv][2*(Tr_dconv-1)+K_dconv][2*(Tc_dconv-1)+K_dconv],
		int custom_Tr,
		int custom_Tc,
		int custom_s){

	DMA_DATA_128B_FIX ifm_input_dma;
	ifm1:for(int i=0;i<divided_Tn_8_dconv;i++){
		ifm2:for(int j=0;j<(custom_Tr-1)*custom_s+K_dconv;j++){			//change
			ifm3:for(int m=0;m<(custom_Tc-1)*custom_s+K_dconv;m++){		//change
	#pragma HLS PIPELINE II=1
				ifm_input_dma=input_dma_I.read();
				if(i*8<Tn_dconv){
					IFM[0][i*8][j][m]=ifm_input_dma.data.data1;
					IFM[1][i*8][j][m]=ifm_input_dma.data.data1;
				}
				if(i*8+1<Tn_dconv){
					IFM[0][i*8+1][j][m]=ifm_input_dma.data.data2;
					IFM[1][i*8+1][j][m]=ifm_input_dma.data.data2;
				}
				if(i*8+2<Tn_dconv){
					IFM[0][i*8+2][j][m]=ifm_input_dma.data.data3;
					IFM[1][i*8+2][j][m]=ifm_input_dma.data.data3;
				}
				if(i*8+3<Tn_dconv){
					IFM[0][i*8+3][j][m]=ifm_input_dma.data.data4;
					IFM[1][i*8+3][j][m]=ifm_input_dma.data.data4;
				}
				if(i*8+4<Tn_dconv){
					IFM[0][i*8+4][j][m]=ifm_input_dma.data.data5;
					IFM[1][i*8+4][j][m]=ifm_input_dma.data.data5;
				}
				if(i*8+5<Tn_dconv){
					IFM[0][i*8+5][j][m]=ifm_input_dma.data.data6;
					IFM[1][i*8+5][j][m]=ifm_input_dma.data.data6;
				}
				if(i*8+6<Tn_dconv){
					IFM[0][i*8+6][j][m]=ifm_input_dma.data.data7;
					IFM[1][i*8+6][j][m]=ifm_input_dma.data.data7;
				}
				if(i*8+7<Tn_dconv){
					IFM[0][i*8+7][j][m]=ifm_input_dma.data.data8;
					IFM[1][i*8+7][j][m]=ifm_input_dma.data.data8;
				}
			}
		}
	}
}


void dconv_FIRE(  FPGA_DATA_FIX WEIGHT1[2][Tn_dconv][K_dconv][K_dconv],
			FPGA_DATA_FIX IFM[2][Tn_dconv][2*(Tr_dconv-1)+K_dconv][2*(Tc_dconv-1)+K_dconv],
			FPGA_DATA_FIX OFM[Tn_dconv][Tr_dconv][Tc_dconv],
			int row,
			int col,
			int custom_k,
			int custom_s,
			int custom_Tr,
			int custom_Tc){
	for(int i=0;i<custom_k;i++){
#pragma HLS loop_tripcount min=3 max=11 avg=3
		for(int j=0;j<custom_k;j++){
#pragma HLS loop_tripcount min=3 max=11 avg=3
			for(int trr=row;(trr<row+custom_Tr);trr++){
#pragma HLS loop_tripcount min=1 max=Tr_dconv avg=Tr_dconv
				for(int tcc=col;(tcc<col+custom_Tc);tcc++){
#pragma HLS loop_tripcount min=1 max=Tc_dconv avg=Tc_dconv
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=OFM inter false
					for(int tii=0;tii<Tn_dconv;tii++){
						FPGA_DATA_FIX add_res1;
						if(tii<Tn_dconv/2)
							add_res1 = WEIGHT1[0][tii][i][j]*IFM[0][tii][custom_s*trr+i][custom_s*tcc+j];//custom_s
						else
							add_res1 = WEIGHT1[1][tii][i][j]*IFM[1][tii][custom_s*trr+i][custom_s*tcc+j];//custom_s
						OFM[tii][trr][tcc] = OFM[tii][trr][tcc] + add_res1;
					}
				}
			}
		}
	}
}





void dconv_OFM_STORE_NONLINEAR(hls::stream<DMA_DATA_128B_FIX> &output_dma_O,
		FPGA_DATA_FIX OFM[Tn_dconv][Tr_dconv][Tc_dconv],
		FPGA_DATA_FIX BIAS[Tn_dconv],
		int custom_Tr,
		int custom_Tc,
		int NL_Opt){

	DMA_DATA_128B_FIX output_dma_O_data;

	if(NL_Opt==0){
		// No Non-Linear Applied
		dconv_n_ofm1:for(int i=0;i<divided_Tn_8_dconv;i++){
			dconv_n_ofm2:for(int j=0;j<custom_Tr;j++){
				dconv_n_ofm3:for(int K_dconv=0;K_dconv<custom_Tc;K_dconv++){
	#pragma HLS PIPELINE II=1
					output_dma_O_data.last=0;
					if (i==divided_Tn_8_dconv-1 && j==custom_Tr-1 && K_dconv==custom_Tc-1){
						output_dma_O_data.last=1;
						}
					output_dma_O_data.data.data1=OFM[i*8][j][K_dconv]+BIAS[i*8];
					output_dma_O_data.data.data2=OFM[i*8+1][j][K_dconv]+BIAS[i*8+1];
					output_dma_O_data.data.data3=OFM[i*8+2][j][K_dconv]+BIAS[i*8+2];
					output_dma_O_data.data.data4=OFM[i*8+3][j][K_dconv]+BIAS[i*8+3];
					output_dma_O_data.data.data5=OFM[i*8+4][j][K_dconv]+BIAS[i*8+4];
					output_dma_O_data.data.data6=OFM[i*8+5][j][K_dconv]+BIAS[i*8+5];
					output_dma_O_data.data.data7=OFM[i*8+6][j][K_dconv]+BIAS[i*8+6];
					output_dma_O_data.data.data8=OFM[i*8+7][j][K_dconv]+BIAS[i*8+7];
					output_dma_O.write(output_dma_O_data);
				}
			}
		}
	}else if(NL_Opt==1){
		// ReLU
		dconv_r_ofm1:for(int i=0;i<divided_Tn_8_dconv;i++){
			dconv_r_ofm2:for(int j=0;j<custom_Tr;j++){
				dconv_r_ofm3:for(int K_dconv=0;K_dconv<custom_Tc;K_dconv++){
	#pragma HLS PIPELINE II=1
					output_dma_O_data.last=0;
					if (i==divided_Tn_8_dconv-1 && j==custom_Tr-1 && K_dconv==custom_Tc-1){
						output_dma_O_data.last=1;
						}
					output_dma_O_data.data.data1=OFM[i*8+0][j][K_dconv]+BIAS[i*8+0]>=0?OFM[i*8+0][j][K_dconv]+BIAS[i*8+0]:0;
					output_dma_O_data.data.data2=OFM[i*8+1][j][K_dconv]+BIAS[i*8+1]>=0?OFM[i*8+1][j][K_dconv]+BIAS[i*8+1]:0;
					output_dma_O_data.data.data3=OFM[i*8+2][j][K_dconv]+BIAS[i*8+2]>=0?OFM[i*8+2][j][K_dconv]+BIAS[i*8+2]:0;
					output_dma_O_data.data.data4=OFM[i*8+3][j][K_dconv]+BIAS[i*8+3]>=0?OFM[i*8+3][j][K_dconv]+BIAS[i*8+3]:0;
					output_dma_O_data.data.data5=OFM[i*8+4][j][K_dconv]+BIAS[i*8+4]>=0?OFM[i*8+4][j][K_dconv]+BIAS[i*8+4]:0;
					output_dma_O_data.data.data6=OFM[i*8+5][j][K_dconv]+BIAS[i*8+5]>=0?OFM[i*8+5][j][K_dconv]+BIAS[i*8+5]:0;
					output_dma_O_data.data.data7=OFM[i*8+6][j][K_dconv]+BIAS[i*8+6]>=0?OFM[i*8+6][j][K_dconv]+BIAS[i*8+6]:0;
					output_dma_O_data.data.data8=OFM[i*8+7][j][K_dconv]+BIAS[i*8+7]>=0?OFM[i*8+7][j][K_dconv]+BIAS[i*8+7]:0;
					output_dma_O.write(output_dma_O_data);
				}
			}
		}
	}else if(NL_Opt==2){
		// h-switch
		dconv_hs_ofm1:for(int i=0;i<divided_Tn_8_dconv;i++){
			dconv_hs_ofm2:for(int j=0;j<custom_Tr;j++){
				dconv_hs_ofm3:for(int K_dconv=0;K_dconv<custom_Tc;K_dconv++){
	#pragma HLS PIPELINE II=1
					output_dma_O_data.last=0;
					if (i==divided_Tn_8_dconv-1 && j==custom_Tr-1 && K_dconv==custom_Tc-1){
						output_dma_O_data.last=1;
						}

					FPGA_DATA_FIX d1 = OFM[i*8+0][j][K_dconv]+BIAS[i*8+0]+3>=0?OFM[i*8+0][j][K_dconv]+BIAS[i*8+0]+3:0;
					d1 = (d1<=6?d1:6)/float(6)*(OFM[i*8+0][j][K_dconv]+BIAS[i*8+0]);
					output_dma_O_data.data.data1=d1;

					FPGA_DATA_FIX d2 = OFM[i*8+1][j][K_dconv]+BIAS[i*8+1]+3>=0?OFM[i*8+1][j][K_dconv]+BIAS[i*8+1]+3:0;
					d2 = (d2<=6?d2:6)/float(6)*(OFM[i*8+1][j][K_dconv]+BIAS[i*8+1]);
					output_dma_O_data.data.data2=d2;

					FPGA_DATA_FIX d3 = OFM[i*8+2][j][K_dconv]+BIAS[i*8+2]+3>=0?OFM[i*8+2][j][K_dconv]+BIAS[i*8+2]+3:0;
					d3 = (d3<=6?d3:6)/float(6)*(OFM[i*8+2][j][K_dconv]+BIAS[i*8+2]);
					output_dma_O_data.data.data3=d3;

					FPGA_DATA_FIX d4 = OFM[i*8+3][j][K_dconv]+BIAS[i*8+3]+3>=0?OFM[i*8+3][j][K_dconv]+BIAS[i*8+3]+3:0;
					d4 = (d4<=6?d4:6)/float(6)*(OFM[i*8+3][j][K_dconv]+BIAS[i*8+3]);
					output_dma_O_data.data.data4=d4;

					FPGA_DATA_FIX d5 = OFM[i*8+4][j][K_dconv]+BIAS[i*8+4]+3>=0?OFM[i*8+4][j][K_dconv]+BIAS[i*8+4]+3:0;
					d5 = (d5<=6?d5:6)/float(6)*(OFM[i*8+4][j][K_dconv]+BIAS[i*8+4]);
					output_dma_O_data.data.data5=d5;

					FPGA_DATA_FIX d6 = OFM[i*8+5][j][K_dconv]+BIAS[i*8+5]+3>=0?OFM[i*8+5][j][K_dconv]+BIAS[i*8+5]+3:0;
					d6 = (d6<=6?d6:6)/float(6)*(OFM[i*8+5][j][K_dconv]+BIAS[i*8+5]);
					output_dma_O_data.data.data6=d6;

					FPGA_DATA_FIX d7 = OFM[i*8+6][j][K_dconv]+BIAS[i*8+6]+3>=0?OFM[i*8+6][j][K_dconv]+BIAS[i*8+6]+3:0;
					d7 = (d7<=6?d7:6)/float(6)*(OFM[i*8+6][j][K_dconv]+BIAS[i*8+6]);
					output_dma_O_data.data.data7=d7;

					FPGA_DATA_FIX d8 = OFM[i*8+7][j][K_dconv]+BIAS[i*8+7]+3>=0?OFM[i*8+7][j][K_dconv]+BIAS[i*8+7]+3:0;
					d8 = (d8<=6?d8:6)/float(6)*(OFM[i*8+7][j][K_dconv]+BIAS[i*8+7]);
					output_dma_O_data.data.data8=d8;

					output_dma_O.write(output_dma_O_data);
				}
			}
		}
	}

}



void dconv_OFM_STORE( hls::stream<DMA_DATA_128B_FIX> &output_dma_O,
				hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
				FPGA_DATA_FIX OFM[Tn_dconv][Tr_dconv][Tc_dconv],
				FPGA_DATA_FIX BIAS[Tn_dconv],
				int custom_Tr,
				int custom_Tc,
				int NL_Opt){

	DMA_DATA_128B_FIX output_dma_O_data;

	DMA_DATA_128B_FIX bias_input_dma;
	bias1:for(int i=0;i<divided_Tn_8_dconv;i++){
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

	dconv_OFM_STORE_NONLINEAR(output_dma_O,OFM,BIAS,custom_Tr,custom_Tc,NL_Opt);

//	ofm1:for(int i=0;i<divided_Tn_8_dconv;i++){
//		ofm2:for(int j=0;j<custom_Tr;j++){
//			ofm3:for(int K_dconv=0;K_dconv<custom_Tc;K_dconv++){
//#pragma HLS PIPELINE II=1
//				output_dma_O_data.last=0;
//				if (i==divided_Tn_8_dconv-1 && j==custom_Tr-1 && K_dconv==custom_Tc-1){
//					output_dma_O_data.last=1;
//					}
//				output_dma_O_data.data.data1=OFM[i*8][j][K_dconv]+BIAS[i*8];
//				output_dma_O_data.data.data2=OFM[i*8+1][j][K_dconv]+BIAS[i*8+1];
//				output_dma_O_data.data.data3=OFM[i*8+2][j][K_dconv]+BIAS[i*8+2];
//				output_dma_O_data.data.data4=OFM[i*8+3][j][K_dconv]+BIAS[i*8+3];
//				output_dma_O_data.data.data5=OFM[i*8+4][j][K_dconv]+BIAS[i*8+4];
//				output_dma_O_data.data.data6=OFM[i*8+5][j][K_dconv]+BIAS[i*8+5];
//				output_dma_O_data.data.data7=OFM[i*8+6][j][K_dconv]+BIAS[i*8+6];
//				output_dma_O_data.data.data8=OFM[i*8+7][j][K_dconv]+BIAS[i*8+7];
//				output_dma_O.write(output_dma_O_data);
//			}
//		}
//	}

	bias2:for(int j=0;j<custom_Tr;j++){
		for(int K_dconv=0;K_dconv<custom_Tc;K_dconv++){
#pragma HLS PIPELINE II=1
			for(int i=0;i<Tn_dconv;i++){
				OFM[i][j][K_dconv] = 0;
			}
		}
	}
}

void dconv_Load_Fire(hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		FPGA_DATA_FIX WEIGHT[2][2][Tn_dconv][K_dconv][K_dconv],
		FPGA_DATA_FIX IFM1[2][Tn_dconv][2*(Tr_dconv-1)+K_dconv][2*(Tc_dconv-1)+K_dconv],
		FPGA_DATA_FIX IFM2[2][Tn_dconv][2*(Tr_dconv-1)+K_dconv][2*(Tc_dconv-1)+K_dconv],
		FPGA_DATA_FIX OFM[Tn_dconv][Tr_dconv][Tc_dconv],
		int row,
		int col,
		int N,
		int custom_k,
		int custom_s,
		int custom_Tr,
		int custom_Tc
		){
	static int idx = 0;
//	for(int i=0;i<N;i+=Tn_dconv){
//#pragma HLS loop_tripcount min=192 max=192 avg=192
//#pragma HLS dependence variable=WEIGHT intra false
//	if(idx%2==0){
		dconv_LOAD_WEIGHT_128B(input_dma_W,WEIGHT[0],custom_k,custom_s);
		dconv_LOAD_IFM(input_dma_I,IFM1,custom_Tr,custom_Tc,custom_s);
		dconv_FIRE(WEIGHT[0],IFM1,OFM, row, col, custom_k,custom_s,custom_Tr,custom_Tc);
//		dconv_FIRE(WEIGHT[1],IFM2,OFM, row, col, custom_k,custom_Tr,custom_Tc);
//	}else{
//		dconv_LOAD_WEIGHT_128B(input_dma_W,WEIGHT[1], custom_k);
//		dconv_LOAD_IFM(input_dma_I,IFM2,custom_Tr,custom_Tc);
//		dconv_FIRE(WEIGHT[0],IFM1,OFM, row, col, custom_k,custom_Tr,custom_Tc);
//	}
	idx+=1;
//	}
}

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
		){

#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=row bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=col bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=num bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=N   bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=custom_k   bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=custom_Tr   bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=custom_Tc  bundle=CRTL_BUS


#pragma HLS INTERFACE axis port=input_dma_W
#pragma HLS INTERFACE axis port=input_dma_I
#pragma HLS INTERFACE axis port=input_dma_B
#pragma HLS INTERFACE axis port=output_dma_O

	static FPGA_DATA_FIX WEIGHT1[2][2][Tn_dconv][K_dconv][K_dconv];
#pragma HLS ARRAY_PARTITION variable=WEIGHT1 complete dim=3


	static FPGA_DATA_FIX IFM[2][Tn_dconv][2*(Tr_dconv-1)+K_dconv][2*(Tc_dconv-1)+K_dconv];
#pragma HLS ARRAY_PARTITION variable=IFM complete dim=1
#pragma HLS ARRAY_PARTITION variable=IFM complete dim=2

	static FPGA_DATA_FIX IFM_DB[2][Tn_dconv][2*(Tr_dconv-1)+K_dconv][2*(Tc_dconv-1)+K_dconv];
#pragma HLS ARRAY_PARTITION variable=IFM_DB complete dim=1
#pragma HLS ARRAY_PARTITION variable=IFM_DB complete dim=2

	static FPGA_DATA_FIX OFM[Tn_dconv][Tr_dconv][Tc_dconv];
#pragma HLS RESOURCE variable=OFM core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=OFM complete dim=1

	static FPGA_DATA_FIX OFM_DB[Tn_dconv][Tr_dconv][Tc_dconv];
#pragma HLS RESOURCE variable=OFM_DB core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=OFM_DB complete dim=1

	static FPGA_DATA_FIX BIAS[Tn_dconv];
#pragma HLS ARRAY_PARTITION variable=BIAS complete dim=1
	static FPGA_DATA_FIX BIAS_DB[Tn_dconv];
#pragma HLS ARRAY_PARTITION variable=BIAS_DB complete dim=1

	if(num%2==0){
		dconv_Load_Fire(input_dma_W,input_dma_I,WEIGHT1,IFM,IFM_DB,OFM_DB,row,col,N,custom_k,custom_s,custom_Tr,custom_Tc);
		dconv_OFM_STORE(output_dma_O,input_dma_B, OFM, BIAS,custom_Tr,custom_Tc,NL_Opt);
	}else{
		dconv_Load_Fire(input_dma_W,input_dma_I,WEIGHT1,IFM,IFM_DB,OFM,row,col,N,custom_k,custom_s,custom_Tr,custom_Tc);
		dconv_OFM_STORE(output_dma_O,input_dma_B, OFM_DB, BIAS_DB,custom_Tr,custom_Tc,NL_Opt);
	}

//	dconv_Load_Fire(input_dma_W,input_dma_I,WEIGHT1,IFM,IFM_DB,OFM_DB,row,col,N,custom_k,custom_Tr,custom_Tc);
//	dconv_OFM_STORE(output_dma_O,input_dma_B, OFM_DB, BIAS_DB,custom_Tr,custom_Tc,NL_Opt);

}








