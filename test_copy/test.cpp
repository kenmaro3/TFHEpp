#include <limits>
#include <iostream>

using namespace std;

int main(){
  int32_t a = 3;
  uint32_t b = 59;

  cout << std::numeric_limits<uint32_t>::max() << endl;
  cout << std::numeric_limits<int32_t>::max() << endl;

  double tmp1 = 0.2;
  tmp1 = tmp1 * 10.;
  int tmp2 = 2;
  cout << static_cast<uint32_t>(tmp1) << endl;
  cout << static_cast<uint32_t>(tmp2) << endl;

}
