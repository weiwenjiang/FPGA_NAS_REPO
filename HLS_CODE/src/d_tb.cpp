#include <hls_stream.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "source_fix.h"
using namespace std;

void dconv(hls::stream<DMA_DATA_128B_FIX> &input_dma_W,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_I,
		hls::stream<DMA_DATA_128B_FIX> &input_dma_B,
		hls::stream<DMA_DATA_128B_FIX> &output_dma_O,
		int row,int col,int num,int N,int custom_k,int custom_Tr,int custom_Tc,int NL_Opt);

int dconv_create_W(FPGA_DATA_FIX given_WEIGHT[Tn][K][K],int custom_k);
int dconv_create_I(FPGA_DATA_FIX given_IFM[Tn][Tr][Tc],int custom_Tr,int custom_Tc);
int dconv_create_O(int row, int col, int custom_k, int custom_Tr, int custom_Tc);
int dconv_create_B(FPGA_DATA_FIX given_BIAS[Tn]);

FPGA_DATA_FIX non_linear(FPGA_DATA_FIX op, int NL_Opt);

extern FPGA_DATA_FIX dconv_sim_WEIGHT[2][2][Tn][K][K];
extern FPGA_DATA_FIX dconv_sim_IFM[2][Tn_8][Tr][Tc];
extern FPGA_DATA_FIX dconv_sim_OFM[Tn][Tr][Tc];
extern FPGA_DATA_FIX dconv_sim_BIAS[Tn];


FPGA_DATA_FIX dconv_given_WEIGHT[Tn][K][K]={0};
FPGA_DATA_FIX dconv_given_IFM[Tn][Tr][Tc]={0};
FPGA_DATA_FIX dconv_given_BIAS[Tn]={0};
FPGA_DATA_FIX dconv_obtained_OFM[Tn][Tr][Tc]={0};

