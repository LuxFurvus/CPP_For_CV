#include <iostream>
#include "HandleFunctions.h"

int main() {
	bool KeepGoing = true;

	while (KeepGoing) {
		auto VectorOne = MakeVectorOne();
		PrintCoordinates(VectorOne);

		auto VectorTwo = MakeVectorTwo(VectorOne);
		std::cout << "\n\nTotal Number of Circles: " << VectorTwo.size() << '\n';

		SortCircles(VectorTwo);
		std::cout << "\n\nRadii of Circles:\n";
		for (const auto& circle : VectorTwo) {
			std::cout << '\t' << circle->getRadius() << '\n';
		}

		double totalSum = TotalSumOfRadii(VectorTwo);
		std::cout << "\n\nTotal Sum of Radii: " << totalSum << '\n';

		std::cout << "\nAgain? y/n\n";

		char Answer{};
		std::cin >> Answer;  // Read the user's input

		if (Answer != 'y' && Answer != 'Y') {
			KeepGoing = false;  // Exit the loop if the user's input is not 'y' or 'Y'
		}
	}

	return 0;
}