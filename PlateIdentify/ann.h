#ifndef _ANN__H_
#define _ANN__H_

#define NEED_MOMENTUM				//是否需要冲量

#include <afx.h>
#include <cmath>
#include <vector>
#include <fstream>
typedef double WEIGHT_TYPE;
typedef std::vector<double> DVECTOR;

const DWORD MY_NEURALNET_VERSION = 12345;		//设立自己的版本头信息
const double BIAS = 1.0;		//偏置项系数
#ifdef NEED_MOMENTUM
const double MOMENTUM = 0.6;	//冲量系数，仅在增加冲量项(#define NEED_MOMENTUM)的时候起作用
#endif

//神经元单元结构
class CNeuron
{
public:
    int m_nInputNum;			//输入数据的个数
    WEIGHT_TYPE* m_pWeights;	//权值数组w
#ifdef NEED_MOMENTUM
    WEIGHT_TYPE* m_pLastUpdates; //保存上一次的更新数据用来表示冲量
#endif
    double m_dResult;			//最后的激励值
    double m_dError;			//计算的误差值

public:
    CNeuron() : m_pLastUpdates(NULL), m_pWeights(NULL) {
    }	//一定要在默认构造后使用Initialize
    CNeuron(int nInputNum) {
        Initialize(nInputNum);
    }
    void Initialize(int nInputNum) {
        m_nInputNum = nInputNum + 1;		//加1是为偏置项也分配一个空间
        m_pWeights = new WEIGHT_TYPE[m_nInputNum];
        m_pLastUpdates = new WEIGHT_TYPE[m_nInputNum];
        m_dResult = m_dError = 0.0;
    }
    ~CNeuron() {
        if (m_pWeights != NULL) {
            delete[]m_pWeights;
            m_pWeights = NULL;
        }
        if (m_pLastUpdates != NULL) {
            delete[]m_pLastUpdates;
            m_pLastUpdates = NULL;
        }
    }
};

//神经元层的结构
class CNeuronLayer
{
public:
    int m_nNeuronNum;			//神经元的数目
    CNeuron* m_pNeurons;		//保存神经元的动态数组

public:
    CNeuronLayer() : m_pNeurons(NULL) {
        //一定要在默认构造后使用Initialize
    }
    CNeuronLayer(int nNeuronNum, int nInputNum) {
        //nInputNum为每个单元输入的个数
        Initialize(nNeuronNum, nInputNum);
    }
    void Initialize(int nNeuronNum, int nInputNum) {
        m_nNeuronNum = nNeuronNum;
        m_pNeurons = new CNeuron[m_nNeuronNum];
        for (int i = 0; i < m_nNeuronNum; ++i) {
            m_pNeurons[i].Initialize(nInputNum);
        }
    }
    ~CNeuronLayer() {
        if (m_pNeurons != NULL)
            delete[]m_pNeurons;
    }
};

struct NEURALNET_HEADER {				//保存文件的信息头
    DWORD dwVersion;				//版本信息

    int m_nInputNum;			//输入信息的个数
    int m_nOutputNum;			//输出的类别个数
    int m_nHNeuronNum;			//隐藏层的神经元个数

    //训练配置信息
    double m_dLearningRate;		//学习率
    unsigned int m_nMaxEpoch;	//最大迭代时代数
    double m_dMinErr;			//最小误差限

    //中间信息保存
    double m_dErrSum;			//一个时代的累积误差
    double m_dMeanErr;			//一个时代的平均到每次训练和输出的误差
    unsigned int m_nEposhNum;	//计算现在已经Epoch的次数
};

//神经网络类
class CNeuralNet
{
    //数据成员
public:
    //包含一个隐藏层和一个输出层
    CNeuronLayer* m_pHiddenL;
    CNeuronLayer* m_pOutputL;

    //构造需求信息
    int m_nInputNum;			//输入信息的个数
    int m_nOutputNum;			//输出的类别个数
    int m_nHNeuronNum;			//隐藏层的神经元个数

    //训练配置信息
    double m_dLearningRate;		//学习率
    unsigned int m_nMaxEpoch;	//最大迭代时代数
    double m_dMinErr;			//最小误差限

    //中间信息保存
    double m_dErrSum;			//一个时代的累积误差
    double m_dMeanErr;			//一个时代的平均到每次训练和输出的误差
    unsigned int m_nEposhNum;	//计算现在已经Epoch的次数
    bool m_bStop;				//用来控制训练训中途停止

