//学習時に使用する関数
#include "stdafx.h"

//このファイルのみで使用する関数の宣言
void Backpropagation( int channel, int TrueNumber );
void CalcDeltaF2    ( IplImage *Image, int TureNumber);
void CalcDeltaF     ( IplImage *Image );
void CalcDeltaF0    ( IplImage *Image );
void Division       ();
void CalcDivNorm    ( IplImage *   Image[FILTER_NUMBER], IplImage * InImageD[FILTER_NUMBER],  IplImage *OutImageD[FILTER_NUMBER] );
void CalcSubNorm    ( IplImage *InImageD[FILTER_NUMBER], IplImage *OutImageD[FILTER_NUMBER] );
IplImage *DeltaPool ( IplImage *Image, IplImage *NextImageD );
IplImage *DeltaConv ( IplImage *NextImageD[FILTER_NUMBER], float H[][FILTER_SIZE][FILTER_SIZE] );
void UpdateF2       ( IplImage *Image );
void UpdateF        ( IplImage *Image );
void Update         ( IplImage *Image[FILTER_NUMBER], IplImage *ImageD, float H[][FILTER_SIZE][FILTER_SIZE],
	float B, int input_number,float Hd[][FILTER_SIZE][FILTER_SIZE], float Bd );
void Update2        ( IplImage *Image, IplImage *ImageD, float H[][FILTER_SIZE],float Hd[][FILTER_SIZE]);
float UpdateFormula ( float a1, float a2, float a3);
void ReleaseDelta   ();


//学習
void LearningImage( IplImage *Image, int TrueNumber ){

//CNNの実行
	IplImage *img = CNN(Image);

//識別結果の書き込み
	WriteCsv (img,TrueNumber);

//誤差逆伝播
	Backpropagation(Image->nChannels, TrueNumber);

//画像の解放
	Release();
	ReleaseDelta();
	cvReleaseImage(&img);
	LearningCount++;
}


//誤差逆伝播法
void Backpropagation(  int channel , int TrueNumber ){

//Deltaの計算
	//全結合層のDeltaの計算
	CalcDeltaF2( FcImage[2] ,TrueNumber);
	CalcDeltaF ( FcImage[1] );
	CalcDeltaF0( FcImage[0] );

	//FcImageDを分割
	Division();

	printf("Layer:");
	for(int l=LAYER-1 ; l>=0 ; l--){
		printf("%d ",l);
		//局所コントラスト正規化のDeltaの計算
		CalcDivNorm( SubNormImage[l], SubNormImageD[l], DivNormImageD[l+1] );
		CalcSubNorm(   PoolImageD[l], SubNormImageD[l]);
		for(int i=0 ; i<FILTER_NUMBER ; i++){
			//プーリング層のDeltaの計算
			ConvImageD[l][i] = DeltaPool( ConvImage[l][i], PoolImageD[l][i] );
//			ShowImage("Pool",ConvImageD[l][i] );
		}
		for(int i=0 ; i<FILTER_NUMBER ; i++){
			DivNormImageD[l][i] = DeltaConv( ConvImageD[l], h[l][i] );
//			ShowImage("Conv", DivNormImageD[l][i] );
		}

	}
	puts("");
	printf("Update Weight Coefficient...\n");
//重み係数の更新
	UpdateF2( FcImage[1] );
	UpdateF ( FcImage[0] );
	for(int l=0 ; l<LAYER ; l++){
	for(int j=0 ; j<FILTER_NUMBER ; j++){  //j:output
		Update(DivNormImage[l], ConvImageD[l][j], h[l][j], b[l][j], (l==0)?channel:FILTER_NUMBER, D_h[l][j], D_b[l][j] );
	}

	}

}


//deltaF2の計算
void CalcDeltaF2(IplImage *Image, int TrueNumber){
	CvScalar s;

	for( int i=0 ; i<fNUMBER3 ; i++ ){
		s = cvGet2D( Image, 0, i );
		deltaF2[i] = (float)s.val[0] - ( i == TrueNumber );
	}

}


//deltaFの計算
void CalcDeltaF(IplImage *Image){
	CvScalar s;

	for( int i=0 ; i<fNUMBER2 ; i++ ){
		deltaF[i] = 0;
		s = cvGet2D( Image, 0, i );
		for( int k=0 ; k<fNUMBER3 ; k++ ){
			deltaF[i] += deltaF2[k] * fh2[k][i];
		}
		deltaF[i] *= (float) (s.val[0]*(1-s.val[0]));
	}
}


