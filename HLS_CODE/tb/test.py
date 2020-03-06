import numpy as np
import torch
from torch.autograd import Variable
import torch.nn.functional as F

CH1 = 16
CH2 = 16
CH3 = 32
R = 32
C = 32
K = 1

class SimpleCNN(torch.nn.Module):

    def __init__(self):
        super(SimpleCNN, self).__init__()
        self.conv1 = torch.nn.Conv2d(CH1, CH2, kernel_size=K, stride=1, padding=0)
        self.conv2 = torch.nn.Conv2d(CH2, CH3, kernel_size=K, stride=1, padding=0)

    def forward(self, x):
        x = self.conv1(x)
        x = self.conv2(x)
        return(x)

CNN = SimpleCNN()
#CNN.type(torch.int)


ifm = torch.randint(0,3,(1, CH1, R, C), dtype=torch.float)
weight_1 = torch.randint(0,3,(CH2, CH1, K, K), dtype=torch.float)
bias_1 = torch.randint(-1,2,(CH2,), dtype=torch.float)

weight_2 = torch.randint(0,3,(CH3, CH2, K, K), dtype=torch.float)
bias_2 = torch.randint(-1,2,(CH3,), dtype=torch.float)


#ifm = torch.ones((1, CH1, R, C), dtype=torch.float)
#weight_1 = torch.ones((CH2, CH1, K, K), dtype=torch.float)
#bias_1 = torch.ones((CH2,), dtype=torch.float)

#weight_2 = torch.ones((CH3, CH2, K, K), dtype=torch.float)
#bias_2 = torch.ones((CH3,), dtype=torch.float)


#ifm = torch.ones((1, N, R, C), dtype=torch.float)
#weight = torch.ones((M, N, K, K), dtype=torch.float)
#bias = torch.ones((M,), dtype=torch.float)
#bias = torch.zeros(192,dtype=torch.float)
#weight = torch.zeros(192, 128, 3, 3)


#for name, param in CNN.state_dict().items():
#    print(name,param)


CNN.state_dict()["conv1.weight"][:] = weight_1
CNN.state_dict()["conv1.bias"][:] = bias_1

CNN.state_dict()["conv2.weight"][:] = weight_2
CNN.state_dict()["conv2.bias"][:] = bias_2


ofm = CNN(ifm)

def print_list(ifm_list):
    for l_idx in range(len(ifm_list)):
        if l_idx!=len(ifm_list)-1:
            print(str(int(ifm_list[l_idx]))+",",end="")
        else:
            print(str(int(ifm_list[l_idx]))+"};",)



print('''#include "source_fix.h"
        
FPGA_DATA_FIX input[CH1*R*C] = {''',end="")

ifm_list = ifm.flatten().tolist()
print_list(ifm_list)

print('''FPGA_DATA_FIX weight_1[CH2*CH1*K*K] = {''',end="")
ifm_list = weight_1.flatten().tolist()
print_list(ifm_list)

print('''FPGA_DATA_FIX bias_1[CH2] = {''',end="")
ifm_list = bias_1.flatten().tolist()
print_list(ifm_list)

print('''FPGA_DATA_FIX weight_2[CH3*CH2*K*K] = {''',end="")
ifm_list = weight_2.flatten().tolist()
print_list(ifm_list)

print('''FPGA_DATA_FIX bias_2[CH3] = {''',end="")
ifm_list = bias_2.flatten().tolist()
print_list(ifm_list)

print('''FPGA_DATA_FIX output[CH3*(R-2*K+2)*(C-2*K+2)] = {''',end="")
ifm_list = ofm.flatten().tolist()
print_list(ifm_list)

#print(ofm.shape)

#print(ofm.flatten().tolist())
