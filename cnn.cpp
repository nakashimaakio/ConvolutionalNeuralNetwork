//cnnで使用する関数
#include "stdafx.h"

//このファイルのみで使用する関数の宣言
void AssignInputImage ( IplImage *img , IplImage *InputImg[FILTER_NUMBER] );
IplImage* Convolution ( IplImage *img[FILTER_NUMBER] ,float H[][FILTER_SIZE][FILTER_SIZE], float B, int input_number);
IplImage* Pooling     ( IplImage *img );
void Norm             ( IplImage *img[FILTER_NUMBER] );
void SubNorm          ( IplImage *img[FILTER_NUMBER] );
void DivNorm          ( IplImage *img[FILTER_NUMBER] );
IplImage* Integration ( IplImage *img[FILTER_NUMBER] );
IplImage* FullyConnection1( IplImage *img ,float H[][fNUMBER1], float B[] );
IplImage* FullyConnection2( IplImage *img ,float H[][fNUMBER2], float B[] );
void SoftMax          ( IplImage *img );
void AveAct           ( IplImage *Image, IplImage *ImageR );


//CNN(畳み込みニューラルネットワーク)
IplImage* CNN( IplImage *Image ){
	IplImage* img; //返数用

	puts("CNN...");
	CheckCnn();                          //初期条件が適切か判断
	AssignInputImage( Image , DivNormImage[0] );  //NormImageに初期画像の代入


	printf("Layer:");
//実際に処理を行う
	for( int j=0 ; j<LAYER         ; j++ ){  //LAYER:層の数
		printf("%d ",j);
		for( int i=0 ; i<FILTER_NUMBER ; i++ ){  //FILTER_NUMBER:並列処理の数

		//畳込み層
			ConvImage[j][i] = Convolution( DivNormImage[j], h[j][i], b[j][i], (j==0)?Image->nChannels:FILTER_NUMBER );

		//プーリング層
			PoolImage[j][i]=Pooling(ConvImage[j][i]);

		}


	//PoolImageをSubNormImageにコピー
		for( int i=0 ; i< FILTER_NUMBER ; i++ ){ SubNormImage[j][i] = cvCloneImage( PoolImage[j][i] ); }

	//局所コントラスト正規化
	//減算正規化
		SubNorm( SubNormImage[j] );

	//SubNormをDivNormにコピー
		for( int i=0 ; i< FILTER_NUMBER ; i++ ){ DivNormImage[j+1][i] = cvCloneImage( SubNormImage[j][i] ); }

	//除算正規化
		DivNorm( DivNormImage[j+1] );  //SubNormImageをコピーさせる必要がある

	//画像表示
//		for( int i=0 ; i<FILTER_NUMBER ; i++ ) ShowImage("T",SubNormImage[j][i]);
	}
	puts("");



//一枚の画像に統合
	FcImage[0] = Integration( DivNormImage[LAYER] );
//	ShowImage("Output",FcImage[0]);

	
//全結合層(Fully Connection)    1層目
	FcImage[1] = FullyConnection1(FcImage[0], fh, fb);

//全結合層(Fully Connection)    2層目
	FcImage[2] = FullyConnection2(FcImage[1], fh2, fb2);//活性化関数なし

//ソフトマックス
	SoftMax           ( FcImage[2] );
	img = cvCloneImage( FcImage[2] );
	return img;
}


//InputImageに初期画像の代入
void AssignInputImage(IplImage *img , IplImage *InputImg[FILTER_NUMBER] ){
	
	//3チャンネルの場合、入力画像をRGBに分解
	if(img->nChannels == 3){
		for(int i=0 ; i<3 ; i++) InputImg[i]=cvCreateImage(cvGetSize(img),8,1);
		cvSplit(img,InputImg[0],InputImg[1],InputImg[2],NULL);
	}

	//1チャンネルの場合、InputImage[0]にそのままコピー
	else if(img->nChannels == 1){
		InputImg[0] = cvCloneImage( img );
	}

}


//畳込み層     h:重み係数    b:切片
IplImage* Convolution( IplImage *img[FILTER_NUMBER] ,float H[][FILTER_SIZE][FILTER_SIZE], float B, int input_number ){
	int X = img[0]->width, Y = img[0]->height;
	float output;
	IplImage *Image = cvCreateImage( cvGetSize(img[0]), IPL_DEPTH_32F, 1);
	CvScalar s;

	for(int y=0 ; y<Y ; y++){
	for(int x=0 ; x<X ; x++){
		output=0.0;
		for(int k =0 ; k <input_number ; k++ ){
		for(int py=0 ; py<FILTER_SIZE  ; py++){  // FILTER_SIZE * FILTER_SIZE 近傍
		for(int px=0 ; px<FILTER_SIZE  ; px++){
			if(x+px-FILTER_SIZE/2>=0 && y+py-FILTER_SIZE/2>=0 && x+px-FILTER_SIZE/2<X && y+py-FILTER_SIZE/2<Y){
				s = cvGet2D(img[k],y+py-FILTER_SIZE/2,x+px-FILTER_SIZE/2);
				output += (float)(H[k][py][px]*s.val[0]);//出力画素の計算
			}
		}
		}
		}
		output += B;
		s.val[0] = Max(0, output);  //活性化関数
		cvSet2D(Image,y,x,s);
	}
	}
	return Image;
}