//deltaF0の計算
void CalcDeltaF0(IplImage *Image){
	int X = Image->width, Y = Image->height;
	FcImageD = cvCreateImage(cvGetSize(Image), IPL_DEPTH_32F, 1);
	CvScalar s;
	double output;

	for(int y =0; y < Y; y++ ){
	for(int x =0; x < X; x++ ){
		output = 0;
		s = cvGet2D( Image, y, x );
		for( int k=0 ; k<fNUMBER2 ; k++ ){
			output += deltaF[k] * fh[k][y*X+x];
		}
//		output *= s.val[0]*(1-s.val[0]);
//		cout << "output:" << output << endl;
		s.val[0]=output;
		cvSet2D(FcImageD, y, x, s); 
	}
	}
}


//分割
void Division(){
//	FcImageD;
//	DivNormImageD [LAYER][FILTER_NUMBER];
	int X = FcImageD->width, Y = FcImageD->height;
	int Xdiv = X / FILTER_NUMBER;
	CvScalar s;

	if(X % FILTER_NUMBER != 0){ Error("Error of division process in learning"); }

	for(int i=0 ; i<FILTER_NUMBER ; i++ ){
		DivNormImageD[LAYER][i] = cvCreateImage(cvSize(Xdiv,Y), IPL_DEPTH_32F, 1);
	}

	for(int y =0; y < Y; y++ ){
	for(int x =0; x < X; x++ ){
		s = cvGet2D( FcImageD, y ,x );
		cvSet2D( DivNormImageD[LAYER][x/Xdiv], y, x%Xdiv, s );
	}
	}

}


//除算正規化の微分の計算
void CalcDivNorm( IplImage *Image[FILTER_NUMBER], IplImage *InImageD[FILTER_NUMBER],  IplImage *OutImageD[FILTER_NUMBER] ){

	IplImage *SigmaImage = cvCreateImage( cvGetSize(Image[0]), IPL_DEPTH_32F, 1);
	CvScalar sigma, s, delta;
	int X = Image[0]->width, Y = Image[0]->height;
	double dif, sum;

	//sigmaの計算
	for( int y=0 ; y<Y ; y++ ){
	for( int x=0 ; x<X ; x++ ){
//		sigma = cvGet2D( SigmaImage,  y,  x );
		sigma.val[0] = 0;
		for(int k=0  ;  k<FILTER_NUMBER ; k++ ){
		for(int py=0 ; py<W_FILTER_SIZE ; py++){
		for(int px=0 ; px<W_FILTER_SIZE ; px++){

			if(x+px-W_FILTER_SIZE/2>=0 && y+py-W_FILTER_SIZE/2>=0 && x+px-W_FILTER_SIZE/2<X && y+py-W_FILTER_SIZE/2<Y){
				s = cvGet2D( Image[k],  y+py-W_FILTER_SIZE/2,  x+px-W_FILTER_SIZE/2 );
				sigma.val[0] += s.val[0]*s.val[0] / ( W_FILTER_SIZE*W_FILTER_SIZE*FILTER_NUMBER );//出力画素の計算
			}

		}
		}
		}
		sigma.val[0] = sqrt(sigma.val[0]);
		cvSet2D(SigmaImage, y, x, sigma);
	}
	}


	//微分の計算
	for(int k=0 ; k<FILTER_NUMBER ; k++){
	InImageD[k] = cvCreateImage(cvGetSize(OutImageD[k]), IPL_DEPTH_32F, 1);
	for(int y=0 ; y<Y ; y++){
	for(int x=0 ; x<X ; x++){
		s = cvGet2D( Image[k],  y,  x );
		sum=0;
		for(int q=0 ; q<W_FILTER_SIZE ; q++){
		for(int p=0 ; p<W_FILTER_SIZE ; p++){
		for(int r=0 ; r<FILTER_NUMBER ; r++){
			dif=0;
			if(x+p-W_FILTER_SIZE/2>=0 && y+q-W_FILTER_SIZE/2>=0 && x+p-W_FILTER_SIZE/2<X && y+q-W_FILTER_SIZE/2<Y){
				sigma = cvGet2D( SigmaImage,  y+q-W_FILTER_SIZE/2, x+p-W_FILTER_SIZE/2);
				if(sigma.val[0] > 0.0001){
					dif += (p==W_FILTER_SIZE/2)*(q==W_FILTER_SIZE/2)*(k==r)/sigma.val[0];
					dif -= s.val[0]*s.val[0]/(sigma.val[0]*sigma.val[0]*sigma.val[0]* W_FILTER_SIZE*W_FILTER_SIZE*FILTER_NUMBER );
				}
				else{
					dif += (p==W_FILTER_SIZE/2)*(q==W_FILTER_SIZE/2)*(k==r)/0.0001;
				}
				delta = cvGet2D(OutImageD[k], y+q-W_FILTER_SIZE/2, x+p-W_FILTER_SIZE/2);
				sum += dif*delta.val[0];
			}
		}
		}
		}
		s.val[0]=sum;
		cvSet2D( InImageD[k], y, x, s);
		//cout << "sigma:" << sigma.val[0] << endl;
		//cout << "ATAI: " << sum << endl;
	}
	}
//		ShowImage("Dif",DifImage[k]);
//		cvWaitKey(0);
	}
	cvReleaseImage(&SigmaImage);
}


