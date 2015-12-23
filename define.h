//文字を定義するためのヘッダ
#pragma once

#include <stdio.h>
#include <tchar.h>
#include <iostream>
//#define _USE_MATH_DEFINES
#include <math.h>
#include <string.h>
#include <time.h>
#include <windows.h>
#include "opencv2\\opencv.hpp"

using namespace std;

#ifdef _DEBUG
    //Debugモードの場合
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_core220d.lib") 
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_imgproc220d.lib")
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_highgui220d.lib") 
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_objdetect220d.lib")
    //以下、必要に応じて追加
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_ml220d.lib") 
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_features2d220d.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_video220d.lib") 
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_calib3d220d.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_flann220d.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_contrib220d.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_legacy220d.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_gpu220d.lib") 
#else
    //Releaseモードの場合
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_core220.lib")  
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_imgproc220.lib")
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_highgui220.lib")
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_objdetect220.lib") 
    //以下、必要に応じて追加
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_ml220.lib") 
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_features2d220.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_video220.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_calib3d220.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_flann220.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_contrib220.lib") 
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_legacy220.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_gpu220.lib")
#endif



//対象データ(動物 or 数字)
#define ImageMode  1    //0:animal, 1:number

//学習時に使う数値の定義
#define ALPHA     100 //学習率    (/unit)
#define MU       9000 //モメンタム(/unit)
#define LAMBDA     10 //重み減衰  (/unit)
#define REDUCTION  10 //STEPSIZE毎ごとに1/(REDUCTION/UNIT)減少
#define STEPSIZE 1000 //上記参照
#define UNIT    10000 //学習率の単位

//具体的な数値の定義
#define LAYER          2 //層の数
#define FILTER_NUMBER 10 //並列処理の数(3以上)
#define FILTER_SIZE    7 //畳み込み層で使うフィルタサイズ
#define W_FILTER_SIZE  3 //局所コントラスト正規化で使用するwのフィルタサイズ
#define fNUMBER1   30000 //Fully Connection1に入るinputの数の上限
#define fNUMBER2     500 //Fully Connection1から出るoutputの数(Fully Connection2に入るinputの数)
#define fNUMBER3      10 //Fully Connection2から出るoutputの数(Fully Connection3に入るinputの数)
#define RecogNum      10 //判別数(RecogNum<fNUMBER3)
#define Xsize         28 //X:画像のサイズ
#define Ysize         28 //Y:画像のサイズ
#define BATCHSIZE      2 //一括で処理する数(1以上)(MultiLearningで使用)(*RecogNum)

//ファイル名の定義
#define ImageName  "C:/Users/XXX1/Desktop/Dataset" //画像のファイル名
#define ImageName2 "C:/Users/XXX2/Desktop/Dataset" //画像のファイル名(ImageNameのファイルが見つからなかった場合)
#define DataName   "DataNumber.txt"                //重み係数に関するファイルの名前

//重み係数の定義
//h[層の数][output][input][フィルタサイズ][フィルタサイズ]
extern float h[LAYER][FILTER_NUMBER][FILTER_NUMBER][FILTER_SIZE][FILTER_SIZE];
extern float b[LAYER][FILTER_NUMBER];
extern float fh[fNUMBER2][fNUMBER1], fh2[fNUMBER3][fNUMBER2]; //全結合層(Fully Connection)1,2での重み係数
extern float fb[fNUMBER2], fb2[fNUMBER3];                     //全結合層での切片

//Deltaの定義(確率的勾配降下法の重み更新に使用)
extern float D_h[LAYER][FILTER_NUMBER][FILTER_NUMBER][FILTER_SIZE][FILTER_SIZE];
extern float D_b[LAYER][FILTER_NUMBER];
extern float D_fh [fNUMBER2][fNUMBER1], D_fh2[fNUMBER3][fNUMBER2]; //全結合層(Fully Connection)1,2での重み係数
extern float D_fb [fNUMBER2],           D_fb2[fNUMBER3];           //全結合層での切片

//画像の定義
extern IplImage*   ConvImage [LAYER][FILTER_NUMBER];
extern IplImage*   PoolImage [LAYER][FILTER_NUMBER];
extern IplImage*DivNormImage [LAYER+1][FILTER_NUMBER];
extern IplImage*SubNormImage [LAYER][FILTER_NUMBER];
extern IplImage*     FcImage [3];

//画像の定義(学習用のDelta)
extern IplImage*      FcImageD;
extern IplImage* DivNormImageD [LAYER+1][FILTER_NUMBER];
extern IplImage* SubNormImageD [LAYER][FILTER_NUMBER];
extern IplImage*    PoolImageD [LAYER][FILTER_NUMBER];
extern IplImage*    ConvImageD [LAYER][FILTER_NUMBER];

//平均活性度ρの定義
extern IplImage*      FcImageR;
extern IplImage* DivNormImageR [LAYER+1][FILTER_NUMBER];
extern IplImage* SubNormImageR [LAYER][FILTER_NUMBER];
extern IplImage*    PoolImageR [LAYER][FILTER_NUMBER];
extern IplImage*    ConvImageR [LAYER][FILTER_NUMBER];

//deltaの定義(重み更新時に使用)
extern float deltaF [fNUMBER2], deltaF2[fNUMBER3]; //全結合層(Fully Connection)1,2での重み係数

//学習回数
extern int LearningCount;

//チャネル
extern int Channel;
