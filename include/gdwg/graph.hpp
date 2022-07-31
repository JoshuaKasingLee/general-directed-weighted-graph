#ifndef GDWG_GRAPH_HPP
#define GDWG_GRAPH_HPP

#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <utility>
#include <vector>

namespace gdwg {
	template<typename N, typename E>
	class graph {
	private:
		class iterator;

	public:
		using iterator = iterator;
		using const_iterator = iterator;
		struct value_type {
			N from;
			N to;
			E weight;
		};

		graph();
		graph(std::initializer_list<N> il);
		template<typename InputIt>
		graph(InputIt first, InputIt last);
		graph(graph<N, E>&& other) noexcept;
		auto operator=(graph<N, E>&& other) noexcept -> graph<N, E>&;
		graph(graph<N, E> const& other);
		auto operator=(graph<N, E> const& other) -> graph<N, E>&;

		~graph() = default;

		auto insert_node(N const& value) -> bool;
		auto is_node(N const& value) const -> bool;
		[[nodiscard]] auto empty() const noexcept -> bool;
		auto insert_edge(N const& src, N const& dst, E const& weight) -> bool;
		auto is_connected(N const& src, N const& dst) const -> bool;
		auto nodes() const noexcept -> std::vector<N>;
		auto replace_node(N const& old_data, N const& new_data) -> bool;
		auto merge_replace_node(N const& old_data, N const& new_data) -> void;
		auto erase_node(N const& value) -> bool;
		auto erase_edge(N const& src, N const& dst, E const& weight) -> bool;
		auto clear() noexcept -> void;
		auto weights(N const& src, N const& dst) const -> std::vector<E>;
		auto connections(N const& src) const -> std::vector<N>;
		auto operator==(graph<N, E> const& other) const noexcept -> bool;
		auto begin() const noexcept -> iterator;
		auto end() const noexcept -> iterator;
		auto find(N const& src, N const& dst, E const& weight) const -> iterator;
		auto erase_edge(iterator i) -> iterator;
		auto erase_edge(iterator i, iterator s) -> iterator;
		friend auto operator<<(std::ostream& os, graph<N, E> const& g) -> std::ostream& {
			auto const& nodes = g.nodes_rep_;
			auto const& edges = g.edges_rep_;

			for (auto iter = nodes.begin(); iter != nodes.end(); iter++) {
				os << **iter << " (\n";
				auto const edges__map_iter = edges.find(*iter);
				if (edges__map_iter != edges.end()) {
					auto const& edges_set = edges__map_iter->second;
					for (auto edges_set_iter = edges_set.begin(); edges_set_iter != edges_set.end();
					     edges_set_iter++) {
						os << "  " << *(edges_set_iter->first) << " | " << edges_set_iter->second << "\n";
					}
				}
				os << ")\n";
			}

			return os;
		}

	private:
		std::set<std::unique_ptr<N>> node_u_ptrs_;
		struct node_compare {
			auto operator()(N const* n1, N const* n2) const -> bool {
				return (*n1 < *n2);
			}
		};

		struct edge_compare {
			auto operator()(std::pair<N const*, E> const& pair_1,
			                std::pair<N const*, E> const& pair_2) const -> bool {
				if (*(pair_1.first) != *(pair_2.first)) {
					return *(pair_1.first) < *(pair_2.first);
				}

				return pair_1.second < pair_2.second;
			}
		};

		std::map<N const*, std::set<std::pair<N const*, E>, edge_compare>, node_compare> edges_rep_;
		std::set<N const*, node_compare> nodes_rep_;

		using edges_map_iter_t =
		   typename std::map<N const*, std::set<std::pair<N const*, E>, edge_compare>, node_compare>::const_iterator;
		using edges_set_iter_t =
		   typename std::set<std::pair<N const*, E>, edge_compare>::const_iterator;

		class iterator {
		public:
			using value_type = graph<N, E>::value_type;
			using reference = value_type;
			using pointer = void;
			using difference_type = std::ptrdiff_t;
			using iterator_category = std::bidirectional_iterator_tag;

			iterator() = default;

			auto operator*() const -> reference {
				return value_type{*(curr_map_iter_->first),
				                  *(curr_set_iter_->first),
				                  curr_set_iter_->second};
			}

