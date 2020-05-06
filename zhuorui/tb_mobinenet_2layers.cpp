#include "source_fix.h"
#include <math.h>
#include <stdlib.h>

int do_conv(int M,int N,int R,int C,int c_k,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * weight,FPGA_DATA_FIX * output, FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		);

int do_conv_pad(int M,int N,int R,int C,int outR,int outC,int pad,int c_k,
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

int do_dconv(int M,int N,int R,int C,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * weight,FPGA_DATA_FIX * output, FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		);

int do_dconv_pad(int M,int N,int R,int C,int pad,
		FPGA_DATA_FIX * input,FPGA_DATA_FIX * weight,FPGA_DATA_FIX * output, FPGA_DATA_FIX * bias,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O
		);

int main(){
	hls::stream<DMA_DATA_128B_FIX> input_dma_W("input_dma_W");
	hls::stream<DMA_DATA_128B_FIX> input_dma_I("input_dma_I");
	hls::stream<DMA_DATA_128B_FIX> input_dma_B("input_dma_B");
	hls::stream<DMA_DATA_128B_FIX> output_dma_O("output_dma_O");

	hls::stream<DMA_DATA_128B_FIX> input_bn_I("input_bn_I");
	hls::stream<DMA_DATA_128B_FIX> running_mean("input_E");
	hls::stream<DMA_DATA_128B_FIX> running_var("input_V");

	// Initialization
	extern FPGA_DATA_FIX input[CH1*(R+2)*(C+2)];
	extern FPGA_DATA_FIX weight_1[CH1*CH2*3*3];
	//16, s=1, n=1
	extern FPGA_DATA_FIX weight_2[CH2*CH2*1*1];
	extern FPGA_DATA_FIX weight_3[CH3*CH2*3*3];
/*	//24, s=2, n=2
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
	//64, s=2, n=4
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
	extern FPGA_DATA_FIX weight_51[6*CH19*CH18*1*1];*/
//mean for batch norm
	extern FPGA_DATA_FIX E1[CH2];
	extern FPGA_DATA_FIX E2[CH2];
	extern FPGA_DATA_FIX E3[CH3];
/*
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
	//64, s=2, n=4
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
	extern FPGA_DATA_FIX E51[CH19];*/

//variance for batch norm
	extern FPGA_DATA_FIX V1[CH2];
	extern FPGA_DATA_FIX V2[CH2];
	extern FPGA_DATA_FIX V3[CH3];
/*
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
	//64, s=2, n=4
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
*/
//	extern FPGA_DATA_FIX x_1[CH2*(R+2)*(C+2)];
//	extern FPGA_DATA_FIX x_2_1[CH2*(R)*(C)];
	extern FPGA_DATA_FIX output[CH3*(R)*(C)];

	FPGA_DATA_FIX y_1[CH2*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_2[CH2*(R)*(C)]={0};
	FPGA_DATA_FIX y_3[CH3*(R)*(C)]={0};
/*
	FPGA_DATA_FIX y_4[6*CH3*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_5[6*CH3*(R)*(C)]={0};
	FPGA_DATA_FIX y_6[CH4*(R)*(C)]={0};

	FPGA_DATA_FIX y_7[6*CH4*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_8[6*CH4*(R)*(C)]={0};
	FPGA_DATA_FIX y_9[CH5*(R)*(C)]={0};
	//32, s=2, n=3
	FPGA_DATA_FIX y_10[6*CH5*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_11[6*CH5*(R)*(C)]={0};
	FPGA_DATA_FIX y_12[CH6*(R)*(C)]={0};

	FPGA_DATA_FIX y_13[6*CH6*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_14[6*CH6*(R)*(C)]={0};
	FPGA_DATA_FIX y_15[CH7*(R)*(C)]={0};

	FPGA_DATA_FIX y_16[6*CH7*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_17[6*CH7*(R)*(C)]={0};
	FPGA_DATA_FIX y_18[CH8*(R)*(C)]={0};
	//64, s=2, n=4
	FPGA_DATA_FIX y_19[6*CH8*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_20[6*CH8*(R)*(C)]={0};
	FPGA_DATA_FIX y_21[CH9*(R)*(C)]={0};

	FPGA_DATA_FIX y_22[6*CH9*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_23[6*CH9*(R)*(C)]={0};
	FPGA_DATA_FIX y_24[CH10*(R)*(C)]={0};

	FPGA_DATA_FIX y_25[6*CH10*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_26[6*CH10*(R)*(C)]={0};
	FPGA_DATA_FIX y_27[CH11*(R)*(C)]={0};

	FPGA_DATA_FIX y_28[6*CH11*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_29[6*CH11*(R)*(C)]={0};
	FPGA_DATA_FIX y_30[CH12*(R)*(C)]={0};
	//96, s=1, n=3
	FPGA_DATA_FIX y_31[6*CH12*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_32[6*CH12*(R)*(C)]={0};
	FPGA_DATA_FIX y_33[CH13*(R)*(C)]={0};

	FPGA_DATA_FIX y_34[6*CH13*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_35[6*CH13*(R)*(C)]={0};
	FPGA_DATA_FIX y_36[CH14*(R)*(C)]={0};

	FPGA_DATA_FIX y_37[6*CH14*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_38[6*CH14*(R)*(C)]={0};
	FPGA_DATA_FIX y_39[CH15*(R)*(C)]={0};
	//160, s=2, n=3
	FPGA_DATA_FIX y_40[6*CH15*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_41[6*CH15*(R)*(C)]={0};
	FPGA_DATA_FIX y_42[CH16*(R)*(C)]={0};

	FPGA_DATA_FIX y_43[6*CH16*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_44[6*CH16*(R)*(C)]={0};
	FPGA_DATA_FIX y_45[CH17*(R)*(C)]={0};

	FPGA_DATA_FIX y_46[6*CH17*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_47[6*CH17*(R)*(C)]={0};
	FPGA_DATA_FIX y_48[CH18*(R)*(C)]={0};
	//320, s=1, n=1
	FPGA_DATA_FIX y_49[6*CH18*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX y_50[6*CH18*(R)*(C)]={0};
	FPGA_DATA_FIX y_51[CH19*(R)*(C)]={0};
*/
	//z is the output of batch normalization layer
	FPGA_DATA_FIX z_1[CH2*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_2[CH2*(R)*(C)]={0};
	FPGA_DATA_FIX z_3[CH3*(R)*(C)]={0};
/*
	FPGA_DATA_FIX z_4[6*CH3*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_5[6*CH3*(R)*(C)]={0};
	FPGA_DATA_FIX z_6[CH4*(R)*(C)]={0};

	FPGA_DATA_FIX z_7[6*CH4*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_8[6*CH4*(R)*(C)]={0};
	FPGA_DATA_FIX z_9[CH5*(R)*(C)]={0};
	//32, s=2, n=3
	FPGA_DATA_FIX z_10[6*CH5*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_11[6*CH5*(R)*(C)]={0};
	FPGA_DATA_FIX z_12[CH6*(R)*(C)]={0};

	FPGA_DATA_FIX z_13[6*CH6*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_14[6*CH6*(R)*(C)]={0};
	FPGA_DATA_FIX z_15[CH7*(R)*(C)]={0};

	FPGA_DATA_FIX z_16[6*CH7*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_17[6*CH7*(R)*(C)]={0};
	FPGA_DATA_FIX z_18[CH8*(R)*(C)]={0};
	//64, s=2, n=4
	FPGA_DATA_FIX z_19[6*CH8*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_20[6*CH8*(R)*(C)]={0};
	FPGA_DATA_FIX z_21[CH9*(R)*(C)]={0};

	FPGA_DATA_FIX z_22[6*CH9*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_23[6*CH9*(R)*(C)]={0};
	FPGA_DATA_FIX z_24[CH10*(R)*(C)]={0};

	FPGA_DATA_FIX z_25[6*CH10*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_26[6*CH10*(R)*(C)]={0};
	FPGA_DATA_FIX z_27[CH11*(R)*(C)]={0};

	FPGA_DATA_FIX z_28[6*CH11*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_29[6*CH11*(R)*(C)]={0};
	FPGA_DATA_FIX z_30[CH12*(R)*(C)]={0};
	//96, s=1, n=3
	FPGA_DATA_FIX z_31[6*CH12*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_32[6*CH12*(R)*(C)]={0};
	FPGA_DATA_FIX z_33[CH13*(R)*(C)]={0};

	FPGA_DATA_FIX z_34[6*CH13*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_35[6*CH13*(R)*(C)]={0};
	FPGA_DATA_FIX z_36[CH14*(R)*(C)]={0};

	FPGA_DATA_FIX z_37[6*CH14*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_38[6*CH14*(R)*(C)]={0};
	FPGA_DATA_FIX z_39[CH15*(R)*(C)]={0};
	//160, s=2, n=3
	FPGA_DATA_FIX z_40[6*CH15*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_41[6*CH15*(R)*(C)]={0};
	FPGA_DATA_FIX z_42[CH16*(R)*(C)]={0};

	FPGA_DATA_FIX z_43[6*CH16*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_44[6*CH16*(R)*(C)]={0};
	FPGA_DATA_FIX z_45[CH17*(R)*(C)]={0};

	FPGA_DATA_FIX z_46[6*CH17*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_47[6*CH17*(R)*(C)]={0};
	FPGA_DATA_FIX z_48[CH18*(R)*(C)]={0};
	//320, s=1, n=1
	FPGA_DATA_FIX z_49[6*CH18*(R+2)*(C+2)]={0};
	FPGA_DATA_FIX z_50[6*CH18*(R)*(C)]={0};
	FPGA_DATA_FIX z_51[CH19*(R)*(C)]={0};
*/
	FPGA_DATA_FIX bias_1[CH2]={0};
	FPGA_DATA_FIX bias_2[CH2]={0};
	FPGA_DATA_FIX bias_3[CH3]={0};
/*
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
	//64, s=2, n=4
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
*/

/*
	do_conv(CH2,CH1,R+2,C+2,3,input,weight_1,y_1,bias_1,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_conv_pad(EXP2,CH2,R,C,R+2,C+2,1,1,y_1,weight_2,y_2,bias_2,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_dconv(EXP2,EXP2,R+2,C+2,y_2,weight_3,y_3,bias_3,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_conv_pad(CH3,EXP2,R,C,R+2,C+2,1,1,y_3,weight_4,y_4,bias_4,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
*/

	do_conv_pad(CH2,CH1,R+2,C+2,R+2,C+2,1,3,input,weight_1,y_1,bias_1,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH2,CH2,R+2,C+2,y_1,E1,V1,z_1,input_bn_I,running_mean,running_var,output_dma_O);
	//16, s=1, n=1
	do_dconv(CH2,CH2,R+2,C+2,z_1,weight_2,y_2,bias_2,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH2,CH2,R,C,y_2,E2,V2,z_2,input_bn_I,running_mean,running_var,output_dma_O);

	do_conv(CH3,CH2,R,C,1,z_2,weight_3,y_3,bias_3,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
	do_bn(CH3,CH3,R,C,y_3,E3,V3,z_3,input_bn_I,running_mean,running_var,output_dma_O);
	//24, s=2, n=2
//	do_conv_pad(CH2,CH1,R+2,C+2,R+2,C+2,1,3,z_3,weight_4,y_4,bias_4,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
//	do_dconv(6*CH3,6*CH3,R+2,C+2,y_4,weight_5,y_5,bias_5,input_dma_W,input_dma_I,input_dma_B,output_dma_O);
//	do_conv(CH4,CH3,R,C,1,y_5,weight_6,y_6,bias_6,input_dma_W,input_dma_I,input_dma_B,output_dma_O);


	int real_R = R;
	int real_C = C;

	printf("\n\n================\n");


	for(int m=0;m<CH3;m++){
		for(int r=0;r<real_R;r++){
			for(int c=0;c<real_C;c++){
				int res;
				int g_res;
				res = z_3[m*(real_R)*(real_C) + r*(real_C) + c];
				g_res = output[m*(real_R)*(real_C) + (r)*(real_C) + (c)];
				printf("%d ",res-g_res);
			}
			printf("\n");
		}
		printf("========%d==========\n",m);
		printf("\n");
	}


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




	return 0;
}
