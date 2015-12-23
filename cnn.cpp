//cnn�Ŏg�p����֐�
#include "stdafx.h"

//���̃t�@�C���݂̂Ŏg�p����֐��̐錾
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


//CNN(��ݍ��݃j���[�����l�b�g���[�N)
IplImage* CNN( IplImage *Image ){
	IplImage* img; //�Ԑ��p

	puts("CNN...");
	CheckCnn();                          //�����������K�؂����f
	AssignInputImage( Image , DivNormImage[0] );  //NormImage�ɏ����摜�̑��


	printf("Layer:");
//���ۂɏ������s��
	for( int j=0 ; j<LAYER         ; j++ ){  //LAYER:�w�̐�
		printf("%d ",j);
		for( int i=0 ; i<FILTER_NUMBER ; i++ ){  //FILTER_NUMBER:���񏈗��̐�

		//�􍞂ݑw
			ConvImage[j][i] = Convolution( DivNormImage[j], h[j][i], b[j][i], (j==0)?Image->nChannels:FILTER_NUMBER );

		//�v�[�����O�w
			PoolImage[j][i]=Pooling(ConvImage[j][i]);

		}


	//PoolImage��SubNormImage�ɃR�s�[
		for( int i=0 ; i< FILTER_NUMBER ; i++ ){ SubNormImage[j][i] = cvCloneImage( PoolImage[j][i] ); }

	//�Ǐ��R���g���X�g���K��
	//���Z���K��
		SubNorm( SubNormImage[j] );

	//SubNorm��DivNorm�ɃR�s�[
		for( int i=0 ; i< FILTER_NUMBER ; i++ ){ DivNormImage[j+1][i] = cvCloneImage( SubNormImage[j][i] ); }

	//���Z���K��
		DivNorm( DivNormImage[j+1] );  //SubNormImage���R�s�[������K�v������

	//�摜�\��
//		for( int i=0 ; i<FILTER_NUMBER ; i++ ) ShowImage("T",SubNormImage[j][i]);
	}
	puts("");



//�ꖇ�̉摜�ɓ���
	FcImage[0] = Integration( DivNormImage[LAYER] );
//	ShowImage("Output",FcImage[0]);

	
//�S�����w(Fully Connection)    1�w��
	FcImage[1] = FullyConnection1(FcImage[0], fh, fb);

//�S�����w(Fully Connection)    2�w��
	FcImage[2] = FullyConnection2(FcImage[1], fh2, fb2);//�������֐��Ȃ�

//�\�t�g�}�b�N�X
	SoftMax           ( FcImage[2] );
	img = cvCloneImage( FcImage[2] );
	return img;
}


//InputImage�ɏ����摜�̑��
void AssignInputImage(IplImage *img , IplImage *InputImg[FILTER_NUMBER] ){
	
	//3�`�����l���̏ꍇ�A���͉摜��RGB�ɕ���
	if(img->nChannels == 3){
		for(int i=0 ; i<3 ; i++) InputImg[i]=cvCreateImage(cvGetSize(img),8,1);
		cvSplit(img,InputImg[0],InputImg[1],InputImg[2],NULL);
	}

	//1�`�����l���̏ꍇ�AInputImage[0]�ɂ��̂܂܃R�s�[
	else if(img->nChannels == 1){
		InputImg[0] = cvCloneImage( img );
	}

}


//�􍞂ݑw     h:�d�݌W��    b:�ؕ�
IplImage* Convolution( IplImage *img[FILTER_NUMBER] ,float H[][FILTER_SIZE][FILTER_SIZE], float B, int input_number ){
	int X = img[0]->width, Y = img[0]->height;
	float output;
	IplImage *Image = cvCreateImage( cvGetSize(img[0]), IPL_DEPTH_32F, 1);
	CvScalar s;

	for(int y=0 ; y<Y ; y++){
	for(int x=0 ; x<X ; x++){
		output=0.0;
		for(int k =0 ; k <input_number ; k++ ){
		for(int py=0 ; py<FILTER_SIZE  ; py++){  // FILTER_SIZE * FILTER_SIZE �ߖT
		for(int px=0 ; px<FILTER_SIZE  ; px++){
			if(x+px-FILTER_SIZE/2>=0 && y+py-FILTER_SIZE/2>=0 && x+px-FILTER_SIZE/2<X && y+py-FILTER_SIZE/2<Y){
				s = cvGet2D(img[k],y+py-FILTER_SIZE/2,x+px-FILTER_SIZE/2);
				output += (float)(H[k][py][px]*s.val[0]);//�o�͉�f�̌v�Z
			}
		}
		}
		}
		output += B;
		s.val[0] = Max(0, output);  //�������֐�
		cvSet2D(Image,y,x,s);
	}
	}
	return Image;
}


//�v�[�����O�w
IplImage* Pooling( IplImage *img ){
	IplImage *Image2 = cvCreateImage( cvSize (img->width / 2, img->height / 2), IPL_DEPTH_32F, 1);
	int X = Image2->width, Y = Image2->height;
	int size=2;//�v�[�����O�w�̃T�C�Y
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


//�d�݌W���̉��P���K�v
//�Ǐ��R���g���X�g���K��
//���Z���K��
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
				sigma += s.val[0]/( W_FILTER_SIZE*W_FILTER_SIZE*FILTER_NUMBER );//�o�͉�f�̌v�Z
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


//���Z���K��
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
				sigma += s.val[0]*s.val[0] / ( W_FILTER_SIZE*W_FILTER_SIZE*FILTER_NUMBER );//�o�͉�f�̌v�Z
			}

		}
		}
		}
		sigma = Max( 0.0001, sqrt( sigma ) );