    //构造和析构函数
public:
    CNeuralNet() {
        //一定要在默认构造后使用Initialize
        m_pHiddenL = NULL;
        m_pOutputL = NULL;
    }
    CNeuralNet(int nInputNum, int nOutputNum, int nHNeuronNum) {
        m_pHiddenL = NULL;
        m_pOutputL = NULL;
        Initialize(nInputNum, nOutputNum, nHNeuronNum);
    }
    CNeuralNet(int nInputNum, int nOutputNum, int nHNeuronNum,
               double dLearningRate, unsigned int unMaxEpoch, double dMinErr) {
        m_pHiddenL = NULL;
        m_pOutputL = NULL;
        Initialize(nInputNum, nOutputNum, nHNeuronNum, dLearningRate,
                   unMaxEpoch, dMinErr);
    }
    void Initialize(int nInputNum, int nOutputNum, int nHNeuronNum, bool bCreate = true) {
        //使用默认的配置值
        m_dErrSum = m_dMeanErr = 0.0;
        m_nEposhNum = 0;
        m_nInputNum = nInputNum;
        m_nOutputNum = nOutputNum;
        m_nHNeuronNum = nHNeuronNum;
        m_dLearningRate = 0.1;
        m_nMaxEpoch = 8000;
        m_dMinErr = 0.001;
        m_bStop = false;

        if (m_pOutputL != NULL)						//为建立新的值做好准备
            delete m_pOutputL;
        if (m_pHiddenL != NULL)
            delete m_pHiddenL;
        m_pHiddenL = new CNeuronLayer(m_nHNeuronNum, m_nInputNum);
        m_pOutputL = new CNeuronLayer(m_nOutputNum, m_nHNeuronNum);

        if(bCreate)
            InitializeNetwork();
#ifdef _CONSOLETEST
        int i,j;
        for (i = 0; i < m_nHNeuronNum; ++i) {
            for (j = 0; j < m_nInputNum + 1; ++j) {
                cout<<m_pHiddenL->m_pNeurons[i].m_pWeights[j]<<"  ";
            }
            cout<<endl<<"*****************************************"<<endl;
        }
        cout<<"-----------------------------------------------"<<endl;
        for (i = 0; i < m_nOutputNum; ++i) {
            for (j = 0; j < m_nHNeuronNum + 1; ++j) {
                cout<<m_pOutputL->m_pNeurons[i].m_pWeights[j]<<"  ";
            }
            cout<<endl;
        }
        cout<<"-------------------------------------------------"<<endl;
#endif
    }
    void Initialize(int nInputNum, int nOutputNum, int nHNeuronNum,
                    double dLearningRate, unsigned int unMaxEpoch, double dMinErr, bool bCreate = true) {
        //使用的制定的配置值
        m_dErrSum = m_dMeanErr = 0.0;
        m_nEposhNum = 0;
        m_nInputNum = nInputNum;
        m_nOutputNum = nOutputNum;
        m_nHNeuronNum = nHNeuronNum;
        m_dLearningRate = dLearningRate;
        m_nMaxEpoch = unMaxEpoch;
        m_dMinErr = dMinErr;
        m_bStop = false;

        if (m_pOutputL != NULL)						//为建立新的值做好准备
            delete m_pOutputL;
        if (m_pHiddenL != NULL)
            delete m_pHiddenL;
        m_pHiddenL = new CNeuronLayer(m_nHNeuronNum, m_nInputNum);
        m_pOutputL = new CNeuronLayer(m_nOutputNum, m_nHNeuronNum);

        if(bCreate)
            InitializeNetwork();
    }
    virtual ~CNeuralNet() {
        if (m_pOutputL != NULL) {
            delete m_pOutputL;
            m_pOutputL = NULL;
        }
        if (m_pHiddenL != NULL) {
            delete m_pHiddenL;
            m_pHiddenL = NULL;
        }
    }

    //get and set 方法
public:
    int GetBStop() const {
        return m_bStop;
    }
    void SetBStop(bool stop) {
        m_bStop = stop;
    }
    unsigned int GetMaxEpoch() const {
        return m_nMaxEpoch;
    }
    void SetMaxEpoch(unsigned int nMaxEpoch) {
        m_nMaxEpoch = nMaxEpoch;
    }
    //内部的方法
protected:
    double Sigmoid(double netinput) {			//sigmoid激励函数
        return (1 / (1 + exp(-netinput)));
    }
    void InitializeNetwork();					//为网络的配置初始的值（如初始的权值，隐藏层的冲量值)
    double WeightRand();						//产生初始（-0.05-0.05）的随机权值
    //对外的接口函数
public:
    bool CalculateOutput(const DVECTOR &eachInput, DVECTOR &eachOutput);
    void UpdateWeight(const DVECTOR& eachInput, const DVECTOR &eachOutput);	//用增量梯度下降法更新权值
    bool TrainingEpoch(const std::vector<DVECTOR>& input, const std::vector<DVECTOR> &output);	//训练一个Epoch
    bool Train(const std::vector<DVECTOR>& input, const std::vector<DVECTOR> &output);		//负责训练的函数
    bool SaveToFile(const char *lpszFileName, bool bCreate = true);			//将计算结果保存到文件中
    bool LoadFromFile(const char *lpszFileName);							//将结果读取会到对象中
    BOOL Recognize(const DVECTOR &eachInput, int &nType, double &dConfidence);	//识别类别
};
#endif