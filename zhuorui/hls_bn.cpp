#include "source_fix.h"
#include <math.h>


void bn_LOAD_IFM(hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		FPGA_DATA_FIX IFM[Tn_bn][Tr_bn][Tc_bn],
		int custom_Tr,
		int custom_Tc){

	DMA_DATA_128B_FIX ifm_input_dma;
	ifm1:for(int i=0;i<divided_Tn_8_bn;i++){
		ifm2:for(int j=0;j<custom_Tr;j++){
			ifm3:for(int m=0;m<custom_Tc;m++){
#pragma HLS PIPELINE II=1
				ifm_input_dma=input_dma_I.read();
				if(i*8<Tn_bn){
					IFM[i*8][j][m]=ifm_input_dma.data.data1;
				}
				if(i*8+1<Tn_bn){
					IFM[i*8+1][j][m]=ifm_input_dma.data.data2;
				}
				if(i*8+2<Tn_bn){
					IFM[i*8+2][j][m]=ifm_input_dma.data.data3;
				}
				if(i*8+3<Tn_bn){
					IFM[i*8+3][j][m]=ifm_input_dma.data.data4;
				}
				if(i*8+4<Tn_bn){
					IFM[i*8+4][j][m]=ifm_input_dma.data.data5;
				}
				if(i*8+5<Tn_bn){
					IFM[i*8+5][j][m]=ifm_input_dma.data.data6;
				}
				if(i*8+6<Tn_bn){
					IFM[i*8+6][j][m]=ifm_input_dma.data.data7;
				}
				if(i*8+7<Tn_bn){
					IFM[i*8+7][j][m]=ifm_input_dma.data.data8;
				}
			}
		}
	}
}


void bn_LOAD_E_V(hls::stream<DMA_DATA_128B_FIX> &running_io,
		FPGA_DATA_FIX V[Tn_bn]){

	DMA_DATA_128B_FIX ifm_input_dma;
	ifm1:for(int i=0;i<divided_Tn_8_bn;i++){
#pragma HLS PIPELINE II=1
		ifm_input_dma=running_io.read();
		if(i*8<Tn_bn){
			V[i*8]=ifm_input_dma.data.data1;
		}
		if(i*8+1<Tn_bn){
			V[i*8+1]=ifm_input_dma.data.data2;
		}
		if(i*8+2<Tn_bn){
			V[i*8+2]=ifm_input_dma.data.data3;
		}
		if(i*8+3<Tn_bn){
			V[i*8+3]=ifm_input_dma.data.data4;
		}
		if(i*8+4<Tn_bn){
			V[i*8+4]=ifm_input_dma.data.data5;
		}
		if(i*8+5<Tn_bn){
			V[i*8+5]=ifm_input_dma.data.data6;
		}
		if(i*8+6<Tn_bn){
			V[i*8+6]=ifm_input_dma.data.data7;
		}
		if(i*8+7<Tn_bn){
			V[i*8+7]=ifm_input_dma.data.data8;
		}
	}
}

void do_norm(FPGA_DATA_FIX IFM[Tn_bn][Tr_bn][Tc_bn],
		FPGA_DATA_FIX OFM[Tn_bn][Tr_bn][Tc_bn],
		FPGA_DATA_FIX E[Tn_bn],
		FPGA_DATA_FIX V[Tn_bn],
		float eps,
		int gamma,
		int beta,
		int custom_Tr,
		int custom_Tc){


	for(int trr=0;trr<custom_Tr;trr++){
#pragma HLS loop_tripcount min=1 max=Tr_bn avg=Tr_bn
		for(int tcc=0;tcc<custom_Tc;tcc++){
#pragma HLS loop_tripcount min=1 max=Tc_bn avg=Tc_bn
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=OFM inter false
			for(int too=0;too<Tn_bn; too++){
				OFM[too][trr][tcc] = round((IFM[too][trr][tcc]-E[too])/sqrt(float(V[too])+eps)*gamma+beta);
			}
		}
	}
}

void bn_output_Store(hls::stream<DMA_DATA_128B_FIX> &output,
		FPGA_DATA_FIX OFM[Tn_bn][Tr_bn][Tc_bn],
		int custom_Tr,
		int custom_Tc
		){

	DMA_DATA_128B_FIX output_dma_O_data;

	n_ofm1:for(int i=0;i<divided_Tn_8_bn;i++){
		n_ofm2:for(int j=0;j<custom_Tr;j++){
			n_ofm3:for(int k=0;k<custom_Tc;k++){
#pragma HLS PIPELINE II=1
				output_dma_O_data.last=0;
				if (i==divided_Tn_8_bn-1 && j==custom_Tr-1 && k==custom_Tc-1){
					output_dma_O_data.last=1;
					}
				output_dma_O_data.data.data1=OFM[i*8+0][j][k];
				output_dma_O_data.data.data2=OFM[i*8+1][j][k];
				output_dma_O_data.data.data3=OFM[i*8+2][j][k];
				output_dma_O_data.data.data4=OFM[i*8+3][j][k];
				output_dma_O_data.data.data5=OFM[i*8+4][j][k];
				output_dma_O_data.data.data6=OFM[i*8+5][j][k];
				output_dma_O_data.data.data7=OFM[i*8+6][j][k];
				output_dma_O_data.data.data8=OFM[i*8+7][j][k];
				output.write(output_dma_O_data);
			}
		}
	}
}

void batch_norm(hls::stream<DMA_DATA_128B_FIX> &input,
		hls::stream<DMA_DATA_128B_FIX> &running_mean,
		hls::stream<DMA_DATA_128B_FIX> &running_var,
		hls::stream<DMA_DATA_128B_FIX> &output,
		float eps,
		int gamma,
		int beta,
		int custom_Tr,
		int custom_Tc,
		int op
		){

#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=eps bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=gamma bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=beta bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=custom_Tr   bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=custom_Tc  bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=op  bundle=CRTL_BUS

#pragma HLS INTERFACE axis port=input
#pragma HLS INTERFACE axis port=running_mean
#pragma HLS INTERFACE axis port=running_var
#pragma HLS INTERFACE axis port=output


	static FPGA_DATA_FIX IFM[Tn_bn][Tr_bn][Tc_bn];
#pragma HLS RESOURCE variable=IFM core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=IFM complete dim=1

	static FPGA_DATA_FIX OFM[Tn_bn][Tr_bn][Tc_bn];
#pragma HLS RESOURCE variable=OFM core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=OFM complete dim=1

	static FPGA_DATA_FIX E[Tn_bn];
	static FPGA_DATA_FIX V[Tn_bn];

	if(op==0){
		// load IFM
		bn_LOAD_IFM(input,IFM,custom_Tr,custom_Tc);
	}else if(op==1){
		// load E and V
		bn_LOAD_E_V(running_mean,E);
		bn_LOAD_E_V(running_var,V);
	}else if(op==2){
		// Normalization & write out
		do_norm(IFM,OFM,E,V,eps,gamma,beta,custom_Tr,custom_Tc);
	}else if(op==3){
		// Output
		bn_output_Store(output,OFM,custom_Tr,custom_Tc);
	}
}
