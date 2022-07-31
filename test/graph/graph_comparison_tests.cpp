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

TEST_CASE("operator== comparison test") {
	SECTION("empty") {
		auto g = gdwg::graph<int, int>();
		auto g2 = gdwg::graph<int, int>();
		CHECK(g == g2);
	}

	SECTION("same nodes") {
		auto g = gdwg::graph<int, int>();
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		auto g2 = gdwg::graph<int, int>();
		g2.insert_node(1);
		g2.insert_node(2);
		g2.insert_node(3);

		CHECK(g == g2);
	}

	SECTION("same edges") {
		auto g = gdwg::graph<int, int>();
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(1, 3, 10);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 3);
		g.insert_edge(3, 2, 5);
		g.insert_edge(3, 2, 30);

		auto g2 = gdwg::graph<int, int>();
		g2.insert_node(1);
		g2.insert_node(2);
		g2.insert_node(3);
		g2.insert_edge(1, 3, 10);
		g2.insert_edge(1, 2, 5);
		g2.insert_edge(1, 2, 3);
		g2.insert_edge(3, 2, 5);
		g2.insert_edge(3, 2, 30);

		CHECK(g == g2);
	}

	SECTION("not same edges") {
		auto g = gdwg::graph<int, int>();
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(1, 3, 10);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 3);
		g.insert_edge(3, 2, 5);
		g.insert_edge(3, 2, 30);

		auto g2 = gdwg::graph<int, int>();
		g2.insert_node(1);
		g2.insert_node(2);
		g2.insert_node(3);
		g2.insert_edge(3, 2, 5);
		g2.insert_edge(3, 2, 30);

		CHECK(g != g2);
	}

	SECTION("not same nodes") {
		auto g = gdwg::graph<int, int>();
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);

		auto g2 = gdwg::graph<int, int>();
		g2.insert_node(1);
		g2.insert_node(2);
		g2.insert_node(312);

		CHECK(g != g2);
	}

	SECTION("same string edges") {
		auto g = gdwg::graph<std::string, int>();
		g.insert_node("a");
		g.insert_node("b");
		g.insert_node("c");
		g.insert_edge("a", "b", 10);
		g.insert_edge("b", "c", 5);

		auto g2 = gdwg::graph<std::string, int>();
		g2.insert_node("a");
		g2.insert_node("b");
		g2.insert_node("c");
		g2.insert_edge("a", "b", 10);
		g2.insert_edge("b", "c", 5);

		CHECK(g == g2);
	}
}