			auto operator++() -> iterator& {
				auto const curr_set_end = curr_map_iter_->second.end();
				curr_set_iter_++;
				if (curr_set_iter_ == curr_set_end) {
					curr_map_iter_++;
					if (curr_map_iter_ != edges_map_end_) {
						curr_set_iter_ = curr_map_iter_->second.begin();
					}
					else {
						curr_set_iter_ = edges_set_iter_t();
					}
				}
				return *this;
			}
			auto operator++(int) -> iterator {
				auto before_iterator = *this;
				++*this;
				return before_iterator;
			}
			auto operator--() -> iterator& {
				if (curr_map_iter_ == edges_map_end_) {
					curr_map_iter_ = std::prev(edges_map_end_, 1);
					curr_set_iter_ = std::prev(curr_map_iter_->second.end(), 1);
					return *this;
				}

				auto const curr_set_begin = curr_map_iter_->second.begin();
				if (curr_set_iter_ == curr_set_begin) {
					if (curr_map_iter_ != edges_map_begin_) {
						curr_map_iter_--;
						curr_set_iter_ = std::prev(curr_map_iter_->second.end(), 1);
					}
				}
				else {
					curr_set_iter_--;
				}
				return *this;
			}
			auto operator--(int) -> iterator {
				auto before_iterator = *this;
				--*this;
				return before_iterator;
			}

			auto operator==(iterator const& other) const -> bool {
				return (curr_map_iter_ == other.curr_map_iter_ && curr_set_iter_ == other.curr_set_iter_);
			}

		private:
			friend class graph<N, E>;
			edges_map_iter_t edges_map_begin_;
			edges_map_iter_t edges_map_end_;
			edges_map_iter_t curr_map_iter_;
			edges_set_iter_t curr_set_iter_;

			explicit iterator(
			   std::map<N const*, std::set<std::pair<N const*, E>, edge_compare>, node_compare> const& edges)
			: edges_map_begin_(edges.begin())
			, edges_map_end_(edges.end())
			, curr_map_iter_(edges.begin()) {
				if (!edges.empty()) {
					curr_set_iter_ = edges.begin()->second.begin();
				}
			}

			explicit iterator(
			   std::map<N const*, std::set<std::pair<N const*, E>, edge_compare>, node_compare> const& edges,
			   bool)
			: edges_map_begin_(edges.begin())
			, edges_map_end_(edges.end())
			, curr_map_iter_(edges.end()) {}

			explicit iterator(
			   std::map<N const*, std::set<std::pair<N const*, E>, edge_compare>, node_compare> const& edges,
			   edges_map_iter_t map_iter,
			   edges_set_iter_t set_iter)
			: edges_map_begin_(edges.begin())
			, edges_map_end_(edges.end())
			, curr_map_iter_(map_iter)
			, curr_set_iter_(set_iter) {}
		};
	};

	template<typename N, typename E>
	graph<N, E>::graph() = default;

	template<typename N, typename E>
	graph<N, E>::graph(std::initializer_list<N> il)
	: graph(il.begin(), il.end()) {}

	template<typename N, typename E>
	template<typename InputIt>
	graph<N, E>::graph(InputIt first, InputIt last) {
		for (auto iter = first; iter != last; iter++) {
			insert_node(*iter);
		}
	}

	template<typename N, typename E>
	graph<N, E>::graph(graph<N, E>&& other) noexcept
	: node_u_ptrs_(std::exchange(other.node_u_ptrs_, std::set<std::unique_ptr<N>>{}))
	, edges_rep_(std::exchange(
	     other.edges_rep_,
	     std::map<N const*, std::set<std::pair<N const*, E>, edge_compare>, node_compare>{}))
	, nodes_rep_(std::exchange(other.nodes_rep_, std::set<N const*, node_compare>{})) {}

	template<typename N, typename E>
	auto graph<N, E>::operator=(graph<N, E>&& other) noexcept -> graph<N, E>& {
		node_u_ptrs_ = std::exchange(other.node_u_ptrs_, std::set<std::unique_ptr<N>>{});
		edges_rep_ = std::exchange(
		   other.edges_rep_,
		   std::map<N const*, std::set<std::pair<N const*, E>, edge_compare>, node_compare>{});
		nodes_rep_ = std::exchange(other.nodes_rep_, std::set<N const*, node_compare>{});
		return *this;
	}

	template<typename N, typename E>
	graph<N, E>::graph(graph<N, E> const& other) {
		for (auto const& ptr : other.nodes_rep_) {
			insert_node(*ptr);
		}

		for (auto iter = other.edges_rep_.begin(); iter != other.edges_rep_.end(); iter++) {
			auto const& curr_edges = iter->second;
			auto const& curr_src = iter->first;
			for (auto iter2 = curr_edges.begin(); iter2 != curr_edges.end(); iter2++) {
				insert_edge(*curr_src, *(iter2->first), iter2->second);
			}
		}
	}

