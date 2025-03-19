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


      TINYGRAPH_WARN_UNUSED
      bool empty() const {
        return ::tinygraph_is_empty(handle.get());
      }

      TINYGRAPH_WARN_UNUSED
      std::uint32_t num_nodes() const {
        return ::tinygraph_get_num_nodes(handle.get());
      }

      TINYGRAPH_WARN_UNUSED
      std::uint32_t num_edges() const {
        return ::tinygraph_get_num_nodes(handle.get());
      }

      void get_out_edges(std::uint32_t node, std::uint32_t& first, std::uint32_t& last) const {
        ::tinygraph_get_out_edges(handle.get(), node, &first, &last);
      }

      TINYGRAPH_WARN_UNUSED
      std::uint32_t get_edge_target(std::uint32_t edge) const {
        return ::tinygraph_get_edge_target(handle.get(), edge);
      }

      TINYGRAPH_WARN_UNUSED
      std::uint32_t get_out_degree(std::uint32_t node) const {
        return ::tinygraph_get_out_degree(handle.get(), node);
      }

      TINYGRAPH_WARN_UNUSED
      bool has_node(std::uint32_t node) const {
        return ::tinygraph_has_node(handle.get(), node);
      }

      TINYGRAPH_WARN_UNUSED
      bool has_edge(std::uint32_t edge) const {
        return ::tinygraph_has_edge(handle.get(), edge);
      }

      TINYGRAPH_WARN_UNUSED
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

      // support static_cast<::tinygraph*> to get ptr
      explicit operator ::tinygraph*() const {
        return handle.get();
      }

    private:
      std::unique_ptr<::tinygraph, decltype(&::tinygraph_destruct)> handle;
  };

  class builder {
    public:

      TINYGRAPH_WARN_UNUSED
      builder& reserve(std::uint32_t n) {
        sources.reserve(n);
        targets.reserve(n);

        return *this;
      }

      TINYGRAPH_WARN_UNUSED
      builder& add_edge(std::uint32_t s, std::uint32_t t) {
        sources.push_back(s);
        targets.push_back(t);

        return *this;
      }

      TINYGRAPH_WARN_UNUSED
      graph build() {
        auto g = graph{sources, targets, edges_are_unsorted};

        sources.clear();
        sources.shrink_to_fit();

        targets.clear();
        targets.shrink_to_fit();

        return g;
      }

    private:
      std::vector<std::uint32_t> sources;
      std::vector<std::uint32_t> targets;
  };

  class dijkstra {
    public:

      // construct from graph view and egde weights vector
      dijkstra(const graph& g, const std::vector<std::uint16_t>& weights)
        : handle{::tinygraph_dijkstra_construct(static_cast<::tinygraph*>(g), weights.data()), &::tinygraph_dijkstra_destruct} { }

      // construct from graph view and edge weights ptr
      dijkstra(const graph& g, const std::uint16_t * weights)
        : handle{::tinygraph_dijkstra_construct(static_cast<::tinygraph*>(g), weights), &::tinygraph_dijkstra_destruct} { }


      TINYGRAPH_WARN_UNUSED
      bool shortest_path(std::uint32_t s, std::uint32_t t) {
        return ::tinygraph_dijkstra_shortest_path(handle.get(), s, t);
      }

      TINYGRAPH_WARN_UNUSED
      std::uint32_t get_distance() {
        return ::tinygraph_dijkstra_get_distance(handle.get());
      }

      TINYGRAPH_WARN_UNUSED
      bool get_path(std::vector<uint32_t>& v) {
        const std::uint32_t *it, *last;

        if (::tinygraph_dijkstra_get_path(handle.get(), &it, &last)) {
          v.assign(it, last);

          return true;
        }

        return false;
      }

      // not copy constructible
      dijkstra(const dijkstra& other) = delete;

      // not copy assignable
      dijkstra& operator=(const dijkstra& other) = delete;

      // move constructable
      dijkstra(dijkstra&&) noexcept = default;

      // move assignable
      dijkstra& operator=(dijkstra&&) noexcept = default;

      // support static_cast<::tinygraph_dijkstra*> to get ptr
      explicit operator ::tinygraph_dijkstra*() const {
        return handle.get();
      }

  private:
    std::unique_ptr<::tinygraph_dijkstra, decltype(&::tinygraph_dijkstra_destruct)> handle;
  };


  TINYGRAPH_WARN_UNUSED
  inline bool reorder(
      std::vector<std::uint32_t>& nodes,
      const std::vector<std::uint16_t> lngs,
      const std::vector<std::uint16_t> lats
  ) {
    return ::tinygraph_reorder(nodes.data(), lngs.data(), lats.data(), nodes.size());
  }

  TINYGRAPH_WARN_UNUSED
  inline bool reorder(
      std::uint32_t * nodes,
      const std::uint16_t * lngs,
      const std::uint16_t * lats,
      std::uint32_t n
  ) {
    return ::tinygraph_reorder(nodes, lngs, lats, n);
  }

} // namespace tg

#endif
