#include <complex>
#include <cmath>

template <class T, size_t LENGTH>
class SlidingDFT
{
private:
  std::array<T, LENGTH> samples;
  size_t xPos;
  std::array<std::complex<T>, LENGTH> exponents;
  std::array<std::complex<T>, LENGTH> S;
public:
  SlidingDFT() :
    S{},
    x{},
    lastX(0),
    xPos(0) {
    for (size_t i = 0; i < LENGTH; i++) 
      exponents[i] = std::polar(1,  T(2.0 * M_PI) * i / LENGTH);
  }
  
  void update(T x) {
    auto xLast = samples[xPos];
    samples[xPos] = x;
    xPos = (xPos + 1)%LENGTH;
    
    for (size_t i = 0; i < LENGTH; i++)
      S[i] = exponents[i]*S[i] - lastX + x;
  }
};
