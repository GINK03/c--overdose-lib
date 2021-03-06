[![CircleCI](https://circleci.com/gh/GINK03/cpp-overdose-lib/tree/master.svg?style=svg)](https://circleci.com/gh/GINK03/cpp-overdose-lib/tree/master)

# cpp-overdose-lib
C++17のラムダ式と、テンプレートを用いることで、Rのdplyr, Kotlin, Scalaなどに似たシンタックスで、同等の機能が動かせることを示します

## もっと関数型ライクにC++を使いたい！
C++の言語の特徴として、その手続き的な側面が多く強調されます  
これはC言語を背景としているからなのですが、近年のC++の言語仕様の拡張によって面白いことができるようになってきました  

例えば、Rという統計言語にはデータのオペレーションが感覚的にわかりやすい、dplyerというライブラリがあります  

C++がデータ分析の文脈で使われないのはなぜでしょうか。いろんな人がいろんなことを言いますが、早い話、サッと組んで、サッと動かすということができないからです  

`The answers already cover the main reason data scientists don't use C and that is the convenience of rapid prototype building.`  

[1](https://www.quora.com/Why-dont-data-scientists-use-C-C++)
これを解決できそうな機運が少しあり、dplyrやScala Likeなコードスタイルを用いることで、手続き型特有の大量の一時変数を隠蔽することに成功しました  

これに似せたライブラリ overdose-libを作成したので、ご紹介させてください  

## Requirement
- clang++( 5.0>= )
- gnumake  

要求するコンパイラのバージョンが高いので、ArchLinuxなど最新のソフトウェアが積極的に導入されるものが望ましいです  

## ラムダとTemplateとautoによる型推定  
ラムダは匿名関数と呼ばれ、リテラルの中に埋め込むことができ、また変数に束縛することもできる名前のない関数です。ラムダ式は変数に束縛することも可能で、変数に代入したラムダ式を取り回して、任意の処理をすることができます  

TemplateはもともとジェネリックなSTLの型を指定することができるものでしたが、型以外にも実数値やラムダ式を引数に取れるし、コンパイル時に動作するので、コンパイル時に実行し計算するものとなっています  

autoは型推論で、型宣言を簡潔にするものですが、一部戻り値などをジェネリックに動作することが期待できて、戻り値の型がテンプレートやコンパイル時の型の取得のdecletypeなどで、自動的に推論させることが可能です。  

これを用いると、関数型言語の一つの要件である第1級関数という関数をデータと同じように扱うことが可能になります  

例えば、任意の関数Fを受け取り、型SのベクタをFで変換してから型Tで返却する関数を返す関数はこのようにかけます。
```cpp
  template<typename S, typename R, typename F>
  auto mapper(const F& f) {
    return [f](std::vector<S> source) {
      std::vector<R> tmp; 
      for(auto s:source) {
        R r = f(s); 
        tmp.push_back(r);
      }
      return tmp;
    };
  };
```

これを>>(ダブルアロー)のオペレータを定義すると、このように書くことができます  
この例では、[1, 2, 3, 4]のベクタを各要素を二乗する操作です  
echoは表中出力に対する出力する関数を返す関数  
```cpp
  auto src = vector{1, 2, 3, 4};
  src >> mapper<int,int>([](int i ){ return i*i;}) >> echo<int>();
```
出力はこのようになります
```console
$ ./bin/a.out
[1,4,9,16]
```

## 色々な関数を実装してみた
RのdplyrやPythonのpandas、Apache SparkのRDDなどを参考にして実装しました
dplyrとSpark RDDと今回作成したoverdose(仮称)の関数と機能の比較表です

| 機能        | dplyr(R)   | Spark RDD(Scala,Java)| overdose(C++) |
|:-----------:|:-----------:|:------------:|:------------:|
| パイプライン操作 | %>%        | .(dot)      | >>(ダブルアロー)|
| 任意の関数で処理 | map        | map         | mapper       |
| 畳み込み処理    | reduce     | reduce      | reduce      |
| フィルタ操作    | filter     | filter      | filter       |
| グルーピング    | group_by   | group_by    | groupBy      |
| 並び替え       | arrange    | sortBy      | sortBy      |
| 二つのデータのペアリング | -          | zip         | zipMap       | 
| 合計値を計算 | accumulate | sum         | sum          |
| 最小値を計算　| -          | min         | min          | 
| 最大値を計算 | -          | max         | max          |
| 平均値を計算 | -          | mean        | mean         |
| 要素を取り出す | flatten    | flatten     | flatten      |
| ユニークにする | distinct   | distinct    | distinct     |
| 要素を追加する | mutate     | (mapで代替) | (mapperで代替) | 
| index付きmap | -          | withIndex  | mapperIndexed | 
| シリアライザ | -           | toString() | SERIAL,DESERIAL | 
| 並列性 | multidplyr | 並列性がある | concurrent::mapper | 

## minimal install 

** ご注意!! **  
** 完全にclang++以外の存在を忘れていましが、このプログラムはclang 5.0.0(final)で開発と動作確認しています **  
```console
$ clang++ --version
clang version 5.0.0 (tags/RELEASE_500/final)
```

C++で作られているので、/usr/includeにインクルードファイルを設置し、/usr/lib(64)にsoをおけば、システム全体から利用できるようになりますが、
とりま、example.cppを編集することで任意の機能を成り立たせることができます。

```console
$ git clone https://github.com/GINK03/cpp-overdose-lib
$ cd cpp-overdose-lib/
$ make 
$ make test <- errorが出なければOK
$ sudo make install <- システム全体に反映する際にはやる（任意）
```
**サンプルの実行**
example.cppというFizzBuzzの10000までの数のFizz, Buzz, FizzBuzz, Otherをカウントします  
コンパイルと実行にするにはこうします  
```console
$ make 
$ ./a.out
KEY: Buzz VALUE_SIZE: 1333 
KEY: Fizz VALUE_SIZE: 2667
KEY: FizzBuzz VALUE_SIZE: 666 
KEY: Other VALUE_SIZE: 5333   
```

## mapper
 一般的なScala, Ruby, Kotlinなどのmap処理に該当します。vectorの要素の中身に、ラムダ式でデータを操作することで、任意の形に変形します　　
 
例えば、[1, 2, 3, 4, 5, 6]のリストを全て二乗して、[1, 4, 9, 16, 25, 36]というデータを得たいとします  
 
この時、overdoseではこのようにします  
```cpp
void mapperTest() {
  vector<int>({1,2,3,4,5,6})
    >> mapper<int,int>([](int i){ return i*i;}) 
    >> echo<int>();
}
(出力)-> [1,4,9,16,25,36]
```
インターフェース
```cpp
mapper<INPUT, OUTPUT> -> std::vector<OUTPUT>
```
左にデータとなる構造を置き、>>(ダブルアロー)で射とするべき関数を用意します  
この時、mapperのラムダ式に入力される型情報をINPUT,戻り値の型をOUTPUTとします  
なので、型変換などにも用いることができます  

## reducer
Scala, Ruby, KotlinのReduceとほぼ同じ働きをします  
Reduceは何らかの畳み込みを行うのですが、掛け算と、足し算であるべき初期値が異なったりします(足し算では初期値が0.0, 掛け算などでは1.0など)  
```cpp
void reducerTest() {
  vector<int>({1,2,3,4,5,6})
    >> reducer<int, int>(0, [](int& y, int x){ y = y+x;} )
    >> let<int>([](auto out){ cout << "REDUCED: " << out << endl;});
}
(出力)-> REDUCED: 21
```
インターフェース
```
reducer<INPUT,OUTPUT> -> std::vector<OUTPUT>
```

入力される型情報と、出力される型情報を指定します  

コンパイラの解釈力に依存しますが、文脈から型情報を指定しなくても通る場合があります  

## filter
Scala, Ruby, Kotlinなどのfilterとおなじです  
条件に該当するデータのみを取り出します  
例えば、2の倍数になるもののみを取り出すとこのようになります  
```cpp
void filterTest() {
  vector<int>({1,2,3,4,5,6,7}) 
    >> filter<int, int>([](int i){
      return i%2 == 0;
    }) >> echo<int>() ;
}
(出力)-> 
[2,4,6]
```
インターフェース
```cpp
filter<INPUT, OUTPUT>(FUNCTOR) -> std::vector<OUTPUT>
```
このようなインターフェースです

## groupBy
KotlinのgroupByに触発されて使いました  
KotlinのgroupByではList型に変換すると、このようなデータ構造で帰ってきます。
```kotlin
List<Pair<KEY,List<ORIGINAL>>>
```
この構造が便利で特定のキーで集約したい場合によく使います。

キーの型と、出力する型を指定し、キーを作るラムダ式を指定することで、同等の機能を実現します  
この例では、1から100までのintのvectorを作成して、その値を5で割った値のあまりをキーにグルーピングをします  

グルーピングした値はvector<tuple<KEY, ORIGINAL>>となります  
```cpp
void groupByTest() {
  vector<int> src = OD::Range(1,100);
  src >> groupBy<int, int>( [](auto i){ 
    return i%5;
  }) >> mapper<tuple<int,vector<int>>, int>( [](auto tup){
    auto [key,value] = tup;
    auto join = value >> joinToString<int>(",");
    cout << "GROUPBY TEST KEY: " << key << " VALUE: " << join << endl;//
    return 0;
  });
}
(出力)-> 
GROUPBY TEST KEY: 0 VALUE: 5,10,15,20,25,30,35,40,45,50,55,60,65,70,75,80,85,90,95
GROUPBY TEST KEY: 1 VALUE: 1,6,11,16,21,26,31,36,41,46,51,56,61,66,71,76,81,86,91,96
GROUPBY TEST KEY: 2 VALUE: 2,7,12,17,22,27,32,37,42,47,52,57,62,67,72,77,82,87,92,97
GROUPBY TEST KEY: 3 VALUE: 3,8,13,18,23,28,33,38,43,48,53,58,63,68,73,78,83,88,93,98
GROUPBY TEST KEY: 4 VALUE: 4,9,14,19,24,29,34,39,44,49,54,59,64,69,74,79,84,89,94,99
```
インターフェース
```cpp
groupBy<KEY,ORIGINAL>(FUNCTOR) -> std::tuple<KEY,std::vector<ORIGINAL>>
```
このようなインターフェースになります  

## sortBy 
色々な言語と一緒でsortで用いる評価式をラムダ式で与えることで、実現しています  

特にPythonとKotlinのsortByがいいと思っていて、評価式をラムダ式で与えるのではなく、キーとなる値を返すラムダ式を与えるとシンプルで良いと思います  

Python3で降順にソートする際は、このような表現を取ることができます  
```python
>>> sorted([1,2,3,4,5,6,7], key=lambda x:x*-1)
[7, 6, 5, 4, 3, 2, 1]
```

これをC++で似たシンタックスで実現すると、このようになります  
```cpp
void sortByTest() {
  vector<int> src = {1,2,3,4,5,6,7};
  src >> sortBy<int, int>([](int i){ return i*-1;}) >> echo<int>();
}
(出力)-> 
[7,6,5,4,3,2,1]
```
インターフェース
```cpp
sortBy<KET,ORIGINAL>(FUNCTOR) -> vector<ORIGINAL>
```

## zip 
Pythonでzip関数もあるのですが、Kotlinのようなzipを定義したいと思います  

Kotlinではzipはこのようになっています  
```kotlin
>>> listOf(1,2,3,4,5).zip(listOf("a", "b", "c", "d", "e"))
[(1, a), (2, b), (3, c), (4, d), (5, e)]
```

今回作成したoverdoseでは次のようになります  
```cpp
void zipTest() {
  vector<int> source = {1,2,3,4,5,6,7};
  vector<string> target = {"a","b","c","d","e","f","g"};
  source >> zip<int, string>(target) >> let<vector<pair<int,string>>>([](auto ts){
    for(auto t1:ts) {
      auto [src, tgt] = t1;
      cout << "SOURCE: " << src << " TARGET: " << tgt << endl;
    }
  });
}
(出力)-> 
SOURCE: 1 TARGET: a
SOURCE: 2 TARGET: b
SOURCE: 3 TARGET: c
SOURCE: 4 TARGET: d
SOURCE: 5 TARGET: e
SOURCE: 6 TARGET: f
SOURCE: 7 TARGET: g
```
インターフェース
```cpp
zip<SOURCE,TARGET>(std::vector<TARGET>) -> std::vector<std::pair<SOURCE,TARGET>>
```

## sum, mean, min, max
他の言語でも色々実装されていますが、こんな感じです  
```cpp
void sumMeanMaxMinTest() {
  vector<int> source = {1,2,3,4,5,6};
  auto print = [](double i) { cout << i << endl; };
  source >> sum<int>() >> let<int>(print);
  source >> mean<int>() >> let<double>(print);
  source >> max<int>() >> let<int>(print);
  source >> min<int>() >> let<int>(print);
}
(出力)-> 
21
3.66667
6
1
```
インターフェース  
```cpp
sum<INPUT> -> INPUT
mean<INPUT> -> double
max<INPUT> -> INPUT
min<INPUT> -> INPUT
```

## flatten
入れ子構造になったvectorを展開します  
```cpp
void flattenTest() {
  vector<vector<int>> src = { {1,2},{2,3},{3,4},{4,5} };
  src >> flatten<int>() >> echo<int>();
}
(出力)-> 
[1,2,2,3,3,4,4,5]
```
インターフェースはこのようになっています  
```cpp
flatten<INPUT>() -> std::vector<INPUT>
```

## distinct
特定のキーで重複を消します(発見した最後の値のみが保存されます)  
```cpp
void distinctTest() {
  vector<pair<string,int>> src = { {"a",1}, {"b",2}, {"b",3}, {"c",4}, {"a",5} };
  src >> distinct<string, pair<string,int>>([](auto t){
    return get<0>(t);
  }) >> mapper<pair<string,int>,int>( [](auto t){
    auto [key,val] = t;
    cout << "DISTINCT: " << key << " VALUE: " << val << endl;
    return 0;
  }) ;
}
(出力)-> 
DISTINCT: a VALUE: 5
DISTINCT: b VALUE: 3
DISTINCT: c VALUE: 4
```

## mapperIndexed（インデックス付きマップ）
関数型におけるmapは並列性が担保されるべきで、本来はindexとかつけられないと思うのですが、一部のScalaやRubyなどの実装系においてはindexがついた状態でのmapが可能です  

同様のことをc++で行うと、このように用いることができます  
```cpp
void mapperIndexedTest() {
  vector<string> src = {"a", "b", "b", "c", "a"};
  src >> mapperIndexed<string, int>( [](int index, auto str) {
    cout << "Index: " << index << " Val: " << str << endl;
    return 0;
  });
}
(出力)-> 
Index: 0 Val: a
Index: 1 Val: b
Index: 2 Val: b
Index: 3 Val: c
Index: 4 Val: a
```
インターフェースは普通のmapと変わらないのですが、functorがindexを受け取れるようになっています  
```cpp
mapperIndexed<INPUT, OUTPUT>(FUNCTOR) -> std::vector<OUTPUT>
```

## シリアライザ, デシリアライザ
今回実装した内容は、プリミティブ型 + std::stringのみに対応したクラスや構造体のシリアライザとデシリアライザです  
C++はOSのメモリ空間を直接参照できるのでプリミティブな型(int, double)などはそのデータをそのままdumpすることができます  
std::stringなどは、クラスの値なので内部でメモリを内容をどれを取るか決める必要がありますが、char\*として表現できるので、char\*として保存して、復旧時にstd::stringに変換します  

C++というよりC言語芸なのですが、C++のtemplateとconstexprの型情報による分岐を組み合わせることで、このように、シリアライズとデシリアライズを行えます  
```cpp
struct Sample {
  int a;
  double b;
  std::string c;
  std::string serial() {
    return OD::SERIAL::dump(&a,this) + OD::SERIAL::SEPARATOR + OD::SERIAL::dump(&b,this) + OD::SERIAL::SEPARATOR + OD::SERIAL::dump(&c,this);
  };
};

// シリアライズとデシリアライズテスト
void testSerialDesrial() {
  auto sa1 = Sample{1, 2.0, "Hoge"};
  auto serial = sa1.serial();
  cout << "SERIAL: " << serial << endl;
  auto sa = OD::DESERIAL::recover<Sample>(serial);
  cout << "desrialized " << sa.a << " " << sa.b << " " << sa.c << " ," <<  endl;
}
(出力)-> 
SERIAL: int<Left>0<Left>01 00 00 00 <Left>4<Right>double<Left>8<Left>00 00 00 00 00 00 00 64 <Left>8<Right>std::string<Left>16<Left>Hoge<Left>32
desrialized 1 2 Hoge
```

## 並列性
マシン間をまたいでの並列性は担保していませんが、コンピュータ内部のCPUのコアぶんだけ並列でmap処理を行うことができます  
これはJavaとかの並列性が担保されたmapなどと同等だと思われ、処理順序が担保されないので、データのindexなどを気にしない処理に向いています  
例えば、一般的に重いと思われる素数判定を1から1,000,000まで行うとこの程度の時間が必要で、これはPythonの役10倍です  

```console
[C++でconcurrent::mapperを用いる場合]
$ time ./bin/a.out
real    3m4.833s
```
実験に用いたコード
```cpp
void testPrimeCheck() {
  OD::Range(1, 1000000) >> concurrent::mapper<int,pair<int,bool>>( [](int i) {
    bool isPrime = true;
    for(int s=2; s <= i/2; s++) {
      if(i%s == 0) {
        isPrime = false;
        break;
      }
    }
    return make_pair(i, isPrime);
  });
}
```

```console
[Python3で行う場合]
time python3 test.py  > d
real    33m21.345s 
```

実験に用いたコード
```python
for i in range(2,1000000):
  isPrime = True
  for j in range(2, i//2):
    if i%j == 0:
      isPrime = False
      break

  if isPrime:
    print(i)
```

## 総括
C++で他の言語で用いられているような、いくつかの便利な関数を用意することができました  
C++は、動的な言語に比べてコンパイル時に静的に型の情報が矛盾がないようにチェックされます  

例えば、この方法ばバッチ的に回す分析でできるだけ、高速に処理したいなどのモチベーションに向いており、AWSのサービスと組み合わせて定常分析に用いるなどが考えられるかもしれません  

