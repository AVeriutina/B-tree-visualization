#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <vector>

struct Node {
  using KeyType = int64_t;

  bool is_leaf = false;
  std::vector<Node*> children;
  std::vector<KeyType> keys;
  Node* parent = nullptr;
  Node* left = nullptr;
  Node* right = nullptr;
};

class BPlusTree {
 public:
  explicit BPlusTree(size_t max_degree)
      : max_degree_(max_degree), root_(nullptr) {}

  bool FindKey(const Node::KeyType& key);
  bool Insert(const Node::KeyType& key);
  bool Delete(const Node::KeyType& key);

 private:
  void Split(Node* old_node);
  void DeleteInNode(Node* node, const Node::KeyType& key);
  void Merge(Node* node);
  void BorrowFromLeft(Node* node);
  void BorrowFromRight(Node* node);
  void UpdateKeys(Node* node);

  Node* FindLeafWithKey(const Node::KeyType& key);
  Node* FindLeafWithKeyFromNode(const Node::KeyType& key, Node* start_node);

  size_t max_degree_;
  Node* root_;
};
