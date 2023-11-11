#include <algorithm>
#include <ios>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <string>
#include <sstream>
#include <array>
#include <vector>
#include <sys/stat.h>
// INPUTS:
// n k a b
// S
// MAP:
// stringLength suffixLength outputIndexStart outputIndexEnd
// inputString
// EXPLANATION:
// n - characters in S (2 -> 1,000,000)
// k - characters in suffix R (1 -> 999,999)
// a - start index of output (starting at 1, inclusive)
// b - end index of output (inclusive)
// ^ output last b-a+1 letters of the S'
// S - the n-long string, (a-z)*n
// CLARIFICATIONS:
// as far as i understand, S is already polluted on input with R of length k placed on the end,
// what i don't understand is why is S' size_troduced then right at the very beggining of the example
// if it's just equal to regular S, S = S'
// * We isolate the R by taking last k characters out of the raw S.
// * We index every previous occurence of R, as well as the character which appeared straight after it.
// * We take the most common character appearing right after our R's occurences, we append that letter to the end.
// * We reisolate the R from our new S, then repeat this process until we extend S enough to output it's values specified by a and b variables.

// TODO: UPDATE 3/11/2023: WE SHOULD INDEX EVERY PATTERN WHICH IS QUERIED, ALSO ADD ANY NEW PATTERNS TO THAT INDEX, THIS WILL MAKE THIS SO FAST 

int main() {
	unsigned long long stringLength, suffixLength, outputIndexStart, outputIndexEnd;
	std::string inputString;
	std::cin >> stringLength >> suffixLength >> outputIndexStart >> outputIndexEnd;

	// FIXME: with large numbers, input by string is not feaseble, termination initiated by string after throwing 'std::out_of_range'
	std::cin >> inputString;

	// convert input to vector form, todo: test performance of this vs direct std::string	
	std::vector<char> baseString(stringLength);
	for (size_t i = 0; i < stringLength; i++) {
		baseString[i] = inputString.at(i);	
	}

	do {
		// find current suffix | verified
		std::vector<char> currentSuffix(suffixLength);
		for (size_t  i = baseString.size() - suffixLength, j = 0; j < suffixLength; i++, j++) {
			currentSuffix[j] = baseString[i];
			//std::cout << currentSuffix[j];
		}
		//std::cout << " : Current suffix" << std::endl;

		// find each occurence of suffix | verified 
		// (the hardcode is ugly but it will be easier to test and debug it later)
		std::map<char, size_t> occurenceCounter {{'a', 0}, {'b', 0}, {'c', 0}, {'d', 0}, {'e', 0}, {'f', 0}, {'g', 0}, {'h', 0}, {'i', 0}, {'j', 0}, {'k', 0}, {'l', 0}, {'m', 0}, {'n', 0}, {'o', 0}, {'p', 0}, {'q', 0}, {'r', 0}, {'s', 0}, {'t', 0}, {'u', 0}, {'v', 0}, {'w', 0}, {'x', 0}, {'y', 0}, {'z', 0}};
		for (size_t i = 0; i < baseString.size(); i++) {
			//std::cout << "checking occurence for i = " << i << " : ";
			bool isPresent = true;
			for (size_t suffixIdx = 0; suffixIdx < suffixLength && i + suffixIdx < baseString.size(); suffixIdx++) {
				//std::cout << baseString[suffixIdx + i];
				if (baseString[suffixIdx + i] != currentSuffix[suffixIdx]) {
					//std::cout << " terminated: " << baseString[suffixIdx + i] << "!=" << currentSuffix[suffixIdx];
					isPresent = false;
					break;
				}
			}
			//std::cout << std::endl;
			if (isPresent) {
				//std::cout << "occurence present... ";
				// if checks if this occurence isn't just the final, guaranteed occurence | verified
				if (i + suffixLength < baseString.size()) {
					//std::cout << "and confirmed";
					occurenceCounter.at(baseString[i+suffixLength])++;
				}
				//std::cout << std::endl; 
			}
		}
		
		// find most common letter of all | verified
		char mostCommonLetter = 'a'; // apparently if no other option is available, 'a' takes the priority as it's the first letter of the set of all letters equally eligible
		size_t mostOccurenceCount = 0;
		for (auto it = occurenceCounter.begin(); it != occurenceCounter.end(); it++) {
			//std::cout << it->first << ':' << it->second << std::endl;
			// note: earlier letter in the alphabet takes priority in case of ties
			if (it->second > mostOccurenceCount) {
				mostCommonLetter = it->first;
				mostOccurenceCount = it->second;
				
				//std::cout << "appended letter: " << it->first << std::endl;
			}
		}

		baseString.push_back(mostCommonLetter);
	} while (baseString.size() < outputIndexEnd);

	//std::cout << baseString.size() << std::endl;
	// it looks like 99% of runtime is just io operations
	std::ios_base::sync_with_stdio(false);
	std::cout << std::unitbuf;
	// vvv sets buffer size to optimal bulk size, ref: https://en.cppreference.com/w/cpp/io/c/setvbuf
	struct stat stats;
	std::setvbuf(stdin, nullptr, _IOFBF, stats.st_blksize);
	// ^^^ code above does not change runtime speed too much, will test it with the last test if i manage to get it working

	std::string outputString;
	for (size_t i = outputIndexStart - 1; i < outputIndexEnd; i++) {
		outputString.push_back(baseString[i]);
		// std::cout << i << ':' << baseString[i] << std::endl;
	}
	std::cout << outputString << std::flush;

	return 0;
}