	template<typename N, typename E>
	auto graph<N, E>::operator=(graph<N, E> const& other) -> graph<N, E>& {
		if (this == &other) {
			return *this;
		}
		for (auto const& ptr : other.nodes_rep_) {
			insert_node(*ptr);
		}

		for (auto iter = other.edges_rep_.begin(); iter != other.edges_rep_.end(); iter++) {
			auto const& curr_edges = iter->second;
			auto const& curr_src = iter->first;
			for (auto iter2 = curr_edges.begin(); iter2 != curr_edges.end(); iter2++) {
				insert_edge(*curr_src, *(iter2->first), iter2->second);
			}
		}
		return *this;
	}

	template<typename N, typename E>
	auto graph<N, E>::insert_node(N const& value) -> bool {
		if (is_node(value)) {
			return false;
		}
		auto const emplaced = node_u_ptrs_.emplace(std::make_unique<N>(value));
		nodes_rep_.emplace((*(emplaced.first)).get());
		return true;
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::is_node(N const& value) const -> bool {
		return nodes_rep_.contains(&value);
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::empty() const noexcept -> bool {
		return nodes_rep_.empty();
	}

	template<typename N, typename E>
	auto graph<N, E>::insert_edge(N const& src, N const& dst, E const& weight) -> bool {
		if (!is_node(src) || !is_node(dst)) {
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::insert_edge when either src or "
			                         "dst node does not exist");
		}

		auto src_search = edges_rep_.find(&src);
		if (src_search == edges_rep_.end()) {
			auto const empty_edges = std::set<std::pair<N const*, E>, edge_compare>();
			auto const& src_nodes_rep_search = nodes_rep_.find(&src);
			edges_rep_.emplace(*src_nodes_rep_search, empty_edges);
			src_search = edges_rep_.find(&src);
		}

		auto dst_search = nodes_rep_.find(&dst);
		auto const edge_search = src_search->second.find(std::pair<N const*, E>({*dst_search, weight}));
		if (edge_search == src_search->second.end()) {
			src_search->second.emplace(*dst_search, weight);
			return true;
		}
		return false;
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::is_connected(N const& src, N const& dst) const -> bool {
		if (!is_node(src) || !is_node(dst)) {
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::is_connected if src or dst node "
			                         "don't exist in the graph");
		}
		auto src_search = edges_rep_.find(&src);
		if (src_search == edges_rep_.end()) {
			return false;
		}

		auto dst_search = nodes_rep_.find(&dst);
		auto dst_ptr = *dst_search;
		auto const& src_edges = src_search->second;
		for (auto iter = src_edges.begin(); iter != src_edges.end(); iter++) {
			if (iter->first == dst_ptr) {
				return true;
			}
		}
		return false;
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::nodes() const noexcept -> std::vector<N> {
		auto result_vec = std::vector<N>{};
		for (auto iter = nodes_rep_.begin(); iter != nodes_rep_.end(); iter++) {
			result_vec.emplace_back(**iter);
		}
		return result_vec;
	}

	template<typename N, typename E>
	auto graph<N, E>::replace_node(N const& old_data, N const& new_data) -> bool {
		if (!is_node(old_data)) {
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::replace_node on a node that "
			                         "doesn't exist");
		}
		if (is_node(new_data)) {
			return false;
		}

		auto const emplaced = node_u_ptrs_.emplace(std::make_unique<N>(new_data));
		nodes_rep_.emplace((*(emplaced.first)).get());

		auto old_data_search = edges_rep_.find(&old_data);
		if (old_data_search != edges_rep_.end()) {
			auto old_data_node = edges_rep_.extract(old_data_search->first);
			old_data_node.key() = (*(emplaced.first)).get();
			edges_rep_.insert(std::move(old_data_node));
		}

		auto old_nodes_rep_search = nodes_rep_.find(&old_data);

		for (auto iter = edges_rep_.begin(); iter != edges_rep_.end(); iter++) {
			auto& curr_edges = iter->second;
			for (auto iter2 = curr_edges.begin(); iter2 != curr_edges.end();) {
				if (*old_nodes_rep_search == iter2->first) {
					auto const& weight = iter2->second;
					auto extracted_edge = curr_edges.extract(iter2++);
					extracted_edge.value() = std::pair<N const*, E>((*(emplaced.first)).get(), weight);
					curr_edges.insert(std::move(extracted_edge));
				}
				else {
					iter2++;
				}
			}
		}
		nodes_rep_.erase(old_nodes_rep_search);
		for (auto iter = node_u_ptrs_.begin(); iter != node_u_ptrs_.end(); iter++) {
			if (**iter == old_data) {
				node_u_ptrs_.erase(iter);
				break;
			}
		}
		return true;
	}

	template<typename N, typename E>
	auto graph<N, E>::merge_replace_node(N const& old_data, N const& new_data) -> void {
		if (!is_node(old_data) || !is_node(new_data)) {
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::merge_replace_node on old or new "
			                         "data if they don't exist in the graph");
		}

		auto old_nodes_rep_search = nodes_rep_.find(&old_data);
		auto new_nodes_rep_search = nodes_rep_.find(&new_data);

		for (auto iter = edges_rep_.begin(); iter != edges_rep_.end(); iter++) {
			auto& curr_edges = iter->second;
			for (auto iter2 = curr_edges.begin(); iter2 != curr_edges.end();) {
				if (*old_nodes_rep_search == iter2->first) {
					auto const& weight = iter2->second;
					auto extracted_edge = curr_edges.extract(iter2++);
					extracted_edge.value() = std::pair<N const*, E>(*new_nodes_rep_search, weight);
					curr_edges.insert(std::move(extracted_edge));
				}
				else {
					iter2++;
				}
			}
		}

		auto old_data_search = edges_rep_.find(&old_data);

		if (old_data_search != edges_rep_.end()) {
			auto const& old_data_edges = old_data_search->second;
			auto new_data_search = edges_rep_.find(&new_data);
			if (new_data_search == edges_rep_.end()) {
				auto const empty_edges = std::set<std::pair<N const*, E>, edge_compare>();
				edges_rep_.emplace(*new_nodes_rep_search, empty_edges);
				new_data_search = edges_rep_.find(&new_data);
			}
			auto& new_data_edges = new_data_search->second;

			for (auto iter = old_data_edges.begin(); iter != old_data_edges.end(); iter++) {
				if (!new_data_edges.contains(std::pair<N const*, E>(iter->first, iter->second))) {
					insert_edge(*(new_data_search->first), *(iter->first), iter->second);
				}
			}
			edges_rep_.erase(old_data_search);
		}
		nodes_rep_.erase(old_nodes_rep_search);
		for (auto iter = node_u_ptrs_.begin(); iter != node_u_ptrs_.end(); iter++) {
			if (**iter == old_data) {
				node_u_ptrs_.erase(iter);
				break;
			}
		}
	}

