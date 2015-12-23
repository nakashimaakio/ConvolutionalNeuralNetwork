//関数の定義
#include "stdafx.h"

//このファイルのみで使用する関数の宣言
int CreateFileName  ( char* name, char* imgname, int TrueNum, int DataNum );
IplImage* ChangeSize( IplImage *img, int X, int Y);
int  NameToNumber   ( char* name);
void NumberToName   ( char* name, int number);

//画像の生成(引数がargcとargv)（指定のサイズで出力）
IplImage* CreateImage(int argc, _TCHAR* argv[]){

	char FileName[50]={0};

	//ファイル名が記述されている場合、そのファイルを読み込む
	if(argc>4) WideCharToMultiByte(CP_ACP,0,argv[4],-1,FileName,sizeof(FileName),NULL,NULL);

	//数字により画像ファイル名を読み込む
	else if(argc>3) CreateFileName(FileName, _ttoi(argv[2]), _ttoi(argv[3]));

	else Error();

	log( FileName,_ttoi(argv[1]) );

	return CreateImage( FileName );
}


//画像の生成(引数がファイル名)（指定のサイズで出力）
IplImage* CreateImage( char* FileName ){

	IplImage *img;

	printf("FileName: %s\n",FileName);
	IplImage *Image  = cvLoadImage( FileName, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR );
	if(Image==NULL){
		Error("File is not found.");
	}

	//画像サイズの変更
	if(Xsize == Image->width && Ysize == Image->height) img=cvCloneImage(Image);
	else img=ChangeSize(Image, Xsize, Ysize);
	
	cvReleaseImage(&Image);

	Channel=img->nChannels;

	return img;
}


//ファイル名の作成
void CreateFileName( char* name, int TrueNum, int DataNum ){

	//ファイル名の生成、生成されたファイル名が存在するかの確認
	if(	!CreateFileName( name, ImageName, TrueNum, DataNum )){

		//別のファイル名で、ファイルが存在するかの確認
		if(	!CreateFileName( name, ImageName2, TrueNum, DataNum )) Error("File is not found.");

	}

}


//ファイル名の生成（サブ）name:生成されたファイル名を返す (返り値：ファイル有=1,ファイル無=0)
int CreateFileName( char* name, char* imgname, int TrueNum, int DataNum ){
	char Name[70], cNum[10];
	int FileExistence=0;

	NumberToName(name, TrueNum);
	strcpy(Name,imgname);
	if(ImageMode!=0) strcat(Name,"Number");
	strcat(Name,"/");
	strcat(Name,name);
	strcat(Name,"/ (");
	sprintf(cNum, "%d", DataNum+1);
	strcat(Name,cNum);
	if(ImageMode==0) strcat(Name,").jpg");
	else             strcat(Name,").bmp");
	strcpy(name,Name);

	IplImage *Image  = cvLoadImage( name, CV_LOAD_IMAGE_ANYDEPTH | CV_LOAD_IMAGE_ANYCOLOR );
	if(Image!=NULL) FileExistence=1;
	cvReleaseImage(&Image);
	return FileExistence;

}


//画像のサイズを変更
IplImage *ChangeSize(IplImage *img, int X, int Y){
	CvSize size = {X,Y};
	IplImage *Image=cvCreateImage( size, img->depth, img->nChannels);
	CvScalar s={0};

	if(img->nChannels!=3)Error("Error at ChangeSize. (Please unify Xsize and Ysize or change channel to 3.)");

	cv::Mat src_img = cv::cvarrToMat(img);
	cv::Mat dst_img1;
	cv::Mat dst_img(Y, X, src_img.type());
	cv::resize(src_img, dst_img, dst_img.size(), cv::INTER_CUBIC);

	for(int y=0 ; y<Y ; y++){
	for(int x=0 ; x<X ; x++){
		for(int d=0 ; d< Image->nChannels ; d++){
			s.val[d] = dst_img.at<cv::Vec3b>(y, x)[d];
		}
		cvSet2D(Image,y,x,s);
	}
	}
	
	return Image;
}


