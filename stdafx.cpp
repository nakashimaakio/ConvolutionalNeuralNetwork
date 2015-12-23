// stdafx.cpp : 標準インクルード pr1.pch のみを
// 含むソース ファイルは、プリコンパイル済みヘッダーになります。
// stdafx.obj にはプリコンパイル済み型情報が含まれます。

#include "stdafx.h"

//重み係数の定義
float h[LAYER][FILTER_NUMBER][FILTER_NUMBER][FILTER_SIZE][FILTER_SIZE];
float b[LAYER][FILTER_NUMBER];
float fh [fNUMBER2][fNUMBER1], fh2[fNUMBER3][fNUMBER2]; //全結合層(Fully Connection)1,2での重み係数
float fb [fNUMBER2], fb2[fNUMBER3];                     //全結合層での切片

//Deltaの定義(確率的勾配降下法の重み更新に使用)
float D_h[LAYER][FILTER_NUMBER][FILTER_NUMBER][FILTER_SIZE][FILTER_SIZE];
float D_b[LAYER][FILTER_NUMBER];
float D_fh [fNUMBER2][fNUMBER1], D_fh2[fNUMBER3][fNUMBER2]; //全結合層(Fully Connection)1,2での重み係数
float D_fb [fNUMBER2],           D_fb2[fNUMBER3];           //全結合層での切片

//画像の定義
IplImage*    ConvImage  [LAYER][FILTER_NUMBER];
IplImage*    PoolImage  [LAYER][FILTER_NUMBER];
IplImage* SubNormImage  [LAYER][FILTER_NUMBER];
IplImage* DivNormImage  [LAYER+1][FILTER_NUMBER];
IplImage*      FcImage  [3];

//画像の定義(学習用のDelta)
IplImage*      FcImageD;
IplImage* DivNormImageD [LAYER+1][FILTER_NUMBER];
IplImage* SubNormImageD [LAYER][FILTER_NUMBER];
IplImage*    PoolImageD [LAYER][FILTER_NUMBER];
IplImage*    ConvImageD [LAYER][FILTER_NUMBER];

//平均活性度の定義
IplImage*      FcImageR;
IplImage* DivNormImageR [LAYER+1][FILTER_NUMBER];
IplImage* SubNormImageR [LAYER][FILTER_NUMBER];
IplImage*    PoolImageR [LAYER][FILTER_NUMBER];
IplImage*    ConvImageR [LAYER][FILTER_NUMBER];

//deltaの定義(重み更新時に使用)
float deltaF [fNUMBER2], deltaF2[fNUMBER3]; //全結合層(Fully Connection)1,2での重み係数

int LearningCount;
int Channel;

// TODO: このファイルではなく、STDAFX.H で必要な
// 追加ヘッダーを参照してください。


