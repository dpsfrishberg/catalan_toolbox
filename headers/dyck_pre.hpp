#pragma once

#include "global.hpp"
#include "tree.hpp"

#include <string>

const static int NUM_OF_TESTS_DYCK = 1e5;
const static int TEST_MAX_EDGES_DYCK = 1e4;
const static int TEST_MAX_BRANCHES_DYCK = 1e2;

/**
 * Transform a dyck path to its pre-order tree representation.
 * If the dyck path is not valid, throw an exception.
 *
 * @param path:  A dyck path of string type
 *
 * @return The root of the tree represented by the dyck path
 */
Node *dyck_path_to_tree_pre_order(std::string path);

/**
 * Create a random Dyck Path
 *
 * @param deg:   Degree of the path
 * @param len:   Length of the path
 *
 * @return A dyck path of string type
 */
std::string get_random_dyck_path_pre_order(int deg, int len);

/**
 * Convert a tree into its pre-order dyck path representation
 *
 * @param root:     The root of the tree
 *
 * @return Its dyck path of string type
 */
std::string tree_to_dyck_path_pre_order(const Node *root);

/**
 * Plot a dyck path
 *
 * @param dyck:  A dyck path
 */
void plot_dyck_path(const Dyck dyck, std::string file);

/**
 * Plot all dyck paths of length `length`
 *
 * @param length:  Dyck path length, for 2-dyck path only, between 2 and 8,
 * inclusive.
 */
void plot_all_dyck_path(int length);

/**
 * Plot `count` number of random dyck graph
 *
 * @param length:   Dyck path length
 * @param r:        r-dyck path
 * @param count:    Number of random dyck graphs
 */
void plot_random_dyck_path(int length, int r, int count);

/**
 * Flip a mountain of a dyck path
 *
 * @param dyck:  A dyck path
 */
void flip_mountain(Dyck dyck);

/**
 * Verify if a dyck path is valid or not
 *
 * @param path:  A dyck path
 *
 * @return True if it is a valid dyck path, false otherwise
 */
bool is_valid_dyck_path_pre_order(std::string path);

/**
 * Find the `r` value for a given dyck path in pre-order. It assumes that the given
 * dyck path is valid.
 *
 * @param path:  A dyck path in pre-order
 *
 * @return r
 */
int get_r_pre_order(const Dyck dyck);

/**
 * 100,000 test cases of creating a random dyck path, converting it to tree
 * and back to dyck path, then comparing if they are identical. By default,
 * the maximum of degree generated is 100 with a maximum of 10,000 edges.
 *
 * Every 4%, it prints the progress status to the console.
 *
 * In the end, it displays the stats of all test cases sorted by degrees
 * if all 100,000 test cases pass; otherwise, it asserts in error and displays
 * the discrepancy of the 2 dyck paths.
 */
void test_conversion_dyck_path_pre_order();

/**
 * From degree 2 to 5 (due to time), sample 10k random trees with
 * 100k internal nodes, and then compare it to the expected height.
 *
 * Assert in error if they differ by more than 1%.
 */
void test_expected_height_pre_order();