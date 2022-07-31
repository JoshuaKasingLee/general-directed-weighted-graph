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
#include <list>

TEST_CASE("Default Constructor") {
	SECTION("default") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
	}
}

TEST_CASE("Initializer List Constructor") {
	SECTION("empty") {
		auto g = gdwg::graph<int, int>({});
		CHECK(g.empty());
	}

	SECTION("integer nodes") {
		auto g = gdwg::graph<int, int>{1, 2, 3};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
	}

	SECTION("string nodes") {
		auto g = gdwg::graph<std::string, int>{std::string("nice"),
		                                       std::string("nicer"),
		                                       std::string("nicest")};
		CHECK(!g.empty());
		CHECK(g.is_node(std::string("nice")));
		CHECK(g.is_node(std::string("nicer")));
		CHECK(g.is_node(std::string("nicest")));
	}
}

TEST_CASE("Iterator Constructor") {
	SECTION("empty") {
		auto l = std::list<int>();
		auto g = gdwg::graph<int, int>(l.begin(), l.end());
		CHECK(g.empty());
	}

	SECTION("integer nodes") {
		auto l = std::list{1, 2, 3};
		auto g = gdwg::graph<int, int>(l.begin(), l.end());
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
	}

	SECTION("string nodes") {
		auto l = std::list{std::string("nice"), std::string("nicer"), std::string("nicest")};
		auto g = gdwg::graph<std::string, int>(l.begin(), l.end());
		CHECK(g.is_node(std::string("nice")));
		CHECK(g.is_node(std::string("nicer")));
		CHECK(g.is_node(std::string("nicest")));
	}
}

TEST_CASE("Move Constructor") {
	SECTION("empty") {
		auto g = gdwg::graph<int, int>{1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		g.insert_edge(1, 1, 5);
		auto g2 = gdwg::graph<int, int>(std::move(g));
		CHECK(!g2.empty());
		CHECK(g2.is_node(1));
	}
	SECTION("integer nodes and edges") {
		auto g = gdwg::graph<int, int>{1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		g.insert_edge(1, 1, 5);
		g.insert_edge(1, 1, 10);
		auto g2 = gdwg::graph<int, int>(std::move(g));
		CHECK(!g2.empty());
		CHECK(g2.is_node(1));
		auto v = g2.weights(1, 1);
		CHECK(v.at(0) == 5);
		CHECK(v.at(1) == 10);
	}

	SECTION("string nodes") {
		auto s = std::string("duh");
		auto g = gdwg::graph<std::string, int>{s};
		CHECK(!g.empty());
		CHECK(g.is_node(s));
		g.insert_edge(s, s, 5);
		g.insert_edge(s, s, 10);
		auto g2 = gdwg::graph<std::string, int>(std::move(g));
		CHECK(!g2.empty());
		CHECK(g2.is_node(s));
		auto v = g2.weights(s, s);
		CHECK(v.at(0) == 5);
		CHECK(v.at(1) == 10);
	}
}

TEST_CASE("Move Assignment") {
	SECTION("empty") {
		auto g = gdwg::graph<int, int>{1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		g.insert_edge(1, 1, 5);
		auto g2 = std::move(g);
		CHECK(!g2.empty());
		CHECK(g2.is_node(1));
	}
	SECTION("integer nodes and edges") {
		auto g = gdwg::graph<int, int>{1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		g.insert_edge(1, 1, 5);
		g.insert_edge(1, 1, 10);
		auto g2 = std::move(g);
		CHECK(!g2.empty());
		CHECK(g2.is_node(1));
		auto v = g2.weights(1, 1);
		CHECK(v.at(0) == 5);
		CHECK(v.at(1) == 10);
	}

	SECTION("string nodes") {
		auto s = std::string("duh");
		auto g = gdwg::graph<std::string, int>{s};
		CHECK(!g.empty());
		CHECK(g.is_node(s));
		g.insert_edge(s, s, 5);
		g.insert_edge(s, s, 10);
		auto g2 = std::move(g);
		CHECK(!g2.empty());
		CHECK(g2.is_node(s));
		auto v = g2.weights(s, s);
		CHECK(v.at(0) == 5);
		CHECK(v.at(1) == 10);
	}
}

TEST_CASE("Copy Constructor") {
	SECTION("empty") {
		auto g = gdwg::graph<int, int>{1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		g.insert_edge(1, 1, 5);
		auto g2 = gdwg::graph<int, int>(g);
		CHECK(!g2.empty());
		CHECK(g2.is_node(1));
	}
	SECTION("integer nodes and edges") {
		auto g = gdwg::graph<int, int>{1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		g.insert_edge(1, 1, 5);
		g.insert_edge(1, 1, 10);
		auto g2 = gdwg::graph<int, int>(g);
		CHECK(!g2.empty());
		CHECK(g2.is_node(1));
		auto v = g2.weights(1, 1);
		CHECK(v.at(0) == 5);
		CHECK(v.at(1) == 10);
	}

	SECTION("string nodes") {
		auto s = std::string("duh");
		auto g = gdwg::graph<std::string, int>{s};
		CHECK(!g.empty());
		CHECK(g.is_node(s));
		g.insert_edge(s, s, 5);
		g.insert_edge(s, s, 10);
		auto g2 = gdwg::graph<std::string, int>(g);
		CHECK(!g2.empty());
		CHECK(g2.is_node(s));
		auto v = g2.weights(s, s);
		CHECK(v.at(0) == 5);
		CHECK(v.at(1) == 10);
	}
}

TEST_CASE("Copy Assignment") {
	SECTION("empty") {
		auto g = gdwg::graph<int, int>{1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		g.insert_edge(1, 1, 5);
		auto g2 = g;
		CHECK(!g2.empty());
		CHECK(g2.is_node(1));
	}
	SECTION("integer nodes and edges") {
		auto g = gdwg::graph<int, int>{1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		g.insert_edge(1, 1, 5);
		g.insert_edge(1, 1, 10);
		auto g2 = g;
		CHECK(!g2.empty());
		CHECK(g2.is_node(1));
		auto v = g2.weights(1, 1);
		CHECK(v.at(0) == 5);
		CHECK(v.at(1) == 10);
	}

	SECTION("string nodes") {
		auto s = std::string("duh");
		auto g = gdwg::graph<std::string, int>{s};
		CHECK(!g.empty());
		CHECK(g.is_node(s));
		g.insert_edge(s, s, 5);
		g.insert_edge(s, s, 10);
		auto g2 = g;
		CHECK(!g2.empty());
		CHECK(g2.is_node(s));
		auto v = g2.weights(s, s);
		CHECK(v.at(0) == 5);
		CHECK(v.at(1) == 10);
	}
}