//プーリング層
IplImage* Pooling( IplImage *img ){
	IplImage *Image2 = cvCreateImage( cvSize (img->width / 2, img->height / 2), IPL_DEPTH_32F, 1);
	int X = Image2->width, Y = Image2->height;
	int size=2;//プーリング層のサイズ
	double max;
	CvScalar s;

	for( int y=0 ; y<Y; y++ ){
	for( int x=0 ; x<X; x++ ){
		for( int i=0 ; i<size ; i++){
		for( int j=0 ; j<size ; j++){
			s = cvGet2D(img,size*y+i,size*x+j);
			if(i==0 && j==0){
				max = s.val[0];
			}
			else{
				max = Max(max, s.val[0]);
			}
		}
		}
		s.val[0] = max;
		cvSet2D(Image2,y,x,s);
	}
	}

	return Image2;
}


//重み係数の改善が必要
//局所コントラスト正規化
//減算正規化
void SubNorm( IplImage *img[FILTER_NUMBER] ){
	IplImage *Image[FILTER_NUMBER];
	int X = img[0]->width, Y = img[0]->height;
	double output, sigma;
	CvScalar s;

	for( int k=0 ; k<FILTER_NUMBER ; k++ ) Image[k] = cvCreateImage( cvGetSize(img[0]), IPL_DEPTH_32F, 1);

	for( int y=0 ; y<Y ; y++ ){
	for( int x=0 ; x<X ; x++ ){
		sigma = 0;
		for(int k=0  ;  k<FILTER_NUMBER ; k++ ){
		for(int py=0 ; py<W_FILTER_SIZE ; py++){
		for(int px=0 ; px<W_FILTER_SIZE ; px++){

			if(x+px-W_FILTER_SIZE/2>=0 && y+py-W_FILTER_SIZE/2>=0 && x+px-W_FILTER_SIZE/2<X && y+py-W_FILTER_SIZE/2<Y){
				s = cvGet2D( img[k],  y+py-W_FILTER_SIZE/2,  x+px-W_FILTER_SIZE/2 );
				sigma += s.val[0]/( W_FILTER_SIZE*W_FILTER_SIZE*FILTER_NUMBER );//出力画素の計算
//				if(5<x && 5<y)cout << k << py << px << "sigma:"  << sigma << "  input:" << s.val[0]  << endl;
			}

		}
		}
		}


		for(int k=0  ;  k<FILTER_NUMBER ; k++ ){
			s = cvGet2D(img[k],y,x);
			output = s.val[0] - sigma;
//			cout << "input"  << s.val[0] << endl;
//			cout << "output" << output   << endl;
//			puts("");
			s.val[0] = output;
			cvSet2D(Image[k],y,x,s);
		}
//		if(5<x && 5<y)puts("------------");

	}
	}

	for(int k=0  ;  k<FILTER_NUMBER ; k++ ){
		img[k] = cvCloneImage(Image[k]);
		cvReleaseImage(&Image[k]);
	}
}


//除算正規化
void DivNorm( IplImage *img[FILTER_NUMBER] ){
	IplImage *Image[FILTER_NUMBER];
	int X = img[0]->width, Y = img[0]->height;
	double output, sigma;
	CvScalar s;

	for( int k=0 ; k<FILTER_NUMBER ; k++ ){
		Image[k] = cvCloneImage(img[k]);
	}

	for( int y=0 ; y<Y ; y++ ){
	for( int x=0 ; x<X ; x++ ){
		sigma = 0;
		for(int k=0  ;  k<FILTER_NUMBER ; k++ ){
		for(int py=0 ; py<W_FILTER_SIZE ; py++){
		for(int px=0 ; px<W_FILTER_SIZE ; px++){

			if(x+px-W_FILTER_SIZE/2>=0 && y+py-W_FILTER_SIZE/2>=0 && x+px-W_FILTER_SIZE/2<X && y+py-W_FILTER_SIZE/2<Y){
				s = cvGet2D( Image[k],  y+py-W_FILTER_SIZE/2,  x+px-W_FILTER_SIZE/2 );
				sigma += s.val[0]*s.val[0] / ( W_FILTER_SIZE*W_FILTER_SIZE*FILTER_NUMBER );//出力画素の計算
			}

		}
		}
		}
		sigma = Max( 0.0001, sqrt( sigma ) );
//		if( sigma==0.0001 )puts("0に限りなく近い"); 
		for(int k=0  ;  k<FILTER_NUMBER ; k++ ){
			s = cvGet2D(Image[k],y,x);
			output = s.val[0] / sigma;
			//cout << "input"  << s.val[0] << endl;
			//cout << "output" << output   << endl;
			//puts("");
			s.val[0] = output;
			cvSet2D(img[k],y,x,s);
		}
	}
	}


//使用した画像の解放
	for( int k=0 ; k<FILTER_NUMBER ; k++ ){
		cvReleaseImage(&Image[k]);
	}
	
}


