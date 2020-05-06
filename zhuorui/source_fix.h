#include <hls_stream.h>
#include <ap_axi_sdata.h>
#include <ap_fixed.h>
#include <ap_int.h>

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))



// divided_Tm_8 cannot be 1 since the weight load needs it to be at least 2
const int Tm=32;
const int divided_Tm_8=4;

const int Tn=32;
const int divided_Tn_8=4;
const int Tn_8=divided_Tn_8*8;

const int Tr=7;
const int Tc=14;

const int Tn_dconv=128;
const int divided_Tn_8_dconv=16;
const int Tr_dconv=14;
const int Tc_dconv=14;
const int K_dconv=3;

const int Tn_bn=64;
const int divided_Tn_8_bn=8;
const int Tr_bn=7;
const int Tc_bn=14;



//const int M = 32;
//const int N = 32;
//const int R = 32;
//const int C = 32;

//const int M = 32;
//const int N = 32;
//const int R = 32;
//const int C = 32;

/*
const int CH1 = 3;
const int CH2 = 16;
const int EXP2 = 16;
const int CH3 = 16;
const int R = 32;
const int C = 32;
const int K=3;
*/

const int CH1 = 3;
const int CH2 = 32;
const int CH3 = 16;
const int CH4 = 24;
const int CH5 = 24;
const int CH6 = 32;
const int CH7 = 32;
const int CH8 = 32;
const int CH9 = 64;
const int CH10 = 64;
const int CH11 = 64;
const int CH12 = 64;
const int CH13 = 96;
const int CH14 = 96;
const int CH15 = 96;
const int CH16 = 160;
const int CH17 = 160;
const int CH18 = 160;
const int CH19 = 320;

const int R = 3;
const int C = 3;
const int K = 3;

typedef float FPGA_DATA;
//typedef float FPGA_DATA_FIX;
//typedef ap_fixed<16,10,AP_TRN_ZERO, AP_SAT> FPGA_DATA_FIX;
typedef short FPGA_DATA_FIX;
struct DOUBLE{
	FPGA_DATA data1;
	FPGA_DATA data2;
};

struct TRIPLE{
	FPGA_DATA data1;
	FPGA_DATA data2;
	FPGA_DATA data3;
};

struct QUAD{
	FPGA_DATA data1;
	FPGA_DATA data2;
	FPGA_DATA data3;
	FPGA_DATA data4;
};

struct SIXTEEN{
	FPGA_DATA_FIX data1;
	FPGA_DATA_FIX data2;
	FPGA_DATA_FIX data3;
	FPGA_DATA_FIX data4;
	FPGA_DATA_FIX data5;
	FPGA_DATA_FIX data6;
	FPGA_DATA_FIX data7;
	FPGA_DATA_FIX data8;
};

struct DMA_DATA{
	DOUBLE data;
	bool last;
};



struct DMA_DATA_128B{
	QUAD data;
	bool last;
};

struct DMA_DATA_128B_FIX{
	SIXTEEN data;
	bool last;
};

struct DMA_DATA_B{
	FPGA_DATA data;
	bool last;
};


struct AURORA_DATA_128B_FIX{
	SIXTEEN data;
};

