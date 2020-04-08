from __future__ import print_function
import datetime
import os
import time
import sys

import torch
import torch.utils.data
from torch import nn
import torchvision
from torchvision import transforms

import utils
from optimizer import PruneAdam

try:
    from apex import amp
except ImportError:
    amp = None


#一次迭代过程
def train_one_epoch(model, criterion, optimizer, mask, data_loader, device, epoch, print_freq, apex=False):
    model.train()
    metric_logger = utils.MetricLogger(delimiter="  ") 
    metric_logger.add_meter('lr', utils.SmoothedValue(window_size=1, fmt='{value}'))
    metric_logger.add_meter('img/s', utils.SmoothedValue(window_size=10, fmt='{value}'))

    header = 'Epoch: [{}]'.format(epoch)
    for image, target in metric_logger.log_every(data_loader, print_freq, header):
        start_time = time.time()
        image, target = image.to(device), target.to(device)
        output = model(image)
        loss = criterion(output, target)

        optimizer.zero_grad()#梯度置为0
        if apex:
            with amp.scale_loss(loss, optimizer) as scaled_loss:
                scaled_loss.backward()
        else:
            loss.backward()#前向传播
        optimizer.prune_step(mask)#剪枝优化器

        acc1, acc5 = utils.accuracy(output, target, topk=(1, 5))
        batch_size = image.shape[0]
        metric_logger.update(loss=loss.item(), lr=optimizer.param_groups[0]["lr"])
        metric_logger.meters['acc1'].update(acc1.item(), n=batch_size)
        metric_logger.meters['acc5'].update(acc5.item(), n=batch_size)
        metric_logger.meters['img/s'].update(batch_size / (time.time() - start_time))


#测试
def evaluate(model, criterion, data_loader, device, print_freq=100):
    model.eval()#更改调用它的模块的forward()行为，它禁用dropout，并有batch norm使用整个统计
    metric_logger = utils.MetricLogger(delimiter="  ")
    header = 'Test:'
    with torch.no_grad():#在autograd中禁用梯度追踪
        for image, target in metric_logger.log_every(data_loader, print_freq, header):
            image = image.to(device, non_blocking=True)
            target = target.to(device, non_blocking=True)
            output = model(image)
            loss = criterion(output, target)

            acc1, acc5 = utils.accuracy(output, target, topk=(1, 5))
            # FIXME need to take into account that the datasets
            # could have been padded in distributed setup
            batch_size = image.shape[0]
            metric_logger.update(loss=loss.item())
            metric_logger.meters['acc1'].update(acc1.item(), n=batch_size)
            metric_logger.meters['acc5'].update(acc5.item(), n=batch_size)
    # gather the stats from all processes
    metric_logger.synchronize_between_processes()

    print(' * Acc@1 {top1.global_avg:.3f} Acc@5 {top5.global_avg:.3f}'
          .format(top1=metric_logger.acc1, top5=metric_logger.acc5))
    return metric_logger.acc1.global_avg


def _get_cache_path(filepath):
    import hashlib
    h = hashlib.sha1(filepath.encode()).hexdigest()#使用SHA1算法进行字符加密
    cache_path = os.path.join("~", ".torch", "vision", "datasets", "imagefolder", h[:10] + ".pt")
    cache_path = os.path.expanduser(cache_path)
    print("cache_path:",cache_path)
    return cache_path

