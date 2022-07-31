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
#include <sstream>

TEST_CASE("extractor") {
	SECTION("output integers") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_node(4);
		g.insert_node(5);
		g.insert_node(6);
		g.insert_edge(4, 1, -4);
		g.insert_edge(3, 2, 2);
		g.insert_edge(2, 4, 2);
		g.insert_edge(2, 1, 1);
		g.insert_edge(6, 2, 5);
		g.insert_edge(6, 3, 10);
		g.insert_edge(1, 5, -1);
		g.insert_edge(3, 6, -8);
		g.insert_edge(4, 5, 3);
		g.insert_edge(5, 2, 7);
		g.insert_node(64);
		auto out = std::ostringstream{};
		out << g;

		auto const expected_output = std::string_view(R"(1 (
  5 | -1
)
2 (
  1 | 1
  4 | 2
)
3 (
  2 | 2
  6 | -8
)
4 (
  1 | -4
  5 | 3
)
5 (
  2 | 7
)
6 (
  2 | 5
  3 | 10
)
64 (
)
)");
		CHECK(out.str() == expected_output);
	}

	SECTION("output strings") {
		auto g = gdwg::graph<std::string, int>();
		CHECK(g.empty());
		g.insert_node("a");
		g.insert_node("b");
		g.insert_edge("a", "b", -4);
		g.insert_edge("b", "a", 2);
		g.insert_edge("b", "a", 5);
		g.insert_node("c");
		auto out = std::ostringstream{};
		out << g;

		auto const expected_output = std::string_view(R"(a (
  b | -4
)
b (
  a | 2
  a | 5
)
c (
)
)");
		CHECK(out.str() == expected_output);
	}

	SECTION("empty") {
		auto g = gdwg::graph<int, int>();
		auto out = std::ostringstream{};
		out << g;
		CHECK(out.str().empty());
	}
}