//減算正規化の微分の計算
void CalcSubNorm( IplImage *InImageD[FILTER_NUMBER],  IplImage *OutImageD[FILTER_NUMBER] ){

	CvScalar s, delta;
	int X = OutImageD[0]->width, Y = OutImageD[0]->height;
	double dif, sum;


	//微分の計算
	for(int k=0 ; k<FILTER_NUMBER ; k++){
	InImageD[k] = cvCreateImage(cvGetSize(OutImageD[k]), IPL_DEPTH_32F, 1);
	for(int y=0 ; y<Y ; y++){
	for(int x=0 ; x<X ; x++){
		sum=0;
		for(int q=0 ; q<W_FILTER_SIZE ; q++){
		for(int p=0 ; p<W_FILTER_SIZE ; p++){
		for(int r=0 ; r<FILTER_NUMBER ; r++){
			dif=0;
			if(x+p-W_FILTER_SIZE/2>=0 && y+q-W_FILTER_SIZE/2>=0 && x+p-W_FILTER_SIZE/2<X && y+q-W_FILTER_SIZE/2<Y){
				dif  += (p==W_FILTER_SIZE/2)*(q==W_FILTER_SIZE/2)*(k==r);
				dif  -= 1.0/(W_FILTER_SIZE*W_FILTER_SIZE*FILTER_NUMBER );
				delta = cvGet2D(OutImageD[k], y+q-W_FILTER_SIZE/2, x+p-W_FILTER_SIZE/2);
				sum  += dif*delta.val[0];
			}
		}
		}
		}
		s=cvGet2D( InImageD[k], y, x);
		s.val[0]=sum;
		cvSet2D( InImageD[k], y, x, s);
//		cout << "ATAI: " << sum << endl;
	}
	}
	}
}


//PoolDeltaの計算
IplImage *DeltaPool( IplImage *Image, IplImage *NextImageD ){

	IplImage *ImageD = cvCreateImage( cvGetSize(Image), IPL_DEPTH_32F , 1);
	int size=2;//プーリング層のサイズ
	int X = Image->width, Y = Image->height;
	int imax, jmax;
	double max;
	CvScalar s;

	for( int y=0 ; y<Y/size; y++ ){
	for( int x=0 ; x<X/size; x++ ){

		for( int i=0 ; i<size ; i++){
		for( int j=0 ; j<size ; j++){
			s = cvGet2D(Image,size*y+i,size*x+j);
			if(i==0 && j==0){
				max = s.val[0];
				imax=0;
				jmax=0;
			}
			else if( s.val[0] > max ){
				imax=i;
				jmax=j;
			}
		}
		}

		for( int i=0 ; i<size ; i++){
		for( int j=0 ; j<size ; j++){
			if(i==imax && j==jmax){
				s = cvGet2D(NextImageD,y,x);
				cvSet2D(ImageD,size*y+i,size*x+j,s);
//				if(s.val[0]>1000)printf("Pool:%f\n",s.val[0]);
			}
			else{
				s.val[0] = 0;
				cvSet2D(ImageD,size*y+i,size*x+j,s);
			}
		}
		}

	}
	}

	s.val[0] = 0;
	if(X%2==1) for( int y=0 ; y<Y; y++ ) cvSet2D(ImageD,y,X-1,s);
	if(Y%2==1) for( int x=0 ; x<X; x++ ) cvSet2D(ImageD,Y-1,x,s);


	//活性化関数の微分の計算
	for( int y=0 ; y<Y; y++ ){
	for( int x=0 ; x<X; x++ ){
		s = cvGet2D(Image,y,x);
		if(s.val[0] < 0){
			s.val[0] = 0;
			cvSet2D(ImageD,y,x,s);
		}
	}
	}

	return ImageD;
}