void Norm( IplImage *img[FILTER_NUMBER] ){
	IplImage *Image[FILTER_NUMBER];
	int X = img[0]->width, Y = img[0]->height;
	double output, sigma;
	CvScalar s;

	for( int k=0 ; k<FILTER_NUMBER ; k++ ) Image[k] = cvCreateImage( cvGetSize(img[0]), IPL_DEPTH_32F, 1);

	//for( int k=0 ; k<FILTER_NUMBER ; k++ ){
	//	Image[k] = cvCloneImage(img[k]);
	//}

	//減算正規化
	for( int y=0 ; y<Y ; y++ ){
	for( int x=0 ; x<X ; x++ ){
		sigma = 0;
		for(int k=0  ;  k<FILTER_NUMBER ; k++ ){
		for(int py=0 ; py<W_FILTER_SIZE ; py++){
		for(int px=0 ; px<W_FILTER_SIZE ; px++){

			if(x+px-FILTER_SIZE/2>=0 && y+py-FILTER_SIZE/2>=0 && x+px-FILTER_SIZE/2<X && y+py-FILTER_SIZE/2<Y){
				s = cvGet2D( img[k],  y+py-FILTER_SIZE/2,  x+px-FILTER_SIZE/2 );
				sigma += s.val[0]/( W_FILTER_SIZE*W_FILTER_SIZE*FILTER_NUMBER );//出力画素の計算
//				if(5<x && 5<y)cout << k << py << px << "sigma:"  << sigma << "  input:" << s.val[0]  << endl;
			}

		}
		}
		}


		for(int k=0  ;  k<FILTER_NUMBER ; k++ ){
			s = cvGet2D(img[k],y,x);
			output = s.val[0] - sigma;
//			cout << "input"  << s.val[0] << endl;
//			cout << "output" << output   << endl;
//			puts("");
			s.val[0] = output;
			cvSet2D(Image[k],y,x,s);
		}
//		if(5<x && 5<y)puts("------------");

	}
	}


	//除算正規化
	for( int y=0 ; y<Y ; y++ ){
	for( int x=0 ; x<X ; x++ ){
		sigma = 0;
		for(int k=0  ;  k<FILTER_NUMBER ; k++ ){
		for(int py=0 ; py<W_FILTER_SIZE ; py++){
		for(int px=0 ; px<W_FILTER_SIZE ; px++){

			if(x+px-FILTER_SIZE/2>=0 && y+py-FILTER_SIZE/2>=0 && x+px-FILTER_SIZE/2<X && y+py-FILTER_SIZE/2<Y){
				s = cvGet2D( Image[k],  y+py-FILTER_SIZE/2,  x+px-FILTER_SIZE/2 );
				sigma += s.val[0]*s.val[0] / ( W_FILTER_SIZE*W_FILTER_SIZE*FILTER_NUMBER );//出力画素の計算
			}

		}
		}
		}
		sigma = Max( 0.0001, sqrt( sigma ) );
//		if( sigma==0.0001 )puts("0に限りなく近い"); 
		for(int k=0  ;  k<FILTER_NUMBER ; k++ ){
			s = cvGet2D(Image[k],y,x);
			output = s.val[0] / sigma;
			s.val[0] = output;
			cvSet2D(img[k],y,x,s);
		}
	}
	}


//使用した画像の解放
	for( int k=0 ; k<FILTER_NUMBER ; k++ ){
//		img[k] = cvCloneImage(Image[k]);
		cvReleaseImage(&Image[k]);
	}
	

}


//複数の画像を一枚の画像に統合（Fully Connectionの直前に使用）
IplImage * Integration( IplImage *img[FILTER_NUMBER]){
	int X = img[0]->width, Y = img[0]->height;
	CvScalar s;
	IplImage *Image = cvCreateImage( cvSize(X*FILTER_NUMBER,Y), 8, 3);
	for(int i=0; i < FILTER_NUMBER; i++ ){
		for(int y =0; y < Y; y++ ){
		for(int x =0; x < X; x++ ){
			s = cvGet2D( img[i], y ,x );
			cvSet2D( Image, y, (x+i*X), s );
		}
		}
	}
	return Image;
}


