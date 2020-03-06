#include <hls_stream.h>
#include <iostream>
#include <math.h>
#include <time.h>
#include "source_fix.h"
using namespace std;

FPGA_DATA_FIX sim_WEIGHT[2][2][Tm][Tn][K][K]={0};
FPGA_DATA_FIX sim_IFM[2][Tn_8][Tr][Tc]={0};
FPGA_DATA_FIX sim_OFM[Tm][Tr][Tc]={0};
FPGA_DATA_FIX sim_BIAS[Tm]={0};


int create_weight(FPGA_DATA_FIX given_WEIGHT[Tm][Tn][K][K],FPGA_DATA_FIX WEIGHT1[2][Tm][Tn][K][K],int custom_k){
	for(int i=0;i<divided_Tm_8;i++){
		for(int j=0;j<Tn;j++){
			for(int m=0;m<custom_k;m++){
				for(int l=0;l<custom_k;l++){
					if(i<divided_Tm_8/2){
						WEIGHT1[0][i*8][j][m][l] = given_WEIGHT[i*8][j][m][l];
						if(i*8+1<Tm)
							WEIGHT1[0][i*8+1][j][m][l] = given_WEIGHT[i*8+1][j][m][l];
						if(i*8+2<Tm)
							WEIGHT1[0][i*8+2][j][m][l] = given_WEIGHT[i*8+2][j][m][l];
						if(i*8+3<Tm)
							WEIGHT1[0][i*8+3][j][m][l] = given_WEIGHT[i*8+3][j][m][l];
						if(i*8+4<Tm)
							WEIGHT1[0][i*8+4][j][m][l] = given_WEIGHT[i*8+4][j][m][l];
						if(i*8+5<Tm)
							WEIGHT1[0][i*8+5][j][m][l] = given_WEIGHT[i*8+5][j][m][l];
						if(i*8+6<Tm)
							WEIGHT1[0][i*8+6][j][m][l] = given_WEIGHT[i*8+6][j][m][l];
						if(i*8+7<Tm)
							WEIGHT1[0][i*8+7][j][m][l] = given_WEIGHT[i*8+7][j][m][l];
					}else{
						WEIGHT1[1][i*8][j][m][l] = given_WEIGHT[i*8+0][j][m][l];
						if(i*8+1<Tm)
							WEIGHT1[1][i*8+1][j][m][l] = given_WEIGHT[i*8+1][j][m][l];
						if(i*8+2<Tm)
							WEIGHT1[1][i*8+2][j][m][l] = given_WEIGHT[i*8+2][j][m][l];
						if(i*8+3<Tm)
							WEIGHT1[1][i*8+3][j][m][l] = given_WEIGHT[i*8+3][j][m][l];
						if(i*8+4<Tm)
							WEIGHT1[1][i*8+4][j][m][l] = given_WEIGHT[i*8+4][j][m][l];
						if(i*8+5<Tm)
							WEIGHT1[1][i*8+5][j][m][l] = given_WEIGHT[i*8+5][j][m][l];
						if(i*8+6<Tm)
							WEIGHT1[1][i*8+6][j][m][l] = given_WEIGHT[i*8+6][j][m][l];
						if(i*8+7<Tm)
							WEIGHT1[1][i*8+7][j][m][l] = given_WEIGHT[i*8+7][j][m][l];
					}
				}
			}
		}
	}
}