//ConvDeltaの計算
IplImage *DeltaConv( IplImage *NextImageD[FILTER_NUMBER], float H[][FILTER_SIZE][FILTER_SIZE] ){

	IplImage *ImageD = cvCreateImage( cvGetSize(NextImageD[0]), IPL_DEPTH_32F, 1);
	double output;
	int X = ImageD->width, Y = ImageD->height;
	CvScalar s;

	for(int y=0 ; y<Y ; y++){
	for(int x=0 ; x<X ; x++){
		output=0.0;
		for(int k =0 ; k <FILTER_NUMBER ;  k++){
		for(int py=0 ; py<FILTER_SIZE   ; py++){  // FILTER_SIZE * FILTER_SIZE 近傍
		for(int px=0 ; px<FILTER_SIZE   ; px++){
			if(x+px-FILTER_SIZE/2>=0 && y+py-FILTER_SIZE/2>=0 && x+px-FILTER_SIZE/2<X && y+py-FILTER_SIZE/2<Y){
				s = cvGet2D( NextImageD[k], y+py-FILTER_SIZE/2, x+px-FILTER_SIZE/2);
				s.val[0]=Max(s.val[0],0);
//				if(s.val[0]>1000)printf("Conv:%f\n",s.val[0]);
				output +=  H[k][py][px]*s.val[0]; //出力画素の計算
			}
		}
		}
		}
		s.val[0] = output;
		cvSet2D( ImageD, y, x, s );
	}
	}
	return ImageD;
}


//fh2,fb2の更新
void UpdateF2(IplImage *Image ){

	CvScalar s;
	float w1,w2,w3; //重み係数

	w1=(float)(-ALPHA*1.0/UNIT);
	w2=(float)(MU*1.0/UNIT);
	w3=(float)( -(ALPHA*1.0/UNIT) * (LAMBDA*1.0/UNIT) );

	for( int i=0 ; i<fNUMBER3 ; i++ ){

//		D_fb2[i] = w1 * deltaF2[i] + w2 * D_fb2[i] + w3 * fb2[i];
		D_fb2[i] = UpdateFormula(deltaF2[i], D_fb2[i],fb2[i]);
		fb2[i] += D_fb2[i];

		for( int j=0 ; j<fNUMBER2 ; j++ ){
			s = cvGet2D( Image, 0, j );
//			D_fh2[i][j] = w1 * deltaF2[i] * s.val[0] + w2 * D_fh2[i][j] + w3 * fh2[i][j];
			D_fh2[i][j] = UpdateFormula(deltaF2[i] * (float)s.val[0], D_fh2[i][j],fh2[i][j]);
			fh2[i][j] += (float)(D_fh2[i][j]/10.0);
		}

	}

}


//fh,fbの更新
void UpdateF(IplImage *Image ){
	int X = Image->width, Y = Image->height;
	CvScalar s;
	float w1,w2,w3; //重み係数

	w1=(float)(-ALPHA*1.0/UNIT);
	w2=(float)(MU*1.0/UNIT);
	w3=(float)(-(ALPHA*1.0/UNIT) * (LAMBDA*1.0/UNIT) );


	for( int i=0 ; i<fNUMBER2 ; i++ ){
//		D_fb[i] = w1 * deltaF[i] + w2 * D_fb[i] + w3 * fb[i];
		D_fb[i] = UpdateFormula(deltaF[i],D_fb[i],fb[i]);
		fb[i] += D_fb[i];
		for(int y=0 ; y<Y ; y++){
		for(int x=0 ; x<X ; x++){
			s = cvGet2D( Image, y, x );
//			D_fh[i][y*X+x] = w1*deltaF[i]*s.val[0] + w2*D_fh[i][y*X+x] + w3 * fh[i][y*X+x];
			D_fh[i][y*X+x] = UpdateFormula(deltaF[i]*(float)s.val[0],D_fh[i][y*X+x],fh[i][y*X+x]);
			fh[i][y*X+x]  += D_fh[i][y*X+x];
		}
		}
	}

}


