#include <array>
#include <iomanip>
#include <iostream>

#include "../WodCpp/WodChanceCounter.h"

int main() {

const long double WithoutCancel =
    WodChanceCounter::GetProbabilityChance(
        5,
        6,
        6,
        false,
        true);

const long double WithCancel =
    WodChanceCounter::GetProbabilityChance(
        5,
        6,
        6,
        true,
        true);

std::cout << "Without cancel: " << std::fixed << std::setprecision(4) << WithoutCancel * 100.0L << '%' << std::endl;
std::cout << "With cancel: " << std::fixed << std::setprecision(4) << WithCancel * 100.0L << '%' << std::endl;
}