#include <iostream>
#include <map>
#include <array>

struct Point {
	int x = -1;
	int y = -1;
	bool isOn = false;
};

int panelSize = -1, buttonAmount = -1, activatedButtons = 0;
std::array<bool, 100000> verticalParity({0});
std::array<bool, 100000> horizontalParity({0}); 
std::array<Point, 500000> buttonList;

int optimisticToEven = 0;
int optimisticToOdd = 0;

bool isParityHomogeneous() {
	optimisticToOdd = 0;
	optimisticToEven = 0;
	bool isHomogeneous = true;
	bool desiredParity;
	desiredParity = horizontalParity[0];
	for (int i = 1; i < panelSize; i++) {
		if (horizontalParity[i] != desiredParity || verticalParity[i] != desiredParity) {
			isHomogeneous = false;
		}
		if (horizontalParity[i] % 2 == 0) {
			optimisticToOdd++;
		} else {
			optimisticToEven++;
		}
		if (verticalParity[i] % 2 == 0) {
			optimisticToOdd++;
		} else {
			optimisticToEven++;
		}
	}
	return isHomogeneous;
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

	// another approach instead of brute forcing every single combination would be to check every single outcome.
	// first we try for odd, but no, we can't do that as 'odd' may mean 1, 3, 5 or anything like that up to
	// 100000, and then in any combination possible

	/* PLAN FOR V2:
		- identify sole points of decision about parity, exclude them from the bruteforcing list
	   	- 'optimistic distance' optimalization
		- identify required points, as well as impossible points, and required rows.
		 ^ example would be a row with no buttons, or a row with 2 sole buttons only,
		 ^ in case of 2 sole points in one row, both HAVE TO be disabled, 
		 ^ otherwise their row would be even while their columns both odd
	*/

	std::cin >> panelSize >> buttonAmount;

	// transfer data to field vvv
	for (int i = 0; i < buttonAmount; i++) {
		int x, y;
		// orig: cin >> row >> column
		std::cin >> y >> x;
		Point newPoint;
		newPoint.x = x - 1; // offset from min 1 to min 0
		newPoint.y = y - 1; // offset from min 1 to min 0
		buttonList[i] = newPoint;
	}

	// ok so with this approach for now, treat all buttons as a sigle binary number, keep on incrementing first one by 1
	// until, oh well that is 2^500000 max, isn't it? 2^500000 increments is an ungodly number, but if we implement the optimisticDistance function
	// this may make some sense, but how do we check if it's the end of the loop? it's not like we can just see if i >= 2^500000
	// Oh no i just calculated what this number is and there is no way any computer could do that, will still try just in case im mistaken with the number
	// of iterations needed :) Ah repeating myself, but how much will it be? For m=4 it will be 1000, 0100, 1100, 0010, 1010, 0110, 1110, 0001, 1001, 0101, 1101, 0011, 1011, 0111, 1111
	// oh yeah this is definietely 2^m-1 of iterations alone, damn, still 24pts for just m <= 20
	
	// anoter approach would be to jump through possible parities? not sure how that would work now that i am thinking about it.

	do {
		bool carryOver = true; // true for the first element
		for (int additionIndex = 0; additionIndex <= buttonAmount; additionIndex++) {
			if (additionIndex == buttonAmount) {
				// todo: if addition has gone out of bounds this means our competion reached 100%, meaning our buttonList is now representing the number number 1111...1,
				// we should finally evaluate everything and break the loop
				std::cout << "NIE";
				return 0;
			}
			// iterate first member
			// if current index already 1, increment index, carry over
			if (carryOver == true) {
				auto btnObj = &buttonList[additionIndex];
				// parity switched either way
				horizontalParity[btnObj->x] = !horizontalParity[btnObj->x];
				verticalParity[btnObj->y] = !verticalParity[btnObj->y];
				if (btnObj->isOn == true) {
					activatedButtons--;
					btnObj->isOn = false;
					carryOver = true;
					if (horizontalParity[btnObj->x] % 2 == 0) {
						optimisticToOdd++;
						optimisticToEven++;
					} else {
						optimisticToEven++;
						optimisticToOdd++;
					}
				} else {
					activatedButtons++;
					btnObj->isOn = true;
					carryOver = false;
					break;
				}
			} else {
				break;
			}
		}

		// optimalization condition met
		if (optimisticToOdd <= 0 || optimisticToEven <= 0) {
			// success condition + output
			if (isParityHomogeneous()) {
				std::cout << "TAK" << std::endl;
				std::cout << activatedButtons << std::endl;
				// find switched buttons, list their index + 1
				for (int i = 0; i < buttonAmount; i++) {
					if (buttonList[i].isOn == true) {
						std::cout << i + 1 << ' ';
					}
				}
				return 0;
			}
		}
	} while (1);

	std::cout << "NIE";
	return 0;
}
