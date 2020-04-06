import numpy as np
import torch
from torch.autograd import Variable
import torch.nn.functional as F


M = 32
N = 32
R = 32  #行数
C = 32  #列数
K = 3   # kernel size

class SimpleCNN(torch.nn.Module):

    def __init__(self):
        super(SimpleCNN, self).__init__()
        self.conv1 = torch.nn.Conv2d(M, N, kernel_size=K, stride=1, padding=0,dilation=2)


    def forward(self, x):
        x = self.conv1(x) #括号里的x相当于IFM，=右边的相当于OFM
        return(x)

CNN = SimpleCNN()
#CNN.type(torch.int)

ifm = torch.randint(0,3,(1, N, R, C), dtype=torch.float)
weight_1 = torch.randint(0,3,(M, N, K, K), dtype=torch.float)
bias_1 = torch.randint(-1,2,(M,), dtype=torch.float)

CNN.state_dict()["conv1.weight"][:] = weight_1
CNN.state_dict()["conv1.bias"][:] = bias_1


ofm = CNN(ifm)  #相当于CNN.forward

# for name, param in CNN.state_dict().items():
#    print(name,param)

def print_list(ifm_list):
    for l_idx in range(len(ifm_list)):
        if l_idx!=len(ifm_list)-1:
            print(str(int(ifm_list[l_idx]))+",",end="")
        else:
            print(str(int(ifm_list[l_idx]))+"};",)

print('''#include "source_fix.h"

FPGA_DATA_FIX input[N*R*C] = {''',end="")
ifm_list = ifm.flatten().tolist()
print_list(ifm_list)

print('''FPGA_DATA_FIX weight[M*N*K*K] = {''',end="")
ifm_list = weight_1.flatten().tolist()
print_list(ifm_list)

print('''FPGA_DATA_FIX bias[M] = {''',end="")
ifm_list = bias_1.flatten().tolist()
print_list(ifm_list)


print('''FPGA_DATA_FIX output[M*((R-5+1)*(C-5+1)] = {''',end="")
ifm_list = ofm.flatten().tolist()
print_list(ifm_list)


print(ofm.shape)

# print(ofm.flatten().tolist())
