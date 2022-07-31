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

TEST_CASE("insert_node") {
	SECTION("integer nodes") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		auto completed = g.insert_node(5);
		CHECK(completed);
		CHECK(g.is_node(5));
	}

	SECTION("string nodes") {
		auto g = gdwg::graph<std::string, int>();
		CHECK(g.empty());
		auto str = std::string("what");
		auto completed = g.insert_node(std::string("what"));
		CHECK(completed);
		CHECK(g.is_node(str));
	}

	SECTION("already exists") {
		auto g = gdwg::graph<std::string, int>();
		CHECK(g.empty());
		auto str = std::string("what");
		auto completed = g.insert_node(std::string("what"));
		CHECK(g.is_node(str));
		CHECK(completed);
		auto completed2 = g.insert_node(std::string("what"));
		CHECK(!completed2);
		auto v = g.nodes();
		CHECK(v.size() == 1);
		CHECK(v[0] == std::string("what"));
	}
}

TEST_CASE("insert_edge") {
	SECTION("string edges") {
		auto g = gdwg::graph<std::string, std::string>();
		CHECK(g.empty());
		auto str = std::string("what");
		auto str2 = std::string("im cool");
		auto str3 = std::string("third");
		g.insert_node(str);
		g.insert_node(str2);
		g.insert_node(str3);

		CHECK(g.is_node(str));
		CHECK(g.is_node(str2));
		CHECK(g.is_node(str3));
		auto w = std::string("cows");
		auto w2 = std::string("large");
		auto w3 = std::string("donkey");
		auto completed = g.insert_edge(str, str2, w);
		CHECK(completed);
		auto iter = g.find(str, str2, w);
		CHECK((*iter).from == str);
		CHECK((*iter).to == str2);
		CHECK((*iter).weight == w);
		auto completed2 = g.insert_edge(str3, str2, w3);
		CHECK(completed2);
		auto iter2 = g.find(str3, str2, w3);
		CHECK((*iter2).from == str3);
		CHECK((*iter2).to == str2);
		CHECK((*iter2).weight == w3);
	}

	SECTION("integer edges") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);

		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		auto completed = g.insert_edge(1, 2, 5);
		CHECK(completed);
		auto iter = g.find(1, 2, 5);
		CHECK((*iter).from == 1);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 5);
		auto completed2 = g.insert_edge(3, 2, 30);
		CHECK(completed2);
		auto iter2 = g.find(3, 2, 30);
		CHECK((*iter2).from == 3);
		CHECK((*iter2).to == 2);
		CHECK((*iter2).weight == 30);
	}

	SECTION("src does not exist") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);

		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		CHECK_THROWS_WITH(g.insert_edge(10, 2, 5),
		                  "Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node "
		                  "does "
		                  "not exist");
	}

	SECTION("dst does not exist") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);

		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		CHECK_THROWS_WITH(g.insert_edge(1, 20, 5),
		                  "Cannot call gdwg::graph<N, E>::insert_edge when either src or dst node "
		                  "does not exist");
	}

	SECTION("edge exists already") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);

		CHECK(g.is_node(1));
		CHECK(g.is_node(2));
		CHECK(g.is_node(3));
		auto completed2 = g.insert_edge(1, 2, 5);
		CHECK(completed2);
		auto iter = g.find(1, 2, 5);
		CHECK((*iter).from == 1);
		CHECK((*iter).to == 2);
		CHECK((*iter).weight == 5);
		auto completed = g.insert_edge(1, 2, 5);
		CHECK(!completed);
	}
}

