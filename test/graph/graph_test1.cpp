#include "gdwg/graph.hpp"

#include <catch2/catch.hpp>

TEST_CASE("basic test") {
	// ignore this test
	auto g = gdwg::graph<int, int>{};
	g.insert_node(3);
	g.insert_node(5);
	g.insert_node(1);
	g.insert_node(4);
	g.insert_node(2);
	CHECK(g.is_node(3));
	CHECK(g.is_node(5));
	g.insert_edge(5, 3, 10);
	g.insert_edge(2, 4, 1);
	g.insert_edge(2, 3, 1);
	g.insert_edge(5, 4, 3);
	g.erase_edge(5, 4, 3);
	auto iter = g.begin();
	CHECK((*iter).from == 2);
	CHECK((*iter).to == 3);
	CHECK((*iter).weight == 1);
	iter++;
	CHECK((*iter).from == 2);
	CHECK((*iter).to == 4);
	CHECK((*iter).weight == 1);
	iter++;
	CHECK((*iter).from == 5);
	CHECK((*iter).to == 3);
	CHECK((*iter).weight == 10);
	iter--;
	iter--;
	CHECK((*iter).from == 2);
	CHECK((*iter).to == 3);
	CHECK((*iter).weight == 1);
	iter++;
	iter++;
	iter++;
	auto ender = g.end();
	CHECK(ender == iter);

	auto iter2 = g.begin();
	auto iter3 = g.begin();
	iter2++;
	iter3++;
	iter3++;
	iter3--;
	CHECK(iter3 == iter2);

	auto itfind = g.find(2, 4, 1);
	CHECK((*itfind).from == 2);
	CHECK((*itfind).to == 4);
	CHECK((*itfind).weight == 1);
	CHECK(itfind == iter2);

	auto itfind2 = g.find(2, 41, 5);
	CHECK(itfind2 == ender);
}
