//ファイルを編集する関数の定義
#include "stdafx.h"

//このファイルのみで使用する関数の宣言
void WriteCoef(FILE *file,int fnumber1, float H[LAYER][FILTER_NUMBER][FILTER_NUMBER][FILTER_SIZE][FILTER_SIZE], 
	float B[LAYER][FILTER_NUMBER], float FH[fNUMBER2][fNUMBER1], float FB[fNUMBER2], float FH2[fNUMBER3][fNUMBER2], 
	float FB2[fNUMBER3], int Delta=0);
void ReadCoef(FILE *file,int fnumber1, float H[LAYER][FILTER_NUMBER][FILTER_NUMBER][FILTER_SIZE][FILTER_SIZE], 
	float B[LAYER][FILTER_NUMBER], float FH[fNUMBER2][fNUMBER1], float FB[fNUMBER2], float FH2[fNUMBER3][fNUMBER2], 
	float FB2[fNUMBER3], char* filename, int Delta=0);
void WriteAveAct  (FILE *file);
void WriteAveAct  (FILE *file, IplImage *Image, char *name);
void ReadAveAct   (FILE *file, char* filename);
void ReadAveAct(FILE *file, char* filename, IplImage *Image, char *name);
void CopyData     ( char* name1, char *name2);
void MatchHeader  ( int num, int num2, char* name );
void ReadError    ( char* name, char *filename );

//-----------------------batファイル------------------------

//batファイルの作成
void CreateBat(){
	FILE *file;

	puts("Create bat file...");
	file = fopen("execution.bat","w");
	time_t now = time(NULL);
	struct tm *pnow = localtime(&now);

	fprintf(file,"pr1.exe 1 0 0\n");
	//30回ループ
	for(int k=0 ; k<1    ; k++){
	for(int i=0 ; i<1000 ; i++){

		if(i%BATCHSIZE==0)fprintf(file,"pr1.exe 4 0 %d\n", i);

		for(int j=0 ; j<RecogNum ; j++){
//			fprintf(file,"pr1.exe 3 %d %d\n", j,i);
		}
	}
	}
	fclose(file);

}


//-------------------重み係数txtファイル--------------------


//WeightCoefficient.txtに，重み係数の情報の書き込み
void WriteCoefData(char *Name){
	FILE *file;
	char NameCopy[40]={0};
	file = fopen(Name,"w");
	int X = Xsize, Y = Ysize;
	int fnumber1;

	puts("WritingData...");

	//fnumber1の計算
	for(int i=0; i<LAYER ; i++){ X/=2; Y/=2; }
	fnumber1 = X*Y*FILTER_NUMBER;


	//defineで定義した数字の書き込み
	fprintf(file,"LAYER %d\n"        , LAYER        );
	fprintf(file,"FILTER_NUMBER %d\n", FILTER_NUMBER);
	fprintf(file,"FILTER_SIZE %d\n"  , FILTER_SIZE  );
	fprintf(file,"W_FILTER_SIZE %d\n", W_FILTER_SIZE);
	fprintf(file,"fNUMBER1 %d\n"     , fNUMBER1     );
	fprintf(file,"fNUMBER2 %d\n"     , fNUMBER2     );
	fprintf(file,"fNUMBER3 %d\n"     , fNUMBER3     );
	fprintf(file,"FC_NUMBER %d\n"    , 2            );

	//入力画像情報の書き込み
	fprintf(file,"Channel %d\n"      , Channel      );
	fprintf(file,"X %d\n"            , Xsize        );
	fprintf(file,"Y %d\n"            , Ysize        );

	//学習情報の書き込み
	fprintf(file,"ALPHA %d\n"        , ALPHA        );
	fprintf(file,"MU %d\n"           , MU           );
	fprintf(file,"LAMBDA %d\n"       , LAMBDA       );
	fprintf(file,"REDUCTION %d\n"    , REDUCTION    );
	fprintf(file,"STEPSIZE %d\n"     , STEPSIZE     );
	fprintf(file,"UNIT %d\n"         , UNIT         );
	fprintf(file,"LearningCount %d\n", LearningCount);

	//重み係数の書き込み
	WriteCoef(file,fnumber1,h,b,fh,fb,fh2,fb2);

	//重み係数更新の時に用いるDeltaの更新
	WriteCoef(file,fnumber1,D_h,D_b,D_fh,D_fb,D_fh2,D_fb2,1);

	//平均活性度の書き込み
//	if(LearningCount>1)WriteAveAct(file);

	fclose(file);


	strncat(NameCopy, Name, strlen(Name)-4);
	strcat(NameCopy, "Copy.txt");
	CopyData(Name, NameCopy);

	printf("\n");
}


