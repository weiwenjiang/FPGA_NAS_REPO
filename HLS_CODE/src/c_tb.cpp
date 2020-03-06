#include <hls_stream.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "source_fix.h"
using namespace std;

void cconv(hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O,
		int row,int col,int num,int N,int custom_k,int custom_Tr,int custom_Tc,
		int custom_Tm,int custom_Tn,int NL_Opt);

int create_W(FPGA_DATA_FIX given_WEIGHT[Tm][Tn][K][K],int custom_k);
int create_I(FPGA_DATA_FIX given_IFM[Tn][Tr][Tc],int custom_Tr,int custom_Tc);
int create_O(int row, int col, int custom_k, int custom_Tr, int custom_Tc);
int print_weight(FPGA_DATA_FIX WEIGHT1[2][Tm][Tn][K][K],int custom_k);
int create_B(FPGA_DATA_FIX given_BIAS[Tm]);


extern FPGA_DATA_FIX sim_WEIGHT[2][2][Tm][Tn][K][K];
extern FPGA_DATA_FIX sim_IFM[2][Tn_8][Tr][Tc];
extern FPGA_DATA_FIX sim_OFM[Tm][Tr][Tc];
extern FPGA_DATA_FIX sim_BIAS[Tm];


FPGA_DATA_FIX given_WEIGHT[Tm][Tn][K][K]={0};
FPGA_DATA_FIX given_IFM[Tn][Tr][Tc]={0};
FPGA_DATA_FIX given_BIAS[Tm]={0};
FPGA_DATA_FIX obtained_OFM[Tm][Tr][Tc]={0};


FPGA_DATA_FIX non_linear(FPGA_DATA_FIX op, int NL_Opt){
	if(NL_Opt==0){
		return op;
	}else if(NL_Opt==1){
		return op>=0?op:0;
	}else if(NL_Opt==2){
		FPGA_DATA_FIX tmp = op+3>=0?op+3:0;
		return (tmp<=6?tmp:6)/float(6)*(op);
	}
}