#加载数据
def load_data(traindir, valdir, cache_dataset, distributed):
    # Data loading code
    print("--Loading data--")
    normalize = transforms.Normalize(mean=[0.485, 0.456, 0.406],
                                     std=[0.229, 0.224, 0.225])

    print("1.Loading training data")
    st = time.time()
    cache_path = _get_cache_path(traindir)
    if cache_dataset and os.path.exists(cache_path):
        # Attention, as the transforms are also cached!
        print("Loading dataset_train from {}".format(cache_path))
        dataset, _ = torch.load(cache_path)
    else:
        dataset = torchvision.datasets.ImageFolder(
            traindir,
            transforms.Compose([
                transforms.RandomResizedCrop(224),#随机裁剪
                transforms.RandomHorizontalFlip(),#随机水平翻转
                transforms.ToTensor(),
                normalize,
            ]))
        if cache_dataset:
            print("Saving dataset_train to {}".format(cache_path))
            utils.mkdir(os.path.dirname(cache_path))
            utils.save_on_master((dataset, traindir), cache_path)
    print("Loading training data Time", time.time() - st)

    print("2.Loading validation data")
    cache_path = _get_cache_path(valdir)
    if cache_dataset and os.path.exists(cache_path):
        # Attention, as the transforms are also cached!
        print("Loading dataset_test from {}".format(cache_path))
        dataset_test, _ = torch.load(cache_path)
    else:
        dataset_test = torchvision.datasets.ImageFolder(
            valdir,
            transforms.Compose([
                transforms.Resize(256),
                transforms.CenterCrop(224),
                transforms.ToTensor(),
                normalize,
            ]))
        if cache_dataset:
            print("Saving dataset_test to {}".format(cache_path))
            utils.mkdir(os.path.dirname(cache_path))
            utils.save_on_master((dataset_test, valdir), cache_path)

    print("3.Creating data loaders")
    if distributed:
        train_sampler = torch.utils.data.distributed.DistributedSampler(dataset)
        test_sampler = torch.utils.data.distributed.DistributedSampler(dataset_test)
    else:
        train_sampler = torch.utils.data.RandomSampler(dataset)#随机采样
        test_sampler = torch.utils.data.SequentialSampler(dataset_test)#顺序采样

    return dataset, dataset_test, train_sampler, test_sampler


def modify_model(vgg):

    for param in vgg.parameters():
        param.requires_grad = False
    layers = list(vgg.layer4[0].children())[:-1]

    bck1 = vgg.state_dict()["layer4.0.conv1.weight"][:]
    bck2 = vgg.state_dict()["layer4.0.bn1.weight"][:]
    bck3 = vgg.state_dict()["layer4.0.bn1.bias"][:]
    bck4 = vgg.state_dict()["layer4.0.conv2.weight"][:]
    bck5 = vgg.state_dict()["layer4.0.bn1.running_mean"][:]
    bck6 = vgg.state_dict()["layer4.0.bn1.running_var"][:]

    ch = 460

    print("="*100)
    for name, param in vgg.named_parameters():
        print (name,param.requires_grad,param.data.shape,param.data.min())


    layers[0] = torch.nn.Conv2d(256, ch, kernel_size=(3, 3), stride=(2, 2), padding=(1, 1), bias=False)
    layers[1] = torch.nn.BatchNorm2d(ch, eps=1e-05, momentum=0.1, affine=True, track_running_stats=True)
    layers[3] = torch.nn.Conv2d(ch, 512, kernel_size=(3, 3), stride=(1, 1), padding=(1, 1), bias=False)

    vgg.layer4[0].conv1 = layers[0]
    vgg.layer4[0].bn1 = layers[1]
    vgg.layer4[0].conv2 = layers[3]
    
    print(bck1.shape, bck4.shape)

    vgg.state_dict()["layer4.0.conv1.weight"][:] = bck1[0:ch,:,:,:]
    vgg.state_dict()["layer4.0.bn1.weight"][:] = bck2[0:ch]
    vgg.state_dict()["layer4.0.bn1.bias"][:] = bck3[0:ch]
    vgg.state_dict()["layer4.0.conv2.weight"][:] = bck4[:,0:ch,:,:]
    vgg.state_dict()["layer4.0.bn1.running_mean"][:] = bck5[0:ch]
    vgg.state_dict()["layer4.0.bn1.running_var"][:] =bck6[0:ch]



    print("="*100)
    for name, param in vgg.named_parameters():
        print (name,param.requires_grad,param.data.shape,param.data.min())


    ''' VGG Modifications
    for param in vgg.parameters():
        param.requires_grad = False
    layers = list(vgg.features.children())[:-1]
    
    features_3_weight = vgg.state_dict()["features.3.weight"][:]
    features_3_bias = vgg.state_dict()["features.3.bias"][:]
    features_6_weight = vgg.state_dict()["features.6.weight"][:]
    features_6_bias = vgg.state_dict()["features.6.bias"][:]

    ch = 126

    layers[3] = torch.nn.Conv2d(64, ch, kernel_size=(3, 3), stride=(1, 1), padding=(1, 1))
    layers[6] = torch.nn.Conv2d(ch, 256, kernel_size=(3, 3), stride=(1, 1), padding=(1, 1))

    features = torch.nn.Sequential(*layers)
    vgg.features = features

    vgg.state_dict()["features.3.weight"][:] = features_3_weight[0:ch,:,:,:]
    vgg.state_dict()["features.3.bias"][:] = features_3_bias[0:ch]
    vgg.state_dict()["features.6.weight"][:] = features_6_weight[:,0:ch,:,:]
    vgg.state_dict()["features.6.bias"][:] = features_6_bias[:]

    '''
    return vgg