//WeightCoefficient.txtに，重み係数の情報の読み込み
void ReadCoefData(char *filename){
	FILE *file;
	file = fopen(filename,"r");
	int X = Xsize, Y = Ysize;
	int fnumber1, number;
	char name[15];

	puts("ReadingData...");
	while(1){
		if(fscanf( file, "%s", name) == EOF){Error("Error:ReadingData"); break;}
		if( strcmp(name,"h" ) == 0 ) break;
		fscanf( file , "%d" , &number);

		//ヘッダ情報が一致しているか確認
		if     ( strcmp(name,"LAYER"        ) == 0 ) MatchHeader(number, LAYER         , name);
		else if( strcmp(name,"FILTER_NUMBER") == 0 ) MatchHeader(number, FILTER_NUMBER , name);
		else if( strcmp(name,"FILTER_SIZE"  ) == 0 ) MatchHeader(number, FILTER_SIZE   , name);
		else if( strcmp(name,"W_FILTER_SIZE") == 0 ) MatchHeader(number, W_FILTER_SIZE , name);
		else if( strcmp(name,"fNUMBER1"     ) == 0 ) MatchHeader(number, fNUMBER1      , name);
		else if( strcmp(name,"fNUMBER2"     ) == 0 ) MatchHeader(number, fNUMBER2      , name);
		else if( strcmp(name,"fNUMBER3"     ) == 0 ) MatchHeader(number, fNUMBER3      , name);
		else if( strcmp(name,"FC_NUMBER"    ) == 0 ) MatchHeader(number, 2             , name);

		//画像情報
		else if( strcmp(name,"Channel"      ) == 0 ) MatchHeader(number, Channel       , name);
		else if( strcmp(name,"X"            ) == 0 ) MatchHeader(number, X             , name);
		else if( strcmp(name,"Y"            ) == 0 ) MatchHeader(number, Y             , name);

		//学習情報
		else if( strcmp(name,"ALPHA"        ) == 0 ) MatchHeader(number, ALPHA         , name);
		else if( strcmp(name,"MU"           ) == 0 ) MatchHeader(number, MU            , name);
		else if( strcmp(name,"LAMBDA"       ) == 0 ) MatchHeader(number, LAMBDA        , name);
		else if( strcmp(name,"REDUCTION"    ) == 0 ) MatchHeader(number, REDUCTION     , name);
		else if( strcmp(name,"STEPSIZE"     ) == 0 ) MatchHeader(number, STEPSIZE      , name);
		else if( strcmp(name,"UNIT"         ) == 0 ) MatchHeader(number, UNIT          , name);
		else if( strcmp(name,"LearningCount") == 0 ) LearningCount = number;

	}

	//fnumber1の計算
	for(int i=0; i<LAYER ; i++){ X/=2; Y/=2; }
	fnumber1 = X*Y*FILTER_NUMBER;

	//重み係数の読み込み
	ReadCoef(file,fnumber1,h,b,fh,fb,fh2,fb2,filename);

	//Deltaの読み込み
	ReadCoef(file,fnumber1,D_h,D_b,D_fh,D_fb,D_fh2,D_fb2,filename,1);

	fclose(file);
}


