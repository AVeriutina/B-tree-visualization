#pragma once

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <vector>

namespace detail {
struct Node {
  using KeyType = int64_t;

  std::vector<std::unique_ptr<Node>> children;
  std::vector<KeyType> keys;
  Node* parent = nullptr;
  Node* left = nullptr;
  Node* right = nullptr;
};

bool IsLeaf(const Node* node);
Node* LeftSibling(Node* right_sibling);
Node* RightSibling(Node* left_sibling);
bool IsKeyInNode(const Node* node, Node::KeyType key);
auto FindIterOfKey(const std::vector<Node::KeyType>& kyes, Node::KeyType key);
void Link(Node* left_node, Node* right_node);
void UpdateParent(const std::vector<std::unique_ptr<Node>>& children,
                  Node* new_parent);
bool IsNodeStateCorrect(Node* node);
bool IsLinkWithChildCorrect(Node* parent, Node* child);

template <typename T>
void AssignLeftHalf(std::vector<T>* from, std::vector<T>* to) {
  assert(to);
  to->assign(std::make_move_iterator(from->begin()),
             std::make_move_iterator(from->begin() + from->size() / 2));
}

template <typename T>
void AssignRightHalf(std::vector<T>* from, std::vector<T>* to) {
  assert(to);
  to->assign(std::make_move_iterator(from->begin() + from->size() / 2),
             std::make_move_iterator(from->end()));
}
}  // namespace detail

class BPlusTree {
  using Node = detail::Node;
  using KeyType = Node::KeyType;
  static constexpr auto IsLeaf = detail::IsLeaf;
  static constexpr auto LeftSibling = detail::LeftSibling;
  static constexpr auto RightSibling = detail::RightSibling;
  static constexpr auto IsKeyInNode = detail::IsKeyInNode;
  static constexpr auto Link = detail::Link;
  static constexpr auto UpdateParent = detail::UpdateParent;
  static constexpr auto IsNodeStateCorrect = detail::IsNodeStateCorrect;
  static constexpr auto IsLinkWithChildCorrect = detail::IsLinkWithChildCorrect;

 public:
  explicit BPlusTree(int32_t max_degree);
  BPlusTree(const BPlusTree&) = delete;
  BPlusTree& operator=(const BPlusTree&) = delete;
  BPlusTree(BPlusTree&&) noexcept = default;
  BPlusTree& operator=(BPlusTree&&) noexcept = default;
  ~BPlusTree() = default;

  bool FindKey(KeyType key);
  bool Insert(KeyType key);
  bool Delete(KeyType key);

 private:
  static bool IsStateCorrect(Node* ptr);
  void InsertKeyInNode(Node* node, KeyType key);
  void Split(Node* old_node);
  Node* FindLeafWithKey(Node::KeyType key);
  Node* FindLeafWithKeyFromNode(Node::KeyType key, Node* start_node);
  void DeleteInNode(Node* node, Node::KeyType key);
  void BorrowFromLeft(Node* node, Node::KeyType prev_key);
  void BorrowFromRight(Node* node);
  void Merge(Node* node, Node::KeyType key_of_node_in_parent);
  void UpdateKeys(Node* node, Node::KeyType prev_key, Node::KeyType new_key);

  size_t max_degree_;
  std::unique_ptr<Node> root_ = nullptr;
};
