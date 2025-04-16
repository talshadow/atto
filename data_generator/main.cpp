#include <fstream>
#include <iomanip>
#include <iostream>
#include <random>
#include <types.hpp>

bclasses::MessageStruct genMessage()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<decltype(bclasses::MessageStruct::MessageId)> id(0, 0x7FF);
    static std::uniform_int_distribution<decltype(bclasses::MessageStruct::MessageType)> type(0, 0x78);
    static std::uniform_int_distribution<decltype(bclasses::MessageStruct::MessageData)> data(0, 0xFF);
    return {sizeof(bclasses::MessageStruct), type(gen), id(gen), data(gen)};
}

int main()
{
    std::cout << "\n{\n";
    constexpr unsigned kLimit = 100000;
    unsigned Data10{0};
    for (unsigned i = 0; i < kLimit; ++i) {
        bclasses::MessageStruct data{genMessage()};
        std::cout << data << ",\n";
        if (data.MessageData == 10) {
            ++Data10;
        }
    }
    bclasses::MessageStruct tmp{sizeof(bclasses::MessageStruct),
                                static_cast<uint8_t>(-1),
                                static_cast<uint64_t>(-1),
                                static_cast<uint64_t>(-1)};
    std::cout << tmp << "\n};\n";
    std::cout << "Count of data 10: " << std::dec << Data10 << '\n';
    return 0;
}