//全結合層1  (   H[outputデータ数][intputデータ数],  B[outputデータ数]  )
IplImage* FullyConnection1( IplImage *img ,float H[][fNUMBER1], float B[] ){
	int X = img->width, Y = img->height;
	float output;
	IplImage *Image = cvCreateImage( cvSize(fNUMBER2,1), IPL_DEPTH_32F, 1);
	CvScalar s;

	for(int i=0 ; i<fNUMBER2 ; i++){
		output=0.0;
		for(int y=0 ; y<Y ; y++){
		for(int x=0 ; x<X ; x++){
			s = cvGet2D(img, y, x);
			output += (float)( H[i][y*X+x] * s.val[0] );   //出力画素の計算
		}
		}
		output += B[i];
		s.val[0] = 1/(1+exp(-output));  //活性化関数
		cvSet2D(Image,0,i,s);
	}
	return Image;
}


//全結合層2  (   H[outputデータ数][intputデータ数],  B[outputデータ数]  )
IplImage* FullyConnection2( IplImage *img ,float H[][fNUMBER2], float B[] ){
	int X = img->width, Y = img->height;
	float output;
	IplImage *Image = cvCreateImage( cvSize(fNUMBER3,1), IPL_DEPTH_32F, 1);
	CvScalar s;

	for(int i=0 ; i<fNUMBER3 ; i++){
		output=0.0;
		for(int y=0 ; y<Y ; y++){
		for(int x=0 ; x<X ; x++){
			s = cvGet2D(img, y, x);
			output += (float)( H[i][y*X+x] * s.val[0] );   //出力画素の計算
		}
		}
		output += B[i];
		s.val[0] = output ;  //活性化関数
		cvSet2D(Image,0,i,s);
	}
	return Image;
}


//Soft Max 関数
void SoftMax( IplImage *img ){
	int X = img->width, Y = img->height;
	CvScalar s;
	double total=0, total2=0, output;

//exp(要素)の合計
	for(int y=0 ; y<Y ; y++){
	for(int x=0 ; x<X ; x++){
		s = cvGet2D(img, y, x);
		total += exp(s.val[0]);
	}
	}

//変換後のデータを代入
	for(int y=0 ; y<Y ; y++){
	for(int x=0 ; x<X ; x++){
		s = cvGet2D(img, y, x);
		output = exp(s.val[0])/total;
		s.val[0] = output;
		total2 += s.val[0];
		cvSet2D(img, y, x, s);
	}
	}

}


//平均活性度ρの計算
void AveAct(){

	for( int j=0 ; j<LAYER ; j++ ){  //LAYER:層の数
		for( int i=0 ; i<FILTER_NUMBER ; i++ ){  //FILTER_NUMBER:並列処理の数

			AveAct(     ConvImage[j][i],     ConvImageR[j][i]);
			AveAct(     PoolImage[j][i],     PoolImageR[j][i]);
			AveAct(  SubNormImage[j][i],  SubNormImageR[j][i]);
			AveAct(DivNormImage[j+1][i],DivNormImageR[j+1][i]);

		}
	}
	AveAct(FcImage[1],FcImageR);

}


//個々の平均活性度ρの計算
void AveAct( IplImage *Image, IplImage *ImageR ){

	int X = Image->width, Y = Image->height;
	double lambda=0.9;
	CvScalar s, sr;

	if(LearningCount==1) ImageR=cvCloneImage(Image);

	else{
		for( int y=0 ; y<Y; y++ ){
		for( int x=0 ; x<X; x++ ){

			s  = cvGet2D(Image ,y,x);
			sr = cvGet2D(ImageR,y,x);
			s.val[0]=(1-lambda)*s.val[0]+lambda*sr.val[0];
			cvSet2D(ImageR,y,x,s);

		}
		}
	}

}


//画像の解放
void Release(){

		cvReleaseImage( &FcImage[2] );
		cvReleaseImage( &FcImage[1] );
		cvReleaseImage( &FcImage[0] );

		for( int j=0 ; j<LAYER         ; j++ ){  //LAYER:層の数
		for( int i=0 ; i<FILTER_NUMBER ; i++ ){  //FILTER_NUMBER:並列処理の数
			cvReleaseImage(   &ConvImage[j][i] );
			cvReleaseImage(   &PoolImage[j][i] );
			cvReleaseImage(&SubNormImage[j][i] );
			cvReleaseImage(&DivNormImage[j][i] );
			if(j==LAYER-1)cvReleaseImage(&DivNormImage[j+1][i] );
		}
		}

}