def main(args):
    if args.apex:#混合精度训练
        if sys.version_info < (3, 0):
            raise RuntimeError("Apex currently only supports Python 3. Aborting.")
        if amp is None:
            raise RuntimeError("Failed to import apex. Please install apex from https://www.github.com/nvidia/apex "
                               "to enable mixed-precision training.")

    if args.output_dir:#保存路径
        utils.mkdir(args.output_dir)

    utils.init_distributed_mode(args)
    print(args)

    device = torch.device(args.device)

    torch.backends.cudnn.benchmark = True#为整个网络的每个卷积层搜索最适合它的卷积实现算法，进而实现网络的加速

    train_dir = os.path.join(args.data_path, 'train')#路径拼接，data_path\train
    val_dir = os.path.join(args.data_path, 'val')
    #加载数据
    dataset, dataset_test, train_sampler, test_sampler = load_data(train_dir, val_dir,
                                                                   args.cache_dataset, args.distributed)
    data_loader = torch.utils.data.DataLoader(
        dataset, batch_size=args.batch_size,
        sampler=train_sampler, num_workers=args.workers, pin_memory=True)

    data_loader_test = torch.utils.data.DataLoader(
        dataset_test, batch_size=args.batch_size,
        sampler=test_sampler, num_workers=args.workers, pin_memory=True)
    #创建模型
    print("--Creating model--")
    model = torchvision.models.__dict__[args.model](pretrained=args.pretrained)

    model.to(device)
    if args.distributed and args.sync_bn:
        model = torch.nn.SyncBatchNorm.convert_sync_batchnorm(model)

    criterion = nn.CrossEntropyLoss()#交叉熵损失函数，对分类问题有用
    optimizer = PruneAdam(model.named_parameters(), lr=args.lr, eps=args.adam_epsilon)#使用剪枝优化器
    mask = utils.apply_pattern_prune(model,device,'19')#选择剪枝标准,可以输入0或者其他1-9的组合

    if args.apex:#使用混合精度训练
        model, optimizer = amp.initialize(model, optimizer,
                                          opt_level=args.apex_opt_level)
    model_without_ddp = model
    if args.distributed:#分布式训练
        model = torch.nn.parallel.DistributedDataParallel(model, device_ids=[args.gpu])
        model_without_ddp = model.module

    if args.resume:#从检查点重新开始
        checkpoint = torch.load(args.resume, map_location='cpu')
        model_without_ddp.load_state_dict(checkpoint['model'])
        optimizer.load_state_dict(checkpoint['optimizer'])
        args.start_epoch = checkpoint['epoch'] + 1

    if args.test_only:#只测试模型
        evaluate(model, criterion, data_loader_test, device=device)
        return
    #开始训练
    print("--Start training--")
    start_time = time.time()
    for epoch in range(args.start_epoch, args.epochs):
        if args.distributed:
            train_sampler.set_epoch(epoch)
        train_one_epoch(model, criterion, optimizer, mask, data_loader, device, epoch, args.print_freq, args.apex)
        evaluate(model, criterion, data_loader_test, device=device)#测试
        if args.output_dir:#保存路径
            checkpoint = {
                'model': model_without_ddp.state_dict(),
                'optimizer': optimizer.state_dict(),
                'epoch': epoch,
                'args': args}
            utils.save_on_master(
                checkpoint,
                os.path.join(args.output_dir, 'model_{}.pth'.format(epoch)))
            utils.save_on_master(
                checkpoint,
                os.path.join(args.output_dir, 'checkpoint.pth'))

    total_time = time.time() - start_time
    total_time_str = str(datetime.timedelta(seconds=int(total_time)))
    print('Training time {}'.format(total_time_str))