//		if( sigma==0.0001 )puts("0�Ɍ���Ȃ��߂�"); 
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


//�g�p�����摜�̉��
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

	//���Z���K��
	for( int y=0 ; y<Y ; y++ ){
	for( int x=0 ; x<X ; x++ ){
		sigma = 0;
		for(int k=0  ;  k<FILTER_NUMBER ; k++ ){
		for(int py=0 ; py<W_FILTER_SIZE ; py++){
		for(int px=0 ; px<W_FILTER_SIZE ; px++){

			if(x+px-FILTER_SIZE/2>=0 && y+py-FILTER_SIZE/2>=0 && x+px-FILTER_SIZE/2<X && y+py-FILTER_SIZE/2<Y){
				s = cvGet2D( img[k],  y+py-FILTER_SIZE/2,  x+px-FILTER_SIZE/2 );
				sigma += s.val[0]/( W_FILTER_SIZE*W_FILTER_SIZE*FILTER_NUMBER );//�o�͉�f�̌v�Z
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


	//���Z���K��
	for( int y=0 ; y<Y ; y++ ){
	for( int x=0 ; x<X ; x++ ){
		sigma = 0;
		for(int k=0  ;  k<FILTER_NUMBER ; k++ ){
		for(int py=0 ; py<W_FILTER_SIZE ; py++){
		for(int px=0 ; px<W_FILTER_SIZE ; px++){

			if(x+px-FILTER_SIZE/2>=0 && y+py-FILTER_SIZE/2>=0 && x+px-FILTER_SIZE/2<X && y+py-FILTER_SIZE/2<Y){
				s = cvGet2D( Image[k],  y+py-FILTER_SIZE/2,  x+px-FILTER_SIZE/2 );
				sigma += s.val[0]*s.val[0] / ( W_FILTER_SIZE*W_FILTER_SIZE*FILTER_NUMBER );//�o�͉�f�̌v�Z
			}

		}
		}
		}
		sigma = Max( 0.0001, sqrt( sigma ) );
//		if( sigma==0.0001 )puts("0�Ɍ���Ȃ��߂�"); 
		for(int k=0  ;  k<FILTER_NUMBER ; k++ ){
			s = cvGet2D(Image[k],y,x);
			output = s.val[0] / sigma;
			s.val[0] = output;
			cvSet2D(img[k],y,x,s);
		}
	}
	}


//�g�p�����摜�̉��
	for( int k=0 ; k<FILTER_NUMBER ; k++ ){
//		img[k] = cvCloneImage(Image[k]);
		cvReleaseImage(&Image[k]);
	}
	

}


//�����̉摜���ꖇ�̉摜�ɓ����iFully Connection�̒��O�Ɏg�p�j
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


//�S�����w1  (   H[output�f�[�^��][intput�f�[�^��],  B[output�f�[�^��]  )
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
			output += (float)( H[i][y*X+x] * s.val[0] );   //�o�͉�f�̌v�Z
		}
		}
		output += B[i];
		s.val[0] = 1/(1+exp(-output));  //�������֐�
		cvSet2D(Image,0,i,s);
	}
	return Image;
}


//�S�����w2  (   H[output�f�[�^��][intput�f�[�^��],  B[output�f�[�^��]  )
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
			output += (float)( H[i][y*X+x] * s.val[0] );   //�o�͉�f�̌v�Z
		}
		}
		output += B[i];
		s.val[0] = output ;  //�������֐�
		cvSet2D(Image,0,i,s);
	}
	return Image;
}


//Soft Max �֐�
void SoftMax( IplImage *img ){
	int X = img->width, Y = img->height;
	CvScalar s;
	double total=0, total2=0, output;

//exp(�v�f)�̍��v
	for(int y=0 ; y<Y ; y++){
	for(int x=0 ; x<X ; x++){
		s = cvGet2D(img, y, x);
		total += exp(s.val[0]);
	}
	}

//�ϊ���̃f�[�^����
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


//���ϊ����x�ς̌v�Z
void AveAct(){

	for( int j=0 ; j<LAYER ; j++ ){  //LAYER:�w�̐�
		for( int i=0 ; i<FILTER_NUMBER ; i++ ){  //FILTER_NUMBER:���񏈗��̐�

			AveAct(     ConvImage[j][i],     ConvImageR[j][i]);
			AveAct(     PoolImage[j][i],     PoolImageR[j][i]);
			AveAct(  SubNormImage[j][i],  SubNormImageR[j][i]);
			AveAct(DivNormImage[j+1][i],DivNormImageR[j+1][i]);

		}
	}
	AveAct(FcImage[1],FcImageR);

}


//�X�̕��ϊ����x�ς̌v�Z
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


//�摜�̉��
void Release(){

		cvReleaseImage( &FcImage[2] );
		cvReleaseImage( &FcImage[1] );
		cvReleaseImage( &FcImage[0] );

		for( int j=0 ; j<LAYER         ; j++ ){  //LAYER:�w�̐�
		for( int i=0 ; i<FILTER_NUMBER ; i++ ){  //FILTER_NUMBER:���񏈗��̐�
			cvReleaseImage(   &ConvImage[j][i] );
			cvReleaseImage(   &PoolImage[j][i] );
			cvReleaseImage(&SubNormImage[j][i] );
			cvReleaseImage(&DivNormImage[j][i] );
			if(j==LAYER-1)cvReleaseImage(&DivNormImage[j+1][i] );
		}
		}

}

