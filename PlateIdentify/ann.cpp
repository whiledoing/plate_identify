#include "StdAfx.h"
#include "ann.h"
#include <time.h>

void WaitForIdle()
{
    MSG msg;
    while (::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        ::TranslateMessage(&msg);
        ::DispatchMessage(&msg);
    }
}
void CNeuralNet::InitializeNetwork()
{
    //初始化隐含层的权值
    int i, j;
    srand(unsigned(time(NULL)));
    for (i = 0; i < m_nHNeuronNum; ++i) {
        for (j = 0; j < m_nInputNum + 1; ++j) {		//加一是因为有偏置项的权值也要分配
            m_pHiddenL->m_pNeurons[i].m_pWeights[j] = WeightRand();
#ifdef NEED_MOMENTUM
            m_pHiddenL->m_pNeurons[i].m_pLastUpdates[j] = 0;
#endif
        }
    }
    //初始化输出层的权值
    for (i = 0; i < m_nOutputNum; ++i) {
        for (j = 0; j < m_nHNeuronNum + 1; ++j) {
            m_pOutputL->m_pNeurons[i].m_pWeights[j] = WeightRand();
#ifdef NEED_MOMENTUM
            m_pOutputL->m_pNeurons[i].m_pLastUpdates[j] = 0;
#endif
        }
    }
}

double CNeuralNet::WeightRand()
{
    double temp = (double)(rand()) / (RAND_MAX + 1);			//temp 在0-1之间
    return ((temp - 0.5) * 0.1);
}

bool CNeuralNet::CalculateOutput(const DVECTOR &eachInput,	//EachInput为单一一次数据的输入
                                 DVECTOR& eachOutput)							//EachOutput保存一次计算的结果
{
    if (eachInput.size() != m_nInputNum)
        return false;
    int i, j;
    double sum;
    //计算隐藏层的结果
    for (i = 0; i < m_nHNeuronNum; ++i) {
        sum = 0.0;
        for (j = 0; j < m_nInputNum; ++j)
            sum += m_pHiddenL->m_pNeurons[i].m_pWeights[j] * (eachInput[j]);
        sum += m_pHiddenL->m_pNeurons[i].m_pWeights[j] * BIAS;		//这里j刚好表示的就是偏置项的坐标
        m_pHiddenL->m_pNeurons[i].m_dResult = Sigmoid(sum);
    }
    //计算输出层的结果
    for (i = 0; i < m_nOutputNum; ++i) {
        sum = 0.0;
        for (j = 0; j < m_nHNeuronNum; ++j)
            sum += (m_pOutputL->m_pNeurons[i].m_pWeights[j]) * (m_pHiddenL->m_pNeurons[j].m_dResult);
        sum += m_pOutputL->m_pNeurons[i].m_pWeights[j] * BIAS;
        m_pOutputL->m_pNeurons[i].m_dResult = Sigmoid(sum);
        eachOutput.push_back(m_pOutputL->m_pNeurons[i].m_dResult);
    }
    return true;
}

