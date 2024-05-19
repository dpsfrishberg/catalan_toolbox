#include "poly.hpp"
#include "tree.hpp"
#include "util.hpp"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <stdexcept>
#include <unordered_set>

Tree *Poly::to_tree() {
  enum Dir { Up, Down };
  int tables[this->sides][2];
  std::memset(tables, 0, sizeof(tables));
  std::vector<Node *> nodes(this->sides);
  std::vector<std::vector<int>> buckets(this->sides);

  // pre-processing
  for (int i{}; i < this->sides - 1; ++i) {
    nodes[i] = new Node(i);
    tables[i][Dir::Up] = i;
    tables[i + 1][Dir::Down] = i;
  }
  for (int i{}; i < int(poly.size()); ++i) {
    auto [l, r] = poly[i];
    assert(r > l);
    buckets[r - l].push_back(i);
  }

  // build tree
  int id{this->sides - 2};
  for (int len{}; len < this->sides; ++len) {
    for (int idx : buckets[len]) {
      auto [l, r] = poly[idx];
      auto parent = new Node(++id, 2);
      parent->children[0] = nodes[tables[l][Dir::Up]];
      parent->children[1] = nodes[tables[r][Dir::Down]];
      int pos{tables[l][Dir::Up]};
      nodes[pos] = parent;
      tables[r][Dir::Down] = pos;
      tables[l][Dir::Up] = pos;
    }
  }

  return new Tree(nodes[tables[0][Dir::Up]]);
}

Tree *Poly::into_tree() {
  auto tree{this->to_tree()};
  delete this;
  return tree;
}

void Poly::flip_and_plot() {
  // pre-processing - O(n)
  enum Dir { Up, Down };
  enum Relation { Left, Right, Parent }; // left, right, parent
  int id{int(this->poly.size())};
  int sz{int(this->poly.size())};
  int relationship[sz + this->sides][3];
  int tables[this->sides][2];
  std::memset(tables, 0, sizeof(tables));
  std::memset(tables, 0, sizeof(relationship));
  std::vector<std::vector<int>> buckets(this->sides);
  std::function<Edge(int)> get_edge = [&](int idx) {
    return idx < sz ? this->poly[idx] : std::array<int, 2>{{idx - sz, idx - sz + 1}};
  };
  for (int i{}; i < this->sides - 1; ++i) {
    tables[i][Dir::Up] = id;
    tables[i + 1][Dir::Down] = id;
    ++id;
  }
  for (int i{}; i < int(this->poly.size()); ++i) {
    auto [l, r] = this->poly[i];
    assert(r > l);
    buckets[r - l].push_back(i);
  }
  for (int len{}; len < this->sides; ++len) {
    for (int idx : buckets[len]) {
      auto [l, r] = poly[idx];
      int lc{tables[l][Dir::Up]};
      int rc{tables[r][Dir::Down]};
      relationship[idx][Relation::Left] = lc;
      relationship[idx][Relation::Right] = rc;
      relationship[lc][Relation::Parent] = idx;
      relationship[rc][Relation::Parent] = idx;
      tables[l][Dir::Up] = idx;
      tables[r][Dir::Down] = idx;
    }
  }

  // plot it to user
  this->plot();

  // flip an edge - O(1)
  while (1) {
    int idx{};
    while (idx < 1 || idx >= sz) {
      std::cout << "Select an edge to flip: ";
      std::cin >> idx;
    }

    // locate the other 2 vertices
    int parent{relationship[idx][Relation::Parent]};
    auto [l, r] = this->poly[idx];
    auto [p_l, p_r] = get_edge(parent);
    int u = get_edge(relationship[idx][Relation::Left])[1];
    int v = p_l != l && p_l != r ? p_l : p_r;
    if (u > v) { // make u < v
      std::swap(u, v);
    }

    // bookkeeping stuff
    int is_left_child = relationship[parent][Relation::Left] == idx;
    int sibling{is_left_child ? relationship[parent][Relation::Right]
                              : relationship[parent][Relation::Left]};
    auto [a, b] = get_edge(relationship[idx][Relation::Left]);
    int opposite{v >= b && a >= u}; // left child remains a child or not
    int tmp{relationship[idx][Relation::Left ^ opposite]};
    relationship[idx][Relation::Left ^ opposite] =
        relationship[idx][Relation::Right ^ opposite];
    relationship[idx][Relation::Right ^ opposite] = sibling;
    relationship[sibling][Relation::Parent] = idx;
    relationship[tmp][Relation::Parent] = parent;
    relationship[parent][Relation::Left] = is_left_child ? tmp : idx;
    relationship[parent][Relation::Right] = is_left_child ? idx : tmp;

    // update graph
    this->poly[idx] = {{u, v}};
    std::ofstream out{Poly::_WATCHDOG_FILE};
    if (!out) {
      std::cerr << std::format("{} cannot be opened.\n", Poly::_WATCHDOG_FILE);
      throw std::invalid_argument("");
    }
    out << idx << "\n";
    out << u << "," << v << "\n";
  }
}

