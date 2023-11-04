#include <algorithm>
#include <cstdint>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <string>
#include <sstream>
#include <array>
#include <vector>

// my idea for this test is to create a tree of all possible choices, and then to expand that tree,
// if we remove branches that are obliviously inefficient, we can immidiately focus only on a few relevant branches,
// making our runtime reasonable.
// then we pick the longest-running branch we have saved, and display that as our solution.
// it's hard thinking right now but i have to figure out the right way to map this problem onto a Map or other strucutre.
// i've been thinking either a freely allocated Node-system, or a dedicated tree structure for this purpose.
// ok a thing that comes to my mind besides tree is also a layered structure, basically we have a vector of root
// elements, then place all of their potential kids into a 2nd vector, then all of their kids into a 3rd vector,
// then we traverse the thing back (we need to attach a back-link to every node placed)
// But to keep this in a reasonable runtime we need to actively eliminate paths that are straight out unoptimal,
// for example when we see the same element in 2nd and 3rd layer, we only keep the one in 3rd layer, also removing all kids
// of the one in 2nd layer. After adding each lecture, we back-check if it's present anywhere.
// Also, to avoid into running into crazy runtimes when optimising, we have to attach a hard limit to where we check, and what we place
// inside the vectors, for example if in the 1st layer we have something that ends at 11m, then there is no need to add an element that begins at 12m,
// that would be straight out inefficient.
// Firstly it would probably also be wise to eliminate any inefficiencies, and also immidiately only leave courses that have an overlapping backup available.

// UPDATE:
// Oh wait, wouldn't it be just way simpler to just pick the Lecture that ends the earliest out of the valid ones, then append it to the solution vector?

// ANOTHER UPDATE:
// The backup lecture does not have to be contained within the primary one, it just has to not collide with any other lecture,
// oh and now it got way more tricky because we are balancing and deciding whether to choose a backup or a primary lecture
// I think the original tree idea might have been the best, but the runtime is killing me, there has to be a simple solution

// a simple struct just for readability
struct Lecture {
	size_t id;
	size_t start;
	size_t end;
};

