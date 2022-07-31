/* TESTING RATIONALE:

Some common test cases I decided to test in general are:
- string and int based graphs (for template testing)
- any exceptions as mentioned in the spec
- For accessors:
   - return value being true or false based on graphs with nodes and edges or being empty/doesn't
have the node/edge being looked for
   - testing functions such as weights or nodes returning filled vectors (based on node/edge
populated graphs) or empty vectors as well
- For iterator:
   - empty or edge populated graphs
   - edge cases e.g. being able to do the --operator on .end()
- For comparison:
   - empty
   - unequal nodes or edges
- For extractor:
   - empty
   - filled graphs with some nodes with no edges and some with
- For modifiers:
   - checking success of functions by the return e.g. false if node/edge being added already exists
   - for replace_node and merge_replace node, checking edge cases for replacing nodes as source as
well as destination
   - checking if erase_edge(iterator) and erase_edge(iterator, iterator) returns iterator to the
correct element afterwards

There is a particular order that I followed so that all functions are tested properly and error
sources can be traced easily. While this order is not perfect as some function tests rely on other
functions to work and vice versa, an appropriate ordering will still be listed below:

In general, constructor tests -> accessor tests -> iterator tests -> extractor tests -> comparisons
testing -> modifier tests (despite overlapping usage in tests); Some of the most important functions
to be tested first are of course, the constructors, followed by certain accessors such as empty,
is_node, weights and find.

Some important other testing orderings to consider are:
- iterator dereferencing, begin, end, ++ and -- increment testing should be done before testing find
(as it returns an iterator).
- Some modifier functions such as insert_node and insert_edge(src, dst, weight) should also be
tested earlier as these were also heavily used testing almost everything.
*/

#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>

TEST_CASE("iterator begin") {
	SECTION("integer") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		g.insert_edge(3, 2, 3);
		g.insert_edge(1, 2, 3);
		g.insert_edge(2, 2, 5);
		auto iter = g.begin();
		CHECK((*iter).from == 1);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 3);
	}

	SECTION("string") {
		auto g = gdwg::graph<std::string, int>{"c", "b", "a"};
		g.insert_edge("c", "b", 3);
		g.insert_edge("a", "b", 3);
		g.insert_edge("b", "b", 5);
		auto iter = g.begin();
		CHECK((*iter).from == "a");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 3);
	}

	SECTION("no edges") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		auto iter = g.begin();
		CHECK(iter == g.end());
	}
}

TEST_CASE("iterator end") {
	SECTION("integer") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		g.insert_edge(3, 2, 3);
		g.insert_edge(1, 2, 3);
		g.insert_edge(2, 2, 5);
		auto iter = g.begin();
		auto end = g.end();
		iter++;
		iter++;
		iter++;
		CHECK(iter == end);
	}

	SECTION("string") {
		auto g = gdwg::graph<std::string, int>{"c", "b", "a"};
		g.insert_edge("c", "b", 3);
		g.insert_edge("a", "b", 3);
		g.insert_edge("b", "b", 5);
		auto iter = g.begin();
		auto end = g.end();
		iter++;
		iter++;
		iter++;
		CHECK(iter == end);
	}

	SECTION("no edges") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		auto iter = g.begin();
		auto end = g.end();
		CHECK(iter == end);
	}
}

TEST_CASE("iterator compare") {
	SECTION("integer and non equal") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		g.insert_edge(3, 2, 3);
		g.insert_edge(1, 2, 3);
		g.insert_edge(2, 2, 5);
		auto iter = g.find(2, 2, 5);
		auto iter2 = g.find(3, 2, 3);
		auto iter3 = g.find(1, 2, 3);
		auto end = g.end();
		CHECK(iter != end);
		CHECK(iter != iter2);
		CHECK(iter != iter3);
	}

	SECTION("string and non equal") {
		auto g = gdwg::graph<std::string, int>{"c", "b", "a"};
		g.insert_edge("c", "b", 3);
		g.insert_edge("a", "b", 3);
		g.insert_edge("b", "b", 5);
		auto iter = g.find("b", "b", 5);
		auto iter2 = g.find("c", "b", 3);
		auto iter3 = g.find("a", "b", 3);
		auto end = g.end();
		CHECK(iter != end);
		CHECK(iter != iter2);
		CHECK(iter != iter3);
	}

	SECTION("are equal") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		g.insert_edge(3, 2, 3);
		g.insert_edge(1, 2, 3);
		g.insert_edge(2, 2, 5);
		auto iter3 = g.find(1, 2, 3);
		auto begin = g.begin();
		CHECK(iter3 == begin);
		iter3++;
		auto iter = g.find(2, 2, 5);
		CHECK(iter3 == iter);
		iter3++;
		auto iter2 = g.find(3, 2, 3);
		CHECK(iter3 == iter2);
		iter3++;
		auto end = g.end();
		CHECK(iter3 == end);
	}
}

