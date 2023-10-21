#include <algorithm>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <string>
#include <sstream>
#include <array>
#include <vector>

enum class Stat {
	X = 0,
	Y = 1,
	ORIENTATION = 2,
	LENGTH = 3
};

// this whole bit section is a subject to change
enum class Bit {
	FILLED = 		0b00000001, // both versions
	HOR_CHECKED = 	0b00000010, // old - 1.0
	VER_CHECKED = 	0b00000100, // old - 1.0
	// vvv latest implementation bits vvv
	USED = 			0b00001000, // used in 2.0
	START = 		0b00010000, // used in 2.0
};

struct StartingPoint {
	int x = -1, y = -1, rot = -1; // rot (0=h, 1=v)
	int length = -2;
};

// collision detection: (1, 2 - runways, O, T - origin and target points)
// vvv collision vvv
// 1hor, 2ver X_O1 <= X_O2 && X_T1 >= X_T2 && Y_O1 >= Y_O2 && Y_T1 <= Y_T2 // Xs of 2 within Xs of 1, Ys of 1 within Ys of 2
// 1ver, 2hor X_O1 >= X_O2 && X_T1 <= X_T2 && Y_O1 <= Y_O2 && Y_T1 >= Y_T2 // Xs of 1 within Xs of 2, Ys of 2 within Ys of 1
// vvv overlap vvv
// 1hor, 2hor Y_O1 == Y_O2 && (X_O2 <= X_O1 <= X_T2 || X_O2 <= X_T1 <= X_T2) // O1 between O2 and T2, or T1 between O2 and T2
// 1ver, 2ver X_O1 == X_O2 && (Y_O2 <= Y_O1 <= Y_T2 || Y_O2 <= Y_T1 <= Y_T2) // O1 between O2 and T2, or T1 between O2 and T2
bool doesCollide(int x1, int y1, int len1, int r1, int x2, int y2, int len2, int r2) {
	// TODO: UNIT TEST THIS FUNCTION FOR ALL EXTREME CASES
	int X_O1 = x1, Y_O1 = y1;
	int X_T1 = x1, Y_T1 = y1;
	int X_O2 = x2, Y_O2 = y2;
	int X_T2 = x2, Y_T2 = y2;

	if (r1 == 0) {
		X_T1 += len1 + 1;
	} else {
		Y_T1 += len1 + 1;
	}
	if (r2 == 0) {
		X_T2 += len2 + 1;
	} else {
		Y_T2 += len2 + 1;
	}

	if (r1 == 0 && r2 == 1) {
		return X_O1 <= X_O2 && X_T1 >= X_T2 && Y_O1 >= Y_O2 && Y_T1 <= Y_T2;
	}
	if (r1 == 1 && r2 == 0) {
		return X_O1 >= X_O2 && X_T1 <= X_T2 && Y_O1 <= Y_O2 && Y_T1 >= Y_T2;
	}
	if (r1 == 0 && r2 == 0) {
		// overlap will never occur in our circumstances
		// return Y_O1 == Y_O2 && (X_O2 <= X_O1 <= X_T2 || X_O2 <= X_T1 <= X_T2);
		return false;
	}
	if (r1 == 1 && r2 == 1) {
		// overlap will never occur in our circumstances
		// return X_O1 == X_O2 && (Y_O2 <= Y_O1 <= Y_T2 || Y_O2 <= Y_T1 <= Y_T2);
		return false;
	}

	return false;
}

