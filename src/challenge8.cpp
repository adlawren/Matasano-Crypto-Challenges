#include <algorithm>
#include <fstream>
#include <vector>

#include "ByteSequence.hpp"

int main(int argc, char *argv[]) {
  std::ifstream fileStream("challenge_8_ciphertext.txt");

  std::vector<std::pair<ByteSequence, float>>
      byteSequenceAverageHammingDistancePairs;

  std::string nextLine;
  while (std::getline(fileStream, nextLine)) {
    ByteSequence byteSequence;
    byteSequence.initializeFromAsciiBytes(
        std::vector<char>(&nextLine[0], &nextLine[nextLine.size()]));

    auto averageHammingDistance = 0.0f;

    auto blockCount = byteSequence.getByteCount() / 16;
    for (unsigned i = 1; i < blockCount; ++i) {
      auto subByteSequence1 = byteSequence.getSubSequence((i - 1) * 16, 16);
      auto subByteSequence2 = byteSequence.getSubSequence(i * 16, 16);

      auto normalizedHammingDistance =
          (float)subByteSequence1.getHammingDistance(subByteSequence2) / 16.0f;

      averageHammingDistance += normalizedHammingDistance / blockCount;
    }

    byteSequenceAverageHammingDistancePairs.push_back(
        std::pair<ByteSequence, float>(byteSequence, averageHammingDistance));
  }

  std::sort(byteSequenceAverageHammingDistancePairs.begin(),
            byteSequenceAverageHammingDistancePairs.end(),
            [](const std::pair<ByteSequence, float> &pair1,
               const std::pair<ByteSequence, float> &pair2) {
              return pair1.second > pair2.second;
            });

  auto average = 0.0f;
  for (unsigned i = 1; i < byteSequenceAverageHammingDistancePairs.size();
       ++i) {
    average += byteSequenceAverageHammingDistancePairs[i].second /
               byteSequenceAverageHammingDistancePairs.size();
  }

  auto differenceVariance = 0.0f;
  for (unsigned i = 1; i < byteSequenceAverageHammingDistancePairs.size();
       ++i) {
    auto temp = byteSequenceAverageHammingDistancePairs[i].second - average;
    differenceVariance +=
        (temp * temp) / byteSequenceAverageHammingDistancePairs.size();
  }

  auto quartile1 = 0.0f;
  if ((byteSequenceAverageHammingDistancePairs.size() + 1) % 4 == 0) {
    auto position = (byteSequenceAverageHammingDistancePairs.size() + 1) / 4;

    quartile1 = byteSequenceAverageHammingDistancePairs[position].second;
  } else {
    auto position1 = (byteSequenceAverageHammingDistancePairs.size() + 1) / 4;
    auto position2 = position1 + 1;

    quartile1 = (byteSequenceAverageHammingDistancePairs[position1].second +
                 byteSequenceAverageHammingDistancePairs[position2].second) /
                2.0f;
  }

  auto quartile3 = 0.0f;
  if ((byteSequenceAverageHammingDistancePairs.size() + 1) % 4 == 0) {
    auto position =
        3 * ((byteSequenceAverageHammingDistancePairs.size() + 1) / 4);

    quartile3 = byteSequenceAverageHammingDistancePairs[position].second;
  } else {
    auto position1 =
        3 * ((byteSequenceAverageHammingDistancePairs.size() + 1) / 4);
    auto position2 = position1 + 1;

    quartile3 = (byteSequenceAverageHammingDistancePairs[position1].second +
                 byteSequenceAverageHammingDistancePairs[position2].second) /
                2;
  }

  auto lowerOutlierBoundary = quartile1 - (1.5f * (quartile3 - quartile1));

  std::vector<std::pair<ByteSequence, float>> lowerOutliers;
  for (auto byteSequenceAverageHammingDistancePair :
       byteSequenceAverageHammingDistancePairs) {
    if (byteSequenceAverageHammingDistancePair.second < lowerOutlierBoundary) {
      lowerOutliers.push_back(byteSequenceAverageHammingDistancePair);
    }
  }

  if (lowerOutliers.size() == 0) {
    std::cout << "No outliers found" << std::endl;
  } else {
    lowerOutliers[0].first.printBase64EncodedString();
  }
}
