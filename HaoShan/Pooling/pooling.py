
# for name, param import numpy as np
import torch
from torch.autograd import Variable
import torch.nn.functional as F


M = 32
N = 32
R = 32
C = 32
K = 16   # kernel size

class SimpleCNN(torch.nn.Module):

    def __init__(self):
        super(SimpleCNN, self).__init__()
        self.pool1 = torch.nn.MaxPool2d(kernel_size=K, stride=K, padding=0)


    def forward(self, x):
        x = self.pool1(x) #æ‹¬å·é‡Œçš„xç›¸å½“äºŽIFMï¼Œ=å³è¾¹çš„ç›¸å½“äºŽOFM
        return(x)

CNN = SimpleCNN()
#CNN.type(torch.int)

ifm = torch.randint(0,50,(1, N, R, C), dtype=torch.float)


ofm = CNN(ifm)  #CNN.forward

# for name, param in CNN.state_dict().items():
#    print(name,param)

def print_list(ifm_list):
    for l_idx in range(len(ifm_list)):
        if l_idx!=len(ifm_list)-1:
            print(str(int(ifm_list[l_idx]))+",",end="")
        else:
            print(str(int(ifm_list[l_idx]))+"};",)

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

print('''FPGA_DATA_FIX output[M*(R/K_maxpool)*(C/K_maxpool)] = {''',end="")
ifm_list = ofm.flatten().tolist()
print_list(ifm_list)


print(ofm.shape)

# print(ofm.flatten().tolist())