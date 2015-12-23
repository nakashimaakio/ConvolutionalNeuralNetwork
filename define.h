//�������`���邽�߂̃w�b�_
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
    //Debug���[�h�̏ꍇ
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_core220d.lib") 
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_imgproc220d.lib")
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_highgui220d.lib") 
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_objdetect220d.lib")
    //�ȉ��A�K�v�ɉ����Ēǉ�
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_ml220d.lib") 
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_features2d220d.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_video220d.lib") 
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_calib3d220d.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_flann220d.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_contrib220d.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_legacy220d.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_gpu220d.lib") 
#else
    //Release���[�h�̏ꍇ
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_core220.lib")  
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_imgproc220.lib")
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_highgui220.lib")
    #pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_objdetect220.lib") 
    //�ȉ��A�K�v�ɉ����Ēǉ�
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_ml220.lib") 
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_features2d220.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_video220.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_calib3d220.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_flann220.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_contrib220.lib") 
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_legacy220.lib")
    //#pragma comment(lib,"C:\\OpenCV2.2\\lib\\opencv_gpu220.lib")
#endif



//�Ώۃf�[�^(���� or ����)
#define ImageMode  1    //0:animal, 1:number

//�w�K���Ɏg�����l�̒�`
#define ALPHA     100 //�w�K��    (/unit)
#define MU       9000 //�������^��(/unit)
#define LAMBDA     10 //�d�݌���  (/unit)
#define REDUCTION  10 //STEPSIZE�����Ƃ�1/(REDUCTION/UNIT)����
#define STEPSIZE 1000 //��L�Q��
#define UNIT    10000 //�w�K���̒P��

//��̓I�Ȑ��l�̒�`
#define LAYER          2 //�w�̐�
#define FILTER_NUMBER 10 //���񏈗��̐�(3�ȏ�)
#define FILTER_SIZE    7 //��ݍ��ݑw�Ŏg���t�B���^�T�C�Y
#define W_FILTER_SIZE  3 //�Ǐ��R���g���X�g���K���Ŏg�p����w�̃t�B���^�T�C�Y
#define fNUMBER1   30000 //Fully Connection1�ɓ���input�̐��̏��
#define fNUMBER2     500 //Fully Connection1����o��output�̐�(Fully Connection2�ɓ���input�̐�)
#define fNUMBER3      10 //Fully Connection2����o��output�̐�(Fully Connection3�ɓ���input�̐�)
#define RecogNum      10 //���ʐ�(RecogNum<fNUMBER3)
#define Xsize         28 //X:�摜�̃T�C�Y
#define Ysize         28 //Y:�摜�̃T�C�Y
#define BATCHSIZE      2 //�ꊇ�ŏ������鐔(1�ȏ�)(MultiLearning�Ŏg�p)(*RecogNum)

//�t�@�C�����̒�`
#define ImageName  "C:/Users/XXX1/Desktop/Dataset" //�摜�̃t�@�C����
#define ImageName2 "C:/Users/XXX2/Desktop/Dataset" //�摜�̃t�@�C����(ImageName�̃t�@�C����������Ȃ������ꍇ)
#define DataName   "DataNumber.txt"                //�d�݌W���Ɋւ���t�@�C���̖��O

//�d�݌W���̒�`
//h[�w�̐�][output][input][�t�B���^�T�C�Y][�t�B���^�T�C�Y]
extern float h[LAYER][FILTER_NUMBER][FILTER_NUMBER][FILTER_SIZE][FILTER_SIZE];
extern float b[LAYER][FILTER_NUMBER];
extern float fh[fNUMBER2][fNUMBER1], fh2[fNUMBER3][fNUMBER2]; //�S�����w(Fully Connection)1,2�ł̏d�݌W��
extern float fb[fNUMBER2], fb2[fNUMBER3];                     //�S�����w�ł̐ؕ�

//Delta�̒�`(�m���I���z�~���@�̏d�ݍX�V�Ɏg�p)
extern float D_h[LAYER][FILTER_NUMBER][FILTER_NUMBER][FILTER_SIZE][FILTER_SIZE];
extern float D_b[LAYER][FILTER_NUMBER];
extern float D_fh [fNUMBER2][fNUMBER1], D_fh2[fNUMBER3][fNUMBER2]; //�S�����w(Fully Connection)1,2�ł̏d�݌W��
extern float D_fb [fNUMBER2],           D_fb2[fNUMBER3];           //�S�����w�ł̐ؕ�

//�摜�̒�`
extern IplImage*   ConvImage [LAYER][FILTER_NUMBER];
extern IplImage*   PoolImage [LAYER][FILTER_NUMBER];
extern IplImage*DivNormImage [LAYER+1][FILTER_NUMBER];
extern IplImage*SubNormImage [LAYER][FILTER_NUMBER];
extern IplImage*     FcImage [3];

//�摜�̒�`(�w�K�p��Delta)
extern IplImage*      FcImageD;
extern IplImage* DivNormImageD [LAYER+1][FILTER_NUMBER];
extern IplImage* SubNormImageD [LAYER][FILTER_NUMBER];
extern IplImage*    PoolImageD [LAYER][FILTER_NUMBER];
extern IplImage*    ConvImageD [LAYER][FILTER_NUMBER];

//���ϊ����x�ς̒�`
extern IplImage*      FcImageR;
extern IplImage* DivNormImageR [LAYER+1][FILTER_NUMBER];
extern IplImage* SubNormImageR [LAYER][FILTER_NUMBER];
extern IplImage*    PoolImageR [LAYER][FILTER_NUMBER];
extern IplImage*    ConvImageR [LAYER][FILTER_NUMBER];

//delta�̒�`(�d�ݍX�V���Ɏg�p)
extern float deltaF [fNUMBER2], deltaF2[fNUMBER3]; //�S�����w(Fully Connection)1,2�ł̏d�݌W��

//�w�K��
extern int LearningCount;

//�`���l��
extern int Channel;
