#include <cstdlib>
#include <vector>

#include "ByteSequence.hpp"

// Parameters
const size_t BUFFER_LENGTH = 256;

int main(int argc, char* argv[])
{
  char buffer[BUFFER_LENGTH];
  int bytesRead;

  scanf("%s%n", buffer, &bytesRead);

  ByteSequence byteSequence;
  byteSequence.initializeFromHexEncodedAsciiBytes(
    std::vector<char>(&buffer[0], &buffer[bytesRead]));
  byteSequence.printBase64EncodedString();

  return 0;
}
