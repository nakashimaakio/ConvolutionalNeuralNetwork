// stdafx.cpp : �W���C���N���[�h pr1.pch �݂̂�
// �܂ރ\�[�X �t�@�C���́A�v���R���p�C���ς݃w�b�_�[�ɂȂ�܂��B
// stdafx.obj �ɂ̓v���R���p�C���ς݌^��񂪊܂܂�܂��B

#include "stdafx.h"

//�d�݌W���̒�`
float h[LAYER][FILTER_NUMBER][FILTER_NUMBER][FILTER_SIZE][FILTER_SIZE];
float b[LAYER][FILTER_NUMBER];
float fh [fNUMBER2][fNUMBER1], fh2[fNUMBER3][fNUMBER2]; //�S�����w(Fully Connection)1,2�ł̏d�݌W��
float fb [fNUMBER2], fb2[fNUMBER3];                     //�S�����w�ł̐ؕ�

//Delta�̒�`(�m���I���z�~���@�̏d�ݍX�V�Ɏg�p)
float D_h[LAYER][FILTER_NUMBER][FILTER_NUMBER][FILTER_SIZE][FILTER_SIZE];
float D_b[LAYER][FILTER_NUMBER];
float D_fh [fNUMBER2][fNUMBER1], D_fh2[fNUMBER3][fNUMBER2]; //�S�����w(Fully Connection)1,2�ł̏d�݌W��
float D_fb [fNUMBER2],           D_fb2[fNUMBER3];           //�S�����w�ł̐ؕ�

//�摜�̒�`
IplImage*    ConvImage  [LAYER][FILTER_NUMBER];
IplImage*    PoolImage  [LAYER][FILTER_NUMBER];
IplImage* SubNormImage  [LAYER][FILTER_NUMBER];
IplImage* DivNormImage  [LAYER+1][FILTER_NUMBER];
IplImage*      FcImage  [3];

//�摜�̒�`(�w�K�p��Delta)
IplImage*      FcImageD;
IplImage* DivNormImageD [LAYER+1][FILTER_NUMBER];
IplImage* SubNormImageD [LAYER][FILTER_NUMBER];
IplImage*    PoolImageD [LAYER][FILTER_NUMBER];
IplImage*    ConvImageD [LAYER][FILTER_NUMBER];

//���ϊ����x�̒�`
IplImage*      FcImageR;
IplImage* DivNormImageR [LAYER+1][FILTER_NUMBER];
IplImage* SubNormImageR [LAYER][FILTER_NUMBER];
IplImage*    PoolImageR [LAYER][FILTER_NUMBER];
IplImage*    ConvImageR [LAYER][FILTER_NUMBER];

//delta�̒�`(�d�ݍX�V���Ɏg�p)
float deltaF [fNUMBER2], deltaF2[fNUMBER3]; //�S�����w(Fully Connection)1,2�ł̏d�݌W��

int LearningCount;
int Channel;

// TODO: ���̃t�@�C���ł͂Ȃ��ASTDAFX.H �ŕK�v��
// �ǉ��w�b�_�[���Q�Ƃ��Ă��������B


