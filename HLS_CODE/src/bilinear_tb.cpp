#include <hls_stream.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include <stdlib.h>
#include "source_fix.h"
using namespace std;

void bilinear(hls::stream<DMA_DATA_128B_FIX> &input,
		hls::stream<DMA_DATA_128B_FIX> &output,
		int custom_Tr,
		int custom_Tc,
		int op
		);


FPGA_DATA_FIX bl_given_input[Tm][Tr][Tc];
FPGA_DATA_FIX bl_obtained_output[Tm][2*Tr][2*Tc]={0};

int bilinear_main(){
	static FPGA_DATA_FIX OFM[Tm][2*Tr][2*Tc];

	hls::stream<DMA_DATA_128B_FIX> input_dma_I("input");
	hls::stream<DMA_DATA_128B_FIX> input_dma_O("output");

	DMA_DATA_128B_FIX I;


	const int custom_Tr = 7;
	const int custom_Tc = 14;

	srand(time(0));
	for(int i=0;i<Tm;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				bl_given_input[i][j][m] = rand()%10 + 1;
			}
		}
	}

	for(int i=0;i<Tm;i++){
		for(int j=0;j<custom_Tr*2;j++){
			for(int m=0;m<custom_Tc*2;m++){
				if(j%2==0 and m%2==0)
					bl_obtained_output[i][j][m] = bl_given_input[i][j/2][m/2];
				else if(j%2==0 and m%2!=0){
					if(m!=custom_Tc*2-1)
						bl_obtained_output[i][j][m] = (bl_given_input[i][j/2][(m-1)/2]+bl_given_input[i][j/2][(m+1)/2])/2;
					else
						bl_obtained_output[i][j][m] = (bl_given_input[i][j/2][(m-1)/2])/2;
				}
				else if(j%2!=0 and m%2==0){
					if(j!=custom_Tr*2-1)
						bl_obtained_output[i][j][m] = (bl_given_input[i][(j-1)/2][m/2]+bl_given_input[i][(j+1)/2][m/2])/2;
					else
						bl_obtained_output[i][j][m] = (bl_given_input[i][(j-1)/2][m/2])/2;
				}
				else if(j%2!=0 and m%2!=0){
					FPGA_DATA_FIX a = bl_given_input[i][(j-1)/2][(m-1)/2];
					FPGA_DATA_FIX b = bl_given_input[i][(j-1)/2][(m+1)/2];
					FPGA_DATA_FIX c = bl_given_input[i][(j+1)/2][(m-1)/2];
					FPGA_DATA_FIX d = bl_given_input[i][(j+1)/2][(m+1)/2];

					if(m==custom_Tc*2-1){
						b=0;
						d=0;
					}
					if(j==custom_Tr*2-1){
						c=0;
						d=0;
					}


					bl_obtained_output[i][j][m] = (FPGA_DATA_FIX((a+b)/2)+FPGA_DATA_FIX((c+d)/2))/2;
				}

			}
		}
	}

	cout<<"======="<<"Results from Direct Computation"<<"=========="<<endl;
	for(int i=0;i<Tm;i++){
		cout<<"The "<<i<<"th [Tr][Tc]"<<endl;
		for(int j=0;j<custom_Tr*2;j++){
			for(int m=0;m<custom_Tc*2;m++){
				cout<<bl_obtained_output[i][j][m]<<" ";
			}
			cout<<endl;
		}
		cout<<endl<<endl;
	}




	for(int i=0;i<divided_Tm_8;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int m=0;m<custom_Tc;m++){
				I.data.data1 = bl_given_input[i*8+0][j][m];
				I.data.data2 = bl_given_input[i*8+1][j][m];
				I.data.data3 = bl_given_input[i*8+2][j][m];
				I.data.data4 = bl_given_input[i*8+3][j][m];
				I.data.data5 = bl_given_input[i*8+4][j][m];
				I.data.data6 = bl_given_input[i*8+5][j][m];
				I.data.data7 = bl_given_input[i*8+6][j][m];
				I.data.data8 = bl_given_input[i*8+7][j][m];
				if(i==divided_Tm_8-1 && j==custom_Tr-1 && m==custom_Tc-1)
					I.last = true;
				else
					I.last = false;
				input_dma_I.write(I);
			}
		}
	}

	bilinear(input_dma_I,input_dma_O,custom_Tr,custom_Tc,0);
	bilinear(input_dma_I,input_dma_O,custom_Tr,custom_Tc,1);
	bilinear(input_dma_I,input_dma_O,custom_Tr,custom_Tc,2);


	for(int i=0;i<divided_Tm_8;i++){
		for(int j=0;j<custom_Tr*2;j++){
			for(int k=0;k<custom_Tc*2;k++){
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
		for(int j=0;j<custom_Tr*2;j++){
			for(int m=0;m<custom_Tc*2;m++){
				cout<<OFM[i][j][m]<<" ";
			}
			cout<<endl;
		}
		cout<<endl<<endl;
	}

	cout<<"======="<<"Difference"<<"=========="<<endl;
	for(int i=0;i<Tm;i++){
		cout<<"The "<<i<<"th [Tr][Tc]"<<endl;
		for(int j=0;j<custom_Tr*2;j++){
			for(int m=0;m<custom_Tc*2;m++){
				cout<<OFM[i][j][m]-bl_obtained_output[i][j][m]<<" ";
			}
			cout<<endl;
		}
		cout<<endl<<endl;
	}
	return 0;
}