int cconv_main(){
	static FPGA_DATA_FIX OFM[Tm][Tr][Tc];

	hls::stream<DMA_DATA_128B_FIX> input_dma_W("input_dma_W");
	hls::stream<DMA_DATA_128B_FIX> input_dma_I("input_dma_I");
	hls::stream<DMA_DATA_128B_FIX> input_dma_B("input_dma_B");
	hls::stream<DMA_DATA_128B_FIX> output_dma_O("output_dma_O");

	DMA_DATA_128B_FIX weight;
	DMA_DATA_128B_FIX ifm;
	DMA_DATA_128B_FIX bias;

	const int custom_k = 3;
	const int custom_Tr = 7;
	const int custom_Tc = 14;
	const int custom_Tm = 16;
	const int custom_Tn = 16;

	int NL_Opt = 2;

	srand(time(0));
	for(int i=0;i<Tm;i++){
		for(int j=0;j<Tn;j++){
			for(int m=0;m<K;m++){
				for(int n=0;n<K;n++){
					given_WEIGHT[i][j][m][n] = rand()%10 + 1;
					given_WEIGHT[i][j][m][n] = given_WEIGHT[i][j][m][n]%2==1?given_WEIGHT[i][j][m][n]:-1*given_WEIGHT[i][j][m][n];
//					given_WEIGHT[i][j][m][n] = -1 * given_WEIGHT[i][j][m][n];
				}
			}
		}
	}

	for(int i=0;i<Tn;i++){
		for(int j=0;j<Tr;j++){
			for(int m=0;m<Tc;m++){
				given_IFM[i][j][m] = rand()%10 + 1;
			}
		}
	}

	for(int i=0;i<Tm;i++){
		given_BIAS[i] = rand()%10 + 1;
		given_BIAS[i] = given_BIAS[i]%2==0?given_BIAS[i]:-1*given_BIAS[i];
//		given_BIAS[i] = -1 * given_BIAS[i];
	}





	for(int i=0;i<K;i++){
		for(int j=0;j<K;j++){
			for(int r=0;r<Tr;r++){
				for(int c=0;c<Tc;c++){
					for(int m=0;m<Tm;m++){
						for(int n=0;n<Tn;n++){
							FPGA_DATA_FIX add_res1 = given_WEIGHT[m][n][i][j]*given_IFM[n][r][c];
							obtained_OFM[m][r][c] = obtained_OFM[m][r][c] + add_res1;
						}
					}
				}
			}
		}
	}

	cout<<"======="<<"Results from Direct Computation"<<"=========="<<endl;
	for(int i=0;i<Tm;i++){
		cout<<"The "<<i<<"th [Tr][Tc]"<<endl;
		for(int j=0;j<Tr;j++){
			for(int m=0;m<Tc;m++){
				obtained_OFM[i][j][m] = non_linear(obtained_OFM[i][j][m]+given_BIAS[i],NL_Opt);
				cout<<obtained_OFM[i][j][m]<<" ";
			}
			cout<<endl;
		}
		cout<<endl<<endl;
	}




	create_W(given_WEIGHT,custom_k);
	create_I(given_IFM,custom_Tr,custom_Tc);
	create_B(given_BIAS);
	create_O(0, 0, custom_k, custom_Tr, custom_Tc);


//	cout<<"======="<<"Results from Simulation"<<"=========="<<endl;
//	for(int i=0;i<Tm;i++){
//		cout<<"The "<<i<<"th [Tr][Tc]"<<endl;
//		for(int j=0;j<Tr;j++){
//			for(int m=0;m<Tc;m++){
//				cout<<sim_OFM[i][j][m]<<" ";
//			}
//			cout<<endl;
//		}
//		cout<<endl<<endl;
//	}



	for(int i=0;i<divided_Tm_8;i++){
		for(int j=0;j<Tn;j++){
			for(int m=0;m<custom_k;m++){
				for(int l=0;l<custom_k;l++){
					if(i<divided_Tm_8/2){
						weight.data.data1 = sim_WEIGHT[0][0][i*8][j][m][l];
						weight.data.data2 = sim_WEIGHT[0][0][i*8+1][j][m][l];
						weight.data.data3 = sim_WEIGHT[0][0][i*8+2][j][m][l];
						weight.data.data4 = sim_WEIGHT[0][0][i*8+3][j][m][l];
						weight.data.data5 = sim_WEIGHT[0][0][i*8+4][j][m][l];
						weight.data.data6 = sim_WEIGHT[0][0][i*8+5][j][m][l];
						weight.data.data7 = sim_WEIGHT[0][0][i*8+6][j][m][l];
						weight.data.data8 = sim_WEIGHT[0][0][i*8+7][j][m][l];
					}else{

						weight.data.data1 = sim_WEIGHT[0][1][i*8][j][m][l];
						weight.data.data2 = sim_WEIGHT[0][1][i*8+1][j][m][l];
						weight.data.data3 = sim_WEIGHT[0][1][i*8+2][j][m][l];
						weight.data.data4 = sim_WEIGHT[0][1][i*8+3][j][m][l];
						weight.data.data5 = sim_WEIGHT[0][1][i*8+4][j][m][l];
						weight.data.data6 = sim_WEIGHT[0][1][i*8+5][j][m][l];
						weight.data.data7 = sim_WEIGHT[0][1][i*8+6][j][m][l];
						weight.data.data8 = sim_WEIGHT[0][1][i*8+7][j][m][l];
					}
					if(i==divided_Tm_8-1 && j==Tn-1 && m==custom_k-1 && l==custom_k-1)
						weight.last = true;
					else
						weight.last = false;
					input_dma_W.write(weight);
				}
			}
		}
	}

	for(int i=0;i<divided_Tm_8;i++){
		for(int j=0;j<Tn;j++){
			for(int m=0;m<custom_k;m++){
				for(int l=0;l<custom_k;l++){

					weight.data.data1 = 0;
					weight.data.data2 = 0;
					weight.data.data3 = 0;
					weight.data.data4 = 0;
					weight.data.data5 = 0;
					weight.data.data6 = 0;
					weight.data.data7 = 0;
					weight.data.data8 = 0;

					if(i==divided_Tm_8-1 && j==Tn-1 && m==custom_k-1 && l==custom_k-1)
						weight.last = true;
					else
						weight.last = false;
					input_dma_W.write(weight);
				}
			}
		}
	}

	for(int i=0;i<divided_Tn_8;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				ifm.data.data1 = sim_IFM[0][i*8][j][m];
				ifm.data.data2 = sim_IFM[0][i*8+1][j][m];
				ifm.data.data3 = sim_IFM[0][i*8+2][j][m];
				ifm.data.data4 = sim_IFM[0][i*8+3][j][m];
				ifm.data.data5 = sim_IFM[0][i*8+4][j][m];
				ifm.data.data6 = sim_IFM[0][i*8+5][j][m];
				ifm.data.data7 = sim_IFM[0][i*8+6][j][m];
				ifm.data.data8 = sim_IFM[0][i*8+7][j][m];
				if(i==divided_Tn_8-1 && j==custom_Tr-1 && m==custom_Tc-1)
					ifm.last = true;
				else
					ifm.last = false;
				input_dma_I.write(ifm);
			}
		}
	}


	for(int i=0;i<divided_Tn_8;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				ifm.data.data1 = 0;
				ifm.data.data2 = 0;
				ifm.data.data3 = 0;
				ifm.data.data4 = 0;
				ifm.data.data5 = 0;
				ifm.data.data6 = 0;
				ifm.data.data7 = 0;
				ifm.data.data8 = 0;
				if(i==divided_Tn_8-1 && j==custom_Tr-1 && m==custom_Tc-1)
					ifm.last = true;
				else
					ifm.last = false;
				input_dma_I.write(ifm);
			}
		}
	}

	for(int i=0;i<divided_Tm_8;i++){
		bias.data.data1 = sim_BIAS[i*8+0];
		bias.data.data2 = sim_BIAS[i*8+1];
		bias.data.data3 = sim_BIAS[i*8+2];
		bias.data.data4 = sim_BIAS[i*8+3];
		bias.data.data5 = sim_BIAS[i*8+4];
		bias.data.data6 = sim_BIAS[i*8+5];
		bias.data.data7 = sim_BIAS[i*8+6];
		bias.data.data8 = sim_BIAS[i*8+7];
		if(i==divided_Tm_8-1)
			bias.last = true;
		else
			bias.last = false;
		input_dma_B.write(bias);
	}

	cconv(input_dma_W,input_dma_I,input_dma_B,output_dma_O,\
			0,0,0,Tn*2,custom_k,custom_Tr,custom_Tc,custom_Tm,custom_Tn,NL_Opt);

	for(int i=0;i<divided_Tm_8;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int k=0;k<custom_Tc;k++){
				DMA_DATA_128B_FIX output_dma_O_data = output_dma_O.read();
				OFM[i*8+0][j][k] = output_dma_O_data.data.data1;
				OFM[i*8+1][j][k] = output_dma_O_data.data.data2;
				OFM[i*8+2][j][k] = output_dma_O_data.data.data3;
				OFM[i*8+3][j][k] = output_dma_O_data.data.data4;
				OFM[i*8+4][j][k] = output_dma_O_data.data.data5;
				OFM[i*8+5][j][k] = output_dma_O_data.data.data6;
				OFM[i*8+6][j][k] = output_dma_O_data.data.data7;
				OFM[i*8+7][j][k] = output_dma_O_data.data.data8;
			}
		}
	}



	for(int i=0;i<divided_Tm_8;i++){
		for(int j=0;j<Tn;j++){
			for(int m=0;m<custom_k;m++){
				for(int l=0;l<custom_k;l++){
					if(i<divided_Tm_8/2){
						weight.data.data1 = sim_WEIGHT[0][0][i*8][j][m][l];
						weight.data.data2 = sim_WEIGHT[0][0][i*8+1][j][m][l];
						weight.data.data3 = sim_WEIGHT[0][0][i*8+2][j][m][l];
						weight.data.data4 = sim_WEIGHT[0][0][i*8+3][j][m][l];
						weight.data.data5 = sim_WEIGHT[0][0][i*8+4][j][m][l];
						weight.data.data6 = sim_WEIGHT[0][0][i*8+5][j][m][l];
						weight.data.data7 = sim_WEIGHT[0][0][i*8+6][j][m][l];
						weight.data.data8 = sim_WEIGHT[0][0][i*8+7][j][m][l];
					}else{

						weight.data.data1 = sim_WEIGHT[0][1][i*8][j][m][l];
						weight.data.data2 = sim_WEIGHT[0][1][i*8+1][j][m][l];
						weight.data.data3 = sim_WEIGHT[0][1][i*8+2][j][m][l];
						weight.data.data4 = sim_WEIGHT[0][1][i*8+3][j][m][l];
						weight.data.data5 = sim_WEIGHT[0][1][i*8+4][j][m][l];
						weight.data.data6 = sim_WEIGHT[0][1][i*8+5][j][m][l];
						weight.data.data7 = sim_WEIGHT[0][1][i*8+6][j][m][l];
						weight.data.data8 = sim_WEIGHT[0][1][i*8+7][j][m][l];
					}
					if(i==divided_Tm_8-1 && j==Tn-1 && m==custom_k-1 && l==custom_k-1)
						weight.last = true;
					else
						weight.last = false;
					input_dma_W.write(weight);
				}
			}
		}
	}

	for(int i=0;i<divided_Tm_8;i++){
			for(int j=0;j<Tn;j++){
				for(int m=0;m<custom_k;m++){
					for(int l=0;l<custom_k;l++){

						weight.data.data1 = 0;
						weight.data.data2 = 0;
						weight.data.data3 = 0;
						weight.data.data4 = 0;
						weight.data.data5 = 0;
						weight.data.data6 = 0;
						weight.data.data7 = 0;
						weight.data.data8 = 0;

						if(i==divided_Tm_8-1 && j==Tn-1 && m==custom_k-1 && l==custom_k-1)
							weight.last = true;
						else
							weight.last = false;
						input_dma_W.write(weight);
					}
				}
			}
		}


	for(int i=0;i<divided_Tn_8;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				ifm.data.data1 = sim_IFM[0][i*8][j][m];
				ifm.data.data2 = sim_IFM[0][i*8+1][j][m];
				ifm.data.data3 = sim_IFM[0][i*8+2][j][m];
				ifm.data.data4 = sim_IFM[0][i*8+3][j][m];
				ifm.data.data5 = sim_IFM[0][i*8+4][j][m];
				ifm.data.data6 = sim_IFM[0][i*8+5][j][m];
				ifm.data.data7 = sim_IFM[0][i*8+6][j][m];
				ifm.data.data8 = sim_IFM[0][i*8+7][j][m];
				if(i==divided_Tn_8-1 && j==custom_Tr-1 && m==custom_Tc-1)
					ifm.last = true;
				else
					ifm.last = false;
				input_dma_I.write(ifm);
			}
		}
	}

	for(int i=0;i<divided_Tn_8;i++){
			for(int j=0;j<custom_Tr;j++){
				for(int m=0;m<custom_Tc;m++){
					ifm.data.data1 = 0;
					ifm.data.data2 = 0;
					ifm.data.data3 = 0;
					ifm.data.data4 = 0;
					ifm.data.data5 = 0;
					ifm.data.data6 = 0;
					ifm.data.data7 = 0;
					ifm.data.data8 = 0;
					if(i==divided_Tn_8-1 && j==custom_Tr-1 && m==custom_Tc-1)
						ifm.last = true;
					else
						ifm.last = false;
					input_dma_I.write(ifm);
				}
			}
		}

	for(int i=0;i<divided_Tm_8;i++){
		bias.data.data1 = sim_BIAS[i*8+0];
		bias.data.data2 = sim_BIAS[i*8+1];
		bias.data.data3 = sim_BIAS[i*8+2];
		bias.data.data4 = sim_BIAS[i*8+3];
		bias.data.data5 = sim_BIAS[i*8+4];
		bias.data.data6 = sim_BIAS[i*8+5];
		bias.data.data7 = sim_BIAS[i*8+6];
		bias.data.data8 = sim_BIAS[i*8+7];
		if(i==divided_Tm_8-1)
			bias.last = true;
		else
			bias.last = false;
		input_dma_B.write(bias);
	}

	cconv(input_dma_W,input_dma_I,input_dma_B,output_dma_O,\
			0,0,1,Tn*2,custom_k,custom_Tr,custom_Tc,custom_Tm,custom_Tn,NL_Opt);

	for(int i=0;i<divided_Tm_8;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int k=0;k<custom_Tc;k++){
				DMA_DATA_128B_FIX output_dma_O_data = output_dma_O.read();
				OFM[i*8+0][j][k] = output_dma_O_data.data.data1;
				OFM[i*8+1][j][k] = output_dma_O_data.data.data2;
				OFM[i*8+2][j][k] = output_dma_O_data.data.data3;
				OFM[i*8+3][j][k] = output_dma_O_data.data.data4;
				OFM[i*8+4][j][k] = output_dma_O_data.data.data5;
				OFM[i*8+5][j][k] = output_dma_O_data.data.data6;
				OFM[i*8+6][j][k] = output_dma_O_data.data.data7;
				OFM[i*8+7][j][k] = output_dma_O_data.data.data8;
			}
		}
	}

	cout<<"======="<<"Results from Accelerator"<<"=========="<<endl;
	for(int i=0;i<Tm;i++){
		cout<<"The "<<i<<"th [Tr][Tc]"<<endl;
		for(int j=0;j<Tr;j++){
			for(int m=0;m<Tc;m++){
				cout<<OFM[i][j][m]<<" ";
			}
			cout<<endl;
		}
		cout<<endl<<endl;
	}

	cout<<"======="<<"Difference"<<"=========="<<endl;
	for(int i=0;i<Tm;i++){
		cout<<"The "<<i<<"th [Tr][Tc]"<<endl;
		for(int j=0;j<Tr;j++){
			for(int m=0;m<Tc;m++){
				cout<<obtained_OFM[i][j][m]-OFM[i][j][m]<<" ";
			}
			cout<<endl;
		}
		cout<<endl<<endl;
	}



}
