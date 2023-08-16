#pragma once

#ifndef HANDLE_FUNCTIONS_H
#define HANDLE_FUNCTIONS_H

#include <vector>
#include <memory>
#include "CurvesClasses.h"

std::vector<std::unique_ptr<Curve>> MakeVectorOne(
	size_t CurvesNumber = 10, size_t MaxRariusSize = 100);

void PrintCoordinates(const std::vector<std::unique_ptr<Curve>>& circles);

std::vector<std::unique_ptr<Circle>> MakeVectorTwo(
	const std::vector<std::unique_ptr<Curve>>& circles);

void SortCircles(std::vector<std::unique_ptr<Circle>>& circles);

double TotalSumOfRadii(const std::vector<std::unique_ptr<Circle>>& circles);

#endif // HANDLE_FUNCTIONS_H
