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

TEST_CASE("is_node function") {
	SECTION("integer nodes") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
	}

	SECTION("string nodes") {
		auto str = std::string("abra");
		auto str2 = std::string("bradabra");
		auto str3 = std::string("cadabra");
		auto g = gdwg::graph<std::string, int>{str3, str, str2};
		CHECK(g.is_node(str));
		CHECK(g.is_node(str2));
		CHECK(g.is_node(str3));
	}

	SECTION("non-exist") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		CHECK(!g.is_node(5));
	}
}

TEST_CASE("empty function") {
	SECTION("integer nodes not empty") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		CHECK(!g.empty());
	}

	SECTION("string nodes not empty") {
		auto str = std::string("abra");
		auto str2 = std::string("bradabra");
		auto str3 = std::string("cadabra");
		auto g = gdwg::graph<std::string, int>{str3, str, str2};
		CHECK(!g.empty());
	}

	SECTION("empty") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
	}
}

TEST_CASE("is_connected function") {
	SECTION("integer nodes connected") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		g.insert_edge(3, 2, 5);
		CHECK(g.is_connected(3, 2));
	}
	SECTION("string nodes connected") {
		auto str = std::string("abra");
		auto str2 = std::string("bradabra");
		auto str3 = std::string("cadabra");
		auto g = gdwg::graph<std::string, int>{str3, str, str2};
		g.insert_edge(str3, str2, 5);
		CHECK(g.is_connected(str3, str2));
		;
	}
	SECTION("not connected") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		g.insert_edge(3, 2, 5);
		CHECK(!g.is_connected(1, 2));
	}
	SECTION("src doesn't exist") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		g.insert_edge(3, 2, 5);
		CHECK_THROWS_WITH(g.is_connected(17, 2),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");
	}
	SECTION("dst doesn't exist") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		g.insert_edge(3, 2, 5);
		CHECK_THROWS_WITH(g.is_connected(1, 12),
		                  "Cannot call gdwg::graph<N, E>::is_connected if src or dst node don't "
		                  "exist in the graph");
	}
}

TEST_CASE("nodes function") {
	SECTION("integers") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));

		auto vec = g.nodes();
		CHECK(vec[0] == 1);
		CHECK(vec[1] == 2);
		CHECK(vec[2] == 3);
	}

	SECTION("strings") {
		auto str = std::string("abra");
		auto str2 = std::string("bradabra");
		auto str3 = std::string("cadabra");
		auto g = gdwg::graph<std::string, int>{str3, str, str2};
		CHECK(!g.empty());
		CHECK(g.is_node(str));
		CHECK(g.is_node(str2));
		CHECK(g.is_node(str3));

		auto vec = g.nodes();
		CHECK(vec[0] == str);
		CHECK(vec[1] == str2);
		CHECK(vec[2] == str3);
	}
}

TEST_CASE("weights function") {
	SECTION("integers") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		g.insert_edge(2, 3, 1);
		g.insert_edge(2, 3, 0);
		g.insert_edge(2, 3, 20);
		auto vec = g.weights(2, 3);
		CHECK(vec[0] == 0);
		CHECK(vec[1] == 1);
		CHECK(vec[2] == 20);
		g.insert_edge(1, 3, 40);
		auto vec2 = g.weights(1, 3);
		CHECK(vec2[0] == 40);
	}

	SECTION("strings") {
		auto str = std::string("a");
		auto str2 = std::string("b");
		auto str3 = std::string("c");
		auto g = gdwg::graph<std::string, std::string>{str3, str, str2};
		CHECK(!g.empty());
		CHECK(g.is_node(str));
		CHECK(g.is_node(str2));
		CHECK(g.is_node(str3));
		g.insert_edge(str2, str3, str3);
		g.insert_edge(str2, str3, str);
		g.insert_edge(str2, str3, str2);
		auto vec = g.weights(str2, str3);
		CHECK(vec[0] == str);
		CHECK(vec[1] == str2);
		CHECK(vec[2] == str3);
		g.insert_edge(str, str3, str3);
		auto vec2 = g.weights(str, str3);
		CHECK(vec2[0] == str3);
	}

	SECTION("src non exist") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		g.insert_edge(2, 3, 1);
		g.insert_edge(2, 3, 0);
		g.insert_edge(2, 3, 20);
		CHECK_THROWS_WITH(g.weights(20, 3),
		                  "Cannot call gdwg::graph<N, E>::weights if src or dst node don't exist in "
		                  "the graph");
	}

	SECTION("dst non exist") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		g.insert_edge(2, 3, 1);
		g.insert_edge(2, 3, 0);
		g.insert_edge(2, 3, 20);
		CHECK_THROWS_WITH(g.weights(1, 30),
		                  "Cannot call gdwg::graph<N, E>::weights if src or dst node don't exist in "
		                  "the graph");
	}
}