//重み係数の書き込み(サブ関数)
void WriteCoef(FILE *file,int fnumber1, float H[LAYER][FILTER_NUMBER][FILTER_NUMBER][FILTER_SIZE][FILTER_SIZE], 
	float B[LAYER][FILTER_NUMBER], float FH[fNUMBER2][fNUMBER1], float FB[fNUMBER2], float FH2[fNUMBER3][fNUMBER2], 
	float FB2[fNUMBER3], int Delta){

	//hの書き込み
	if(Delta==1) fprintf(file,"D_");
	fprintf(file,"h ");
	for(int p=0 ; p<FILTER_SIZE   ; p++){
	for(int q=0 ; q<FILTER_SIZE   ; q++){
	for(int i=0 ; i<FILTER_NUMBER ; i++){
	for(int o=0 ; o<FILTER_NUMBER ; o++){
	for(int l=0 ; l<LAYER         ; l++){
		fprintf(file,"%f ", H[l][o][i][q][p]);
	}
	}
	}
	}
	}
	fprintf(file,"\n");

	//bの書き込み
	if(Delta==1) fprintf(file,"D_");
	fprintf(file,"b ");
	for(int o=0 ; o<FILTER_NUMBER ; o++){
	for(int l=0 ; l<LAYER         ; l++){
		fprintf(file,"%f ", B[l][o]);
	}
	}
	fprintf(file,"\n");

	//fhの書き込み
	if(Delta==1) fprintf(file,"D_");
	fprintf(file,"fh ");
	for(int i=0 ; i<fnumber1 ; i++){
	for(int o=0 ; o<fNUMBER2 ; o++){
		fprintf(file,"%f ", FH[o][i]);
	}
	}
	fprintf(file,"\n");

	//fbの書き込み
	if(Delta==1) fprintf(file,"D_");
	fprintf(file,"fb ");
	for(int o=0 ; o<fNUMBER2 ; o++){
		fprintf(file,"%f ", FB[o]);
	}
	fprintf(file,"\n");

	//fh2の書き込み
	if(Delta==1) fprintf(file,"D_");
	fprintf(file,"fh2 ");
	for(int i=0 ; i<fNUMBER2 ; i++){
	for(int o=0 ; o<fNUMBER3 ; o++){
		fprintf(file,"%f ", FH2[o][i]);
	}
	}
	fprintf(file,"\n");

	//fb2の書き込み
	if(Delta==1) fprintf(file,"D_");
	fprintf(file,"fb2 ");
	for(int o=0 ; o<fNUMBER3 ; o++){
		fprintf(file,"%f ", FB2[o]);
	}
	fprintf(file,"\n");
}


//重み係数の読み込み(サブ関数)
void ReadCoef(FILE *file,int fnumber1, float H[LAYER][FILTER_NUMBER][FILTER_NUMBER][FILTER_SIZE][FILTER_SIZE], 
	float B[LAYER][FILTER_NUMBER], float FH[fNUMBER2][fNUMBER1], float FB[fNUMBER2], float FH2[fNUMBER3][fNUMBER2], 
	float FB2[fNUMBER3], char* filename, int Delta){

	float number_float;
	char name[15];

	//hの読み込み

	if(Delta==1){
		fscanf( file, "%s", name);
		if( (Delta==0 && strcmp(name,"h" ) != 0) || ( Delta==1 && strcmp(name,"D_h" ) != 0 )){
			fclose(file); ReadError("h or D_h doesn't exist.",filename); return;
		}
	}

	for(int p=0 ; p<FILTER_SIZE   ; p++){
	for(int q=0 ; q<FILTER_SIZE   ; q++){
	for(int i=0 ; i<FILTER_NUMBER ; i++){
	for(int o=0 ; o<FILTER_NUMBER ; o++){
	for(int l=0 ; l<LAYER         ; l++){
		fscanf( file, "%f", &number_float);
		H[l][o][i][q][p] = number_float;
	}
	}
	}
	}
	}

	//bの読み込み
	fscanf( file, "%s", name);
	if( (Delta==0 && strcmp(name,"b" ) != 0) || ( Delta==1 && strcmp(name,"D_b" ) != 0 )){
		fclose(file); ReadError("b or D_b doesn't exist.",filename); return;
	}

	for(int o=0 ; o<FILTER_NUMBER ; o++){
	for(int l=0 ; l<LAYER         ; l++){
		fscanf( file, "%f", &number_float);
		B[l][o] = number_float;
	}
	}

	//fhの読み込み
	fscanf( file, "%s", name);
	if( (Delta==0 && strcmp(name,"fh" ) != 0) || ( Delta==1 && strcmp(name,"D_fh" ) != 0 )){
		fclose(file); ReadError("fh or D_fh doesn't exist.",filename); return;
	}

	for(int i=0 ; i<fnumber1 ; i++){
	for(int o=0 ; o<fNUMBER2 ; o++){
		fscanf( file, "%f", &number_float);
		FH[o][i]=number_float;
	}
	}

	//fbの読み込み
	fscanf( file, "%s", name);
	if( (Delta==0 && strcmp(name,"fb" ) != 0) || ( Delta==1 && strcmp(name,"D_fb" ) != 0 )){
		fclose(file); ReadError("fb or D_fb doesn't exist.",filename); return;
	}

	for(int o=0 ; o<fNUMBER2 ; o++){ fscanf( file, "%f", &number_float); FB[o]=number_float; }

	//fh2の読み込み
	fscanf( file, "%s", name);
	if( (Delta==0 && strcmp(name,"fh2" ) != 0) || ( Delta==1 && strcmp(name,"D_fh2" ) != 0 )){
		fclose(file); ReadError("fh2 or D_fh2 doesn't exist.",filename); return;
	}

	for(int i=0 ; i<fNUMBER2 ; i++){
	for(int o=0 ; o<fNUMBER3 ; o++){
		fscanf( file, "%f", &number_float);
		FH2[o][i]=number_float;
	}
	}

	//fb2の読み込み
	fscanf( file, "%s", name);
	if( (Delta==0 && strcmp(name,"fb2" ) != 0) || ( Delta==1 && strcmp(name,"D_fb2" ) != 0 )){
		fclose(file); ReadError("fb or D_fb2 doesn't exist.",filename); return;
	}

	for(int o=0 ; o<fNUMBER3 ; o++){ fscanf( file, "%f", &number_float); FB2[o]=number_float; }


}


