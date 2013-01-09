// ANNConsoleTest.cpp : Defines the entry point for the console application.
//

#include "ann.h"
#include "GetPicData.h"
#include <iostream.h>
#include <fstream.h>
const unsigned int MAX_BUFF_LENGTH = 200;
void main(int argc, char* argv[])
{

    CNeuralNet net;
//-----------------train part-----------------------
//------------------First Get Train Data-------------

// 	CTrainData data("D:\\Project\\ANNConsoleTest\\TrainData", 10);
// 	std::vector<DVECTOR> input, output;
// 	data.GetTrainData(input, output);

//------------  Initialize NeuralNet------------------

// 	int dataSize = input[0].size();
// 	net.Initialize(dataSize, 10, 4);
// 	if (!net.Train(input, output))
// 	{
// 	 	cout << "Fail Train" << endl;
// 	};
// 	net.SaveToFile("MyData.net");

//----------------------Test-------------------

// 	DVECTOR outTemp;
// 	net.CalculateOutput(input[13], outTemp);
// 	for (int i = 0; i < outTemp.size(); ++i)
// 		cout << outTemp[i] << endl;
// 	int nType; double nConfidence;
// 	net.Recognize(input[13], nType, nConfidence);
// 	cout << nType << "  " << nConfidence << endl;


//----------------after train -----------------------
//----------------Load From File --------------------
    net.LoadFromFile("MyData.net");

    DVECTOR eachInput;
    CPicData testPic;

    char buff[MAX_BUFF_LENGTH] = "";
    GetCurrentDirectory(MAX_BUFF_LENGTH, buff);
    strcat(buff,"\\TestData\\9_car1.bmp");

    if(!testPic.Initialize(buff)) {
        cout<<"Open failure"<<endl;
        return;
    }
    testPic.GetResampleData(eachInput);

    DVECTOR eachOut;
    net.CalculateOutput(eachInput, eachOut);
    for (int i = 0; i < eachOut.size(); ++i)
        cout << eachOut[i] << endl;

    int nType;
    double nConfidence;
    net.Recognize(eachInput, nType, nConfidence);
    cout << nType << "  " << nConfidence << endl;
}
