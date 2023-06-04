//
// Created by Matq on 04/06/2023.
//

#include "Randomizer.h"

Randomizer::Randomizer()
    : m_32Generator(std::random_device{}()),
      m_64Generator(std::random_device{}()),
      m_FloatPercentageDist(0.0f, 1.0f),
      m_DoublePercentageDist(0.0, 1.0) {

}

Randomizer::~Randomizer() = default;

unsigned int Randomizer::UnsignedInt() {
    return m_UIntDist(m_32Generator);
}

int Randomizer::Int() {
    return m_IntDist(m_32Generator);
}

float Randomizer::Float() {
    return m_FloatDist(m_32Generator);
}

float Randomizer::PercentageFloat() {
    return m_FloatPercentageDist(m_32Generator);
}

unsigned long long Randomizer::UnsignedLongLong() {
    return m_UnsignedLongLongDist(m_64Generator);
}

long long Randomizer::LongLong() {
    return m_LongLongDist(m_64Generator);
}

double Randomizer::Double() {
    return m_DoubleDist(m_64Generator);
}

double Randomizer::PercentageDouble() {
    return m_DoublePercentageDist(m_64Generator);
}