int main() {
	size_t nLectures = 0;
	std::cin >> nLectures;

	// input loop | verified
	std::vector<Lecture> lectureList(nLectures);
	for (size_t i = 0; i < nLectures; i++) {
		size_t s, e;
		std::cin >> s >> e;
		lectureList.push_back({i + 1, s, e});
	}
	
	// first organize each possible lecture into a set of lecture : backup-lecture pairs 
	// each potential lecture has to have at least one backup
	// 2 facts are true: 1: the potential backup is 100% contained within the primary lecture
	// and 2: the backup will always start at the time or right after the primary lecture

	// current plan: follow the instructions escribed in the latest 'UPDATE' paragraph
	
	// NOTE: if start = end, they DO NOT collide, this position is valid.

	// NOTE: woah absolutely no way i managed to do this O(n), i have to go back to CZA since i believe i can actually do it as well
	// 		 very close to O(n) by having a real time map of all previous patterns (patterns are finite, 
	// 		 determined by length of prefix, only previously found patterns will be catalogued). Ah perhaps std::sort is O(n^2), it's still fast enough to not matter

	// sort | verified
	// sort by end, then start
	//std::sort(lectureList.begin(), lectureList.end(), [] (Lecture a, Lecture b) { return a.end == b.end ? (a.start < b.start) : (a.end < b.end);});
	// sort by start only
	std::sort(lectureList.begin(), lectureList.end(), [] (Lecture a, Lecture b) { return a.start < b.start; });
	
	// NOTE: Approach 3.0, this time we consider the fact that backup may extend indefinietely outwards
	// we will consider a 3 step approach: 
	// we need to operate sequentially, we will only know how much we can afford if we try to fit our new pairs in with the old ones.
	// * for the first element, we create a pair with earliest possible combined ending time (prim + backup), until we find the most optimal one
	// * for each next pair, we find the earliest ending pair, 
	//   we stop searching as soon as the starting points of our new prims start being later than the ending points of our best prim
	// 
	std::vector<std::pair<Lecture, Lecture>> outputList;
	size_t bestCombinedEnd = 1000000001;
	for (int i = 0; i < lectureList.size(); i++) {
		auto currentLecture = lectureList[i];
		
		if (currentLecture.start > bestCombinedEnd) {
			// lock in the best found, as no better one will be found
		
			// reset variables

		}

		
	}
	// NOTE: Approach 2.0
	/*
	bool needReset = true;
	Lecture backupCandidate; // a lecture with latest starting point known
	Lecture primaryCandidate; // a lecture with earliest starting point known
	std::vector<std::pair<Lecture, Lecture>> outputList;
	
	primaryCandidate.start = 1000000001;
	primaryCandidate.id = 0;
	backupCandidate.start = 0;
	backupCandidate.id = 0;
	needReset = false;
	
	for (int i = 0; i < lectureList.size(); i++) {
		auto currentLecture = lectureList[i];
		//std::cout << currentLecture.start << ' ' << currentLecture.end << std::endl;
		
		auto checkValidity = [] () {};

		// if both candidates match up, save and reset them
		if (primaryCandidate.id != 0 && backupCandidate.id != 0 && primaryCandidate.id != backupCandidate.id) {
			//std::cout << "pushing back a new pair" << std::endl;
			outputList.push_back(std::pair<Lecture, Lecture>(primaryCandidate, backupCandidate));
			needReset = true;
		}
		
		if (needReset) {
			//std::cout << "resetting" << std::endl;
			primaryCandidate.start = 1000000001;
			primaryCandidate.id = 0;
			backupCandidate.start = 0;
			backupCandidate.id = 0;
			needReset = false;
		}

		// latest possible starting time, will naturally grow
		if (currentLecture.start > backupCandidate.start) {
			backupCandidate = currentLecture;
		}
		// either earliest possible starting time, or earliest that complies with current best backup
		if (currentLecture.start <= backupCandidate.start)
			primaryCandidate = currentLecture;
		
		std::cout << "Current test: " << currentLecture.start << ' ' << primaryCandidate.start << ' ' << backupCandidate.start << std::endl;
		std::cout << "Current primary and backup ids: " << primaryCandidate.id << " " << backupCandidate.id << std::endl;
		std::cout << "Current primary and backup starts: " << primaryCandidate.start << " " << backupCandidate.start << std::endl;
	}
	*/
	/* SCRAP EVERYTHING, NOTE: isn't it the solution to just sort everything by both end and start, and then pair up everything?
	// main loop
	// quick note: we should be simulatneously looking for a backup and consider a new main in case no good backup can be found
	size_t lastEnd = lectureList[0].end, bestStart = 1000000001;
	size_t minimumStart = 1; // global evergrowing variable, indicates earliest point from which we can start 
	signed long long lastCompletedEnd = -1;
	signed long long lastPrimaryIdx = -1; // set normally
	std::vector<std::pair<Lecture, Lecture>> outputList;
	for (size_t i = 0; i < nLectures; i++) {
		auto& lecture = lectureList[i];
		if (lecture.end == lastCompletedEnd) {
			// we already completed the task for this timeframe so just skip untill we find the next timeframe
			continue;
		}
		if (lecture.start < minimumStart) {
			// invalid lecture
			continue;
		}

		if (lecture.end > lastEnd) {
			// if previous lecture remained unused, keep it as a potential backup

			// we incremented our time frame, so we reset the variables 
			minimumStart = lecture.end;
			lastEnd = lecture.end;
			bestStart = 1000000001;
		} 
		// TODO: keep record of latest starting lectures, they could be used as a backup later on

		// if the current lecture has better or same starting frame, the previous one can be our backup
		// otherwise it's the other way around
		if (lastPrimaryIdx == -1) {
			// we set our reference - lecture
			lastPrimaryIdx = i;
		} else {
			// reference already set, setting backup
			// check if current combination is valid
			Lecture primLecture, backLecture;
			bool isValid = true;
			lastCompletedEnd = lecture.end;
			if (lecture.start <= bestStart) {
				// current wider than reference, set current as primary, reference as backup
						
			} else {
				// reference wider than current, set reference as primary, current as backup
			
			}
		}
	}
	*/

	// output loop
	std::cout << outputList.size() << '\n';
	for (size_t i = 0; i < outputList.size(); i++) {
		std::cout << outputList[i].first.id << ' ' << outputList[i].second.id << '\n';
	}	

	return 0;
}
