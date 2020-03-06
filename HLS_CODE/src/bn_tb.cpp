#include <hls_stream.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "source_fix.h"
using namespace std;

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
		);


FPGA_DATA_FIX given_input[Tm][Tr][Tc];
FPGA_DATA_FIX given_mean[Tm];
FPGA_DATA_FIX given_var[Tm];
FPGA_DATA_FIX obtained_output[Tm][Tr][Tc]={0};

int bn_main(){
	static FPGA_DATA_FIX OFM[Tm][Tr][Tc];

	hls::stream<DMA_DATA_128B_FIX> input_dma_I("input");
	hls::stream<DMA_DATA_128B_FIX> input_dma_M("mean");
	hls::stream<DMA_DATA_128B_FIX> input_dma_V("var");
	hls::stream<DMA_DATA_128B_FIX> input_dma_O("output");

	DMA_DATA_128B_FIX I;
	DMA_DATA_128B_FIX M;
	DMA_DATA_128B_FIX V;


	const int custom_Tr = 7;
	const int custom_Tc = 14;

	float eps = 1e-5;
	int gamma = 1;
	int beta = 0;


	srand(time(0));
	for(int i=0;i<Tm;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				given_input[i][j][m] = rand()%10 + 1;
			}
		}
	}

	for(int i=0;i<Tm;i++){
		given_mean[i] = rand()%10 + 1;
	}

	for(int i=0;i<Tm;i++){
		given_var[i] = rand()%10 + 1;
	}


	cout<<"======="<<"Results from Direct Computation"<<"=========="<<endl;
	for(int i=0;i<Tm;i++){
		cout<<"The "<<i<<"th [Tr][Tc]"<<endl;
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				obtained_output[i][j][m] = (given_input[i][j][m]-given_mean[i])/sqrt(given_var[i]+eps)*gamma+beta;
				cout<<obtained_output[i][j][m]<<" ";
			}
			cout<<endl;
		}
		cout<<endl<<endl;
	}




	for(int i=0;i<divided_Tm_8;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				I.data.data1 = given_input[i*8+0][j][m];
				I.data.data2 = given_input[i*8+1][j][m];
				I.data.data3 = given_input[i*8+2][j][m];
				I.data.data4 = given_input[i*8+3][j][m];
				I.data.data5 = given_input[i*8+4][j][m];
				I.data.data6 = given_input[i*8+5][j][m];
				I.data.data7 = given_input[i*8+6][j][m];
				I.data.data8 = given_input[i*8+7][j][m];
				if(i==divided_Tm_8-1 && j==custom_Tr-1 && m==custom_Tc-1)
					I.last = true;
				else
					I.last = false;
				input_dma_I.write(I);
			}
		}
	}

	batch_norm(input_dma_I,input_dma_M,input_dma_V,input_dma_O,eps,gamma,beta,custom_Tr,custom_Tc,0);

	for(int i=0;i<divided_Tm_8;i++){
		M.data.data1 = given_mean[i*8+0];
		M.data.data2 = given_mean[i*8+1];
		M.data.data3 = given_mean[i*8+2];
		M.data.data4 = given_mean[i*8+3];
		M.data.data5 = given_mean[i*8+4];
		M.data.data6 = given_mean[i*8+5];
		M.data.data7 = given_mean[i*8+6];
		M.data.data8 = given_mean[i*8+7];
		if(i==divided_Tm_8-1)
			M.last = true;
		else
			M.last = false;
		input_dma_M.write(M);
	}
	for(int i=0;i<divided_Tm_8;i++){
		V.data.data1 = given_var[i*8+0];
		V.data.data2 = given_var[i*8+1];
		V.data.data3 = given_var[i*8+2];
		V.data.data4 = given_var[i*8+3];
		V.data.data5 = given_var[i*8+4];
		V.data.data6 = given_var[i*8+5];
		V.data.data7 = given_var[i*8+6];
		V.data.data8 = given_var[i*8+7];
		if(i==divided_Tm_8-1)
			V.last = true;
		else
			V.last = false;
		input_dma_V.write(V);
	}

	batch_norm(input_dma_I,input_dma_M,input_dma_V,input_dma_O,eps,gamma,beta,custom_Tr,custom_Tc,1);

	batch_norm(input_dma_I,input_dma_M,input_dma_V,input_dma_O,eps,gamma,beta,custom_Tr,custom_Tc,2);

	batch_norm(input_dma_I,input_dma_M,input_dma_V,input_dma_O,eps,gamma,beta,custom_Tr,custom_Tc,3);


	for(int i=0;i<divided_Tm_8;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int k=0;k<custom_Tc;k++){
				DMA_DATA_128B_FIX output_dma_O_data = input_dma_O.read();
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

	cout<<"======="<<"Results from HW Sim"<<"=========="<<endl;
	for(int i=0;i<Tm;i++){
		cout<<"The "<<i<<"th [Tr][Tc]"<<endl;
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				cout<<OFM[i][j][m]<<" ";
			}
			cout<<endl;
		}
		cout<<endl<<endl;
	}

	cout<<"======="<<"Difference"<<"=========="<<endl;
	for(int i=0;i<Tm;i++){
		cout<<"The "<<i<<"th [Tr][Tc]"<<endl;
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				cout<<OFM[i][j][m]-obtained_output[i][j][m]<<" ";
			}
			cout<<endl;
		}
		cout<<endl<<endl;
	}
	return 0;
}



