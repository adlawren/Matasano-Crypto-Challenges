// References:
// - https://diego.assencio.com/?index=6890b8c50169ef45b74db135063c227c
// - https://www.cplusplus.com/reference/random/mersenne_twister_engine/mersenne_twister_engine/

#include <random>
#include <iostream>
#include <fstream>

int main(int argc, char **argv) {
  int seed = 0;
  if (argc > 1) {
    seed = atoi(argv[1]);
  }

  int count = 1000;
  if (argc > 2) {
    count = atoi(argv[2]);
  }

  std::string file_path = "21.txt";
  if (argc > 3) {
    file_path = argv[3];
  }

  std::mt19937 generator(seed);
  std::ofstream file;
  file.open(file_path);
  for (size_t i = 0; i < count; ++i) {
    file << generator() << std::endl;
  }
  file.close();

  return 0;
}