TEST_CASE("find function") {
	SECTION("can find integer nodes") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		g.insert_edge(2, 3, 1);
		g.insert_edge(2, 3, 0);
		g.insert_edge(2, 3, 20);
		auto iter = g.find(2, 3, 0);
		CHECK((*iter).from == 2);
		CHECK((*iter).to == 3);
		CHECK((*iter).weight == 0);
	}

	SECTION("can find string nodes") {
		auto str = std::string("a");
		auto str2 = std::string("b");
		auto str3 = std::string("c");
		auto g = gdwg::graph<std::string, int>{str3, str2, str};
		g.insert_edge(str2, str3, 1);
		g.insert_edge(str2, str3, 0);
		g.insert_edge(str2, str3, 20);
		auto iter = g.find(str2, str3, 0);
		CHECK((*iter).from == str2);
		CHECK((*iter).to == str3);
		CHECK((*iter).weight == 0);
	}

	SECTION("cannot find src") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		g.insert_edge(2, 3, 1);
		g.insert_edge(2, 3, 0);
		g.insert_edge(2, 3, 20);
		auto iter = g.find(20, 3, 0);
		CHECK(iter == g.end());
	}

	SECTION("cannot find dst") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		g.insert_edge(2, 3, 1);
		g.insert_edge(2, 3, 0);
		g.insert_edge(2, 3, 20);
		auto iter = g.find(2, 30, 0);
		CHECK(iter == g.end());
	}
	SECTION("cannot find edge with the weight") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		g.insert_edge(2, 3, 1);
		g.insert_edge(2, 3, 0);
		g.insert_edge(2, 3, 20);
		auto iter = g.find(2, 3, 10);
		CHECK(iter == g.end());
	}
}

TEST_CASE("connections function") {
	SECTION("connected integer nodes") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		g.insert_edge(2, 3, 1);
		g.insert_edge(2, 1, 0);
		g.insert_edge(2, 2, 20);
		auto v = g.connections(2);
		CHECK(v[0] == 1);
		CHECK(v[1] == 2);
		CHECK(v[2] == 3);
	}

	SECTION("connected string nodes") {
		auto str = std::string("a");
		auto str2 = std::string("b");
		auto str3 = std::string("c");
		auto g = gdwg::graph<std::string, int>{str3, str2, str};
		g.insert_edge(str2, str3, 1);
		g.insert_edge(str2, str, 0);
		g.insert_edge(str2, str2, 20);
		auto v = g.connections(str2);
		CHECK(v[0] == str);
		CHECK(v[1] == str2);
		CHECK(v[2] == str3);
	}

	SECTION("connected to none") {
		auto str = std::string("a");
		auto str2 = std::string("b");
		auto str3 = std::string("c");
		auto g = gdwg::graph<std::string, int>{str3, str2, str};
		g.insert_edge(str3, str3, 1);
		g.insert_edge(str3, str, 0);
		g.insert_edge(str3, str2, 20);
		auto v = g.connections(str2);
		CHECK(v.empty());
	}

	SECTION("cannot find src") {
		auto g = gdwg::graph<int, int>{3, 2, 1};
		CHECK(!g.empty());
		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		g.insert_edge(2, 3, 1);
		g.insert_edge(2, 3, 0);
		g.insert_edge(2, 3, 20);
		CHECK_THROWS_WITH(g.connections(20),
		                  "Cannot call gdwg::graph<N, E>::connections if src doesn't exist in the "
		                  "graph");
	}
}
