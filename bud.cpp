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

enum class Bit {
	FILLED = 0b0001,
	HOR_CHECKED = 0b0010,
	VER_CHECKED = 0b0100,
	USED = 0b1000
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

	// n*n vector
	/* b0 - filled or not (1)
	 * b1 - horizontally checked (1)
	 * b2 - vertically checked (1)
	 * b3 - currently occupied by second runway (1)
	 */
	std::array<std::array<unsigned char, 1500>, 1500> field;

	// transfer data to field
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
				break;
			}	
			
			// extension check
			for (int horVer = 0; horVer <= 1; horVer++) {	
				for (int len = 0; ; len++) {
					int offsetPos = (horVer == 0) ? x + len : y + len; 
					if (offsetPos >= airportSize) {
						// TODO: finalize in this situation as well
						break;
					}
					unsigned char currentByte = (horVer == 0) ? field[y][offsetPos] : field[offsetPos][x];
					// std::cout << "O: [" << x << ',' << y << "], Len: " << len << std::endl;
					if ((currentByte & (unsigned char)Bit::FILLED) || 
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
