#include "getSymbol.cc"
#include <iostream>
#include <fstream>
#include <numeric>
#include <cmath>

int main(){
  constexpr size_t SF = 7;
  constexpr size_t OSF = 80;
  constexpr size_t nUpchirps = 5;
  constexpr uint8_t syncwordNumber = 0x68;

  //simple symbol test
  
  auto sym = gr::LibreLoRa::getSymbol((1 << (SF - 2)), SF, OSF);

  std::ofstream file;
  file.open("out.txt");
  for(auto& x : sym)
    file << x << std::endl;
  file.close();

  //preamble gen test
  std::vector<std::vector<float>> preamble;
  //generate preamble
  std::vector<size_t> symNums;

  //upchirps
  for(size_t i = 0; i < nUpchirps; i++)
    symNums.push_back(0);

  //calculate sync word symbols from sync word number
  symNums.push_back((1 << (SF - 5))*(syncwordNumber >> 4));
  symNums.push_back((1 << (SF - 5))*(syncwordNumber & 0xf));

  for(auto& symNum : symNums) {
    preamble.push_back(gr::LibreLoRa::getSymbol(symNum, SF, OSF));
  }

  auto downchirp = gr::LibreLoRa::getSymbol(0, SF, OSF);
  for(auto& x : downchirp)
    x = -x;

  preamble.push_back(downchirp);
  preamble.push_back(downchirp);

  std::vector<float> fractionOfDownchirp;
  fractionOfDownchirp.insert(fractionOfDownchirp.begin(), downchirp.begin(), downchirp.begin() + (((1 << (SF - 2)) - 1)*OSF - 1));
  preamble.push_back(fractionOfDownchirp);

  size_t preambleSize = 0;
  float mean = 0;
  
  //normalize and make preamble zero-mean
  for(auto& sym : preamble) {
    for(auto& x : sym)
      mean += x;
    preambleSize += sym.size();
  }

  mean /= preambleSize;
  std::cout << "mean " << mean << std::endl;
  
  for(auto& sym : preamble)
    for(auto& x : sym)
      x -= mean;

  float norm = 0;
  for(auto& sym : preamble)
    for(auto& x : sym)
      norm += x*x;

  norm = sqrt(norm);

  std::cout << "preamble norm = " << norm << std::endl;
      
  for(auto& sym : preamble)
    for(auto& x : sym)
      x /= norm;

  //print preamble to file
  file.open("preamble.txt");
  for(auto& sym : preamble)
    for(auto& x : sym)
      file << x << std::endl;
  return 0;
}
