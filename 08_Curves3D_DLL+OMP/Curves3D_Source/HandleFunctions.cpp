#include "HandleFunctions.h"
#include <iostream>
#include <algorithm>
#include <vector>
#include <omp.h>

std::vector<std::unique_ptr<Curve>> MakeVectorOne(size_t CurvesNumber, size_t MaxRariusSize) {
	// Seed the random number generator
	std::srand(static_cast<unsigned int>(std::time(nullptr)));

	std::vector<std::unique_ptr<Curve>> curves;

	size_t HelixStep = MaxRariusSize / 2;

	for (size_t i = 0; i < CurvesNumber; ++i) {
		int choice = std::rand() % 3;

		if (choice == 0) {
			double radius = static_cast<double>(std::rand() % MaxRariusSize + 1);
			curves.push_back(std::make_unique<Circle>(radius));
		}
		else if (choice == 1) {
			double radiusX = static_cast<double>(std::rand() % MaxRariusSize + 1);
			double radiusY = static_cast<double>(std::rand() % MaxRariusSize + 1);
			curves.push_back(std::make_unique<Ellipse>(radiusX, radiusY));
		}
		else {
			double radius = static_cast<double>(std::rand() % MaxRariusSize + 1);
			double step = static_cast<double>(std::rand() % HelixStep + 1);
			curves.push_back(std::make_unique<Helix>(radius, step));
		}
	}
	return curves;
}

void PrintCoordinates(const std::vector<std::unique_ptr<Curve>>& curves) {
	if (curves.empty()) return;

	const double t = std::numbers::pi / 4;

	for (const auto& curve : curves) {
		Point3D point = curve->getPoint(t);
		Vector3D derivative = curve->getDerivative(t);

		std::cout << "Type: ";
		if (dynamic_cast<Circle*>(curve.get())) {
			std::cout << "Circle";
		}
		else if (dynamic_cast<Ellipse*>(curve.get())) {
			std::cout << "Ellipse";
		}
		else if (dynamic_cast<Helix*>(curve.get())) {
			std::cout << "Helix";
		}

		std::cout << "\nPoint at t = PI/4" << ": (" << point.x << ", " << point.y << ", " << point.z << ")\n";
		std::cout << "Derivative at t = PI/4" << ": (" << derivative.x << ", " << derivative.y << ", " << derivative.z << ")\n";
	}
}

std::vector<std::unique_ptr<Circle>> MakeVectorTwo(const std::vector<std::unique_ptr<Curve>>& curves) {
	std::vector<std::unique_ptr<Circle>> circleVector;
	if (curves.empty()) return circleVector;

	for (const auto& curve : curves) {
		if (auto circle = dynamic_cast<Circle*>(curve.get())) {
			circleVector.push_back(std::make_unique<Circle>(*circle));
		}
	}

	return circleVector;
}

void SortCircles(std::vector<std::unique_ptr<Circle>>& circles) {
	if (circles.empty()) return;

	std::sort(circles.begin(), circles.end(),
		[](const std::unique_ptr<Circle>& circle1, const std::unique_ptr<Circle>& circle2) {
			return circle1->getRadius() < circle2->getRadius();
		});
}

double TotalSumOfRadii(const std::vector<std::unique_ptr<Circle>>& circles) {
	if (circles.empty()) 0.0;

	double sum = 0.0;
	int circlesNum = circles.size();

#pragma omp parallel for reduction(+:sum)
	for (int i = 0; i < circlesNum; ++i) {
		sum += circles[i]->getRadius();
	}
	return sum;
}