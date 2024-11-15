//
// Created by Matq on 04/06/2023.
//

#pragma once

#include <random>

class Randomizer {
private:

    // 32-Bit
    std::mt19937 m_32Generator;
    std::uniform_int_distribution<unsigned int> m_UIntDist;
    std::uniform_int_distribution<int> m_IntDist;
    std::uniform_real_distribution<float> m_FloatDist;
    std::uniform_real_distribution<float> m_FloatPercentageDist;

    // 64-Bit
    std::mt19937_64 m_64Generator;
    std::uniform_int_distribution<unsigned long long> m_UnsignedLongLongDist;
    std::uniform_int_distribution<long long> m_LongLongDist;
    std::uniform_real_distribution<double> m_DoubleDist;
    std::uniform_real_distribution<double> m_DoublePercentageDist;

public:
    Randomizer();
    ~Randomizer();

    // 32-Bit Generators
    [[nodiscard]] unsigned int UnsignedInt();
    [[nodiscard]] int Int();
    [[nodiscard]] float Float();
    [[nodiscard]] float PercentageFloat();

    // 64-Bit Generators
    [[nodiscard]] unsigned long long UnsignedLongLong();
    [[nodiscard]] long long LongLong();
    [[nodiscard]] double Double();
    [[nodiscard]] double PercentageDouble();

};