int dconv_main(){
	static FPGA_DATA_FIX dconv_OFM[Tn][Tr][Tc];

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

	int NL_Opt = 2;


	srand(time(0));
	for(int j=0;j<Tn;j++){
		for(int m=0;m<K;m++){
			for(int n=0;n<K;n++){
				dconv_given_WEIGHT[j][m][n] = rand()%10 + 1;
				dconv_given_WEIGHT[j][m][n] = dconv_given_WEIGHT[j][m][n]%2==1?dconv_given_WEIGHT[j][m][n]:-1*dconv_given_WEIGHT[j][m][n];
			}
		}
	}


	for(int i=0;i<Tn;i++){
		for(int j=0;j<Tr;j++){
			for(int m=0;m<Tc;m++){
				dconv_given_IFM[i][j][m] = rand()%10 + 1;
			}
		}
	}

	for(int i=0;i<Tn;i++){
		dconv_given_BIAS[i] = rand()%10 + 1;
		dconv_given_BIAS[i] = dconv_given_BIAS[i]%2==1?dconv_given_BIAS[i]:-1*dconv_given_BIAS[i];
	}





	for(int i=0;i<K;i++){
		for(int j=0;j<K;j++){
			for(int r=0;r<Tr;r++){
				for(int c=0;c<Tc;c++){
					for(int n=0;n<Tn;n++){
						FPGA_DATA_FIX add_res1 = dconv_given_WEIGHT[n][i][j]*dconv_given_IFM[n][r][c];
						dconv_obtained_OFM[n][r][c] = dconv_obtained_OFM[n][r][c] + add_res1;
					}
				}
			}
		}
	}

	cout<<"======="<<"Results from Direct Computation"<<"=========="<<endl;
	for(int i=0;i<Tn;i++){
		cout<<"The "<<i<<"th [Tr][Tc]"<<endl;
		for(int j=0;j<Tr;j++){
			for(int m=0;m<Tc;m++){
				dconv_obtained_OFM[i][j][m] = non_linear(dconv_obtained_OFM[i][j][m]+dconv_given_BIAS[i],NL_Opt);
				cout<<dconv_obtained_OFM[i][j][m]<<" ";
			}
			cout<<endl;
		}
		cout<<endl<<endl;
	}




	dconv_create_W(dconv_given_WEIGHT,custom_k);
	dconv_create_I(dconv_given_IFM,custom_Tr,custom_Tc);
	dconv_create_B(dconv_given_BIAS);
	dconv_create_O(0, 0, custom_k, custom_Tr, custom_Tc);


	cout<<"======="<<"Results from Simulation"<<"=========="<<endl;
	for(int i=0;i<Tn;i++){
		cout<<"The "<<i<<"th [Tr][Tc]"<<endl;
		for(int j=0;j<Tr;j++){
			for(int m=0;m<Tc;m++){
				cout<<dconv_sim_OFM[i][j][m]<<" ";
			}
			cout<<endl;
		}
		cout<<endl<<endl;
	}



	for(int i=0;i<divided_Tn_8;i++){
		for(int m=0;m<custom_k;m++){
			for(int l=0;l<custom_k;l++){
				if(i<divided_Tn_8/2){
					weight.data.data1 = dconv_sim_WEIGHT[0][0][i*8][m][l];
					weight.data.data2 = dconv_sim_WEIGHT[0][0][i*8+1][m][l];
					weight.data.data3 = dconv_sim_WEIGHT[0][0][i*8+2][m][l];
					weight.data.data4 = dconv_sim_WEIGHT[0][0][i*8+3][m][l];
					weight.data.data5 = dconv_sim_WEIGHT[0][0][i*8+4][m][l];
					weight.data.data6 = dconv_sim_WEIGHT[0][0][i*8+5][m][l];
					weight.data.data7 = dconv_sim_WEIGHT[0][0][i*8+6][m][l];
					weight.data.data8 = dconv_sim_WEIGHT[0][0][i*8+7][m][l];
				}else{

					weight.data.data1 = dconv_sim_WEIGHT[0][1][i*8][m][l];
					weight.data.data2 = dconv_sim_WEIGHT[0][1][i*8+1][m][l];
					weight.data.data3 = dconv_sim_WEIGHT[0][1][i*8+2][m][l];
					weight.data.data4 = dconv_sim_WEIGHT[0][1][i*8+3][m][l];
					weight.data.data5 = dconv_sim_WEIGHT[0][1][i*8+4][m][l];
					weight.data.data6 = dconv_sim_WEIGHT[0][1][i*8+5][m][l];
					weight.data.data7 = dconv_sim_WEIGHT[0][1][i*8+6][m][l];
					weight.data.data8 = dconv_sim_WEIGHT[0][1][i*8+7][m][l];
				}
				if(i==divided_Tn_8-1 && m==custom_k-1 && l==custom_k-1)
					weight.last = true;
				else
					weight.last = false;
				input_dma_W.write(weight);
			}

		}
	}

	for(int i=0;i<divided_Tn_8;i++){
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

				if(i==divided_Tn_8-1 && m==custom_k-1 && l==custom_k-1)
					weight.last = true;
				else
					weight.last = false;
				input_dma_W.write(weight);
			}
		}
	}

	for(int i=0;i<divided_Tn_8;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				ifm.data.data1 = dconv_sim_IFM[0][i*8][j][m];
				ifm.data.data2 = dconv_sim_IFM[0][i*8+1][j][m];
				ifm.data.data3 = dconv_sim_IFM[0][i*8+2][j][m];
				ifm.data.data4 = dconv_sim_IFM[0][i*8+3][j][m];
				ifm.data.data5 = dconv_sim_IFM[0][i*8+4][j][m];
				ifm.data.data6 = dconv_sim_IFM[0][i*8+5][j][m];
				ifm.data.data7 = dconv_sim_IFM[0][i*8+6][j][m];
				ifm.data.data8 = dconv_sim_IFM[0][i*8+7][j][m];
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

	for(int i=0;i<divided_Tn_8;i++){
		bias.data.data1 = dconv_sim_BIAS[i*8+0];
		bias.data.data2 = dconv_sim_BIAS[i*8+1];
		bias.data.data3 = dconv_sim_BIAS[i*8+2];
		bias.data.data4 = dconv_sim_BIAS[i*8+3];
		bias.data.data5 = dconv_sim_BIAS[i*8+4];
		bias.data.data6 = dconv_sim_BIAS[i*8+5];
		bias.data.data7 = dconv_sim_BIAS[i*8+6];
		bias.data.data8 = dconv_sim_BIAS[i*8+7];
		if(i==divided_Tn_8-1)
			bias.last = true;
		else
			bias.last = false;
		input_dma_B.write(bias);
	}

	dconv(input_dma_W,input_dma_I,input_dma_B,output_dma_O,\
			0,0,0,Tn*2,custom_k,custom_Tr,custom_Tc,NL_Opt);

	for(int i=0;i<divided_Tn_8;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int k=0;k<custom_Tc;k++){
				DMA_DATA_128B_FIX output_dma_O_data = output_dma_O.read();
				dconv_OFM[i*8+0][j][k] = output_dma_O_data.data.data1;
				dconv_OFM[i*8+1][j][k] = output_dma_O_data.data.data2;
				dconv_OFM[i*8+2][j][k] = output_dma_O_data.data.data3;
				dconv_OFM[i*8+3][j][k] = output_dma_O_data.data.data4;
				dconv_OFM[i*8+4][j][k] = output_dma_O_data.data.data5;
				dconv_OFM[i*8+5][j][k] = output_dma_O_data.data.data6;
				dconv_OFM[i*8+6][j][k] = output_dma_O_data.data.data7;
				dconv_OFM[i*8+7][j][k] = output_dma_O_data.data.data8;
			}
		}
	}



	for(int i=0;i<divided_Tn_8;i++){
		for(int m=0;m<custom_k;m++){
			for(int l=0;l<custom_k;l++){
				if(i<divided_Tn_8/2){
					weight.data.data1 = dconv_sim_WEIGHT[0][0][i*8][m][l];
					weight.data.data2 = dconv_sim_WEIGHT[0][0][i*8+1][m][l];
					weight.data.data3 = dconv_sim_WEIGHT[0][0][i*8+2][m][l];
					weight.data.data4 = dconv_sim_WEIGHT[0][0][i*8+3][m][l];
					weight.data.data5 = dconv_sim_WEIGHT[0][0][i*8+4][m][l];
					weight.data.data6 = dconv_sim_WEIGHT[0][0][i*8+5][m][l];
					weight.data.data7 = dconv_sim_WEIGHT[0][0][i*8+6][m][l];
					weight.data.data8 = dconv_sim_WEIGHT[0][0][i*8+7][m][l];
				}else{

					weight.data.data1 = dconv_sim_WEIGHT[0][1][i*8][m][l];
					weight.data.data2 = dconv_sim_WEIGHT[0][1][i*8+1][m][l];
					weight.data.data3 = dconv_sim_WEIGHT[0][1][i*8+2][m][l];
					weight.data.data4 = dconv_sim_WEIGHT[0][1][i*8+3][m][l];
					weight.data.data5 = dconv_sim_WEIGHT[0][1][i*8+4][m][l];
					weight.data.data6 = dconv_sim_WEIGHT[0][1][i*8+5][m][l];
					weight.data.data7 = dconv_sim_WEIGHT[0][1][i*8+6][m][l];
					weight.data.data8 = dconv_sim_WEIGHT[0][1][i*8+7][m][l];
				}
				if(i==divided_Tn_8-1 && m==custom_k-1 && l==custom_k-1)
					weight.last = true;
				else
					weight.last = false;
				input_dma_W.write(weight);
			}
		}
	}

	for(int i=0;i<divided_Tn_8;i++){
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

				if(i==divided_Tn_8-1 && m==custom_k-1 && l==custom_k-1)
					weight.last = true;
				else
					weight.last = false;
				input_dma_W.write(weight);
			}

		}
	}


	for(int i=0;i<divided_Tn_8;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				ifm.data.data1 = dconv_sim_IFM[0][i*8][j][m];
				ifm.data.data2 = dconv_sim_IFM[0][i*8+1][j][m];
				ifm.data.data3 = dconv_sim_IFM[0][i*8+2][j][m];
				ifm.data.data4 = dconv_sim_IFM[0][i*8+3][j][m];
				ifm.data.data5 = dconv_sim_IFM[0][i*8+4][j][m];
				ifm.data.data6 = dconv_sim_IFM[0][i*8+5][j][m];
				ifm.data.data7 = dconv_sim_IFM[0][i*8+6][j][m];
				ifm.data.data8 = dconv_sim_IFM[0][i*8+7][j][m];
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

	for(int i=0;i<divided_Tn_8;i++){
		bias.data.data1 = dconv_sim_BIAS[i*8+0];
		bias.data.data2 = dconv_sim_BIAS[i*8+1];
		bias.data.data3 = dconv_sim_BIAS[i*8+2];
		bias.data.data4 = dconv_sim_BIAS[i*8+3];
		bias.data.data5 = dconv_sim_BIAS[i*8+4];
		bias.data.data6 = dconv_sim_BIAS[i*8+5];
		bias.data.data7 = dconv_sim_BIAS[i*8+6];
		bias.data.data8 = dconv_sim_BIAS[i*8+7];
		if(i==divided_Tn_8-1)
			bias.last = true;
		else
			bias.last = false;
		input_dma_B.write(bias);
	}

	dconv(input_dma_W,input_dma_I,input_dma_B,output_dma_O,\
			0,0,1,Tn*2,custom_k,custom_Tr,custom_Tc,NL_Opt);

	for(int i=0;i<divided_Tn_8;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int k=0;k<custom_Tc;k++){
				DMA_DATA_128B_FIX output_dma_O_data = output_dma_O.read();
				dconv_OFM[i*8+0][j][k] = output_dma_O_data.data.data1;
				dconv_OFM[i*8+1][j][k] = output_dma_O_data.data.data2;
				dconv_OFM[i*8+2][j][k] = output_dma_O_data.data.data3;
				dconv_OFM[i*8+3][j][k] = output_dma_O_data.data.data4;
				dconv_OFM[i*8+4][j][k] = output_dma_O_data.data.data5;
				dconv_OFM[i*8+5][j][k] = output_dma_O_data.data.data6;
				dconv_OFM[i*8+6][j][k] = output_dma_O_data.data.data7;
				dconv_OFM[i*8+7][j][k] = output_dma_O_data.data.data8;
			}
		}
	}

	cout<<"======="<<"Results from Accelerator"<<"=========="<<endl;
	for(int i=0;i<Tn;i++){
		cout<<"The "<<i<<"th [Tr][Tc]"<<endl;
		for(int j=0;j<Tr;j++){
			for(int m=0;m<Tc;m++){
				cout<<dconv_OFM[i][j][m]<<" ";
			}
			cout<<endl;
		}
		cout<<endl<<endl;
	}

	cout<<"======="<<"Difference"<<"=========="<<endl;
	for(int i=0;i<Tn;i++){
		cout<<"The "<<i<<"th [Tr][Tc]"<<endl;
		for(int j=0;j<Tr;j++){
			for(int m=0;m<Tc;m++){
				cout<<dconv_obtained_OFM[i][j][m]-dconv_OFM[i][j][m]<<" ";
			}
			cout<<endl;
		}
		cout<<endl<<endl;
	}



}