//重み係数の初期値をランダムに決める(ただしDeltaはすべて0)
void WeightRandom( IplImage *Image ){
	float h_total;
	int fnumber1;
	int X=Xsize, Y=Ysize;
	char Name[30]={0};

	CheckCnn();  //初期条件が適しているか計算

	//fnumber1の計算
	for(int i=0; i<LAYER ; i++){ X/=2; Y/=2; }
	fnumber1 = X*Y*FILTER_NUMBER;

	//正解率データの初期化
	FILE *file;

	//名前の定義
	if(ImageMode==0) strcpy(Name,"AccuracyRateAnimal.csv");
	else             strcpy(Name,"AccuracyRateNumber.csv");

	file = fopen(Name,"w");
	fclose(file);

	puts("Creating Initinal Value...");

	//畳込み層の初期値をランダムに決める    h[層の数][outputの並列処理の数][inputの並列処理の数][フィルタサイズ][フィルタサイズ]
	for(int j=0;j<LAYER        ;j++){
	for(int i=0;i<FILTER_NUMBER;i++){ //output
		h_total=0.0;
		b[j][i]=(float) ((rand()%200-100)/500.0);
		for( int k=0 ; k<FILTER_NUMBER && (k<Image->nChannels || j!=0) ; k++ ){ //input
			for(int py =0; py < FILTER_SIZE; py++){
			for(int px =0; px < FILTER_SIZE; px++){
				h[j][i][k][py][px]  = (float) ( (rand()%200 - 100)/200.0);
				h[j][i][k][py][px] += (float) (  1.0 / ( FILTER_SIZE*FILTER_SIZE * ( (j==0)?Image->nChannels:FILTER_NUMBER ) ) );
				h_total+=h[j][i][k][py][px];
			}
			}
		}
		h[j][i][0][FILTER_SIZE/2][FILTER_SIZE/2]=1-(h_total-h[j][i][0][FILTER_SIZE/2][FILTER_SIZE/2]);
	}
	}


	//Fully Connection1の初期値をランダムに決める  fh[層の数][全ての要素数]
	for(int j=0;j<fNUMBER2 ;j++){
	for(int i=0;i<fnumber1 ;i++){
		fh[j][i]=(float) ((rand()%100-50)/500.0);
	}
	fb[j]=(float) ((rand()%100-50)/500.0);
	}

	//Fully Connection2の初期値をランダムに決める  fh2[層の数][全ての要素数]
	for(int j=0;j<fNUMBER3 ;j++){
	for(int i=0;i<fNUMBER2 ;i++){
		fh2[j][i]=(float) ((rand()%100-50)/500.0);
	}
	fb2[j]=(float) ((rand()%100-50)/500.0);
	}



//Deltaをすべて0にする  D_h
	for(int p=0 ; p<FILTER_SIZE   ; p++){
	for(int q=0 ; q<FILTER_SIZE   ; q++){
	for(int i=0 ; i<FILTER_NUMBER ; i++){
	for(int o=0 ; o<FILTER_NUMBER ; o++){
	for(int l=0 ; l<LAYER         ; l++){
		D_h[l][o][i][q][p]=0;
	}
	}
	}
	}
	}

	//D_b
	for(int o=0 ; o<FILTER_NUMBER ; o++){ for(int l=0 ; l<LAYER ; l++){ D_b[l][o]=0; }}

	//fh
	for(int i=0 ; i<fnumber1 ; i++){ for(int o=0 ; o<fNUMBER2 ; o++){ D_fh[o][i]=0; }}

	//fb
	for(int o=0 ; o<fNUMBER2 ; o++){ D_fb[o]=0; }

	//fh2
	for(int i=0 ; i<fNUMBER2 ; i++){ for(int o=0 ; o<fNUMBER3 ; o++){ D_fh2[o][i]=0; }}

	//fb2
	for(int o=0 ; o<fNUMBER3 ; o++){ D_fb2[o]=0; }
	
	LearningCount=0;

	//重み係数の書き込み
	WriteCoefData (DataName);
	log("Success");
}


//初期条件のチェック
void CheckCnn(){
	int X=Xsize, Y=Ysize;

//全結合層に入る直前のデータ数を算出
	for(int i=0; i<LAYER ; i++){
		X/=2;
		Y/=2;
	}

	if(X*Y*FILTER_NUMBER > fNUMBER1){ Error("Please increase fNUMBER1.\nOr increase LAYER.\n"); }

	if(X*Y*FILTER_NUMBER <= 0){ Error("The size of Input Image is too small.\n"); }

	//並列処理は3以上にしてください
	if(FILTER_NUMBER < 3){ Error("Please set FILTER_NUMBER 3 or over.\n"); }

}


