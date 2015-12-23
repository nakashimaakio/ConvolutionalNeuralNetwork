#include "stdafx.h"

//関数の宣言
void Recognition( IplImage* Image );
void Learning   ( IplImage* Image, int TrueNumber );
void Learning   ( int DataNumber );
void Manual();

//<モード> <正解データナンバー（教師信号）> <画像ファイルナンバー> <画像ファイル名>
int _tmain(int argc, _TCHAR* argv[]){
	
	int   Mode=-1;

	//モードの読み込み(  0:batファイル作成  1:重み係数ランダム作成  2:識別  3:学習(単数) 4:学習(複数) )
	if(argc>1) Mode=_ttoi(argv[1]);


	//batファイルを作成
	if     ( Mode==0 ) CreateBat();

	//重み係数をランダム生成
	else if( Mode==1 ) WeightRandom( CreateImage( argc, argv ) );

	//識別
	else if( Mode==2 ) Recognition ( CreateImage( argc, argv ) );

	//学習
	else if( Mode==3 ) Learning    ( CreateImage( argc, argv ), _ttoi(argv[2]) );

	//学習(複数)
	else if( Mode==4 && argc>3 ) Learning( _ttoi(argv[3]));

	//引数がない場合
	else if( Mode==-1 ) Manual();

	//エラー
	else Error();

	return 0;

}


//識別フェーズ
void Recognition( IplImage *Image ){

	//重み係数の読み込み
	ReadCoefData(DataName);

	puts("Recognition...\n");
//	start = clock();    //時間計測時に必要
	IplImage *img = CNN(Image);
	Release();
	ShowImageData  (img);
	cvReleaseImage(&img);

//重み係数の書き込み
	WriteCoefData(DataName);


	log("Success");
	getchar();
}


//学習フェーズ
void Learning( IplImage *Image, int TrueNumber ){

	//重み係数の読み込み
	ReadCoefData(DataName);

	//学習
	puts("Learning...");
	LearningImage(Image, TrueNumber);

	//重み係数の書き込み
	WriteCoefData (DataName);

	log("Success");
}


//学習フェーズ（複数）
void Learning( int DataNumber ){

	char FileName[50]={0};

	//重み係数の読み込み(チャネル数を調べるため、画像データを読み込む)
	CreateFileName( FileName, 0, 0 );
	IplImage *Image = CreateImage( FileName );
	ReadCoefData  (DataName);

	for( int DataNum=DataNumber ; DataNum<DataNumber+BATCHSIZE ; DataNum++ ){
	for( int TrueNum=0          ; TrueNum<RecogNum             ; TrueNum++ ){

		cvReleaseImage(&Image);

		//入力画像の読み込み
		CreateFileName( FileName, TrueNum, DataNum );
		Image = CreateImage( FileName );
		log( FileName, 4 );

		//学習
		puts("Learning(Multi)...");
		LearningImage(Image, TrueNum);
		log("Success");

	}
	}

	//重み係数の書き込み
	WriteCoefData (DataName);
	cvReleaseImage(&Image);

	log("WriteCoefData:Success");
}


//マニュアルモード
void Manual(){

	int Mode;
	char FileName[50];

	//画像ファイル名の入力
	printf("File Name:");
	scanf("%s",FileName);

	//画像の読み込み
	IplImage* Image = CreateImage( FileName );

	//モードの入力
	puts("Mode=  1:Initialization  2:Recognition  3:Learning");
	while(1){
		printf("Select Mode:");
		scanf("%d",&Mode);

		//初期化
		if     ( Mode==1 ){ WeightRandom(Image); break; }

		//識別
		else if( Mode==2 ){ Recognition (Image); break; }

		//学習
		else if( Mode==3 ){ Learning (Image, TrueNumber()); break; }
	
		else puts("Type \"1\", \"2\", or \"3\"");
	}

}
