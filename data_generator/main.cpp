#include <fstream>
#include <iostream>
#include <iomanip>
#include <random>
#include <types.hpp>

bclasses::MessageStruct genMessage() {
  static std::random_device rd;
  static std::mt19937 gen(rd());
  static std::uniform_int_distribution<decltype(bclasses::MessageStruct::MessageId)> id(0, 20);
  static std::uniform_int_distribution<decltype(bclasses::MessageStruct::MessageType)> type(0, 50);
  static std::uniform_int_distribution<decltype(bclasses::MessageStruct::MessageData)> data(0, 0xFFFF);
  return {sizeof(bclasses::MessageStruct), type(gen), id(gen), data(gen)};
}

int main() {
  std::cout << "\n{\n";
  constexpr unsigned kLimit = 100-1;
  for(unsigned i = 0; i < kLimit; ++i){
    std::cout << genMessage() << ",\n";
  }
  std::cout << genMessage() << " \n}\n";
  bclasses::MessageStruct tmp{sizeof(bclasses::MessageStruct),static_cast<uint8_t>(-1),static_cast<uint64_t>(-1),static_cast<uint64_t>(-1)};
  std::cout << '\n' << tmp << '\n';
  return 0;
}
