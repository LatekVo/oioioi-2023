#include <algorithm>
#include <iostream>
#include <limits>
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

int main() {
	int airportSize = 0;
	short runways = 0;
	std::cin >> airportSize >> runways;

	// x, y, orientation (h=0), length
	int longestOne[4] = {0,0,0,0}; 
	int longestFinal[4] = {0,0,0,0}; // termination and parity achieved

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
	std::vector<StartingPoint> startingPoints;

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
				startingPoints.push_back(newPoint);
			}
		}
		//std::cout << std::endl;
	}

	// NEW CODE (v2.0), currently WIP, segment by segment
	// --- BORDER ---
	// OLD CODE (v1.0), currently functional

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
	return 0;
}