def parse_args():
    import argparse
    parser = argparse.ArgumentParser(description='PyTorch Classification Training')

    parser.add_argument('--data-path', default='/dataset/imagenet', help='dataset')
    parser.add_argument('--model', default='vgg11', help='model')
    parser.add_argument('--device', default='cuda', help='device')
    parser.add_argument('-b', '--batch-size', default=256, type=int)
    parser.add_argument('--epochs', default=30, type=int, metavar='N',
                        help='number of total epochs to run')
    parser.add_argument('-j', '--workers', default=16, type=int, metavar='N',
                        help='number of data loading workers (default: 16)')
    parser.add_argument('--lr', default=1e-3, type=float, help='initial learning rate')
    parser.add_argument('--momentum', default=0.9, type=float, metavar='M',
                        help='momentum')
    parser.add_argument('--wd', '--weight-decay', default=1e-4, type=float,
                        metavar='W', help='weight decay (default: 1e-4)',
                        dest='weight_decay')
    parser.add_argument('--lr-step-size', default=30, type=int, help='decrease lr every step-size epochs')
    parser.add_argument('--lr-gamma', default=0.1, type=float, help='decrease lr by a factor of lr-gamma')
    parser.add_argument('--print-freq', default=10, type=int, help='print frequency')
    parser.add_argument('--output-dir', default='.', help='path where to save')
    parser.add_argument('--resume', default='', help='resume from checkpoint')
    parser.add_argument('--start-epoch', default=0, type=int, metavar='N',
                        help='start epoch')
    parser.add_argument('--adam_epsilon', type=float, default=1e-8, metavar='E',#避免除数为0
                        help='adam epsilon (default: 1e-8)')
    parser.add_argument(
        "--cache-dataset",
        dest="cache_dataset",
        help="Cache the datasets for quicker initialization. It also serializes the transforms",
        action="store_true",
    )
    parser.add_argument(
        "--sync-bn",
        dest="sync_bn",#指定参数位置
        help="Use sync batch norm",#描述这个选项的作用
        action="store_true",#该选项要执行的操作
    )
    parser.add_argument(
        "--test-only",
        dest="test_only",
        help="Only test the model",
        action="store_true",
    )
    parser.add_argument(
        "--pretrained",
        dest="pretrained",
        help="Use pre-trained models from the modelzoo",
        action="store_true",
    )

    # Mixed precision training parameters混合精度训练参数
    parser.add_argument('--apex', action='store_true',
                        help='Use apex for mixed precision training')
    parser.add_argument('--apex-opt-level', default='O1', type=str,
                        help='For apex mixed precision training'
                             'O0 for FP32 training, O1 for mixed precision training.'
                             'For further detail, see https://github.com/NVIDIA/apex/tree/master/examples/imagenet'
                        )

    # distributed training parameters
    parser.add_argument('--world-size', default=1, type=int,
                        help='number of distributed processes')
    parser.add_argument('--dist-url', default='env://', help='url used to set up distributed training')

    args = parser.parse_args()

    return args


if __name__ == "__main__":
    args = parse_args()
    main(args)