int main() {
	int airportSize = 0;
	short runways = 0;
	std::cin >> airportSize >> runways;

	// x, y, orientation (h=0), length
	int longestOne[4] = {0,0,0,0}; 
	int longestFinal[4] = {0,0,0,0}; // termination and parity achieved
	int maxLength = -1;

	// 4 approaches for 2 airstrips:
	// 1: Find max then find match (quickest method, but it enables local lows for airstrip 1, with no counterpart for airstrip 2)
	// 2: Find for n=1, n=2, n=3, n=..., n=airportSize (100% precise, easy to redo from 1, but slowest of 1, 2 and 3)
	// 3: Find for n=airportSize, n=..., n=3, n=2, n=1 (100% precise, no traps and lookbacks unlike in the approach 1)
	// 4: Find for n=airportSize/2, n=binarySearch (logarithmic speed, QUICKEST OF ALL, does not allow traps, may be overkill for n<1500)
	// for 1 airstrip approach 1 is always the best
	// 3 may be near impossible to imprement as we will have to use the algorithm for 1 anyways, 
	// ^ better to cache results, then perhaps find the best match with a binary algorithm

	// IMPORTANT NOTES: max size 1500x1500, only starting points are those with wall before them
	// current plan: i will combine plan 1 with 4, first use plan 1 to mark all the starting points and find the max length,
	// then use that as a reference point for binary search walkback
	// thought: if we find a longer than previous airstrip, then the shorter one is automatically 100% possible as well

	// final approach, the least complex and the most optimal approach:
	// * find all starting points, measure their length
	// if runways=1: 
	// 		* get longest, output longest, end of script
	// if runways=2:
	//		* get largest single runway, divide by 2, cache as a potential output (redundant if todo is completed) 
	//		* check every possible combination in a binary search style, 
	//		  with each check being performed on every possible runway 
	//		  until a solution for specified length is found
	//		* TODO: check starting points that are created by splitting other runways with the one currently checked (hardest and last to implement)
	//		* output largest one, end of script
	//

	// n*n vector
	/* b0 - filled or not (1)
	 * b1 - horizontally checked (1)
	 * b2 - vertically checked (1)
	 * b3 - currently occupied by second runway (1)
	 */
	std::array<std::array<unsigned char, 1500>, 1500> field;
	std::multimap<int, StartingPoint> startingPoints;

	// transfer data to field vvv
	for (int y = 0; y < airportSize; y++) {
		std::string line;
		//std::cin.ignore(); 
		std::cin >> line; // note: getline() is absolutely broken when cin >> is used before it
		for (int x = 0; x < airportSize; x++) {
			field[y][x] = 0;
			if (line[x] == 'X') {
				field[y][x] = (unsigned char)Bit::FILLED;
			}
			// input verified by: std::cout << (int)field[y][x];
		}
	}


	// transfer field into starting points vvv (verified by a set of cout's)
	for (int y = 0; y < airportSize; y++) {
		for (int x = 0; x < airportSize; x++) {
			// check if space is available
			if (field[y][x] & (unsigned char)Bit::FILLED) {
				//std::cout << "..";
				continue;
			}

			// extend horizontally and vertically vvv
			for (int horVer = 0; horVer <= 1; horVer++) {
				// starting points are places with stoppers behind them, in other words first points of a series
				if (horVer == 0 ? 
						// away from left wall, previous point is empty space: break
						(x > 0 && !(field[y][x-1] & (unsigned char)Bit::FILLED)) : 
						(y > 0 && !(field[y-1][x] & (unsigned char)Bit::FILLED))) {
					// not the first point of a series: break;
					//std::cout << '.';
					continue;
				}
				
				//std::cout << ((horVer == 0) ? 'H' : 'V');
				// if viable, create new starting point and extend it
				StartingPoint newPoint;
				newPoint.x = x;
				newPoint.y = y;
				newPoint.length = 0;
				newPoint.rot = horVer;
				for (int len = 0;
					 horVer == 0 ? 
						// current point is empty, and not over the border
						(x + len < airportSize && !(field[y][x+len] & (unsigned char)Bit::FILLED)) : 
						(y + len < airportSize && !(field[y+len][x] & (unsigned char)Bit::FILLED)) ; len++) {
					newPoint.length = len;			
				}
				startingPoints.insert(std::pair<int, StartingPoint>(newPoint.length, newPoint));
				if (newPoint.length > maxLength)
					maxLength = newPoint.length;
			}
		}
		//std::cout << std::endl;
	}

	// final conclusion
	if (runways == 1) {
		// get longest	
		std::cout << maxLength + 1;
		return 0;
	} else {
		// binary search for a working length
		int finalLength = maxLength;
		//int lowBand = 0, highBand = maxLength;
		do {
			// we will start by an iterative approach
			// find pair in the higher band, if unavailable, lower expectations by one
			// important: long runways can always be shortened
			// DONE: here comes the hard part, we have to treat long ones as a very high amount of short ones as well,
			// in other words, when we place 1 runway, we have to fill in that space and place new starting points in every possible spot
			// i will (tomorrow) create a second starting point list that will be temporary and exclusive to each runway no. 1 placement
			// then runway no. 2 will look for anything available both in the pernament and the temporary stock
			// DONE: also check collisions
			// DONE: approach for now: we will start with highest band, check every possible combination and lower it by one if nothing works out,
			// with lowered band, we will treat anything longer than the band as still feasable, but anything shorter will be thrown out for now.
			
			// greater or equal to upper bound length (lord have mercy, not the pointers)
			auto boundItr = startingPoints.lower_bound(finalLength);
			for (auto itOne = boundItr; itOne != startingPoints.end(); itOne++) {
				auto obOne = itOne->second;
				// scale current airstrip, placing starting points at each possible starting point
				// important: place SPs both at the right/bottom edge, and the right/bottom stub in case we are covering an airstrip that's shorter
				std::vector<StartingPoint> dynamicStartingPoints;
			
				// NOTE: tested and on small fields, this dynamic point creation works perfectly
				// for (int i = 0; i < finalLength; i++) {}
				for (int len = 0; len < obOne.length + 1; len++) {
					// create new point off to the side of the current runway
					StartingPoint newPoint;
					if (obOne.rot == 0) {
						newPoint.x = obOne.x + len;
						newPoint.y = obOne.y + 1;
						newPoint.rot = 1; 
					} else {
						newPoint.x = obOne.x + 1;
						newPoint.y = obOne.y + len;
						newPoint.rot = 0; 
					}
					newPoint.length = 0;
					dynamicStartingPoints.push_back(newPoint);
				}
				// one more point at the tip of the end
				// TODO: this one unlike the perpendicular DPs does not work.
				StartingPoint newPoint;
				if (obOne.rot == 0) {
					newPoint.x = obOne.x + finalLength + 1;
					newPoint.y = obOne.y;
					newPoint.rot = 0; 
				} else {
					newPoint.x = obOne.x;
					newPoint.y = obOne.y + finalLength + 1;
					newPoint.rot = 1; 
				}
				// newPoint.length = -42;
				// std::cout << "tested true length: " << finalLength + 1 << " x: " << newPoint.x << " y: " << newPoint.y << " rot: " << (newPoint.rot == 0 ? "hor" : "ver") << std::endl;
				dynamicStartingPoints.push_back(newPoint);

				// NOTE before i consider this for 11th time, we do NOT have to consider both extreme positions of runways along their axis of available space
				// 		dynamicSP will always be out of collision bounds, staticSP can not be moved at all if it's being collided with
				for (auto itTwo = boundItr; itTwo != startingPoints.end(); itTwo++) {
					auto obTwo = itTwo->second;
					if (itOne == itTwo)
						continue;
					bool didItCollide = doesCollide(obOne.x, obOne.y, finalLength, obOne.rot, obTwo.x, obTwo.y, finalLength, obTwo.rot);
					if (!didItCollide) {
						// success, no collision, this is our answer
						goto summary;						
					}
				}
				// dynamic loop, measure field until desired finalLength is desired, make sure no collisions occur!
				for (int iDyn = 0; iDyn < dynamicStartingPoints.size(); iDyn++) {
					auto obDyn = dynamicStartingPoints[iDyn];
					// browse the new dynamic path.
					int len = 0;
					bool isValid = false;
					// TODO: for optimalization, this can be one time for the entire obOne testing and then modified
					// ^^^^ TODO: if length = -1, measure length, else see if length is sufficient
					for (; (obDyn.rot == 0 ? obDyn.x + len : obDyn.y + len) < airportSize; len++) {
						if (obDyn.rot == 0) {
							if (field[obDyn.y][obDyn.x+len] & (unsigned short)Bit::FILLED)
								break;
						} else {
							if (field[obDyn.y+len][obDyn.x] & (unsigned short)Bit::FILLED)
								break;
						}
						if (len == finalLength) {
							isValid = true;
							break;
						}
					}
					if (isValid) {
						// std::cout << "valid, but not collision-checked yet: a2 = [x:" << obDyn.x << ", y: " << obDyn.y << ", ptLen: " << obDyn.length << ", true len: " << finalLength + 1 << ']' << std::endl;
						bool didItCollide = doesCollide(obOne.x, obOne.y, finalLength, obOne.rot, obDyn.x, obDyn.y, finalLength, obDyn.rot);
						if (!didItCollide) {
							// success
							//std::cout << "no collision detected";
							goto summary;
						} else {
							//std::cout << "valid but collision detected" << std::endl;
						}
					} else {
						//std::cout << "collision detected with a2 = [x:" << obDyn.x << ", y: " << obDyn.y << ", ptLen: " << obDyn.length << ", true len: " << finalLength + 1 << ']' << std::endl;
					}
				}
			}
			finalLength--;
		} while (finalLength >= 0);
		summary:
		std::cout << finalLength + 1;
		return 0;
	}

	// NEW CODE (v2.0), currently functional
	// --- BORDER ---
	// OLD CODE (v1.0), archived
	/*	
	

	// parse data to complete plan 1 (see line 41)
	for (int y = 0; y < airportSize; y++) {
		// std::cout << std::endl;
		for (int x = 0; x < airportSize; x++) {
			// TODO: clear all temporary markings!!!
			// TODO: if 2 runways, just repeat finding 2 times, with previous run all marked up
	
			//std::cout << (int)(field[y][x] & (unsigned char)Bit::FILLED);
			if (field[y][x] & (unsigned char)Bit::FILLED) {
				// skip checked / occupied path
				// verified by: std::cout << " | " << x << ' ' << y << " BREAK ";
				continue;
			}	
			
			// extension check
			for (int horVer = 0; horVer <= 1; horVer++) {	
				for (int len = 0; ; len++) {
					int offsetPos = (horVer == 0) ? x + len : y + len; 
					 
					// finalize if one of the stopping rules is met
					unsigned char currentByte = (horVer == 0) ? field[y][offsetPos] : field[offsetPos][x];
					//std::cout << "O: [" << x << ',' << y << "], Len: " << len << std::endl;
					if ((offsetPos >= airportSize) ||
						(currentByte & (unsigned char)Bit::FILLED) || 
						(currentByte & (unsigned char)Bit::USED) ||
						((currentByte & (unsigned char)Bit::HOR_CHECKED) && horVer == 0) ||
						((currentByte & (unsigned char)Bit::VER_CHECKED) && horVer == 1)) {
						
						// finalize
						len--;
						if (runways == 2) {
							// stop encountered, caching current result
							if (longestOne[(size_t)Stat::LENGTH] < len) {
								// TODO: MAKES SURE THESE DON'T OVERLAP
								if (longestOne[(size_t)Stat::X] != x &&
									longestOne[(size_t)Stat::Y] != y) {
									// approve the shorter runway after finding a longer one
									longestFinal[0] = longestOne[0];
									longestFinal[1] = longestOne[1];
									longestFinal[2] = longestOne[2];
									longestFinal[3] = longestOne[3];
								}
							}
						}
						
						// cache for both 1 and 2 runways
						if (len > longestOne[(size_t)Stat::LENGTH]) {
							// set new possible length
							longestOne[(size_t)Stat::LENGTH] = len;	
							longestOne[(size_t)Stat::ORIENTATION] = horVer;	
							longestOne[(size_t)Stat::X] = x;	
							longestOne[(size_t)Stat::Y] = y;	
						}
						break;
					}
					// set all neccesary flags

				}	
			}
		}
	}
	if (runways == 1) {
		std::cout << longestOne[(size_t)Stat::LENGTH] + 1;
	} else {
		std::cout << longestFinal[(size_t)Stat::LENGTH] + 1;
	}
	*/

	return 0;
}