int print_weight(FPGA_DATA_FIX WEIGHT1[2][Tm][Tn][K][K],int custom_k){
	for(int i=0;i<divided_Tm_8;i++){
		for(int j=0;j<Tn;j++){
			for(int m=0;m<custom_k;m++){
				for(int l=0;l<custom_k;l++){
					if(i<divided_Tm_8/2){
						cout<< WEIGHT1[0][i][j][m][l]<<" ";
						if(i*8+1<Tm)
							cout<<WEIGHT1[0][i+divided_Tm_8][j][m][l]<<" ";
						if(i*8+2<Tm)
							cout<<WEIGHT1[0][i+divided_Tm_8*2][j][m][l]<<" ";
						if(i*8+3<Tm)
							cout<<WEIGHT1[0][i+divided_Tm_8*3][j][m][l]<<" ";
						if(i*8+4<Tm)
							cout<<WEIGHT1[0][i+divided_Tm_8*4][j][m][l]<<" ";
						if(i*8+5<Tm)
							cout<<WEIGHT1[0][i+divided_Tm_8*5][j][m][l]<<" ";
						if(i*8+6<Tm)
							cout<<WEIGHT1[0][i+divided_Tm_8*6][j][m][l]<<" ";
						if(i*8+7<Tm)
							cout<<WEIGHT1[0][i+divided_Tm_8*7][j][m][l]<<" ";
					}else{
						cout<<WEIGHT1[1][i][j][m][l]<<" ";
						if(i*8+1<Tm)
							cout<<WEIGHT1[1][i+divided_Tm_8][j][m][l]<<" ";
						if(i*8+2<Tm)
							cout<<WEIGHT1[1][i+divided_Tm_8*2][j][m][l]<<" ";
						if(i*8+3<Tm)
							cout<<WEIGHT1[1][i+divided_Tm_8*3][j][m][l]<<" ";
						if(i*8+4<Tm)
							cout<<WEIGHT1[1][i+divided_Tm_8*4][j][m][l]<<" ";
						if(i*8+5<Tm)
							cout<<WEIGHT1[1][i+divided_Tm_8*5][j][m][l]<<" ";
						if(i*8+6<Tm)
							cout<<WEIGHT1[1][i+divided_Tm_8*6][j][m][l]<<" ";
						if(i*8+7<Tm)
							cout<<WEIGHT1[1][i+divided_Tm_8*7][j][m][l]<<" ";
					}
					cout<<endl;
				}
			}
		}
	}
}


void creat_IFM(FPGA_DATA_FIX given_IFM[Tn][Tr][Tc],FPGA_DATA_FIX IFM[2][Tn_8][Tr][Tc],
		int custom_Tr,
		int custom_Tc){
	DMA_DATA_128B_FIX ifm_input_dma;
	ifm1:for(int i=0;i<divided_Tn_8;i++){
		ifm2:for(int j=0;j<custom_Tr;j++){
			ifm3:for(int m=0;m<custom_Tc;m++){
#pragma HLS PIPELINE II=1
				if(i*8<Tn){
					IFM[0][i*8][j][m] = given_IFM[i*8][j][m];
					IFM[1][i*8][j][m] = IFM[0][i*8][j][m];
				}
				if(i*8+1<Tn){
					IFM[0][i*8+1][j][m] = given_IFM[i*8+1][j][m];
					IFM[1][i*8+1][j][m] = IFM[0][i*8+1][j][m];
				}
				if(i*8+2<Tn){
					IFM[0][i*8+2][j][m] = given_IFM[i*8+2][j][m];
					IFM[1][i*8+2][j][m] = IFM[0][i*8+2][j][m];
				}
				if(i*8+3<Tn){
					IFM[0][i*8+3][j][m] = given_IFM[i*8+3][j][m];
					IFM[1][i*8+3][j][m] = IFM[0][i*8+3][j][m];
				}
				if(i*8+4<Tn){
					IFM[0][i*8+4][j][m] = given_IFM[i*8+4][j][m];
					IFM[1][i*8+4][j][m] = IFM[0][i*8+4][j][m];
				}
				if(i*8+5<Tn){
					IFM[0][i*8+5][j][m] = given_IFM[i*8+5][j][m];
					IFM[1][i*8+5][j][m] = IFM[0][i*8+5][j][m];
				}
				if(i*8+6<Tn){
					IFM[0][i*8+6][j][m] = given_IFM[i*8+6][j][m];
					IFM[1][i*8+6][j][m] = IFM[0][i*8+6][j][m];
				}
				if(i*8+7<Tn){
					IFM[0][i*8+7][j][m] = given_IFM[i*8+7][j][m];
					IFM[1][i*8+7][j][m] = IFM[0][i*8+7][j][m];
				}
			}
		}
	}
}

