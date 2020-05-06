import torch
from torch import nn
import torch.nn.functional as F


def build_activation(kind: str, inplace: bool = True):
    if kind == 'relu':
        return nn.ReLU(inplace=inplace)
    elif kind == 'relu6':
        return nn.ReLU6(inplace=inplace)
    elif kind == 'tanh':
        return nn.Tanh()
    elif kind == 'sigmoid':
        return nn.Sigmoid()
    elif kind == 'NoAct':
        return nn.Identity()
    elif kind is None:
        return None
    else:
        raise ValueError('Unsupported activation function: {}'.format(kind))


def get_same_padding(kernel_size: int, dilation: int) -> int:
    assert kernel_size % 2 > 0, 'Kernel size must be odd number'
    p = (kernel_size // 2) * dilation
    return p


class BnRound(
    nn.Module):  # round function after batch normalization, packed into module so it can be added directly to nn.sequential
    def __init__(self):
        super(BnRound, self).__init__()

    def forward(self, x):
        return torch.round(x)


class ConvBlock(nn.Sequential):
    """ Standard 2D convolution
    """

    def __init__(self,
                 in_channels: int,
                 out_channels: int,
                 kernel_size: int,
                 stride: int,
                 act: str,
                 dilation: int = 1,
                 groups: int = 1):
        super().__init__(
            nn.Conv2d(in_channels,
                      out_channels,
                      kernel_size,
                      stride=stride,
                      padding=get_same_padding(kernel_size, dilation),
                      bias=False,
                      dilation=dilation,
                      groups=groups),
            nn.BatchNorm2d(out_channels, affine=False, track_running_stats=True),
            build_activation(act),
            BnRound())


class PointwiseConv(nn.Sequential):
    """ Expand/reduce the channels by 1x1 convolution
    """

    def __init__(self, in_channels: int, out_channels: int, act: str, groups: int = 1):
        super().__init__(
            nn.Conv2d(in_channels,
                      out_channels,
                      kernel_size=1,
                      stride=1,
                      padding=0,
                      groups=groups,
                      bias=False),
            nn.BatchNorm2d(out_channels, affine=False, track_running_stats=True),
            build_activation(act),
            BnRound())


class PointwiseLinear(nn.Sequential):
    def __init__(self,
                 in_channels: int,
                 out_channels: int,
                 groups: int = 1):
        super().__init__(
            nn.Conv2d(in_channels,
                      out_channels,
                      kernel_size=1,
                      stride=1,
                      padding=0,
                      groups=groups,
                      bias=False),
            nn.BatchNorm2d(out_channels, affine=False, track_running_stats=True),
            BnRound()
        )


class DepthwiseConv(nn.Sequential):
    """ Channel-wise convolution
    """

    def __init__(self,
                 channels: int,
                 kernel_size: int,
                 stride: int,
                 act: str,
                 dilation: int = 1,
                 groups: int = 1):
        super().__init__(
            nn.Conv2d(channels,
                      channels,
                      kernel_size=kernel_size,
                      stride=stride,
                      padding=get_same_padding(kernel_size, dilation),
                      dilation=dilation,
                      groups=channels,
                      bias=False),
            nn.BatchNorm2d(channels, affine=False, track_running_stats=True),
            build_activation(act),
            BnRound())


class Bottleneck(nn.Sequential):
    def __init__(self,
                 in_channels: int,
                 out_channels: int,
                 kernel_size: int,
                 stride: int,
                 act: str,
                 dilation: int = 1,
                 groups: int = 1,
                 expand_ratio: int = 1):

        assert expand_ratio >= 0, 'Expand ratio must be positive integer'

        self.spec = "Bottleneck({}, {}, k={}, s={}, d={}, g={}, e={}, '{}')".format(
            in_channels, out_channels, kernel_size, stride, dilation, groups, expand_ratio, act)

        self.shortcut = False #stride == 1 and in_channels == out_channels

        if expand_ratio == 1:
            super().__init__(
                DepthwiseConv(in_channels, kernel_size, stride, act, dilation, groups),
                PointwiseLinear(in_channels, out_channels, groups))
        else:
            mid_channels = round(in_channels * expand_ratio)
            super().__init__(
                PointwiseConv(in_channels, mid_channels, act, groups),
                DepthwiseConv(mid_channels, kernel_size, stride, act, dilation, groups),
                PointwiseLinear(mid_channels, out_channels, groups))

    def __str__(self):
        return self.spec

    def forward(self, x):
        if self.shortcut:
            print("ADD SELF")
            return torch.add(x, super().forward(x))
        else:
            return super().forward(x)


# Reference:
# https://github.com/pytorch/vision/blob/d2c763e14efe57e4bf3ebf916ec243ce8ce3315c/torchvision/models/segmentation/deeplabv3.py#L62
class ASPPPooling(nn.Sequential):
    def __init__(self, in_channels: int, out_channels: int, act: str):
        super().__init__(
            nn.AdaptiveAvgPool2d(1),
            ConvBlock(in_channels, out_channels, 1, 1, act))

    def forward(self, x):
        size = x.shape[-2:]
        for mod in self:
            x = mod(x)
        return F.interpolate(x, size=size, mode='bilinear', align_corners=False)


class ASPP(nn.Module):
    def __init__(self, in_channels: int, out_channels: int, act: str, rates: tuple):
        super(ASPP, self).__init__()

        r1, r2, r3 = rates

        self.convs = nn.ModuleList([
            ConvBlock(in_channels, out_channels, 1, 1, act),
            ConvBlock(in_channels, out_channels, 3, 1, act, r1),
            ConvBlock(in_channels, out_channels, 3, 1, act, r2),
            ConvBlock(in_channels, out_channels, 3, 1, act, r3),
            ASPPPooling(in_channels, out_channels, act)])

        self.project = ConvBlock(5 * out_channels, out_channels, 1, 1, act)

    def forward(self, x):
        return self.project(torch.cat([conv(x) for conv in self.convs], dim=1))


class Example(nn.Sequential):
    def __init__(self, act):
        super().__init__(
            ConvBlock(3, 32, 3, 1, act),  # s=2
            #  16, s=1, n=1
            Bottleneck(32, 16, 3, 1, act),
            #  24, s=2, n=2
            Bottleneck(16, 24, 3, 1, act, expand_ratio=6),
            Bottleneck(24, 24, 3, 1, act, expand_ratio=6),


            #  32, s=2, n=3
            Bottleneck( 24,  32, 3, 1, act, expand_ratio=6),
            Bottleneck( 32,  32, 3, 1, act, expand_ratio=6),
            Bottleneck( 32,  32, 3, 1, act, expand_ratio=6),
            #  64, s=2, n=4
            Bottleneck( 32,  64, 3, 1, act, expand_ratio=6),
            Bottleneck( 64,  64, 3, 1, act, expand_ratio=6),
            Bottleneck( 64,  64, 3, 1, act, expand_ratio=6),
            Bottleneck( 64,  64, 3, 1, act, expand_ratio=6),
            #  96, s=1, n=3
            Bottleneck( 64,  96, 3, 1, act, expand_ratio=6),
            Bottleneck( 96,  96, 3, 1, act, expand_ratio=6),
            Bottleneck( 96,  96, 3, 1, act, expand_ratio=6),
            # 160, s=2, n=3
            Bottleneck( 96, 160, 3, 1, act, expand_ratio=6),
            Bottleneck(160, 160, 3, 1, act, expand_ratio=6),
            Bottleneck(160, 160, 3, 1, act, expand_ratio=6),
            # 320, s=1, n=1
            Bottleneck(160, 320, 3, 1, act, expand_ratio=6))
#            # ASPP
#            ASPP      (320, 256, act, (6, 12, 18)),
#            nn.Dropout(0.5),
#            ConvBlock (256, 256, 3, 1, act),
#            nn.Conv2d (256,  20, 1, 1),
#            nn.Upsample(scale_factor=32, mode='bilinear', align_corners=False))


def add_hooks(model, hook):
    for layer in model.children():
        if list(layer.children()) == []:
            layer.register_forward_hook(hook)
        else:
            for child in layer.children():
                add_hooks(child, hook)


def forward_hook(module, inputs, result):
    print(module)
    for i, x in enumerate(inputs):  # in case there are multiple inputs
        print('input{}: {}'.format(i, x.shape))
    print('result: {}'.format(result.shape))
    #    if isinstance(module, nn.Conv2d):
    print('-' * 100)


def print_list(ifm_list, fh):
    print(str(len(ifm_list)) + '] = {', end="", file=fh)
    for l_idx in range(len(ifm_list)):
        if l_idx != len(ifm_list) - 1:
            print(str(int(ifm_list[l_idx])) + ",", end="", file=fh)
        else:
            print(str(int(ifm_list[l_idx])) + "};", file=fh)


def main():
    torch.manual_seed(7)
    torch.cuda.manual_seed(7)
    model = Example('NoAct')  # model = Example('relu6')
    model.eval()

    #    add_hooks(model, forward_hook)

    fh = open("params/data_mobile.cpp", mode='w')
    print('''#include "source_fix.h"
FPGA_DATA_FIX input[''', end="", file=fh)
    x = torch.randint(1,3,(1, 3, 3, 3), dtype=torch.float)
    # x = torch.zeros((1, 3, 12, 12))  # (1, 3, 12, 12)
    pad1 = nn.ZeroPad2d(1)
    x_pad = pad1(x)
    x_list = x_pad.flatten().tolist()

    print_list(x_list, fh)

    #    model.state_dict()[""]
    counter_i = 0;
    counter_j = 0;



    print(model)

    tmp_x = x.detach()

    for layer_name, layer in model.named_modules():
        if isinstance(layer, nn.Conv2d):
            counter_i = counter_i + 1
            seq = layer_name.split(".")
            para_w = model.state_dict()[layer_name + ".weight"][:]
            # para_b = model.state_dict()[layer_name + ".bias"][:]
            # torch.nn.init.normal_(para_w, mean=0.0, std=1.0)
            # para_w = torch.ones(para_w.size(),
            #                     dtype=torch.float)
            para_w = torch.randint(-1,2,para_w.size(), dtype=torch.float)
            model.state_dict()[layer_name + ".weight"][:] = para_w
            print(layer_name)
            print('CNN weight num:', counter_i)
            w_list = para_w.flatten().tolist()
            # print_list(w_list)
            print('FPGA_DATA_FIX weight_' + str(counter_i) + '[', end="", file=fh)
            print_list(w_list, fh)

            tmp_x = layer(tmp_x)
            print('FPGA_DATA_FIX conv_y_' + str(counter_i) + '[', end="", file=fh)
            print_list(tmp_x.flatten(), fh)

        elif isinstance(layer, nn.BatchNorm2d):
            counter_j = counter_j + 1
            para_E = model.state_dict()[layer_name + ".running_mean"][:]
            para_V = model.state_dict()[layer_name + ".running_var"][:]
            para_E = torch.randint(0, 1, para_E.size(), dtype=torch.float)
            if counter_j>39:
                 v_lower_bound =60
            elif counter_j>30:
                 v_lower_bound =60
            elif counter_j>18:
                 v_lower_bound =40
            elif counter_j>9:
                 v_lower_bound =10
            else:
                 v_lower_bound = 8
            para_V = torch.randint(v_lower_bound, v_lower_bound+3, para_V.size(), dtype=torch.float)  # 30,33
            model.state_dict()[layer_name + ".running_mean"][:] = para_E
            model.state_dict()[layer_name + ".running_var"][:] = para_V
            print('BatchNorm weight num:', counter_j)
            print('FPGA_DATA_FIX E' + str(counter_j) + '[', end="", file=fh)
            e_list = para_E.flatten().tolist()
            print_list(e_list, fh)
            print('FPGA_DATA_FIX V' + str(counter_j) + '[', end="", file=fh)
            v_list = para_V.flatten().tolist()
            print_list(v_list, fh)

            tmp_x = layer(tmp_x)

        elif isinstance(layer, BnRound):

            tmp_x = layer(tmp_x)
            print('FPGA_DATA_FIX bn_y_' + str(counter_i) + '[', end="", file=fh)
            print_list(tmp_x.flatten(), fh)

        elif isinstance(layer,nn.Identity):
            tmp_x = layer(tmp_x)

    model.eval()
    with torch.no_grad():
        y = model(x)
    y_list = y.flatten().tolist()
    print('''FPGA_DATA_FIX output[''', end="", file=fh)
    print_list(y_list, fh)


if __name__ == '__main__':
    main()