	template<typename N, typename E>
	auto graph<N, E>::erase_node(N const& value) -> bool {
		if (!is_node(value)) {
			return false;
		}
		auto old_data_search = edges_rep_.find(&value);
		if (old_data_search != edges_rep_.end()) {
			edges_rep_.erase(old_data_search);
		}
		auto value_nodes_rep_search = nodes_rep_.find(&value);

		for (auto iter = edges_rep_.begin(); iter != edges_rep_.end();) {
			auto& curr_edges = iter->second;
			for (auto iter2 = curr_edges.begin(); iter2 != curr_edges.end();) {
				if (*value_nodes_rep_search == iter2->first) {
					curr_edges.erase(iter2++);
				}
				else {
					iter2++;
				}
			}
			if (curr_edges.empty()) {
				edges_rep_.erase(iter++);
			}
			else {
				iter++;
			}
		}
		nodes_rep_.erase(value_nodes_rep_search);
		for (auto iter = node_u_ptrs_.begin(); iter != node_u_ptrs_.end(); iter++) {
			if (**iter == value) {
				node_u_ptrs_.erase(iter);
				break;
			}
		}
		return true;
	}

	template<typename N, typename E>
	auto graph<N, E>::erase_edge(N const& src, N const& dst, E const& weight) -> bool {
		if (!is_node(src) || !is_node(dst)) {
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::erase_edge on src or dst if they "
			                         "don't exist in the graph");
		}
		auto iter = find(src, dst, weight);
		if (iter == end()) {
			return false;
		}
		erase_edge(iter);
		return true;
	}

	template<typename N, typename E>
	auto graph<N, E>::clear() noexcept -> void {
		edges_rep_.clear();
		nodes_rep_.clear();
		node_u_ptrs_.clear();
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::weights(N const& src, N const& dst) const -> std::vector<E> {
		if (!is_node(src) || !is_node(dst)) {
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::weights if src or dst node don't "
			                         "exist in the graph");
		}

		auto result_vec = std::vector<E>{};

		auto src_search = edges_rep_.find(&src);
		if (src_search == edges_rep_.end()) {
			return result_vec;
		}
		auto dst_search = nodes_rep_.find(&dst);
		auto dst_ptr = *dst_search;
		auto const& src_edges = src_search->second;

		for (auto iter = src_edges.begin(); iter != src_edges.end(); iter++) {
			if (iter->first == dst_ptr) {
				result_vec.emplace_back(iter->second);
			}
		}
		return result_vec;
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::connections(N const& src) const -> std::vector<N> {
		if (!is_node(src)) {
			throw std::runtime_error("Cannot call gdwg::graph<N, E>::connections if src doesn't exist "
			                         "in the graph");
		}

		auto result_vec = std::vector<N>{};

		auto src_search = edges_rep_.find(&src);
		if (src_search == edges_rep_.end()) {
			return result_vec;
		}

		auto const& src_edges = src_search->second;

		auto prev = src_edges.begin()->first;

		for (auto iter = src_edges.begin(); iter != src_edges.end(); iter++) {
			if (result_vec.empty()) {
				result_vec.emplace_back(*(iter->first));
			}
			else {
				if (iter->first != prev) {
					result_vec.emplace_back(*(iter->first));
				}
			}
			prev = iter->first;
		}
		return result_vec;
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::operator==(graph<N, E> const& other) const noexcept -> bool {
		if (edges_rep_.size() != other.edges_rep_.size()
		    || nodes_rep_.size() != other.nodes_rep_.size()) {
			return false;
		}
		auto other_node_iter = other.nodes_rep_.begin();
		for (auto node_iter = nodes_rep_.begin(); node_iter != nodes_rep_.end(); node_iter++) {
			if (**node_iter != **other_node_iter) {
				return false;
			}
			other_node_iter++;
		}

		auto other_iter = other.edges_rep_.begin();
		for (auto iter = edges_rep_.begin(); iter != edges_rep_.end(); iter++) {
			if (*(iter->first) != *(other_iter->first)) {
				return false;
			}
			auto const& edges = iter->second;
			auto const& other_edges = other_iter->second;
			if (edges.size() != other_edges.size()) {
				return false;
			}
			auto other_edge_iter = other_edges.begin();
			for (auto edge_iter = edges.begin(); edge_iter != edges.end(); edge_iter++) {
				if (*(edge_iter->first) != *(other_edge_iter->first)
				    || edge_iter->second != other_edge_iter->second)
				{
					return false;
				}
				other_edge_iter++;
			}
			other_iter++;
		}
		return true;
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::begin() const noexcept -> iterator {
		return iterator(edges_rep_);
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::end() const noexcept -> iterator {
		return iterator(edges_rep_, true);
	}

	template<typename N, typename E>
	[[nodiscard]] auto graph<N, E>::find(N const& src, N const& dst, E const& weight) const
	   -> iterator {
		auto src_search = edges_rep_.find(&src);

		if (src_search == edges_rep_.end()) {
			return iterator(edges_rep_, true);
		}
		auto dst_search = nodes_rep_.find(&dst);
		if (dst_search == nodes_rep_.end()) {
			return iterator(edges_rep_, true);
		}
		auto dst_ptr = *dst_search;
		auto const& src_edges = src_search->second;

		auto edge_search = src_edges.find(std::pair<N const*, E>(dst_ptr, weight));

		if (edge_search == src_edges.end()) {
			return iterator(edges_rep_, true);
		}

		return iterator(edges_rep_, src_search, edge_search);
	}

	template<typename N, typename E>
	auto graph<N, E>::erase_edge(iterator i) -> iterator {
		auto iter = i;
		auto const map_iter = iter.curr_map_iter_;
		auto non_c_iter = edges_rep_.erase(map_iter, map_iter);
		auto const set_iter = iter.curr_set_iter_;
		iter++;

		non_c_iter->second.erase(set_iter);
		if ((non_c_iter->second).empty()) {
			edges_rep_.erase(map_iter);
		}
		return iter;
	}

	template<typename N, typename E>
	auto graph<N, E>::erase_edge(iterator i, iterator s) -> iterator {
		auto iter = i;
		while (iter != s) {
			iter = erase_edge(iter);
		}
		return iter;
	}

} // namespace gdwg

#endif