TEST_CASE("iterator dereference") {
	SECTION("integer") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		g.insert_edge(3, 2, 3);
		g.insert_edge(1, 2, 3);
		g.insert_edge(2, 2, 5);
		auto iter = g.find(1, 2, 3);
		CHECK((*iter).from == 1);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 3);
		iter++;
		CHECK((*iter).from == 2);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 5);
		iter++;
		CHECK((*iter).from == 3);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 3);
	}

	SECTION("string") {
		auto g = gdwg::graph<std::string, int>{"c", "b", "a"};
		g.insert_edge("c", "b", 3);
		g.insert_edge("a", "b", 3);
		g.insert_edge("b", "b", 5);
		auto iter = g.find("a", "b", 3);
		CHECK((*iter).from == "a");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 3);
		iter++;
		CHECK((*iter).from == "b");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 5);
		iter++;
		CHECK((*iter).from == "c");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 3);
	}
}

TEST_CASE("iterator postincrement++") {
	SECTION("integer") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		g.insert_edge(3, 2, 3);
		g.insert_edge(1, 2, 3);
		g.insert_edge(2, 2, 5);
		auto iter = g.find(1, 2, 3);
		CHECK((*iter).from == 1);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 3);
		auto iter2 = iter++;
		CHECK((*iter2).from == 1);
		CHECK((*iter2).to == 2);
		CHECK((*iter2).weight == 3);

		CHECK((*iter).from == 2);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 5);
		iter2 = iter++;
		CHECK((*iter2).from == 2);
		CHECK((*iter2).to == 2);
		CHECK((*iter2).weight == 5);

		CHECK((*iter).from == 3);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 3);
		iter2 = iter++;
		CHECK((*iter2).from == 3);
		CHECK((*iter2).to == 2);
		CHECK((*iter2).weight == 3);
		CHECK(iter == g.end());
	}

	SECTION("string") {
		auto g = gdwg::graph<std::string, int>{"c", "b", "a"};
		g.insert_edge("c", "b", 3);
		g.insert_edge("a", "b", 3);
		g.insert_edge("b", "b", 5);
		auto iter = g.find("a", "b", 3);
		CHECK((*iter).from == "a");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 3);
		auto iter2 = iter++;
		CHECK((*iter2).from == "a");
		CHECK((*iter2).to == "b");
		CHECK((*iter2).weight == 3);

		CHECK((*iter).from == "b");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 5);
		iter2 = iter++;
		CHECK((*iter2).from == "b");
		CHECK((*iter2).to == "b");
		CHECK((*iter2).weight == 5);

		CHECK((*iter).from == "c");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 3);
		iter2 = iter++;
		CHECK((*iter2).from == "c");
		CHECK((*iter2).to == "b");
		CHECK((*iter2).weight == 3);

		CHECK(iter == g.end());
	}
}

TEST_CASE("iterator preincrement++") {
	SECTION("integer") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		g.insert_edge(3, 2, 3);
		g.insert_edge(1, 2, 3);
		g.insert_edge(2, 2, 5);
		auto iter = g.find(1, 2, 3);
		CHECK((*iter).from == 1);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 3);
		auto iter2 = ++iter;

		CHECK((*iter).from == 2);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 5);
		CHECK((*iter2).from == 2);
		CHECK((*iter2).to == 2);
		CHECK((*iter2).weight == 5);
		iter2 = ++iter;

		CHECK((*iter).from == 3);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 3);
		CHECK((*iter2).from == 3);
		CHECK((*iter2).to == 2);
		CHECK((*iter2).weight == 3);
		iter2 = ++iter;
		CHECK(iter == g.end());
	}

	SECTION("string") {
		auto g = gdwg::graph<std::string, int>{"c", "b", "a"};
		g.insert_edge("c", "b", 3);
		g.insert_edge("a", "b", 3);
		g.insert_edge("b", "b", 5);
		auto iter = g.find("a", "b", 3);
		CHECK((*iter).from == "a");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 3);
		auto iter2 = ++iter;

		CHECK((*iter).from == "b");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 5);
		CHECK((*iter2).from == "b");
		CHECK((*iter2).to == "b");
		CHECK((*iter2).weight == 5);
		iter2 = ++iter;

		CHECK((*iter).from == "c");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 3);
		CHECK((*iter2).from == "c");
		CHECK((*iter2).to == "b");
		CHECK((*iter2).weight == 3);
		iter2 = ++iter;
		CHECK(iter == g.end());
		CHECK(iter2 == g.end());
	}
}

