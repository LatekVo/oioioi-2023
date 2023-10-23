#include <iostream>
#include <map>
#include <array>

struct Point {
	int x = -1;
	int y = -1;
	bool isOn = false;
};

bool checkParity(std::array<bool, 100000> &parityContainer, int containerSize) {
	for (int i = 0; i < containerSize; i++) {
		
	}
	return false;
}

int main() {
	// my idea is that we will be treating all of the points as a single number, incrementing it by one,
	// switching all the rows and columns accordingly, and seeing if their parity is right

	/* "TAK" REQUIREMENTS:
		* at least one button activated
		* every column has to have the same parity of clicked buttons
		  ^ (either every column and row has even amount of clicked buttons, or uneven amount)
	   ELSE RESPOND WITH "NIE" */

	// since we are not dealing with a 2d field, but just a list of buttons and 2 lists of parity,
	// the execution time will always be O(n) for most of the operations here.
	// Unfortunately, since every single time we check a point, we also have to check the parity
	// of all rows, the O of time could skyrocket to something like 2*n*m (m is n^2 or less),
	// with all this that could be O(n^3), yikes, parity checking feels like it can be optimised,
	// with some cache, oh yeah, we can see how much is missing to parity in the best case scenario,
	// then we check parity only after doing this much moves, if it does not check out, we calculate new
	// best case scenario and don't check the parity until we reach that! This is a very important note to take,
	// but for now I will only focus on the standard algo, then if it works i will apply this optimalization.
	// That is, if it will be even neccesary, these are some crazy numbers but after completing budzad i am
	// quite surprised with how fast modern computers are.

	int panelSize, buttonAmount;
	std::cin >> panelSize >> buttonAmount;

	std::array<bool, 100000> verticalParity({0});
	std::array<bool, 100000> horizontalParity({0}); 
	std::array<Point, 500000> startingPoints;

	// transfer data to field vvv
	for (int i = 0; i < buttonAmount; i++) {
		int x, y;
		// orig: cin >> row >> column
		std::cin >> y >> x;
		Point newPoint;
		newPoint.x = x;
		newPoint.y = y;
		startingPoints[i] = newPoint;
	}

	for (int i = 0; i < buttonAmount; i++) {
		
	}

}