TEST_CASE("replace_node") {
	SECTION("replacing integer nodes") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);

		auto completed = g.replace_node(1, 5);
		CHECK(completed);
		auto v = g.nodes();
		CHECK(v[0] == 2);
		CHECK(v[1] == 3);
		CHECK(v[2] == 5);
		CHECK(v.size() == 3);
	}

	SECTION("replacing string nodes") {
		auto g = gdwg::graph<std::string, int>();
		CHECK(g.empty());
		auto str = std::string("a");
		auto str2 = std::string("b");
		auto str3 = std::string("c");
		auto str5 = std::string("e");
		g.insert_node(str);
		g.insert_node(str2);
		g.insert_node(str3);

		auto completed = g.replace_node(str, str5);
		CHECK(completed);
		auto v = g.nodes();
		CHECK(v[0] == str2);
		CHECK(v[1] == str3);
		CHECK(v[2] == str5);
		CHECK(v.size() == 3);
	}

	SECTION("replacing edges with old data as src") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(1, 3, 10);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 3);
		auto completed = g.replace_node(1, 5);
		CHECK(completed);
		CHECK(!g.is_node(1));

		auto v = g.nodes();
		CHECK(v[0] == 2);
		CHECK(v[1] == 3);
		CHECK(v[2] == 5);

		auto w = g.weights(5, 2);
		CHECK(w.size() == 2);
		CHECK(w[0] == 3);
		CHECK(w[1] == 5);

		auto w2 = g.weights(5, 3);
		CHECK(w2.size() == 1);
		CHECK(w2[0] == 10);
	}

	SECTION("replacing edges with old data as dst") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(1, 3, 10);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 3);

		auto completed = g.replace_node(2, 5);
		CHECK(completed);
		CHECK(!g.is_node(2));

		auto v = g.nodes();
		CHECK(v[0] == 1);
		CHECK(v[1] == 3);
		CHECK(v[2] == 5);

		CHECK(g.is_connected(1, 5));
		auto w = g.weights(1, 5);
		CHECK(w.size() == 2);
		CHECK(w[0] == 3);
		CHECK(w[1] == 5);
	}

	SECTION("replacing edges with old data as dst but MULTIPLE src nodes") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(1, 3, 10);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 3);
		g.insert_edge(3, 2, 5);
		g.insert_edge(3, 2, 30);

		auto completed = g.replace_node(2, 5);
		CHECK(completed);
		CHECK(!g.is_node(2));

		auto v = g.nodes();
		CHECK(v[0] == 1);
		CHECK(v[1] == 3);
		CHECK(v[2] == 5);

		CHECK(g.is_connected(1, 5));
		auto w = g.weights(1, 5);
		CHECK(w.size() == 2);
		CHECK(w[0] == 3);
		CHECK(w[1] == 5);

		auto w2 = g.weights(3, 5);
		CHECK(w2.size() == 2);
		CHECK(w2[0] == 5);
		CHECK(w2[1] == 30);
	}

	SECTION("replacing edges with old data as dst and src") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(2, 3, 10);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 3);

		auto completed = g.replace_node(2, 5);
		CHECK(completed);
		CHECK(!g.is_node(2));

		auto v = g.nodes();
		CHECK(v[0] == 1);
		CHECK(v[1] == 3);
		CHECK(v[2] == 5);

		CHECK(g.is_connected(1, 5));
		auto w = g.weights(1, 5);
		CHECK(w.size() == 2);
		CHECK(w[0] == 3);
		CHECK(w[1] == 5);

		auto w2 = g.weights(5, 3);
		CHECK(w2.size() == 1);
		CHECK(w2[0] == 10);
	}

	SECTION("new data exists") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(1, 3, 10);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 3);

		auto completed = g.replace_node(2, 1);
		CHECK(!completed);
	}

	SECTION("src isnt a node") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(1, 3, 10);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 3);

		CHECK_THROWS_WITH(g.replace_node(20, 5),
		                  "Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't exist");
	}
}

TEST_CASE("merge_replace_node") {
	SECTION("replacing integer nodes") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);

		g.merge_replace_node(1, 3);
		auto v = g.nodes();
		CHECK(v[0] == 2);
		CHECK(v[1] == 3);
		CHECK(v.size() == 2);
	}

	SECTION("replacing string nodes") {
		auto g = gdwg::graph<std::string, int>();
		CHECK(g.empty());
		auto str = std::string("a");
		auto str2 = std::string("b");
		auto str3 = std::string("c");
		auto str5 = std::string("e");
		g.insert_node(str);
		g.insert_node(str2);
		g.insert_node(str3);

		g.merge_replace_node(str, str3);
		auto v = g.nodes();
		CHECK(v[0] == str2);
		CHECK(v[1] == str3);
		CHECK(v.size() == 2);
	}

	SECTION("replacing edges with old data as src or dsts") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(1, 3, 10);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 3);
		g.insert_edge(3, 1, 99);

		auto w2 = g.weights(3, 1);
		CHECK(w2.size() == 1);
		CHECK(w2[0] == 99);
		g.merge_replace_node(1, 3);

		CHECK(!g.is_node(1));

		auto v = g.nodes();
		CHECK(v[0] == 2);
		CHECK(v[1] == 3);

		auto w = g.weights(3, 2);
		CHECK(w.size() == 2);
		CHECK(w[0] == 3);
		CHECK(w[1] == 5);

		w2 = g.weights(3, 3);
		CHECK(w2.size() == 2);
		CHECK(w2[0] == 10);
		CHECK(w2[1] == 99);
	}

	SECTION("new data node does not exist") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(1, 3, 10);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 3);

		CHECK_THROWS_WITH(g.merge_replace_node(2, 10),
		                  "Cannot call gdwg::graph<N, E>::merge_replace_node on old or new data if "
		                  "they don't exist in the graph");
	}

	SECTION("old data node does not exist") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(1, 3, 10);
		g.insert_edge(1, 2, 5);
		g.insert_edge(1, 2, 3);

		CHECK_THROWS_WITH(g.replace_node(20, 1),
		                  "Cannot call gdwg::graph<N, E>::replace_node on a node that doesn't exist");
	}
}

