//�֐��錾�p�̃w�b�_
#pragma once

#include "targetver.h"
#include "define.h"


//CNN�Ŏg�p����֐�(cnn.cpp)
IplImage* CNN(IplImage *Image);
void CheckCnn();
void AveAct  ();


//�t�@�C���ҏW�Ɋւ���֐�(EditFile.cpp)
void CreateBat    ();
void WriteCoefData( char *name );
void ReadCoefData ( char *name );
void log          ( char* name , int mode=-1 );


//�w�K���Ɏg�p����֐�(learning.cpp)
void LearningImage( IplImage *Image, int TrueNumber );
int TrueNumber();

//���̑��̊֐��̐錾(function.cpp)
IplImage* CreateImage( int argc, _TCHAR* argv[]);
IplImage* CreateImage( char* FileName );
void CreateFileName  ( char* name , int TrueNum, int DataNum);
void WeightRandom    ( IplImage *Image );
void ShowImage       ( char* name, IplImage *img);
void ShowImageData   ( IplImage *img );
void WriteCsv        ( IplImage *img , int TrueNumber );
void Release         ();
void Error           ( char* name );
void Error           ();
double Max           ( double a,double b );


