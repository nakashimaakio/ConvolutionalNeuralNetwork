//関数宣言用のヘッダ
#pragma once

#include "targetver.h"
#include "define.h"


//CNNで使用する関数(cnn.cpp)
IplImage* CNN(IplImage *Image);
void CheckCnn();
void AveAct  ();


//ファイル編集に関する関数(EditFile.cpp)
void CreateBat    ();
void WriteCoefData( char *name );
void ReadCoefData ( char *name );
void log          ( char* name , int mode=-1 );


//学習時に使用する関数(learning.cpp)
void LearningImage( IplImage *Image, int TrueNumber );
int TrueNumber();

//その他の関数の宣言(function.cpp)
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


