#include <vector>
#include <string>
#include <functional>
#include <iostream>
#include <regex>
#include "List.hpp"
#pragma once
namespace OVERDOSE_EXT {
  template<typename F> 
  auto operator>>(const std::string& str, const F& f) { 
    return f(str); 
  }
  template<typename F> 
  void operator%(int source, const F& f) { return f(source); }

  template<typename T>
  void echo1(const T& source) { std::cout << source << std::endl; };  
  void echo1(const int& source) { std::cout << source << std::endl; };  

  template<typename F> 
  auto operator>>(const double& source, const F& f) { 
    return f(source); 
  }
  template<typename F> 
  auto operator>>(const std::vector<std::string>& source, const F& f) { 
    return f(source); 
  }
  template<typename U, typename F> 
  auto operator>>(const std::vector<U>& source, const F& f) { 
    return f(source); 
  }
  std::function< std::vector<std::string>(std::string) > split = [](std::string source) {
    std::vector<std::string> result;
    std::regex reg{" "};
    std::copy(
      std::sregex_token_iterator(source.begin(), source.end(), reg, -1),
      std::sregex_token_iterator(),
      std::back_inserter(result)
    );
    return result;
  };

  template<typename T>
  void echo(const std::vector<T>& source) {
    for(auto s: source) {
      std::cout << s << " ";
    }
    std::cout << std::endl;
  };  
  
  template<typename T>
  T sum(const std::vector<T>& source) {
    auto y = std::decay_t<decltype(source[0])>();
    for(auto s: source) {
      y += s;
    }
    return y;
  };  

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

  template<typename R, typename F>
  auto reducer(const R& r, const F& f) {
    return [r, f](std::vector<R> source) {
      R res = R(r);
      for(auto s:source) {
        f(res, s); 
      }
      return res;
    };
  }

  template<typename R,typename F>
  auto let(const F& f)  { 
    //using Decl = decltype(f());
    return [f](const R& r) {
      return f(r);
    };
  }
};