TEST_CASE("erase_node") {
	SECTION("integer nodes") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		auto completed = g.erase_node(3);
		CHECK(completed);
		auto v = g.nodes();
		CHECK(v[0] == 1);
		CHECK(v[1] == 2);
	}

	SECTION("string nodes") {
		auto g = gdwg::graph<std::string, int>();
		CHECK(g.empty());
		auto str = std::string("a");
		auto str2 = std::string("b");
		auto str3 = std::string("c");
		g.insert_node(str);
		g.insert_node(str2);
		g.insert_node(str3);
		auto completed = g.erase_node(str3);
		CHECK(completed);
		auto v = g.nodes();
		CHECK(v[0] == str);
		CHECK(v[1] == str2);
	}

	SECTION("erase node with outgoing edges") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(1, 2, 10);
		g.insert_edge(1, 2, 30);
		auto completed = g.erase_node(1);
		CHECK(completed);
		auto v = g.nodes();
		CHECK(v[0] == 2);
		CHECK(v[1] == 3);

		auto iter = g.find(1, 2, 10);
		CHECK(iter == g.end());
		auto iter2 = g.find(1, 2, 30);
		CHECK(iter2 == g.end());
	}

	SECTION("erase node with incoming edges") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(1, 2, 10);
		g.insert_edge(1, 2, 30);
		auto completed = g.erase_node(2);
		CHECK(completed);
		auto v = g.nodes();
		CHECK(v[0] == 1);
		CHECK(v[1] == 3);

		auto iter = g.find(1, 2, 10);
		CHECK(iter == g.end());
		auto iter2 = g.find(1, 2, 30);
		CHECK(iter2 == g.end());
	}

	SECTION("non exist node") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(1, 2, 10);
		g.insert_edge(1, 2, 30);
		CHECK(!g.erase_node(2000));
	}
}

TEST_CASE("erase_edge") {
	SECTION("string nodes") {
		auto g = gdwg::graph<std::string, int>();
		CHECK(g.empty());
		auto str = std::string("a");
		auto str2 = std::string("b");
		auto str3 = std::string("c");
		g.insert_node(str);
		g.insert_node(str2);
		g.insert_node(str3);
		g.insert_edge(str3, str2, 5);
		g.insert_edge(str3, str2, 12);
		auto completed = g.erase_edge(str3, str2, 5);
		CHECK(completed);
		auto iter = g.find(str3, str2, 5);
		CHECK(iter == g.end());
	}

	SECTION("integer nodes") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(3, 2, 5);
		g.insert_edge(3, 2, 6);
		auto completed = g.erase_edge(3, 2, 5);
		CHECK(completed);
		auto iter = g.find(3, 2, 5);
		CHECK(iter == g.end());
	}

	SECTION("src does not exist") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(3, 2, 5);
		g.insert_edge(3, 2, 6);
		CHECK_THROWS_WITH(g.erase_edge(20, 2, 5),
		                  "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't "
		                  "exist in the graph");
	}

	SECTION("dst does not exist") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(3, 2, 5);
		g.insert_edge(3, 2, 6);
		CHECK_THROWS_WITH(g.erase_edge(3, 20, 5),
		                  "Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they don't "
		                  "exist in the graph");
	}

	SECTION("can't find edge") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(3, 2, 5);
		g.insert_edge(3, 2, 6);
		CHECK(!g.erase_edge(3, 2, 1));
	}
}