void CNeuralNet::UpdateWeight(const DVECTOR &eachInput,
                              const DVECTOR &eachOutput)
{
    int i, j, k;
    double err;			//中间的误差项的值
    double DeltaWeight;
    double tempOut;
    //更新输出层的权值
    for (i = 0; i < m_nOutputNum; ++i) {
        tempOut = m_pOutputL->m_pNeurons[i].m_dResult;
        err = (double) (tempOut * (1 - tempOut) * (eachOutput[i] - tempOut));
        m_pOutputL->m_pNeurons[i].m_dError = err;
        for (j = 0; j < m_nHNeuronNum; ++j) {	//更新每个权重系数
            DeltaWeight = err * m_dLearningRate * m_pHiddenL->m_pNeurons[j].m_dResult;
            m_pOutputL->m_pNeurons[i].m_pWeights[j] += DeltaWeight;
#ifdef NEED_MOMENTUM
            m_pOutputL->m_pNeurons[i].m_pWeights[j] += MOMENTUM * m_pOutputL->m_pNeurons[i].m_pLastUpdates[j];
            m_pOutputL->m_pNeurons[i].m_pLastUpdates[j] = DeltaWeight;
#endif
        }
        //更新偏置项
        DeltaWeight = err * m_dLearningRate * BIAS;
        m_pOutputL->m_pNeurons[i].m_pWeights[j] += DeltaWeight;
#ifdef NEED_MOMENTUM
        m_pOutputL->m_pNeurons[i].m_pWeights[j] += MOMENTUM * m_pOutputL->m_pNeurons[i].m_pLastUpdates[j];
        m_pOutputL->m_pNeurons[i].m_pLastUpdates[j] = DeltaWeight;
#endif
    }
    //更新隐藏层的权值
    double sum;
    for (i = 0; i < m_nHNeuronNum; ++i) {
        sum = 0.0;
        for (k = 0; k < m_nOutputNum; ++k) {
            sum += m_pOutputL->m_pNeurons[k].m_dError * m_pOutputL->m_pNeurons[k].m_pWeights[i];
        }
        tempOut = m_pHiddenL->m_pNeurons[i].m_dResult;
        err = tempOut * (1 - tempOut) * sum;
        for (j = 0; j < m_nInputNum; ++j) {
            DeltaWeight = err * m_dLearningRate * eachInput[j];
            m_pHiddenL->m_pNeurons[i].m_pWeights[j] += DeltaWeight;
#ifdef NEED_MOMENTUM
            m_pHiddenL->m_pNeurons[i].m_pWeights[j] += MOMENTUM * m_pHiddenL->m_pNeurons[i].m_pLastUpdates[j];
            m_pHiddenL->m_pNeurons[i].m_pLastUpdates[j] = DeltaWeight;
#endif
        }
        //更新偏置项
        DeltaWeight = err * m_dLearningRate * BIAS;
        m_pHiddenL->m_pNeurons[i].m_pWeights[j] += DeltaWeight;
#ifdef NEED_MOMENTUM
        m_pHiddenL->m_pNeurons[i].m_pWeights[j] += MOMENTUM * m_pHiddenL->m_pNeurons[i].m_pLastUpdates[j];
        m_pHiddenL->m_pNeurons[i].m_pLastUpdates[j] = DeltaWeight;
#endif
    }
}
//#define _CONSOLETEST
bool CNeuralNet::TrainingEpoch(const std::vector<DVECTOR>& input,
                               const std::vector<DVECTOR>& output)
{
    for (int i = 0; i < input.size(); ++i) {
        DVECTOR tempOut;			//用来表示一个样本单次的输出值
        if (!CalculateOutput(input[i], tempOut))			//计算结果值
            return false;
#ifdef _CONSOLETEST
        //fstream fout;
        //fout.open("log.txt",ios::out | ios::app);
        for(int xx = 0; xx<tempOut.size(); ++xx)
            cout<<tempOut[xx]<<"  ";
        cout<<"\n"<<"-----------------------------"<<"\n";
        //fout.close();
#endif
        //利用tempOut和output统计误差总和
        for (int j = 0; j < m_nOutputNum; j++)
            m_dErrSum += (double)
                         (tempOut[j] - output[i][j]) * (double)
                         (tempOut[j] - output[i][j]);

        UpdateWeight(input[i], output[i]);				//由此来更新新的权值
    }
    m_nEposhNum++;
    return true;
}

bool CNeuralNet::Train(const std::vector<DVECTOR>& input,
                       const std::vector<DVECTOR>& output)
{
    m_bStop = false;
    while (m_nEposhNum < m_nMaxEpoch) {
        if (TrainingEpoch(input, output) == false)			//训练失败
            return false;

        //计算一个时代平均到每一次训练，每一个输出点的平均值
        m_dMeanErr = m_dErrSum / (m_nOutputNum * input.size());
        m_dErrSum = 0.0;
        if (m_dMeanErr < m_dMinErr)
            return true;

        //从循环中暂停一下去看下有没有要处理的消息（这里主要是为了让用户可以手动用m_bStop来停止循环的运行）
        //	WaitForIdle();
        //	if (m_bStop)
        //		return true;
    }
    return true;
}

BOOL CNeuralNet::Recognize(const DVECTOR &eachInput, int& nType,
                           double& dConfidence)
{
    DVECTOR output;
    if (!CalculateOutput(eachInput, output))
        return false;
    double dmax1 = -1, dmax2 = -1;    //dmax1记录最大的值 dmax2记录次大的值
    for (int i = 0; i < output.size(); ++i) {
        if (output[i] > dmax2) {
            if(output[i] > dmax1) {
                dmax2 = dmax1;
                dmax1 = output[i];
                nType = i;
            } else
                dmax2 = output[i];
        }
    }
    dConfidence = (dmax1 - dmax2)/dmax1;
    return true;
}

