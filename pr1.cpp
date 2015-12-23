#include "stdafx.h"

//�֐��̐錾
void Recognition( IplImage* Image );
void Learning   ( IplImage* Image, int TrueNumber );
void Learning   ( int DataNumber );
void Manual();

//<���[�h> <�����f�[�^�i���o�[�i���t�M���j> <�摜�t�@�C���i���o�[> <�摜�t�@�C����>
int _tmain(int argc, _TCHAR* argv[]){
	
	int   Mode=-1;

	//���[�h�̓ǂݍ���(  0:bat�t�@�C���쐬  1:�d�݌W�������_���쐬  2:����  3:�w�K(�P��) 4:�w�K(����) )
	if(argc>1) Mode=_ttoi(argv[1]);


	//bat�t�@�C�����쐬
	if     ( Mode==0 ) CreateBat();

	//�d�݌W���������_������
	else if( Mode==1 ) WeightRandom( CreateImage( argc, argv ) );

	//����
	else if( Mode==2 ) Recognition ( CreateImage( argc, argv ) );

	//�w�K
	else if( Mode==3 ) Learning    ( CreateImage( argc, argv ), _ttoi(argv[2]) );

	//�w�K(����)
	else if( Mode==4 && argc>3 ) Learning( _ttoi(argv[3]));

	//�������Ȃ��ꍇ
	else if( Mode==-1 ) Manual();

	//�G���[
	else Error();

	return 0;

}


//���ʃt�F�[�Y
void Recognition( IplImage *Image ){

	//�d�݌W���̓ǂݍ���
	ReadCoefData(DataName);

	puts("Recognition...\n");
//	start = clock();    //���Ԍv�����ɕK�v
	IplImage *img = CNN(Image);
	Release();
	ShowImageData  (img);
	cvReleaseImage(&img);

//�d�݌W���̏�������
	WriteCoefData(DataName);


	log("Success");
	getchar();
}


//�w�K�t�F�[�Y
void Learning( IplImage *Image, int TrueNumber ){

	//�d�݌W���̓ǂݍ���
	ReadCoefData(DataName);

	//�w�K
	puts("Learning...");
	LearningImage(Image, TrueNumber);

	//�d�݌W���̏�������
	WriteCoefData (DataName);

	log("Success");
}


//�w�K�t�F�[�Y�i�����j
void Learning( int DataNumber ){

	char FileName[50]={0};

	//�d�݌W���̓ǂݍ���(�`���l�����𒲂ׂ邽�߁A�摜�f�[�^��ǂݍ���)
	CreateFileName( FileName, 0, 0 );
	IplImage *Image = CreateImage( FileName );
	ReadCoefData  (DataName);

	for( int DataNum=DataNumber ; DataNum<DataNumber+BATCHSIZE ; DataNum++ ){
	for( int TrueNum=0          ; TrueNum<RecogNum             ; TrueNum++ ){

		cvReleaseImage(&Image);

		//���͉摜�̓ǂݍ���
		CreateFileName( FileName, TrueNum, DataNum );
		Image = CreateImage( FileName );
		log( FileName, 4 );

		//�w�K
		puts("Learning(Multi)...");
		LearningImage(Image, TrueNum);
		log("Success");

	}
	}

	//�d�݌W���̏�������
	WriteCoefData (DataName);
	cvReleaseImage(&Image);

	log("WriteCoefData:Success");
}


//�}�j���A�����[�h
void Manual(){

	int Mode;
	char FileName[50];

	//�摜�t�@�C�����̓���
	printf("File Name:");
	scanf("%s",FileName);

	//�摜�̓ǂݍ���
	IplImage* Image = CreateImage( FileName );

	//���[�h�̓���
	puts("Mode=  1:Initialization  2:Recognition  3:Learning");
	while(1){
		printf("Select Mode:");
		scanf("%d",&Mode);

		//������
		if     ( Mode==1 ){ WeightRandom(Image); break; }

		//����
		else if( Mode==2 ){ Recognition (Image); break; }

		//�w�K
		else if( Mode==3 ){ Learning (Image, TrueNumber()); break; }
	
		else puts("Type \"1\", \"2\", or \"3\"");
	}

}