void create_bias(FPGA_DATA_FIX given_BIAS[Tm],FPGA_DATA_FIX sim_BIAS[Tm]){
	for(int i=0;i<divided_Tm_8;i++){
		sim_BIAS[i*8+0] = given_BIAS[i*8+0];
		sim_BIAS[i*8+1] = given_BIAS[i*8+1];
		sim_BIAS[i*8+2] = given_BIAS[i*8+2];
		sim_BIAS[i*8+3] = given_BIAS[i*8+3];
		sim_BIAS[i*8+4] = given_BIAS[i*8+4];
		sim_BIAS[i*8+5] = given_BIAS[i*8+5];
		sim_BIAS[i*8+6] = given_BIAS[i*8+6];
		sim_BIAS[i*8+7] = given_BIAS[i*8+7];
	}
}

void sim_FIRE(  FPGA_DATA_FIX WEIGHT1[2][Tm][Tn][K][K],
			FPGA_DATA_FIX IFM[2][Tn_8][Tr][Tc],
			FPGA_DATA_FIX OFM[Tm][Tr][Tc],
			FPGA_DATA_FIX sim_BIAS[Tm],
			int row,
			int col,
			int custom_k,
			int custom_Tr,
			int custom_Tc){
	for(int i=0;i<custom_k;i++){
		for(int j=0;j<custom_k;j++){
			for(int trr=row;(trr<row+custom_Tr);trr++){
				for(int tcc=col;(tcc<col+custom_Tc);tcc++){
					for(int too=0;too<Tm; too++){
						for(int tii=0;tii<Tn;tii++){
							FPGA_DATA_FIX add_res1;
							if(too<Tm/2){
								add_res1 = WEIGHT1[0][too][tii][i][j]*IFM[0][tii][trr-row][tcc-col];
							}
							else{
								add_res1 = WEIGHT1[1][too][tii][i][j]*IFM[1][tii][trr-row][tcc-col];
							}

							OFM[too][trr-row][tcc-col] = OFM[too][trr-row][tcc-col] + add_res1;
						}
					}
				}
			}
		}
	}


	for(int i=0;i<divided_Tm_8;i++){
		for(int j=0;j<custom_Tr;j++){
			for(int k=0;k<custom_Tc;k++){
				OFM[i*8+0][j][k]=OFM[i*8+0][j][k]+sim_BIAS[i*8+0];
				OFM[i*8+1][j][k]=OFM[i*8+1][j][k]+sim_BIAS[i*8+1];
				OFM[i*8+2][j][k]=OFM[i*8+2][j][k]+sim_BIAS[i*8+2];
				OFM[i*8+3][j][k]=OFM[i*8+3][j][k]+sim_BIAS[i*8+3];
				OFM[i*8+4][j][k]=OFM[i*8+4][j][k]+sim_BIAS[i*8+4];
				OFM[i*8+5][j][k]=OFM[i*8+5][j][k]+sim_BIAS[i*8+5];
				OFM[i*8+6][j][k]=OFM[i*8+6][j][k]+sim_BIAS[i*8+6];
				OFM[i*8+7][j][k]=OFM[i*8+7][j][k]+sim_BIAS[i*8+7];

			}
		}
	}
}



int create_W(FPGA_DATA_FIX given_WEIGHT[Tm][Tn][K][K], int custom_k){
	create_weight(given_WEIGHT,sim_WEIGHT[0],custom_k);
	return 0;
}
int create_I(FPGA_DATA_FIX given_IFM[Tn][Tr][Tc], int custom_Tr,int custom_Tc){
	creat_IFM(given_IFM,sim_IFM,custom_Tr,custom_Tc);
	return 0;
}
int create_B(FPGA_DATA_FIX given_BIAS[Tm]){
	create_bias(given_BIAS,sim_BIAS);
	return 0;
}

int create_O(int row, int col, int custom_k, int custom_Tr, int custom_Tc){
	sim_FIRE(sim_WEIGHT[0],sim_IFM,sim_OFM,sim_BIAS,row,col,custom_k,custom_Tr,custom_Tc);
	return 0;
}

