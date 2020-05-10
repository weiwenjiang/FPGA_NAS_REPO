#include "source_fix.h"
#include <math.h>
#include <stdlib.h>

int do_conv(int M,int N,int R,int C,int c_k, int c_s,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * weight,FPGA_DATA_FIX * output, FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		);

int do_conv_pad(int M,int N,int R,int C,int outR,int outC,int pad,int c_k,int c_s,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * weight,FPGA_DATA_FIX * output, FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		);

int do_bn(int M,int N,int R,int C,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * E,FPGA_DATA_FIX * V,FPGA_DATA_FIX *BN_output,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_E,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_V,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		);

int do_dconv(int M,int N,int R,int C,int c_s,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * weight,FPGA_DATA_FIX * output, FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		);
/*
int do_dconv_pad(int M,int N,int R,int C,int pad,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * weight,FPGA_DATA_FIX * output, FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		);
*/
int main(){
	hls::stream<DMA_DATA_128B_FIX> input_dma_W("input_dma_W");
	hls::stream<DMA_DATA_128B_FIX> input_dma_I("input_dma_I");
	hls::stream<DMA_DATA_128B_FIX> input_dma_B("input_dma_B");
	hls::stream<DMA_DATA_128B_FIX> output_dma_O("output_dma_O");

	hls::stream<DMA_DATA_128B_FIX> input_bn_I("input_bn_I");
	hls::stream<DMA_DATA_128B_FIX> running_mean("input_E");
	hls::stream<DMA_DATA_128B_FIX> running_var("input_V");

	// Initialization
	extern FPGA_DATA_FIX input[CH1*(R+2)*(C+2)];//extern FPGA_DATA_FIX input[CH1*(R+2)*(C+2)];
	extern FPGA_DATA_FIX weight_1[CH1*CH2*3*3];
	//16, s=1, n=1
	extern FPGA_DATA_FIX weight_2[CH2*CH2*1*1];
	extern FPGA_DATA_FIX weight_3[CH3*CH2*3*3];
	//24, s=2, n=2
	extern FPGA_DATA_FIX weight_4[6*CH3*CH3*1*1];
	extern FPGA_DATA_FIX weight_5[36*CH3*CH3*3*3];
	extern FPGA_DATA_FIX weight_6[6*CH4*CH3*1*1];
	//
	extern FPGA_DATA_FIX weight_7[6*CH4*CH4*1*1];
	extern FPGA_DATA_FIX weight_8[36*CH4*CH4*3*3];
	extern FPGA_DATA_FIX weight_9[6*CH5*CH4*1*1];
	//32, s=2, n=3
	extern FPGA_DATA_FIX weight_10[6*CH5*CH5*1*1];
	extern FPGA_DATA_FIX weight_11[36*CH5*CH5*3*3];
	extern FPGA_DATA_FIX weight_12[6*CH6*CH5*1*1];
	extern FPGA_DATA_FIX weight_13[6*CH6*CH6*1*1];
	extern FPGA_DATA_FIX weight_14[36*CH6*CH6*3*3];
	extern FPGA_DATA_FIX weight_15[6*CH7*CH6*1*1];
	extern FPGA_DATA_FIX weight_16[6*CH7*CH7*1*1];
	extern FPGA_DATA_FIX weight_17[36*CH7*CH7*3*3];
	extern FPGA_DATA_FIX weight_18[6*CH8*CH7*1*1];
/*	//64, s=2, n=4
	extern FPGA_DATA_FIX weight_19[6*CH8*CH8*1*1];
	extern FPGA_DATA_FIX weight_20[36*CH8*CH8*3*3];
	extern FPGA_DATA_FIX weight_21[6*CH9*CH8*1*1];
	extern FPGA_DATA_FIX weight_22[6*CH9*CH9*1*1];
	extern FPGA_DATA_FIX weight_23[36*CH9*CH9*3*3];
	extern FPGA_DATA_FIX weight_24[6*CH10*CH9*1*1];
	extern FPGA_DATA_FIX weight_25[6*CH10*CH10*1*1];
	extern FPGA_DATA_FIX weight_26[36*CH10*CH10*3*3];
	extern FPGA_DATA_FIX weight_27[6*CH11*CH10*1*1];
	extern FPGA_DATA_FIX weight_28[6*CH11*CH11*1*1];
	extern FPGA_DATA_FIX weight_29[36*CH11*CH11*3*3];
	extern FPGA_DATA_FIX weight_30[6*CH12*CH11*1*1];
	//96, s=1, n=3
	extern FPGA_DATA_FIX weight_31[6*CH12*CH12*1*1];
	extern FPGA_DATA_FIX weight_32[36*CH12*CH12*3*3];
	extern FPGA_DATA_FIX weight_33[6*CH13*CH12*1*1];
	extern FPGA_DATA_FIX weight_34[6*CH13*CH13*1*1];
	extern FPGA_DATA_FIX weight_35[36*CH13*CH13*3*3];
	extern FPGA_DATA_FIX weight_36[6*CH14*CH13*1*1];
	extern FPGA_DATA_FIX weight_37[6*CH14*CH14*1*1];
	extern FPGA_DATA_FIX weight_38[36*CH14*CH14*3*3];
	extern FPGA_DATA_FIX weight_39[6*CH15*CH14*1*1];
	//160, s=2, n=3
	extern FPGA_DATA_FIX weight_40[6*CH15*CH15*1*1];
	extern FPGA_DATA_FIX weight_41[36*CH15*CH15*3*3];
	extern FPGA_DATA_FIX weight_42[6*CH16*CH15*1*1];
	extern FPGA_DATA_FIX weight_43[6*CH16*CH16*1*1];
	extern FPGA_DATA_FIX weight_44[36*CH16*CH16*3*3];
	extern FPGA_DATA_FIX weight_45[6*CH17*CH16*1*1];
	extern FPGA_DATA_FIX weight_46[6*CH17*CH17*1*1];
	extern FPGA_DATA_FIX weight_47[36*CH17*CH17*3*3];
	extern FPGA_DATA_FIX weight_48[6*CH18*CH17*1*1];
	//320, s=1, n=1
	extern FPGA_DATA_FIX weight_49[6*CH18*CH18*1*1];
	extern FPGA_DATA_FIX weight_50[36*CH18*CH18*3*3];
	extern FPGA_DATA_FIX weight_51[6*CH19*CH18*1*1];
	/**/
//mean for batch norm
	extern FPGA_DATA_FIX E1[CH2];
	extern FPGA_DATA_FIX E2[CH2];
	extern FPGA_DATA_FIX E3[CH3];
	extern FPGA_DATA_FIX E4[6*CH3];
	extern FPGA_DATA_FIX E5[6*CH3];
	extern FPGA_DATA_FIX E6[CH4];

	extern FPGA_DATA_FIX E7[6*CH4];
	extern FPGA_DATA_FIX E8[6*CH4];
	extern FPGA_DATA_FIX E9[CH5];
	//32, s=2, n=3
	extern FPGA_DATA_FIX E10[6*CH5];
	extern FPGA_DATA_FIX E11[6*CH5];
	extern FPGA_DATA_FIX E12[CH6];
	extern FPGA_DATA_FIX E13[6*CH6];
	extern FPGA_DATA_FIX E14[6*CH6];
	extern FPGA_DATA_FIX E15[CH7];
	extern FPGA_DATA_FIX E16[6*CH7];
	extern FPGA_DATA_FIX E17[6*CH7];
	extern FPGA_DATA_FIX E18[CH8];
/*	//64, s=2, n=4
	extern FPGA_DATA_FIX E19[6*CH8];
	extern FPGA_DATA_FIX E20[6*CH8];
	extern FPGA_DATA_FIX E21[CH9];
	extern FPGA_DATA_FIX E22[6*CH9];
	extern FPGA_DATA_FIX E23[6*CH9];
	extern FPGA_DATA_FIX E24[CH10];
	extern FPGA_DATA_FIX E25[6*CH10];
	extern FPGA_DATA_FIX E26[6*CH10];
	extern FPGA_DATA_FIX E27[CH11];
	extern FPGA_DATA_FIX E28[6*CH11];
	extern FPGA_DATA_FIX E29[6*CH11];
	extern FPGA_DATA_FIX E30[CH12];
	//96, s=1, n=3
	extern FPGA_DATA_FIX E31[6*CH12];
	extern FPGA_DATA_FIX E32[6*CH12];
	extern FPGA_DATA_FIX E33[CH13];
	extern FPGA_DATA_FIX E34[6*CH13];
	extern FPGA_DATA_FIX E35[6*CH13];
	extern FPGA_DATA_FIX E36[CH14];
	extern FPGA_DATA_FIX E37[6*CH14];
	extern FPGA_DATA_FIX E38[6*CH14];
	extern FPGA_DATA_FIX E39[CH15];
	//160, s=2, n=3
	extern FPGA_DATA_FIX E40[6*CH15];
	extern FPGA_DATA_FIX E41[6*CH15];
	extern FPGA_DATA_FIX E42[CH16];
	extern FPGA_DATA_FIX E43[6*CH16];
	extern FPGA_DATA_FIX E44[6*CH16];
	extern FPGA_DATA_FIX E45[CH17];
	extern FPGA_DATA_FIX E46[6*CH17];
	extern FPGA_DATA_FIX E47[6*CH17];
	extern FPGA_DATA_FIX E48[CH18];
	//320, s=1, n=1
	extern FPGA_DATA_FIX E49[6*CH18];
	extern FPGA_DATA_FIX E50[6*CH18];
	extern FPGA_DATA_FIX E51[CH19];
	/**/

//variance for batch norm
	extern FPGA_DATA_FIX V1[CH2];
	extern FPGA_DATA_FIX V2[CH2];
	extern FPGA_DATA_FIX V3[CH3];
	extern FPGA_DATA_FIX V4[6*CH3];
	extern FPGA_DATA_FIX V5[6*CH3];
	extern FPGA_DATA_FIX V6[CH4];

	extern FPGA_DATA_FIX V7[6*CH4];
	extern FPGA_DATA_FIX V8[6*CH4];
	extern FPGA_DATA_FIX V9[CH5];
	//32, s=2, n=3
	extern FPGA_DATA_FIX V10[6*CH5];
	extern FPGA_DATA_FIX V11[6*CH5];
	extern FPGA_DATA_FIX V12[CH6];
	extern FPGA_DATA_FIX V13[6*CH6];
	extern FPGA_DATA_FIX V14[6*CH6];
	extern FPGA_DATA_FIX V15[CH7];
	extern FPGA_DATA_FIX V16[6*CH7];
	extern FPGA_DATA_FIX V17[6*CH7];
	extern FPGA_DATA_FIX V18[CH8];
/*	//64, s=2, n=4
	extern FPGA_DATA_FIX V19[6*CH8];
	extern FPGA_DATA_FIX V20[6*CH8];
	extern FPGA_DATA_FIX V21[CH9];
	extern FPGA_DATA_FIX V22[6*CH9];
	extern FPGA_DATA_FIX V23[6*CH9];
	extern FPGA_DATA_FIX V24[CH10];
	extern FPGA_DATA_FIX V25[6*CH10];
	extern FPGA_DATA_FIX V26[6*CH10];
	extern FPGA_DATA_FIX V27[CH11];
	extern FPGA_DATA_FIX V28[6*CH11];
	extern FPGA_DATA_FIX V29[6*CH11];
	extern FPGA_DATA_FIX V30[CH12];
	//96, s=1, n=3
	extern FPGA_DATA_FIX V31[6*CH12];
	extern FPGA_DATA_FIX V32[6*CH12];
	extern FPGA_DATA_FIX V33[CH13];
	extern FPGA_DATA_FIX V34[6*CH13];
	extern FPGA_DATA_FIX V35[6*CH13];
	extern FPGA_DATA_FIX V36[CH14];
	extern FPGA_DATA_FIX V37[6*CH14];
	extern FPGA_DATA_FIX V38[6*CH14];
	extern FPGA_DATA_FIX V39[CH15];
	//160, s=2, n=3
	extern FPGA_DATA_FIX V40[6*CH15];
	extern FPGA_DATA_FIX V41[6*CH15];
	extern FPGA_DATA_FIX V42[CH16];
	extern FPGA_DATA_FIX V43[6*CH16];
	extern FPGA_DATA_FIX V44[6*CH16];
	extern FPGA_DATA_FIX V45[CH17];
	extern FPGA_DATA_FIX V46[6*CH17];
	extern FPGA_DATA_FIX V47[6*CH17];
	extern FPGA_DATA_FIX V48[CH18];
	//320, s=1, n=1
	extern FPGA_DATA_FIX V49[6*CH18];
	extern FPGA_DATA_FIX V50[6*CH18];
	extern FPGA_DATA_FIX V51[CH19];
	/**/
	extern FPGA_DATA_FIX output[CH19*(R/32)*(C/32)];

	FPGA_DATA_FIX y_1[CH2*(R/2+2)*(C/2+2)]={0};//padding is added to the NEXT layer, thus R/2'=R/2/2
	FPGA_DATA_FIX y_2[CH2*(R/2)*(C/2)]={0};
	FPGA_DATA_FIX y_3[CH3*(R/2)*(C/2)]={0};

	FPGA_DATA_FIX y_4[6*CH3*(R/4+2)*(C/4+2)]={0};
	FPGA_DATA_FIX y_5[6*CH3*(R/4)*(C/4)]={0};
	FPGA_DATA_FIX y_6[CH4*(R/4)*(C/4)]={0};

	FPGA_DATA_FIX y_7[6*CH4*(R/4+2)*(C/4+2)]={0};
	FPGA_DATA_FIX y_8[6*CH4*(R/4)*(C/4)]={0};
	FPGA_DATA_FIX y_9[CH5*(R/4)*(C/4)]={0};
	//32, s=2, n=3
	FPGA_DATA_FIX y_10[6*CH5*(R/8+2)*(C/8+2)]={0};
	FPGA_DATA_FIX y_11[6*CH5*(R/8)*(C/8)]={0};
	FPGA_DATA_FIX y_12[CH6*(R/8)*(C/8)]={0};
	FPGA_DATA_FIX y_13[6*CH6*(R/8+2)*(C/8+2)]={0};
	FPGA_DATA_FIX y_14[6*CH6*(R/8)*(C/8)]={0};
	FPGA_DATA_FIX y_15[CH7*(R/8)*(C/8)]={0};
	FPGA_DATA_FIX y_16[6*CH7*(R/8+2)*(C/8+2)]={0};
	FPGA_DATA_FIX y_17[6*CH7*(R/8)*(C/8)]={0};
	FPGA_DATA_FIX y_18[CH8*(R/8)*(C/8)]={0};
/*	//64, s=2, n=4
	FPGA_DATA_FIX y_19[6*CH8*(R/16+2)*(C/16+2)]={0};
	FPGA_DATA_FIX y_20[6*CH8*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX y_21[CH9*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX y_22[6*CH9*(R/16+2)*(C/16+2)]={0};
	FPGA_DATA_FIX y_23[6*CH9*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX y_24[CH10*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX y_25[6*CH10*(R/16+2)*(C/16+2)]={0};
	FPGA_DATA_FIX y_26[6*CH10*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX y_27[CH11*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX y_28[6*CH11*(R/16+2)*(C/16+2)]={0};
	FPGA_DATA_FIX y_29[6*CH11*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX y_30[CH12*(R/16)*(C/16)]={0};
	//96, s=1, n=3
	FPGA_DATA_FIX y_31[6*CH12*(R/16+2)*(C/16+2)]={0};
	FPGA_DATA_FIX y_32[6*CH12*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX y_33[CH13*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX y_34[6*CH13*(R/16+2)*(C/16+2)]={0};
	FPGA_DATA_FIX y_35[6*CH13*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX y_36[CH14*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX y_37[6*CH14*(R/16+2)*(C/16+2)]={0};
	FPGA_DATA_FIX y_38[6*CH14*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX y_39[CH15*(R/16)*(C/16)]={0};
	//160, s=2, n=3
	FPGA_DATA_FIX y_40[6*CH15*(R/32+2)*(C/32+2)]={0};
	FPGA_DATA_FIX y_41[6*CH15*(R/32)*(C/32)]={0};
	FPGA_DATA_FIX y_42[CH16*(R/32)*(C/32)]={0};
	FPGA_DATA_FIX y_43[6*CH16*(R/32+2)*(C/32+2)]={0};
	FPGA_DATA_FIX y_44[6*CH16*(R/32)*(C/32)]={0};
	FPGA_DATA_FIX y_45[CH17*(R/32)*(C/32)]={0};
	FPGA_DATA_FIX y_46[6*CH17*(R/32+2)*(C/32+2)]={0};
	FPGA_DATA_FIX y_47[6*CH17*(R/32)*(C/32)]={0};
	FPGA_DATA_FIX y_48[CH18*(R/32)*(C/32)]={0};
	//320, s=1, n=1
	FPGA_DATA_FIX y_49[6*CH18*(R/32+2)*(C/32+2)]={0};
	FPGA_DATA_FIX y_50[6*CH18*(R/32)*(C/32)]={0};
	FPGA_DATA_FIX y_51[CH19*(R/32)*(C/32)]={0};
	/**/
	//z is the output of batch normalization layer
	FPGA_DATA_FIX z_1[CH2*(R/2+2)*(C/2+2)]={0};
	FPGA_DATA_FIX z_2[CH2*(R/2)*(C/2)]={0};
	FPGA_DATA_FIX z_3[CH3*(R/2)*(C/2)]={0};

	FPGA_DATA_FIX z_4[6*CH3*(R/4+2)*(C/4+2)]={0};
	FPGA_DATA_FIX z_5[6*CH3*(R/4)*(C/4)]={0};
	FPGA_DATA_FIX z_6[CH4*(R/4)*(C/4)]={0};

	FPGA_DATA_FIX z_7[6*CH4*(R/4+2)*(C/4+2)]={0};
	FPGA_DATA_FIX z_8[6*CH4*(R/4)*(C/4)]={0};
	FPGA_DATA_FIX z_9[CH5*(R/4)*(C/4)]={0};
	//32, s=2, n=3
	FPGA_DATA_FIX z_10[6*CH5*(R/8+2)*(C/8+2)]={0};
	FPGA_DATA_FIX z_11[6*CH5*(R/8)*(C/8)]={0};
	FPGA_DATA_FIX z_12[CH6*(R/8)*(C/8)]={0};
	FPGA_DATA_FIX z_13[6*CH6*(R/8+2)*(C/8+2)]={0};
	FPGA_DATA_FIX z_14[6*CH6*(R/8)*(C/8)]={0};
	FPGA_DATA_FIX z_15[CH7*(R/8)*(C/8)]={0};
	FPGA_DATA_FIX z_16[6*CH7*(R/8+2)*(C/8+2)]={0};
	FPGA_DATA_FIX z_17[6*CH7*(R/8)*(C/8)]={0};
	FPGA_DATA_FIX z_18[CH8*(R/8)*(C/8)]={0};
/*	//64, s=2, n=4
	FPGA_DATA_FIX z_19[6*CH8*(R/16+2)*(C/16+2)]={0};
	FPGA_DATA_FIX z_20[6*CH8*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX z_21[CH9*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX z_22[6*CH9*(R/16+2)*(C/16+2)]={0};
	FPGA_DATA_FIX z_23[6*CH9*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX z_24[CH10*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX z_25[6*CH10*(R/16+2)*(C/16+2)]={0};
	FPGA_DATA_FIX z_26[6*CH10*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX z_27[CH11*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX z_28[6*CH11*(R/16+2)*(C/16+2)]={0};
	FPGA_DATA_FIX z_29[6*CH11*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX z_30[CH12*(R/16)*(C/16)]={0};
	//96, s=1, n=3
	FPGA_DATA_FIX z_31[6*CH12*(R/16+2)*(C/16+2)]={0};
	FPGA_DATA_FIX z_32[6*CH12*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX z_33[CH13*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX z_34[6*CH13*(R/16+2)*(C/16+2)]={0};
	FPGA_DATA_FIX z_35[6*CH13*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX z_36[CH14*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX z_37[6*CH14*(R/16+2)*(C/16+2)]={0};
	FPGA_DATA_FIX z_38[6*CH14*(R/16)*(C/16)]={0};
	FPGA_DATA_FIX z_39[CH15*(R/16)*(C/16)]={0};
	//160, s=2, n=3
	FPGA_DATA_FIX z_40[6*CH15*(R/32+2)*(C/32+2)]={0};
	FPGA_DATA_FIX z_41[6*CH15*(R/32)*(C/32)]={0};
	FPGA_DATA_FIX z_42[CH16*(R/32)*(C/32)]={0};
	FPGA_DATA_FIX z_43[6*CH16*(R/32+2)*(C/32+2)]={0};
	FPGA_DATA_FIX z_44[6*CH16*(R/32)*(C/32)]={0};
	FPGA_DATA_FIX z_45[CH17*(R/32)*(C/32)]={0};
	FPGA_DATA_FIX z_46[6*CH17*(R/32+2)*(C/32+2)]={0};
	FPGA_DATA_FIX z_47[6*CH17*(R/32)*(C/32)]={0};
	FPGA_DATA_FIX z_48[CH18*(R/32)*(C/32)]={0};
	//320, s=1, n=1
	FPGA_DATA_FIX z_49[6*CH18*(R/32+2)*(C/32+2)]={0};
	FPGA_DATA_FIX z_50[6*CH18*(R/32)*(C/32)]={0};
	FPGA_DATA_FIX z_51[CH19*(R/32)*(C/32)]={0};
	/**/
	FPGA_DATA_FIX bias_1[CH2]={0};
	FPGA_DATA_FIX bias_2[CH2]={0};
	FPGA_DATA_FIX bias_3[CH3]={0};

	FPGA_DATA_FIX bias_4[6*CH3]={0};
	FPGA_DATA_FIX bias_5[6*CH3]={0};
	FPGA_DATA_FIX bias_6[CH4]={0};

	FPGA_DATA_FIX bias_7[6*CH4]={0};
	FPGA_DATA_FIX bias_8[6*CH4]={0};
	FPGA_DATA_FIX bias_9[CH5]={0};
	//32, s=2, n=3
	FPGA_DATA_FIX bias_10[6*CH5]={0};
	FPGA_DATA_FIX bias_11[6*CH5]={0};
	FPGA_DATA_FIX bias_12[CH6]={0};
	FPGA_DATA_FIX bias_13[6*CH6]={0};
	FPGA_DATA_FIX bias_14[6*CH6]={0};
	FPGA_DATA_FIX bias_15[CH7]={0};
	FPGA_DATA_FIX bias_16[6*CH7]={0};
	FPGA_DATA_FIX bias_17[6*CH7]={0};
	FPGA_DATA_FIX bias_18[CH8]={0};
/*	//64, s=2, n=4
	FPGA_DATA_FIX bias_19[6*CH8]={0};
	FPGA_DATA_FIX bias_20[6*CH8]={0};
	FPGA_DATA_FIX bias_21[CH9]={0};
	FPGA_DATA_FIX bias_22[6*CH9]={0};
	FPGA_DATA_FIX bias_23[6*CH9]={0};
	FPGA_DATA_FIX bias_24[CH10]={0};
	FPGA_DATA_FIX bias_25[6*CH10]={0};
	FPGA_DATA_FIX bias_26[6*CH10]={0};
	FPGA_DATA_FIX bias_27[CH11]={0};
	FPGA_DATA_FIX bias_28[6*CH11]={0};
	FPGA_DATA_FIX bias_29[6*CH11]={0};
	FPGA_DATA_FIX bias_30[CH12]={0};
	//96, s=1, n=3
	FPGA_DATA_FIX bias_31[6*CH12]={0};
	FPGA_DATA_FIX bias_32[6*CH12]={0};
	FPGA_DATA_FIX bias_33[CH13]={0};
	FPGA_DATA_FIX bias_34[6*CH13]={0};
	FPGA_DATA_FIX bias_35[6*CH13]={0};
	FPGA_DATA_FIX bias_36[CH14]={0};
	FPGA_DATA_FIX bias_37[6*CH14]={0};
	FPGA_DATA_FIX bias_38[6*CH14]={0};
	FPGA_DATA_FIX bias_39[CH15]={0};
	//160, s=2, n=3
	FPGA_DATA_FIX bias_40[6*CH15]={0};
	FPGA_DATA_FIX bias_41[6*CH15]={0};
	FPGA_DATA_FIX bias_42[CH16]={0};
	FPGA_DATA_FIX bias_43[6*CH16]={0};
	FPGA_DATA_FIX bias_44[6*CH16]={0};
	FPGA_DATA_FIX bias_45[CH17]={0};
	FPGA_DATA_FIX bias_46[6*CH17]={0};
	FPGA_DATA_FIX bias_47[6*CH17]={0};
	FPGA_DATA_FIX bias_48[CH18]={0};
	//320, s=1, n=1
	FPGA_DATA_FIX bias_49[6*CH18]={0};
	FPGA_DATA_FIX bias_50[6*CH18]={0};
	FPGA_DATA_FIX bias_51[CH19]={0};
	/**/

	extern FPGA_DATA_FIX bn_y_2[32768];
	extern FPGA_DATA_FIX bn_y_18[2048];


/*
	do_conv(CH2,CH1,R+2,C+2,3,input,weight_1,y_1,bias_1,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_conv_pad(EXP2,CH2,R,C,R+2,C+2,1,1,y_1,weight_2,y_2,bias_2,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_dconv(EXP2,EXP2,R+2,C+2,y_2,weight_3,y_3,bias_3,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_conv_pad(CH3,EXP2,R,C,R+2,C+2,1,1,y_3,weight_4,y_4,bias_4,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
*/

	do_conv_pad(CH2,CH1,R+2,C+2,R/2+2,C/2+2,1,3,2,input,weight_1,y_1,bias_1,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH2,CH2,R/2+2,C/2+2,y_1,E1,V1,z_1,input_bn_I,running_mean,running_var,output_dma_O);
	//16, s=1, n=1
	do_dconv(CH2,CH2,R/2+2,C/2+2,1,z_1,weight_2,y_2,bias_2,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH2,CH2,R/2,C/2,y_2,E2,V2,z_2,input_bn_I,running_mean,running_var,output_dma_O);
/*	do_conv(CH3,CH2,R/2,C/2,1,1,z_2,weight_3,y_3,bias_3,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH3,CH3,R/2,C/2,y_3,E3,V3,z_3,input_bn_I,running_mean,running_var,output_dma_O);
	//24, s=2, n=2
	do_conv_pad(6*CH3,CH3,R/2,C/2,R/2+2,C/2+2,1,1,1,z_3,weight_4,y_4,bias_4,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH3,6*CH3,R/2+2,C/2+2,y_4,E4,V4,z_4,input_bn_I,running_mean,running_var,output_dma_O);
	do_dconv(6*CH3,6*CH3,R/2+2,C/2+2,2,z_4,weight_5,y_5,bias_5,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH3,6*CH3,R/4,C/4,y_5,E5,V5,z_5,input_bn_I,running_mean,running_var,output_dma_O);
	do_conv(CH4,6*CH3,R/4,C/4,1,1,z_5,weight_6,y_6,bias_6,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH4,CH4,R/4,C/4,y_6,E6,V6,z_6,input_bn_I,running_mean,running_var,output_dma_O);
    //
	do_conv_pad(6*CH4,CH4,R/4,C/4,R/4+2,C/4+2,1,1,1,z_6,weight_7,y_7,bias_7,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH4,6*CH4,R/4+2,C/4+2,y_7,E7,V7,z_7,input_bn_I,running_mean,running_var,output_dma_O);

	do_dconv(6*CH4,6*CH4,R/4+2,C/4+2,1,z_7,weight_8,y_8,bias_8,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH4,6*CH4,R/4,C/4,y_8,E8,V8,z_8,input_bn_I,running_mean,running_var,output_dma_O);

	do_conv(CH5,6*CH4,R/4,C/4,1,1,z_8,weight_9,y_9,bias_9,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH5,CH5,R/4,C/4,y_9,E9,V9,z_9,input_bn_I,running_mean,running_var,output_dma_O);
	//32, s=2, n=3
	do_conv_pad(6*CH5,CH5,R/4,C/4,R/4+2,C/4+2,1,1,1,z_9,weight_10,y_10,bias_10,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH5,6*CH5,R/4+2,C/4+2,y_10,E10,V10,z_10,input_bn_I,running_mean,running_var,output_dma_O);
	do_dconv(6*CH5,6*CH5,R/4+2,C/4+2,2,z_10,weight_11,y_11,bias_11,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH5,6*CH5,R/8,C/8,y_11,E11,V11,z_11,input_bn_I,running_mean,running_var,output_dma_O);
	do_conv(CH6,6*CH5,R/8,C/8,1,1,z_11,weight_12,y_12,bias_12,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH6,CH6,R/8,C/8,y_12,E12,V12,z_12,input_bn_I,running_mean,running_var,output_dma_O);
    //
	do_conv_pad(6*CH6,CH6,R/8,C/8,R/8+2,C/8+2,1,1,1,z_12,weight_13,y_13,bias_13,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH6,6*CH6,R/8+2,C/8+2,y_13,E13,V13,z_13,input_bn_I,running_mean,running_var,output_dma_O);
	do_dconv(6*CH6,6*CH6,R/8+2,C/8+2,1,z_13,weight_14,y_14,bias_14,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH6,6*CH6,R/8,C/8,y_14,E14,V14,z_14,input_bn_I,running_mean,running_var,output_dma_O);
	do_conv(CH7,6*CH6,R/8,C/8,1,1,z_14,weight_15,y_15,bias_15,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH7,CH7,R/8,C/8,y_15,E15,V15,z_15,input_bn_I,running_mean,running_var,output_dma_O);
	//
	do_conv_pad(6*CH7,CH7,R/8,C/8,R/8+2,C/8+2,1,1,1,z_15,weight_16,y_16,bias_16,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH7,6*CH7,R/8+2,C/8+2,y_16,E16,V16,z_16,input_bn_I,running_mean,running_var,output_dma_O);
	do_dconv(6*CH7,6*CH7,R/8+2,C/8+2,1,z_16,weight_17,y_17,bias_17,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH7,6*CH7,R/8,C/8,y_17,E17,V17,z_17,input_bn_I,running_mean,running_var,output_dma_O);
	do_conv(CH8,6*CH7,R/8,C/8,1,1,z_17,weight_18,y_18,bias_18,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH8,CH8,R/8,C/8,y_18,E18,V18,z_18,input_bn_I,running_mean,running_var,output_dma_O);
/*	//64, s=2, n=4
	do_conv_pad(6*CH8,CH8,R/8,C/8,R/8+2,C/8+2,1,1,1,z_18,weight_19,y_19,bias_19,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH8,6*CH8,R/8+2,C/8+2,y_19,E19,V19,z_19,input_bn_I,running_mean,running_var,output_dma_O);
	do_dconv(6*CH8,6*CH8,R/8+2,C/8+2,2,z_19,weight_20,y_20,bias_20,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH8,6*CH8,R/16,C/16,y_20,E20,V20,z_20,input_bn_I,running_mean,running_var,output_dma_O);
	do_conv(CH9,6*CH8,R/16,C/16,1,1,z_20,weight_21,y_21,bias_21,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH9,CH9,R/16,C/16,y_21,E21,V21,z_21,input_bn_I,running_mean,running_var,output_dma_O);

	do_conv_pad(6*CH9,CH9,R/16,C/16,R/16+2,C/16+2,1,1,1,z_21,weight_22,y_22,bias_22,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH9,6*CH9,R/16+2,C/16+2,y_22,E22,V22,z_22,input_bn_I,running_mean,running_var,output_dma_O);
	do_dconv(6*CH9,6*CH9,R/16+2,C/16+2,1,z_22,weight_23,y_23,bias_23,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH9,6*CH9,R/16,C/16,y_23,E23,V23,z_23,input_bn_I,running_mean,running_var,output_dma_O);
	do_conv(CH10,6*CH9,R/16,C/16,1,1,z_23,weight_24,y_24,bias_24,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH10,CH10,R/16,C/16,y_24,E24,V24,z_24,input_bn_I,running_mean,running_var,output_dma_O);

	do_conv_pad(6*CH10,CH10,R/16,C/16,R/16+2,C/16+2,1,1,1,z_24,weight_25,y_25,bias_25,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH10,6*CH10,R/16+2,C/16+2,y_25,E25,V25,z_25,input_bn_I,running_mean,running_var,output_dma_O);
	do_dconv(6*CH10,6*CH10,R/16+2,C/16+2,1,z_25,weight_26,y_26,bias_26,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH10,6*CH10,R/16,C/16,y_26,E26,V26,z_26,input_bn_I,running_mean,running_var,output_dma_O);
	do_conv(CH11,6*CH10,R/16,C/16,1,1,z_26,weight_27,y_27,bias_27,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH11,CH11,R/16,C/16,y_27,E27,V27,z_27,input_bn_I,running_mean,running_var,output_dma_O);

	do_conv_pad(6*CH11,CH11,R/16,C/16,R/16+2,C/16+2,1,1,1,z_27,weight_28,y_28,bias_28,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH11,6*CH11,R/16+2,C/16+2,y_28,E28,V28,z_28,input_bn_I,running_mean,running_var,output_dma_O);
	do_dconv(6*CH11,6*CH11,R/16+2,C/16+2,1,z_28,weight_29,y_29,bias_29,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH11,6*CH11,R/16,C/16,y_29,E29,V29,z_29,input_bn_I,running_mean,running_var,output_dma_O);
	do_conv(CH12,6*CH11,R/16,C/16,1,1,z_29,weight_30,y_30,bias_30,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH12,CH12,R/16,C/16,y_30,E30,V30,z_30,input_bn_I,running_mean,running_var,output_dma_O);

	//96, s=1, n=3
	do_conv_pad(6*CH12,CH12,R/16,C/16,R/16+2,C/16+2,1,1,1,z_30,weight_31,y_31,bias_31,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH12,6*CH12,R/16+2,C/16+2,y_31,E31,V31,z_31,input_bn_I,running_mean,running_var,output_dma_O);
	do_dconv(6*CH12,6*CH12,R/16+2,C/16+2,1,z_31,weight_32,y_32,bias_32,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH12,6*CH12,R/16,C/16,y_32,E32,V32,z_32,input_bn_I,running_mean,running_var,output_dma_O);
	do_conv(CH13,6*CH12,R/16,C/16,1,1,z_32,weight_33,y_33,bias_33,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH13,CH13,R/16,C/16,y_33,E33,V33,z_33,input_bn_I,running_mean,running_var,output_dma_O);

	do_conv_pad(6*CH13,CH13,R/16,C/16,R/16+2,C/16+2,1,1,1,z_33,weight_34,y_34,bias_34,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH13,6*CH13,R/16+2,C/16+2,y_34,E34,V34,z_34,input_bn_I,running_mean,running_var,output_dma_O);
	do_dconv(6*CH13,6*CH13,R/16+2,C/16+2,1,z_34,weight_35,y_35,bias_35,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH13,6*CH13,R/16,C/16,y_35,E35,V35,z_35,input_bn_I,running_mean,running_var,output_dma_O);
	do_conv(CH14,6*CH13,R/16,C/16,1,1,z_35,weight_36,y_36,bias_36,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH14,CH14,R/16,C/16,y_36,E36,V36,z_36,input_bn_I,running_mean,running_var,output_dma_O);

	do_conv_pad(6*CH14,CH14,R/16,C/16,R/16+2,C/16+2,1,1,1,z_36,weight_37,y_37,bias_37,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH14,6*CH14,R/16+2,C/16+2,y_37,E37,V37,z_37,input_bn_I,running_mean,running_var,output_dma_O);
	do_dconv(6*CH14,6*CH14,R/16+2,C/16+2,1,z_37,weight_38,y_38,bias_38,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH14,6*CH14,R/16,C/16,y_38,E38,V38,z_38,input_bn_I,running_mean,running_var,output_dma_O);
	do_conv(CH15,6*CH14,R/16,C/16,1,1,z_38,weight_39,y_39,bias_39,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH15,CH15,R/16,C/16,y_39,E39,V39,z_39,input_bn_I,running_mean,running_var,output_dma_O);
	//160, s=2, n=3
	do_conv_pad(6*CH15,CH15,R/16,C/16,R/16+2,C/16+2,1,1,1,z_39,weight_40,y_40,bias_40,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH15,6*CH15,R/16+2,C/16+2,y_40,E40,V40,z_40,input_bn_I,running_mean,running_var,output_dma_O);
	do_dconv(6*CH15,6*CH15,R/16+2,C/16+2,2,z_40,weight_41,y_41,bias_41,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH15,6*CH15,R/32,C/32,y_41,E41,V41,z_41,input_bn_I,running_mean,running_var,output_dma_O);
	do_conv(CH16,6*CH15,R/32,C/32,1,1,z_41,weight_42,y_42,bias_42,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH16,CH16,R/32,C/32,y_42,E42,V42,z_42,input_bn_I,running_mean,running_var,output_dma_O);

	do_conv_pad(6*CH16,CH16,R/32,C/32,R/32+2,C/32+2,1,1,1,z_42,weight_43,y_43,bias_43,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH16,6*CH16,R/32+2,C/32+2,y_43,E43,V43,z_43,input_bn_I,running_mean,running_var,output_dma_O);
	do_dconv(6*CH16,6*CH16,R/32+2,C/32+2,1,z_43,weight_44,y_44,bias_44,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH16,6*CH16,R/32,C/32,y_44,E44,V44,z_44,input_bn_I,running_mean,running_var,output_dma_O);
	do_conv(CH17,6*CH16,R/32,C/32,1,1,z_44,weight_45,y_45,bias_45,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH17,CH17,R/32,C/32,y_45,E45,V45,z_45,input_bn_I,running_mean,running_var,output_dma_O);

	do_conv_pad(6*CH17,CH17,R/32,C/32,R/32+2,C/32+2,1,1,1,z_45,weight_46,y_46,bias_46,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH17,6*CH17,R/32+2,C/32+2,y_46,E46,V46,z_46,input_bn_I,running_mean,running_var,output_dma_O);
	do_dconv(6*CH17,6*CH17,R/32+2,C/32+2,1,z_46,weight_47,y_47,bias_47,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH17,6*CH17,R/32,C/32,y_47,E47,V47,z_47,input_bn_I,running_mean,running_var,output_dma_O);
	do_conv(CH18,6*CH17,R/32,C/32,1,1,z_47,weight_48,y_48,bias_48,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH18,CH18,R/32,C/32,y_48,E48,V48,z_48,input_bn_I,running_mean,running_var,output_dma_O);
	//320, s=1, n=1
	do_conv_pad(6*CH18,CH18,R/32,C/32,R/32+2,C/32+2,1,1,1,z_48,weight_49,y_49,bias_49,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH18,6*CH18,R/32+2,C/32+2,y_49,E49,V49,z_49,input_bn_I,running_mean,running_var,output_dma_O);
	do_dconv(6*CH18,6*CH18,R/32+2,C/32+2,1,z_49,weight_50,y_50,bias_50,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(6*CH18,6*CH18,R/32,C/32,y_50,E50,V50,z_50,input_bn_I,running_mean,running_var,output_dma_O);
	do_conv(CH19,6*CH18,R/32,C/32,1,1,z_50,weight_51,y_51,bias_51,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH19,CH19,R/32,C/32,y_51,E51,V51,z_51,input_bn_I,running_mean,running_var,output_dma_O);
/*
*/


	int real_R = R;
	int real_C = C;

	printf("\n\n================\n");


	for(int m=0;m<CH2;m++){
		for(int r=0;r<real_R;r++){
			for(int c=0;c<real_C;c++){
				int res;
				int g_res;
				res = z_2[m*(real_R)*(real_C) + r*(real_C) + c];
				g_res = bn_y_2[m*(real_R)*(real_C) + (r)*(real_C) + (c)];
				printf("%d ",res-g_res);
			}
			printf("\n");
		}
		printf("========%d==========\n",m);
		printf("\n");
	}
/*
	for(int m=0;m<CH5;m++){
		for(int r=0;r<real_R;r++){
			for(int c=0;c<real_C;c++){
				int res;
				int g_res;
				res = z_18[m*(real_R)*(real_C) + r*(real_C) + c];
				printf("%d ",res);
			}
			printf("\n");
		}
		printf("========%d==========\n",m);
		printf("\n");
	}
*/

//	// Do dConv.
//	do_dconv_pad(CH2,CH1,R,C,1,input,weight_1,my_output,bias_1,input_dma_W,input_dma_I,input_dma_B,output_dma_O);


//	for(int m=0;m<CH2;m++){
//		for(int r=0;r<R;r++){
//			for(int c=0;c<C;c++){
//				int res;
//				res = my_output[m*(R)*(C) + r*(C) + c];
//				printf("%d ", res);
//			}
//			printf("\n");
//		}
//
//		for(int r=0;r<R-1*K+1;r++){
//			for(int c=0;c<C-1*K+1;c++){
//				int g_res;
//				g_res = output[m*(R-1*K+1)*(C-1*K+1) + r*(C-1*K+1) + c];
//				printf("%d ", g_res);
//			}
//			printf("\n");
//		}
//		printf("========%d==========\n",m);
//		printf("\n");
//	}

/**/


	return 0;
}