//h,bの更新
void Update(IplImage *Image[FILTER_NUMBER],IplImage *ImageD,float H[][FILTER_SIZE][FILTER_SIZE],
float B,int input_number,float Hd[][FILTER_SIZE][FILTER_SIZE], float Bd ){

	float delta_h[FILTER_SIZE][FILTER_SIZE];
	float delta_b;
	int X = ImageD->width, Y = ImageD->height;
	CvScalar s, delta_s;
	float w1,w2,w3; //重み係数

	w1=(float)(-ALPHA*1.0/UNIT);
	w2=(float)(MU*1.0/UNIT);
	w3=(float)( -(ALPHA*1.0/UNIT) * (LAMBDA*1.0/UNIT) );


	delta_b = 0;

	for(int k=0 ; k<input_number ; k++){

		//delta_hの初期化
		for(int i=0 ; i<FILTER_SIZE ; i++) for(int j=0 ; j<FILTER_SIZE ; j++) delta_h[i][j] = 0;


		for(int y=0 ; y<Y ; y++){
		for(int x=0 ; x<X ; x++){
			s = cvGet2D( Image[k], y, x );
			for(int py=0 ; py<FILTER_SIZE   ; py++){  // FILTER_SIZE * FILTER_SIZE 近傍
			for(int px=0 ; px<FILTER_SIZE   ; px++){
				if(x+px-FILTER_SIZE/2>=0 && y+py-FILTER_SIZE/2>=0 && x+px-FILTER_SIZE/2<X && y+py-FILTER_SIZE/2<Y){
					delta_s = cvGet2D( ImageD, y+py-FILTER_SIZE/2, x+px-FILTER_SIZE/2 );
					delta_h[py][px] +=(float) (delta_s.val[0] * s.val[0]);
					delta_b    += (float)delta_s.val[0];
				}
			}
			}
		}
		}

		//delta_hに代入
		for(int i=0 ; i<FILTER_SIZE ; i++) for(int j=0 ; j<FILTER_SIZE ; j++){
//			Hd[k][i][j] = w1*delta_h[i][j] + w2 * Hd[k][i][j] + w3 * H[k][i][j];
			Hd[k][i][j] = UpdateFormula(delta_h[i][j],Hd[k][i][j],H[k][i][j]);
	H[k][i][j] += Hd[k][i][j];
		}

	}

//	Bd= w1*delta_b + w2*Bd + w3*B;
	Bd = UpdateFormula(delta_b,Bd,B);
	B += Bd;
}


//確率的勾配降下法
float UpdateFormula(float a1, float a2, float a3){
	if(LearningCount<0) Error("Error:LearningCount value");

	float w1 =(float)(-ALPHA*1.0/UNIT / pow((double)REDUCTION, (double)(LearningCount/STEPSIZE)));
	float w2 =(float)(    MU*1.0/UNIT);
	float w3 =(float)( -ALPHA*1.0/UNIT / pow((double)REDUCTION, (double)(LearningCount/STEPSIZE)) * (LAMBDA*1.0/UNIT));

	return w1*a1 + w2*a2 + w3*a3;

}


//学習時に使用するdeltaの領域を解放
void ReleaseDelta(){
	cvReleaseImage( &FcImageD );	

	for( int j=0 ; j<LAYER         ; j++ ){  //LAYER:層の数
	for( int i=0 ; i<FILTER_NUMBER ; i++ ){  //FILTER_NUMBER:並列処理の数
		cvReleaseImage(   &ConvImageD[j][i] );
		cvReleaseImage(   &PoolImageD[j][i] );
		cvReleaseImage(&SubNormImageD[j][i] );
		cvReleaseImage(&DivNormImageD[j][i] );
		if(j==LAYER-1)cvReleaseImage(&DivNormImageD[j+1][i] );
	}
	}
}

//教師信号を入力させる（マニュアルモードの学習で使用）
int TrueNumber(){
	int TrueNum;

	while(1){
		printf("True Number:");
		scanf("%d",&TrueNum);
		if(0<=TrueNum && TrueNum<RecogNum) break;
		else printf("Select 0~%d\n",RecogNum-1);
	}

	return TrueNum;
}