#ifndef TINYGRAPH_HPP
#define TINYGRAPH_HPP

#include <cstdint>
#include <memory>
#include <vector>

#include <tinygraph/tinygraph.h>


namespace tg {

  enum edges_are_sorted_s { edges_are_sorted };
  enum edges_are_unsorted_s { edges_are_unsorted };

  class graph {
    public:

      // construct from vectors, sorted
      graph(
          const std::vector<std::uint32_t>& sources,
          const std::vector<std::uint32_t>& targets,
          edges_are_sorted_s)
        : handle{::tinygraph_construct_from_sorted_edges(
            sources.data(), targets.data(), sources.size()), &::tinygraph_destruct} { }

      // construct from vectors, unsorted
      graph(
          const std::vector<std::uint32_t>& sources,
          const std::vector<std::uint32_t>& targets,
          edges_are_unsorted_s)
        : handle{::tinygraph_construct_from_unsorted_edges(
            sources.data(), targets.data(), sources.size()), &::tinygraph_destruct} { }

      // construct from ptr, sorted
      graph(
          const std::uint32_t * sources,
          const std::uint32_t * targets,
          std::uint32_t n,
          edges_are_sorted_s)
        : handle{::tinygraph_construct_from_sorted_edges(
            sources, targets, n), &::tinygraph_destruct} { }

      // construct from ptr, unsorted
      graph(
          const std::uint32_t * sources,
          const std::uint32_t * targets,
          std::uint32_t n,
          edges_are_unsorted_s)
        : handle{::tinygraph_construct_from_unsorted_edges(
            sources, targets, n), &::tinygraph_destruct} { }


      bool empty() const {
        return ::tinygraph_is_empty(handle.get());
      }

      std::uint32_t num_nodes() const {
        return ::tinygraph_get_num_nodes(handle.get());
      }

      std::uint32_t num_edges() const {
        return ::tinygraph_get_num_nodes(handle.get());
      }

      void get_out_edges(std::uint32_t node, std::uint32_t& first, std::uint32_t& last) const {
        ::tinygraph_get_out_edges(handle.get(), node, &first, &last);
      }

      std::uint32_t get_edge_target(std::uint32_t edge) const {
        return ::tinygraph_get_edge_target(handle.get(), edge);
      }

      std::uint32_t get_out_degree(std::uint32_t node) const {
        return ::tinygraph_get_out_degree(handle.get(), node);
      }

      bool has_node(std::uint32_t node) const {
        return ::tinygraph_has_node(handle.get(), node);
      }

      bool has_edge(std::uint32_t edge) const {
        return ::tinygraph_has_edge(handle.get(), edge);
      }

      bool has_edge_from_to(std::uint32_t from, std::uint32_t to) const {
        return ::tinygraph_has_edge_from_to(handle.get(), from, to);
      }

      // copy constructible
      graph(const graph& other)
        : handle{::tinygraph_copy(other.handle.get()), &::tinygraph_destruct} { }

      // copy assignable
      graph& operator=(const graph& other) {
        if (this != &other) {
          handle.reset(::tinygraph_copy(other.handle.get()));
        }

        return *this;
      }

      // move constructable
      graph(graph&&) noexcept = default;

      // move assignable
      graph& operator=(graph&&) noexcept = default;

    private:
      std::unique_ptr<::tinygraph, decltype(&::tinygraph_destruct)> handle;
  };

  bool reorder(
      std::vector<std::uint32_t>& nodes,
      const std::vector<std::uint16_t> lngs,
      const std::vector<std::uint16_t> lats
  ) {
    return ::tinygraph_reorder(nodes.data(), lngs.data(), lats.data(), nodes.size());
  }

  bool reorder(
      std::uint32_t * nodes,
      const std::uint16_t * lngs,
      const std::uint16_t * lats,
      std::uint32_t n
  ) {
    return ::tinygraph_reorder(nodes, lngs, lats, n);
  }

}

#endif