//平均活性度ρの書き込み
void WriteAveAct(FILE *file){

	for( int j=0 ; j<LAYER ; j++ ){  //LAYER:層の数
		for( int i=0 ; i<FILTER_NUMBER ; i++ ){  //FILTER_NUMBER:並列処理の数

			WriteAveAct( file,     ConvImageR[j][i], "ConvR");
			WriteAveAct( file,     PoolImageR[j][i], "PoolR");
			WriteAveAct( file,  SubNormImageR[j][i], "SubNormR");
			WriteAveAct( file,DivNormImageR[j+1][i], "DivNormR");

		}
	}
	WriteAveAct(file,FcImageR,"FcR");

}


//平均活性度ρの書き込み(サブ関数)
void WriteAveAct(FILE *file, IplImage *Image, char *name){
	int X = Image->width, Y = Image->height;
	CvScalar s;

	fprintf(file,name);
	fprintf(file," " );

	for( int y=0 ; y<Y; y++ ){
	for( int x=0 ; x<X; x++ ){
		s  = cvGet2D(Image ,y,x);
		fprintf(file,"%f ",s.val[0]);
	}
	}

	fprintf(file,"\n");

}


//平均活性度ρの読み込み
void ReadAveAct(FILE *file, char* filename){

	for( int j=0 ; j<LAYER ; j++ ){  //LAYER:層の数
		for( int i=0 ; i<FILTER_NUMBER ; i++ ){  //FILTER_NUMBER:並列処理の数

			ReadAveAct( file, filename,     ConvImageR[j][i], "ConvR");
			ReadAveAct( file, filename,     PoolImageR[j][i], "PoolR");
			ReadAveAct( file, filename,  SubNormImageR[j][i], "SubNormR");
			ReadAveAct( file, filename,DivNormImageR[j+1][i], "DivNormR");

		}
	}
	ReadAveAct(file,filename,FcImageR,"FcR");

}


//平均活性度ρの書き込み(サブ関数)
void ReadAveAct(FILE *file, char* filename, IplImage *Image, char *name){
	int X = Image->width, Y = Image->height;
	CvScalar s;
	char Name[20];

	fscanf(file,"%s",Name);

	if( strcmp(Name, name) != 0 ){ fclose(file); ReadError("ConvR doesn't exist.",filename); return; }

	for( int y=0 ; y<Y; y++ ){
	for( int x=0 ; x<X; x++ ){
		fscanf(file,"%f",&s.val[0]);
		cvSet2D(Image,y,x,s);

	}
	}

}