//画像の表示 (IPL_DEPTH_32F 用)
void ShowImage(char* name, IplImage *img){
	int X = img->width, Y = img->height;
	IplImage *Image = cvCreateImage( cvGetSize(img), 8, 1);
	double min=0, max=0;
	uchar p;
	CvScalar s;

//最大値、最小値を見つける
	for(int y=0 ; y<Y ; y++){
	for(int x=0 ; x<X ; x++){
		s = cvGet2D(img,y,x);
		if( min > s.val[0] || (x==0 && y==0) ) min = s.val[0];
		if( max < s.val[0] || (x==0 && y==0) ) max = s.val[0];
	}
	}


//画素の埋め込み
	for(int y=0 ; y<Y ; y++){
	for(int x=0 ; x<X ; x++){
		s = cvGet2D(img,y,x);
		p = (uchar) ( ( s.val[0] - min ) / (max-min) * 255 ); //[min,max]を[0,255]の範囲に変換
		Image -> imageData[Image->widthStep* y + x] = p;
	}
	}
	cvShowImage(name, Image);
	cvWaitKey(500);
	cvReleaseImage(&Image);
}


//画素値の表示
void ShowImageData( IplImage *img ){
	int X = img->width, Y = img->height;
	CvScalar s;
	char name[10];
	int    max_num[RecogNum];        //大きい順に並べたポインタ
	double max_val[RecogNum], temp;  //大きい順に並べたポインタ
	int    max, tmp;

	if(X<RecogNum)Error("RecogNum is too large.");

//値のコピー
	for(int i=0 ; i<RecogNum ; i++){
		s = cvGet2D(img,0,i);
		max_val[i]=s.val[0];
		max_num[i]=i;
	}


//値の大きい順に並べる
	for(int j=0 ; j<X ; j++){
		max=j;
		for(int i=j ; i<X ; i++){ if( max_val[max]<max_val[i] ) max=i; }
		temp=max_val[max];
		max_val[max]=max_val[j];
		max_val[j]=temp;
		tmp=max_num[max];
		max_num[max]=max_num[j];
		max_num[j]=tmp;
	}

	//データの表示
	cout << "--------------" << endl;
	for(int i=0 ; i<RecogNum ; i++){ NumberToName(name,max_num[i]); printf("%s:%f\n",name,max_val[i]); }
	cout << "--------------" << endl;

}


//認識された物体の名前→数字
int NameToNumber(char* name){
	if     (strcmp(name,"cat"   )==0) return 0;
	else if(strcmp(name,"dog"   )==0) return 1;
	else if(strcmp(name,"bird"  )==0) return 2;
	else if(strcmp(name,"horse" )==0) return 3;
	else if(strcmp(name,"zebra" )==0) return 4;
	else if(strcmp(name,"monkey")==0) return 5;
	else if(strcmp(name,"mouse" )==0) return 6;
	else if(strcmp(name,"fish"  )==0) return 7;
	else if(strcmp(name,"lion"  )==0) return 8;
	else if(strcmp(name,"tiger" )==0) return 9;
	else Error("This number doesn't exist.");
	return -1;
}


//数字→認識された物体の名前
void NumberToName(char* name, int number){

	if(ImageMode==0){
		if     ( number==0 ) strcpy(name,"cat"   );
		else if( number==1 ) strcpy(name,"dog"   );
		else if( number==2 ) strcpy(name,"bird"  );
		else if( number==3 ) strcpy(name,"horse" );
		else if( number==4 ) strcpy(name,"zebra" );
		else if( number==5 ) strcpy(name,"monkey");
		else if( number==6 ) strcpy(name,"mouse" );
		else if( number==7 ) strcpy(name,"fish"  );
		else if( number==8 ) strcpy(name,"lion"  );
		else if( number==9 ) strcpy(name,"tiger" );
		else Error("This name doesn't exist.");
	}
	else{
		if     ( number==0 ) strcpy(name,"0");
		else if( number==1 ) strcpy(name,"1");
		else if( number==2 ) strcpy(name,"2");
		else if( number==3 ) strcpy(name,"3");
		else if( number==4 ) strcpy(name,"4");
		else if( number==5 ) strcpy(name,"5");
		else if( number==6 ) strcpy(name,"6");
		else if( number==7 ) strcpy(name,"7");
		else if( number==8 ) strcpy(name,"8");
		else if( number==9 ) strcpy(name,"9");
	}
}


//エラー時に呼び出す関数
void Error(char* name){
	char Name[50]={0};

	printf("%s\n",name);

	strcpy(Name,"Failure:");
	strcat(Name,name);
	log(Name);
	Sleep(5000);
	exit(1);
}


//引数エラーの場合、その説明
void Error(){
	puts("Argument Error:  <Mode(0:WeightCoeff  1:CreateBatFile  2:Recognition  3:Learning)> <TrueNumber> <FileNumber> (<FileName>)");
	Sleep(3000);
	Error("Argument Error");
}

//Max関数
double Max(double a, double b){
	if(a<b) return b;
	return a;
}