bool CNeuralNet::SaveToFile(const char* lpszFileName, bool bCreate /*= true*/)
{
    CFile file;
    if (bCreate) {
        if (!file.Open(lpszFileName, CFile::modeCreate | CFile::modeWrite))
            return false;
    } else {
        if (!file.Open(lpszFileName, CFile::modeWrite))
            return false;
    }
    NEURALNET_HEADER nh = {0};
    nh.dwVersion = MY_NEURALNET_VERSION;
    nh.m_dErrSum = m_dErrSum;
    nh.m_dLearningRate = m_dLearningRate;
    nh.m_dMeanErr = m_dMeanErr;
    nh.m_dMinErr = m_dMinErr;
    nh.m_nEposhNum = m_nEposhNum;
    nh.m_nHNeuronNum = m_nHNeuronNum;
    nh.m_nInputNum = m_nInputNum;
    nh.m_nMaxEpoch = m_nMaxEpoch;
    nh.m_nOutputNum = m_nOutputNum;
    file.Write(&nh, sizeof(nh));
    //写入隐含层的信息
    int i, j;
    for (i = 0; i < m_nHNeuronNum; ++i) {
        file.Write(&m_pHiddenL->m_pNeurons[i].m_dError, sizeof(double));
        file.Write(&m_pHiddenL->m_pNeurons[i].m_dResult, sizeof(double));
        for (j = 0; j < m_nInputNum + 1; ++j) {
            file.Write(&m_pHiddenL->m_pNeurons[i].m_pWeights[j],
                       sizeof(WEIGHT_TYPE));
        }
    }
    //写入输出层的信息
    for (i = 0; i < m_nOutputNum; ++i) {
        file.Write(&m_pOutputL->m_pNeurons[i].m_dError, sizeof(double));
        file.Write(&m_pOutputL->m_pNeurons[i].m_dResult, sizeof(double));
        for (j = 0; j < m_nHNeuronNum + 1; ++j) {
            file.Write(&m_pOutputL->m_pNeurons[i].m_pWeights[j],
                       sizeof(WEIGHT_TYPE));
        }
    }
    file.Close();
    return true;
}

bool CNeuralNet::LoadFromFile(const char* lpszFileName)
{
    CFile file;
    if (!file.Open(lpszFileName, CFile::modeRead))
        return false;

    NEURALNET_HEADER nh = {0};
    file.Read(&nh, sizeof(nh));

    if (nh.dwVersion != MY_NEURALNET_VERSION)			//判别版本信息
        return false;

    this->Initialize(nh.m_nInputNum, nh.m_nOutputNum, nh.m_nHNeuronNum,
                     nh.m_dLearningRate, nh.m_nMaxEpoch, nh.m_dMinErr, false); //false的含义是只是将值付给对象并不一定产生处置的权值
    this->m_nEposhNum = nh.m_nEposhNum;
    this->m_dErrSum = nh.m_dErrSum;
    this->m_dMeanErr = nh.m_dMeanErr;

    //读入隐含层的信息
    int i, j;
    for (i = 0; i < m_nHNeuronNum; ++i) {
        file.Read(&m_pHiddenL->m_pNeurons[i].m_dError, sizeof(double));
        file.Read(&m_pHiddenL->m_pNeurons[i].m_dResult, sizeof(double));
        for (j = 0; j < m_nInputNum + 1; ++j) {
            file.Read(&m_pHiddenL->m_pNeurons[i].m_pWeights[j],
                      sizeof(WEIGHT_TYPE));
        }
    }
    //读入输出层的信息
    for (i = 0; i < m_nOutputNum; ++i) {
        file.Read(&m_pOutputL->m_pNeurons[i].m_dError, sizeof(double));
        file.Read(&m_pOutputL->m_pNeurons[i].m_dResult, sizeof(double));
        for (j = 0; j < m_nHNeuronNum + 1; ++j) {
            file.Read(&m_pOutputL->m_pNeurons[i].m_pWeights[j],
                      sizeof(WEIGHT_TYPE));
        }
    }
    file.Close();
    return true;
}