//重み係数書き込み時に使用
void CopyData(char* name1, char *name2){
	FILE* fpSrc;
	FILE* fpDest;
	char c;


	fpSrc = fopen( name1, "rb" );
	if( fpSrc == NULL ){
		exit( EXIT_FAILURE );
	}

	fpDest = fopen( name2, "wb" );
	if( fpDest == NULL ){
		exit( EXIT_FAILURE );
	}


	while( 1 ){
		/* バイナリデータとして 1Byteずつ読み込み、1Byteずつ書き込む */

		fread( &c, sizeof(c), 1, fpSrc );
		if( feof( fpSrc ) ){
			break;
		}
		if( ferror( fpSrc ) ){
			exit( EXIT_FAILURE );
		}

		fwrite( &c, sizeof(c), 1, fpDest );
	}


	if( fclose( fpDest ) == EOF ){
		fputs( "Fail file close.\n", stderr );
		exit( EXIT_FAILURE );
	}
	if( fclose( fpSrc ) == EOF ){
		fputs( "Fail file close.\n", stderr );
		exit( EXIT_FAILURE );
	}

}


//数字が一致しているか調べる関数（ヘッダ情報の読み込み時に使用）
void MatchHeader(int num, int num2, char* name){
	char Name[50];
	if(num != num2){
		strcpy(Name,name);
		strcat(Name, " doesn't correspond.");
		Error(Name);
	}
}


//エラー時に呼び出す関数（重み係数書き込み時に使用）
void ReadError(char* name, char *filename ){
	if(strstr(filename,"Copy")==NULL){
		char NameCopy[40]={0};
		strncat(NameCopy, filename, strlen(filename)-4);
		strcat(NameCopy, "Copy.txt");
		ReadCoefData(NameCopy);
	}
	else Error(name);
}


//-----------------識別結果csvファイル----------------------

//識別結果の書き込み(csv)
void WriteCsv( IplImage *img , int TrueNumber ){
	int X = img->width, Y = img->height, max=0;
	double   max_val=0.0;
	CvScalar s;
	FILE *file;
	char Name[30]={0};

	if(X<RecogNum)Error("RecogNum is too large.");

	//最大値の計算
	for(int i=0 ; i<RecogNum ; i++){
		s = cvGet2D(img,0,i);
		if(max_val<s.val[0]){ max=i; max_val=s.val[0]; }
	}

	//名前の定義
	if(ImageMode==0) strcpy(Name,"AccuracyRateAnimal.csv");
	else             strcpy(Name,"AccuracyRateNumber.csv");

	file = fopen(Name,"a");
	s = cvGet2D(img,0,TrueNumber);
	fprintf(file,"%d,%f,%d\n",LearningCount,s.val[0],max==TrueNumber);
	fclose(file);

}


//-------------------logファイル----------------------------


//ログの書き込み
void log(char* name, int mode){
	FILE *file;

//	puts("WritingLog...");
	file = fopen("log.txt","a");

	if(mode==-1) fprintf(file," %s\n", name);

	else{
		if(mode==0 || mode==1)fprintf(file,"\n\n");
		time_t now = time(NULL);
		struct tm *pnow = localtime(&now);
		fprintf(file,"%02d/%02d/%02d %02d:%02d:%02d " , pnow->tm_year+1900, pnow->tm_mon+1, pnow->tm_mday, pnow->tm_hour, pnow->tm_min, pnow->tm_sec);
		if     ( mode==0 ) fprintf(file,"Create Bat    ");
		else if( mode==1 ) fprintf(file,"Create Coeff  ");
		else if( mode==2 ) fprintf(file,"Recognition   ");
		else if( mode==3 ) fprintf(file,"OneLearning   ");
		else if( mode==4 ) fprintf(file,"MultiLearning ");
		else               fprintf(file,"Failure       ");
		fprintf(file," (%s)", name);
		fprintf(file," %d", LearningCount);
	}

	fclose(file);
}


