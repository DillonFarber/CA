#include<iostream>

using namespace std; 

int main() {

  float x = -1e+14;
  float y = 1e+14;
  float z = 1.00; 

  cout << x + y + z << endl;
  cout << (x + y) + z << endl;
  cout << x + (y + z) << endl;

  return 0;

}
