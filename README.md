# ConvolutionalNeuralNetwork

##事前準備

visual c++ 2010で実装

* opencv2.2をあらかじめ追加  
* プロパティ→C/C++→追加のインクルードディレクトリ：C:\OpenCV2.2\include  
* プロパティ→全般→文字セット：Unicode 文字セットを使用する  

##使い方

使い方その１、２があるので、自分の好きなやり方で実装してみてください．

以下にそれぞれの特徴をまとめた表を載せておきます．

|使い方|その１|その２|
|--:|:--:|:--:|
|難易度|初心者|中級者|
|コマンドライン引数|なし|あり|

*初期化→学習→学習→・・・（学習を何回も繰り返す）→識別*の順で処理を行います．

未学習の画像で識別を行い、正解したらいい学習が行えています。

※教師信号はあらかじめ自分で数字を割り振っておく

例：ネコ→０、イヌ→１


###使い方その１

初心者向け、コマンドライン引数なし（１つずつ画像を学習したいときに最適）

初期化：画像を選択→モード１の選択

学習：画像を選択→モード３の選択→教師信号の選択　（何回か違う画像で繰り返す）  

識別：画像を選択→モード２の選択

###使い方その２

中級者向け、コマンドライン引数あり（batファイルなどで一気に画像を学習したいときに最適）

○○.exe <モード> <正解データナンバー（教師信号）> <画像ファイルナンバー> <画像ファイル名>

モード：1→初期化、2→識別、3→学習

正解データナンバー（教師信号）：学習時のみ有効、それ以外の場合は0を記入（数字であれば何でもよい）

画像ファイルナンバー：省略（説明文作成中・・・）現時点では0を記入

画像ファイル名：画像ファイル名の記入
