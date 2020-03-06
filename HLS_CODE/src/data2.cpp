#include "source_fix.h"

FPGA_DATA_FIX input[N*R*C];
FPGA_DATA_FIX weight[M*N*K*K];
FPGA_DATA_FIX bias[M];
FPGA_DATA_FIX output[M*(R-K+1)*(C-K+1)];
