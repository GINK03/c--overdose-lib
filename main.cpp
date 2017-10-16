#include <iostream>
#include <string>
#include <vector>
#include "Split.hpp"
#include "Open.hpp"
#include "Vec.hpp"
#include "Ope.hpp"
#include "Range.hpp"
#include "Enumerate.hpp"
#include "List.hpp"
#include "ListArrow.hpp"
#include "Arrow.hpp"
using namespace std;
namespace OD=OVERDOSE;
using namespace OVERDOSE_EXT;

class Data {
  public:
  int id;
  string firstName;
  string lastName;
};
void classMapTest() {
  vector<Data> data = {{1, "Doe", "Smith"}, {2, "Doe", "Alice"}, {3, "Drown", "Bob"} };
  auto ret = data >> mapper<Data,Data>( [](Data& data){ 
    data.id += 1;
    return data;
  })
  >> mapper<Data,int>( [](Data& data) { 
    cout << "Data id:"<< data.id << " " << data.firstName << " " << data.lastName << endl;
    return data.id;
  })
  >> reducer<int>( 0, [](int& y, int x) { 
    y += x;
  });
  cout << "class sum of id: " << ret << endl;
  
  auto ret2 = data >> mapper<Data,string>( [](Data& data) {
    string a = data.firstName + " " + data.lastName;
    return a;
  }) 
  >> reducer<string>( string(""), [](string& y, string x) {
    y += x + " ";
  } );
  cout << "name combined of data: " << ret2 << endl;

  OD::Range(0,100) >> mapper<int,int>( [](int i) {
    string echo = "";
    if( i%15 == 0 ) {
      echo = "FizzBuzz"; 
    } else if( i%5 == 0 ) {
      echo = "Fizz"; 
    } else if( i%3 == 0 ) {
      echo = "Buzz"; 
    } else {
      echo = to_string(i);
    }
    cout << "FizzBuzz: " << i << " " << echo << endl;
    return 0;
  });
}

int main() {
  classMapTest(); 
  return 1;
  cout << "a" << endl;
  vector<int> as = OD::Range(1,100);
  Vec vs(as);
  vs.print();
  vs.print(",");
  vs.test( [](){ cout << "test" << endl; } );
  // いくつかのラムダ式の写像による
  // データ変換の例
  auto y = vs.map<int>( [](int i){ 
      return i*i; 
    } )
    .map<double>( [](int i){
      return double(i) + 0.5;
    })
    .map<int>( [](double i){
      return int(i*10); 
    })
    .filter( [](int i){
      return i%3 == 0;
    })
    .reduce(1, [](int& y, int i) {
      y = y + i;
    });
  cout << "chaine desc," << y << endl;

  // メソッドチェーンの機能に
  // orderByの機能の例
  vs.map<int> ( [](int i) {
    return i*-1;
  })
  .orderBy ( [](int a, int b) {
    return a < b;
  })
  .print(",");

  // 関数の合成で構築する
  auto f = merge<int, int>( [](int i) { return i*3; }, [](int i) { return i*i; } );
  vs.map<int>(f).print(",");
  
  // Enumerateの例
  vector<string> en = {"a", "b", "c"};
  cout << " AAAAA " << endl;
  for( auto [i,s]: OD::Enumerate(en) ) {
    cout << "A:" << i << " " << s << endl;
  }

  // splitしてEnumerateする際
  for(auto [i, str]: OD::Enumerate( OD::Split("1:2:3:4:5", ":")) ) {
    cout << "Split:" << i << " " << str << endl;
  }

  // Openの便利かんすう
  for( auto line : OD::Open("./Makefile") ) {
    cout << "Opened: " << line << endl; 
  }

  // listのstdの拡張
  OD::List l = OD::Range(1,1000);
  l.map<int>( [](int i){ 
    cout << "step1 " << i << endl; 
    return i*3;
  } )
  /*.map<int>( [](int i ){ 
    cout << "step2 " << i << endl;
    return i%5;
  }).toSet<int>()*/
  .map<int>( [](int i) {
    cout << "step3 " << i << endl;
    return i;
  }).groupBy<int>( [](int i) {
    return i%5;
  }).map<int>( [](tuple<int,OD::List<int>> pair) {
    auto key = std::get<0>(pair);
    auto vals = std::get<1>(pair);
    cout << "step4 " << key << endl;
    vals.print(",");
    return 0 ;
  });
  auto a = "10 11 12 1 2 3" >> split >> mapper<string,int>([](string str){return stoi(str);}) >> sum<int>;
  cout << "sum: " << a << endl;
  auto li = "3 5 7 9 11" 
       >> split 
       >> mapper<string,double>([](auto str){return stod(str);}) 
       >> reducer<double>(1.0, [](auto& y, auto d){ return y *= d;}) 
       >> let<double>( [](auto d) { cout << "End: " << d << endl; return d; });
} 