Poly *Poly::get_random(int num_of_sides) {
  auto tree{Tree::get_random(2, num_of_sides - 2)};
  auto res{tree->into_poly()};
  return res;
}

void Poly::plot(std::string file) {
  std::ofstream out{file};
  if (!out) {
    std::cerr << file << " cannot be opened.\n";
    throw std::invalid_argument("");
  }

  // points and coordinates
  out << this->sides << "\n";
  auto [x, y] = Util::rotate(0.0, 1.0, 180.0 / this->sides);
  for (int i{}; i < this->sides; ++i) {
    out << std::fixed << std::setprecision(3) << x << "," << y << "\n";
    auto [nx, ny] = Util::rotate(x, y, 360.0 / this->sides);
    x = nx;
    y = ny;
  }

  // edges
  for (int i{}; i < this->sides - 1; ++i) {
    out << i << "," << i + 1 << "\n";
  }
  for (auto [a, b] : this->poly) {
    out << a << "," << b << "\n";
  }

  Util::plot(Poly::_PLOT_SCRIPT, file);
}

Poly *Poly::next() {
  // TODO: implement this
}

bool Poly::is_valid(const Graph &poly) {
  int num_of_sides{int(poly.size()) + 2};
  int end[num_of_sides];
  int start[num_of_sides];
  std::memset(end, 0, sizeof(end));
  std::memset(start, 0, sizeof(start));
  std::vector<std::unordered_set<int>> lines(num_of_sides);
  std::vector<int> stack{-1};

  for (const auto &edge : poly) {
    auto [s, e] = edge;
    lines[s].insert(e);
    ++end[e];
    ++start[s];
  }
  for (int i{}; i < num_of_sides; ++i) {
    while (end[i]-- && stack.back() >= 0) {
      int start{stack.back()};
      stack.pop_back();
      if (!lines[start].erase(i)) {
        return false;
      }
    }
    while (start[i]--) {
      stack.push_back(i);
    }
  }
  return true;
}

// TODO: swap out std::cerr with std::format
void Poly::test_conversion() {
  std::cout << "Starting testing conversion between poly and tree with num_of_sides "
               "from 3 to "
            << TEST_MAX_SIDES << "\n\n";

  for (int num_of_sides{3}; num_of_sides <= TEST_MAX_SIDES; ++num_of_sides) {
    for (int i{}; i < NUM_OF_TESTS; ++i) {
      auto tree1{Tree::get_random(2, num_of_sides - 2)};
      auto tree2{tree1->to_poly()->into_tree()};
      std::string id1{tree1->serialize()};
      std::string id2{tree2->serialize()};
      if (id1 != id2) {
        std::cerr << "Test Failed:\n"
                  << "num_of_sides = " << num_of_sides << "\n"
                  << "id1 = " << id1 << "\n"
                  << "id2 = " << id2 << "\n";
        assert(false);
      }
      tree1->free_memory();
      tree2->free_memory();
      delete tree1;
      delete tree2;
    }
    std::cout << "num_of_sides = " << num_of_sides << " done for " << NUM_OF_TESTS
              << " random tests!\n";
  }

  std::cout << "\n\nAll Tests Completed!\n\n";
}
