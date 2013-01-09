# 说明

本科毕设时候实现的一个车牌识别系统。

包括了下面几个流程 ： 预处理，车牌定位，车牌分割，和用神经网络算法进行识别

识别率当然是个有待商榷的问题 ：）

## 程序运行说明

*   编译结果放在工程目录下面 ： PlateIdenfify.exe
*   /NetData/TrainData 文件夹中存放的是训练用的样本。/NetData文件夹中后缀为net保存的是ANN网络的信息。
*   /Pic中放置的是展示的效果图。
*   /Pic/SplitePics用于保存分割后的图片

## 编程环境

OS : win7    
IDE：MS2010 
Language : C++
第三方库：opencv