TEST_CASE("clear") {
	SECTION("string nodes") {
		auto g = gdwg::graph<std::string, int>();
		auto str = std::string("what");
		auto str2 = std::string("im cool");
		g.insert_node(str);
		g.insert_node(str2);
		CHECK(g.is_node(str));
		CHECK(g.is_node(str2));
		g.clear();
		CHECK(g.empty());
	}

	SECTION("integer nodes") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.clear();
		CHECK(g.empty());
	}
	SECTION("clearing graph with edges too") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(3, 2, 5);
		g.insert_edge(3, 2, 6);
		g.clear();
		CHECK(g.empty());
	}
}

TEST_CASE("erase_edge(iterator)") {
	SECTION("string nodes") {
		auto g = gdwg::graph<std::string, int>();
		CHECK(g.empty());
		auto str = std::string("a");
		auto str2 = std::string("b");
		auto str3 = std::string("c");
		g.insert_node(str);
		g.insert_node(str2);
		g.insert_node(str3);
		g.insert_edge(str3, str2, 5);
		g.insert_edge(str3, str2, 12);
		auto iter = g.find(str3, str2, 5);
		auto iter3 = g.erase_edge(iter);
		auto iter4 = g.erase_edge(iter3);
		CHECK(iter4 == g.end());
		auto iter2 = g.find(str3, str2, 5);
		CHECK(iter2 == g.end());
		iter2 = g.find(str3, str2, 12);
		CHECK(iter2 == g.end());
	}

	SECTION("integer nodes") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(3, 2, 5);
		g.insert_edge(3, 2, 6);
		auto iter = g.find(3, 2, 5);
		auto iter2 = g.erase_edge(iter);
		auto iter3 = g.erase_edge(iter2);
		CHECK(iter3 == g.end());
		auto iter4 = g.find(3, 2, 5);
		CHECK(iter4 == g.end());
		auto iter5 = g.find(3, 2, 6);
		CHECK(iter5 == g.end());
	}
}

TEST_CASE("erase_edge(iterator, iterator)") {
	SECTION("string nodes") {
		auto g = gdwg::graph<std::string, int>();
		CHECK(g.empty());
		auto str = std::string("a");
		auto str2 = std::string("b");
		auto str3 = std::string("c");
		g.insert_node(str);
		g.insert_node(str2);
		g.insert_node(str3);
		g.insert_edge(str3, str2, 5);
		g.insert_edge(str3, str2, 12);
		g.insert_edge(str3, str2, 16);
		g.insert_edge(str3, str2, 122);
		g.insert_edge(str3, str2, 1220);
		auto iter = g.find(str3, str2, 12);
		auto iter2 = g.find(str3, str2, 122);
		g.erase_edge(iter, iter2);
		CHECK(g.find(str3, str2, 5) != g.end());
		CHECK(g.find(str3, str2, 12) == g.end());
		CHECK(g.find(str3, str2, 16) == g.end());
		CHECK(g.find(str3, str2, 122) != g.end());
		CHECK(g.find(str3, str2, 1220) != g.end());
	}

	SECTION("integer nodes") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(3, 2, 5);
		g.insert_edge(3, 2, 6);
		g.insert_edge(3, 2, 16);
		g.insert_edge(3, 2, 122);
		g.insert_edge(3, 2, 1220);
		auto iter = g.find(3, 2, 6);
		auto iter2 = g.find(3, 2, 122);
		g.erase_edge(iter, iter2);
		CHECK(g.find(3, 2, 5) != g.end());
		CHECK(g.find(3, 2, 6) == g.end());
		CHECK(g.find(3, 2, 16) == g.end());
		CHECK(g.find(3, 2, 122) != g.end());
		CHECK(g.find(3, 2, 1220) != g.end());
	}

	SECTION("second iterator is the end") {
		auto g = gdwg::graph<int, int>();
		CHECK(g.empty());
		g.insert_node(1);
		g.insert_node(2);
		g.insert_node(3);
		g.insert_edge(3, 2, 5);
		g.insert_edge(3, 2, 6);
		g.insert_edge(3, 2, 16);
		g.insert_edge(3, 2, 122);
		g.insert_edge(3, 2, 1220);
		auto iter = g.find(3, 2, 6);

		auto iter2 = g.erase_edge(iter, g.end());
		CHECK(iter2 == g.end());
		CHECK(g.find(3, 2, 5) != g.end());
		CHECK(g.find(3, 2, 6) == g.end());
		CHECK(g.find(3, 2, 16) == g.end());
		CHECK(g.find(3, 2, 122) == g.end());
		CHECK(g.find(3, 2, 1220) == g.end());
	}
}
