#include <algorithm>
#include <iostream>
#include <map>
#include <string>
#include <vector>

struct Satellite {
	int connections = 0;
	std::string serialCode;
	std::vector<Satellite*> peers; // raw vectors are unsanitary when used often but here i will be messing with them only once and later only reading them, 
								   // and i just know that mixing up 0 and 1 indexing system will bite me more stealthly than using pointers
};

int main() {
	int satsPerCompany, competingConnections, maxCodeLength;
	int finalCodeLength = 0;

	// inputs
	std::cin >> satsPerCompany >> competingConnections >> maxCodeLength;
	
	std::vector<Satellite> satellites(satsPerCompany*2);
	// and now to sort the vector WITHOUT messing up the pointer references, or index based references, 
	// i reference every element of that list and sort the reference by the referenced element, 
	// i can only afford this because the max n is 1000 :)
	std::vector<Satellite*> sortedSatellites(satsPerCompany*2);
	
	// inputs
	for (int i = 0; i < competingConnections; i++) {
		int conA, conB;
		std::cin >> conA >> conB;
		conA--; conB--; // for now convert to 0->n indexing space
		satellites[conA].peers.push_back(&(satellites[conB]));
		satellites[conA].connections++;
		satellites[conB].peers.push_back(&(satellites[conA]));
		satellites[conB].connections++;
	}
	// now also establish required connections
	// TODO: now i believe the required connections are 1->2, 2->3, etc, but they very well could be unordered as long as they are forming a loop, check this
	for (int i = 0; i < satsPerCompany; i++) {
		// completely unrelated, just to use a single loop, map satellites onto sortedSatellites vector
		sortedSatellites[i] = &(satellites[i]);

		// last satellite connects to the first one
		if (i == satsPerCompany - 1) {
			satellites[i].peers.push_back(&(satellites[0]));
			satellites[i].connections++;
			satellites[0].peers.push_back(&(satellites[i]));
			satellites[0].connections++;
			break;
		}
		satellites[i].peers.push_back(&(satellites[i+1]));
		satellites[i].connections++;
		satellites[i+1].peers.push_back(&(satellites[i]));
		satellites[i+1].connections++;
	}

	std::sort(sortedSatellites.begin(), sortedSatellites.end(), [] (Satellite* a, Satellite* b) { return a->connections > b->connections; });

	// basic principle: each sattelite has to have an m-letter code, and when two sattelites have the same letter on some position, they connect.
	// letters we can use: {A, B, C}, max code length is (i believe) anywhere between 1002 and 3000 (the lower is harder)
	// and so we have to represent every connection as an alignment of letters between two codes, that's not hard,
	// the real challange is to then set every insignificant letter in a way that will not interfere with any other sattelite,
	// but wtf wouldn't this mean that after forming 3 connections (A-A, B-B, C-C) we have no letters left to use for non-connections?
	// and that we are forced to connect sattelites?
	// Approach: Ok so my current approach would be to sort this whole connection list by sattelites with most connections,
	// then we can use one letter for all star-linked sattelites, for example sat-6 with 3 outgoing connections, will place an A on all 4 sattelites
	// as for keeping track of used letters, i can just iterate through... i will have to create a weird data structure which will, hm, to graph all connections,
	// then... Ok so i will 1: sort everything by n of connections, 2: put everything into a graph db which will show me which sats to link, then we will
	// iterate through available positions on each sat
	// ok but the problem is more complicated than that, if we treat each letter index as a network band, we cannot have multiple networks on the same band, we have
	// to put networks that work fully separately on the same band while keeping intertwined networks on separate bands
	// this would probably mean finding a set of 3 star-networks all of which are not interconnected, then assign a single index but different letters to all of them
	// ^ by network i actually mean a single node with a lot of connections, by no interconnections from this node's children, to a sibling node.
	//   by this I mean that this node's children are not connected to any other node hosting on the same index 
	// how would i categorise this? now that i think about it, since nearly 100% of the available bands have to be occupied, (that means max of 2 noised nodes per the whole solution),
	// that means each node will have n connections +- 1, that is max 2000 sattelites with 1000 connections each.
	// Wait, what significane is in the inter-company connections? Does this change anything? At all? This has to be very important but i am currently missing how so.
	// "Every pair of sattelites of the same company has to establish communication between each other" - is it fair to assume the sattelites are connected in a loop?
	// "Some pairs of competing sattelites will also establish communication" - does this mean we have 2 parrarell loops running with a few connections connecting them?
	// Yes upon visualising both the inputs and outputs of example tests, it is indeed the case that we have 2 parrarell loops running with connections spanning between.
	// TODO: look at larger examples of input output to see how they are dealing with noise, if at all
	// Ok so my current approach would still be as follows:
	// * Pick a point with most connections, assign a band to this point and add all his children to this band,
	// * If 2 of the children are connected to eachother, don't add them to this band, instead traverse to them and repeat this process from the perspective one of the children (one with more connections)
	// * Either do this in this chain way, or keep picking the points with most connections then
	// i see there is some sort of duplication happening, how do we decide to extend the final code length?
	// we have to use up current band and not have any other available, i have to trust the input that the sattelites
	// will never have so little connections that i cannot assign them a valid id.
	// For the last time before i consider it again tomorrow, i cannot in any way recursevively properiate the serial code, because if i did that, EVERY node in the net would be connected to EVERY other, 
	// intead of a start structure we would get an uncontrollable all-to-all NET. Oh no but we may actually want that, the thing i described will always create a net instead of a star,
	// we can store tripples all under one letter, but that's about it, later we would have to somehow detect a full web by just parsing through it, cannot imagine the amount of computation required.
	// Ok from my drawings i see that if we group satellites into all-to-all group, (every satellite in such group has to be connected to every other), 
	// then if we need to connect one of the grouped satellites to a different satellite, we use a separate bandwidth for it, while all other satellites of it's group just duplicate the previous callsign,
	// for example we have 3 satellites all with code A, and 3 all with code B (obliviously this is wrong because whole codes have to be unique),
	// now if we want to connect one of the satellites from the A group to the B group, we extend the callsign to AA and BB, and for 2 connected satellites we set it to AC and BC respectively,
	// now we have to repeat this for all satellites

	// outputs
	std::cout << finalCodeLength << '\n';	
	for (int i = 0; i < satsPerCompany * 2; i++) {
		std::cout << satellites[i].serialCode << '\n';	
	}

	return 0;
}