TEST_CASE("iterator postincrement--") {
	SECTION("integer testing from end to beginning") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		g.insert_edge(3, 2, 3);
		g.insert_edge(1, 2, 3);
		g.insert_edge(2, 2, 5);
		auto iter = g.end();
		auto iter2 = iter--;
		CHECK((*iter).from == 3);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 3);
		CHECK(iter2 == g.end());

		iter2 = iter--;
		CHECK((*iter).from == 2);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 5);
		CHECK((*iter2).from == 3);
		CHECK((*iter2).to == 2);
		CHECK((*iter2).weight == 3);

		iter2 = iter--;
		CHECK((*iter2).from == 2);
		CHECK((*iter2).to == 2);
		CHECK((*iter2).weight == 5);
		CHECK((*iter).from == 1);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 3);

		iter2 = iter--;
		CHECK((*iter2).from == 1);
		CHECK((*iter2).to == 2);
		CHECK((*iter2).weight == 3);
		CHECK(iter == g.begin());
	}

	SECTION("string") {
		auto g = gdwg::graph<std::string, int>{"c", "b", "a"};
		g.insert_edge("c", "b", 3);
		g.insert_edge("a", "b", 3);
		g.insert_edge("b", "b", 5);
		auto iter = g.end();
		auto iter2 = iter--;
		CHECK((*iter).from == "c");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 3);
		CHECK(iter2 == g.end());

		iter2 = iter--;
		CHECK((*iter).from == "b");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 5);
		CHECK((*iter2).from == "c");
		CHECK((*iter2).to == "b");
		CHECK((*iter2).weight == 3);

		iter2 = iter--;
		CHECK((*iter2).from == "b");
		CHECK((*iter2).to == "b");
		CHECK((*iter2).weight == 5);
		CHECK((*iter).from == "a");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 3);

		iter2 = iter--;
		CHECK((*iter2).from == "a");
		CHECK((*iter2).to == "b");
		CHECK((*iter2).weight == 3);
		CHECK(iter == g.begin());
	}
}

TEST_CASE("iterator preincrement--") {
	SECTION("integer testing from end to beginning") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		g.insert_edge(3, 2, 3);
		g.insert_edge(1, 2, 3);
		g.insert_edge(2, 2, 5);
		auto iter = g.end();
		auto iter2 = --iter;
		CHECK((*iter).from == 3);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 3);
		CHECK((*iter2).from == 3);
		CHECK((*iter2).to == 2);
		CHECK((*iter2).weight == 3);

		iter2 = --iter;
		CHECK((*iter).from == 2);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 5);
		CHECK((*iter2).from == 2);
		CHECK((*iter2).to == 2);
		CHECK((*iter2).weight == 5);

		iter2 = --iter;
		CHECK((*iter2).from == 1);
		CHECK((*iter2).to == 2);
		CHECK((*iter2).weight == 3);
		CHECK((*iter).from == 1);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 3);

		iter2 = --iter;

		CHECK(iter == g.begin());
		CHECK(iter2 == g.begin());
	}

	SECTION("string") {
		auto g = gdwg::graph<std::string, int>{"c", "b", "a"};
		g.insert_edge("c", "b", 3);
		g.insert_edge("a", "b", 3);
		g.insert_edge("b", "b", 5);
		auto iter = g.end();
		auto iter2 = --iter;
		CHECK((*iter).from == "c");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 3);
		CHECK((*iter2).from == "c");
		CHECK((*iter2).to == "b");
		CHECK((*iter2).weight == 3);

		iter2 = --iter;
		CHECK((*iter).from == "b");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 5);
		CHECK((*iter2).from == "b");
		CHECK((*iter2).to == "b");
		CHECK((*iter2).weight == 5);

		iter2 = --iter;
		CHECK((*iter2).from == "a");
		CHECK((*iter2).to == "b");
		CHECK((*iter2).weight == 3);
		CHECK((*iter).from == "a");
		CHECK((*iter).to == "b");
		CHECK((*iter).weight == 3);

		iter2 = --iter;

		CHECK(iter == g.begin());
		CHECK(iter2 == g.begin());
	}
}