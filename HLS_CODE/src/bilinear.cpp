#include "source_fix.h"
#include <math.h>


void bilinear_LOAD_IFM(hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		FPGA_DATA_FIX IFM[Tm][Tr][Tc],
		int custom_Tr,
		int custom_Tc){

	DMA_DATA_128B_FIX ifm_input_dma;
	ifm1:for(int i=0;i<divided_Tm_8;i++){
		ifm2:for(int j=0;j<custom_Tr;j++){
			ifm3:for(int m=0;m<custom_Tc;m++){
#pragma HLS PIPELINE II=1
				ifm_input_dma=input_dma_I.read();
				if(i*8<Tm){
					IFM[i*8][j][m]=ifm_input_dma.data.data1;
				}
				if(i*8+1<Tm){
					IFM[i*8+1][j][m]=ifm_input_dma.data.data2;
				}
				if(i*8+2<Tm){
					IFM[i*8+2][j][m]=ifm_input_dma.data.data3;
				}
				if(i*8+3<Tm){
					IFM[i*8+3][j][m]=ifm_input_dma.data.data4;
				}
				if(i*8+4<Tm){
					IFM[i*8+4][j][m]=ifm_input_dma.data.data5;
				}
				if(i*8+5<Tm){
					IFM[i*8+5][j][m]=ifm_input_dma.data.data6;
				}
				if(i*8+6<Tm){
					IFM[i*8+6][j][m]=ifm_input_dma.data.data7;
				}
				if(i*8+7<Tm){
					IFM[i*8+7][j][m]=ifm_input_dma.data.data8;
				}
			}
		}
	}
}



void do_bilinear(FPGA_DATA_FIX IFM[Tm][Tr][Tc],
		FPGA_DATA_FIX OFM[Tm][2*Tr][2*Tc],
		int custom_Tr,
		int custom_Tc){


	for(int trr=0;trr<custom_Tr;trr++){
#pragma HLS loop_tripcount min=1 max=Tr avg=Tr
		for(int tcc=0;tcc<custom_Tc;tcc++){
#pragma HLS loop_tripcount min=1 max=Tc avg=Tc
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=OFM inter false
			for(int too=0;too<Tm; too++){
				OFM[too][2*trr+0][2*tcc+0] = IFM[too][trr][tcc];
			}
		}
	}


	for(int trr=0;trr<custom_Tr;trr++){
#pragma HLS loop_tripcount min=1 max=Tr avg=Tr
		for(int tcc=0;tcc<custom_Tc;tcc++){
#pragma HLS loop_tripcount min=1 max=Tc avg=Tc
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=OFM inter false
			for(int too=0;too<Tm; too++){
				if(tcc==custom_Tc-1)
					OFM[too][2*trr+0][2*tcc+1] = (OFM[too][2*trr+0][2*tcc+0])/2;
				else
					OFM[too][2*trr+0][2*tcc+1] = (OFM[too][2*trr+0][2*tcc+0]+OFM[too][2*trr+0][2*tcc+2])/2;
			}
		}
	}

	for(int trr=0;trr<custom_Tr;trr++){
#pragma HLS loop_tripcount min=1 max=Tr avg=Tr
		for(int tcc=0;tcc<2*custom_Tc;tcc++){
#pragma HLS loop_tripcount min=1 max=Tc avg=Tc
#pragma HLS PIPELINE II=1
#pragma HLS dependence variable=OFM inter false
			for(int too=0;too<Tm; too++){
				if (trr==custom_Tr-1)
					OFM[too][2*trr+1][tcc] = (OFM[too][2*trr+0][tcc])/2;
				else
					OFM[too][2*trr+1][tcc] = (OFM[too][2*trr+0][tcc]+OFM[too][2*trr+2][tcc])/2;
			}
		}
	}
}


void bilinear_output_Store(hls::stream<DMA_DATA_128B_FIX> &output,
		FPGA_DATA_FIX OFM[Tm][2*Tr][2*Tc],
		int custom_Tr,
		int custom_Tc
		){

	DMA_DATA_128B_FIX output_dma_O_data;

	n_ofm1:for(int i=0;i<divided_Tm_8;i++){
		n_ofm2:for(int j=0;j<2*custom_Tr;j++){
			n_ofm3:for(int k=0;k<2*custom_Tc;k++){
#pragma HLS PIPELINE II=1
				output_dma_O_data.last=0;
				if (i==divided_Tm_8-1 && j==2*custom_Tr-1 && k==2*custom_Tc-1){
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

void bilinear(hls::stream<DMA_DATA_128B_FIX> &input,
		hls::stream<DMA_DATA_128B_FIX> &output,
		int custom_Tr,
		int custom_Tc,
		int op
		){

#pragma HLS INTERFACE s_axilite port=return bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=eps bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=gamma bundle=CRTL_BUS
#pragma HLS INTERFACE s_axilite port=betabundle=CRTL_BUS

#pragma HLS INTERFACE axis port=input
#pragma HLS INTERFACE axis port=running_mean
#pragma HLS INTERFACE axis port=running_var
#pragma HLS INTERFACE axis port=output


	static FPGA_DATA_FIX IFM[Tm][Tr][Tc];
#pragma HLS RESOURCE variable=IFM core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=IFM complete dim=1

	static FPGA_DATA_FIX OFM[Tm][2*Tr][2*Tc];
#pragma HLS RESOURCE variable=OFM core=RAM_S2P_BRAM
#pragma HLS ARRAY_PARTITION variable=OFM complete dim=1

	if(op==0){
		// load IFM
		bilinear_LOAD_IFM(input,IFM,custom_Tr,custom_Tc);
	}else if(op==1){
		// do bilinear
		do_bilinear(IFM,OFM,custom_Tr,custom_Tc);
	}else if(op==2){
		// Output
		bilinear_output_Store(output,OFM,custom_Tr,custom_Tc);
	}


}
