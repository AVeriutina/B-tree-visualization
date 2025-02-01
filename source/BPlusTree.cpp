#include "BPlusTree.h"

bool BPlusTree::FindKey(const Node::KeyType& key) {
  Node* node_with_key = FindLeafWithKey(key);
  assert(node_with_key != nullptr);
  for (const Node::KeyType& other_key : node_with_key->keys) {
    if (other_key == key) {
      return true;
    }
  }
  return false;
}

bool BPlusTree::Insert(const Node::KeyType& key) {
  if (root_ == nullptr) {
    root_ = new Node();
    root_->is_leaf = true;
    root_->keys.push_back(key);
    return true;
  }

  if (FindKey(key)) {
    return false;
  }

  Node* leaf = FindLeafWithKey(key);
  assert(leaf != nullptr);

  auto iter = std::lower_bound(leaf->keys.begin(), leaf->keys.end(), key);
  leaf->keys.insert(iter, key);

  if (leaf->keys.size() >= max_degree_) {
    Split(leaf);
  }

  return true;
}

void BPlusTree::Split(Node* old_node) {
  Node* parent = old_node->parent;
  Node* new_left_node = new Node();
  Node* new_right_node = new Node();

  //  size_t mid = max_degree_ / 2;
  //  Node::KeyType middle_key = old_node->keys[mid];

  size_t copy_after;

  if (old_node->is_leaf) {
    new_left_node->right = new_right_node;
    new_right_node->left = new_left_node;
    if (old_node->right) {
      new_right_node->right = old_node->right;
      old_node->right->left = new_right_node;
    }
    if (old_node->left) {
      new_left_node->left = old_node->left;
      old_node->left->right = new_left_node;
    }

    copy_after = max_degree_ / 2;
  } else {
    new_left_node->children.assign(
        old_node->children.begin(),
        old_node->children.begin() + (max_degree_ + 1) / 2);
    new_right_node->children.assign(
        old_node->children.begin() + (max_degree_ + 1) / 2,
        old_node->children.end());

    for (Node* child : new_left_node->children) {
      child->parent = new_left_node;
    }
    for (Node* child : new_right_node->children) {
      child->parent = new_right_node;
    }

    copy_after = max_degree_ / 2 + 1;
  }

  new_left_node->keys.assign(old_node->keys.begin(),
                             old_node->keys.begin() + max_degree_ / 2);
  new_left_node->is_leaf = old_node->is_leaf;

  new_right_node->keys.assign(old_node->keys.begin() + copy_after,
                              old_node->keys.end());
  new_right_node->is_leaf = old_node->is_leaf;

  if (parent == nullptr) {
    root_ = new Node();
    root_->keys.push_back(old_node->keys[max_degree_ / 2]);
    root_->children.push_back(new_left_node);
    root_->children.push_back(new_right_node);
    new_left_node->parent = root_;
    new_right_node->parent = root_;
  } else {
    auto iter_pos_in_children =
        std::find(parent->children.begin(), parent->children.end(), old_node);
    size_t pos_of_old_in_parent =
        std::distance(parent->children.begin(), iter_pos_in_children);

    *iter_pos_in_children = new_left_node;
    parent->children.insert(iter_pos_in_children + 1, new_right_node);
    parent->keys.insert(parent->keys.begin() + pos_of_old_in_parent,
                        old_node->keys[max_degree_ / 2]);

    new_left_node->parent = parent;
    new_right_node->parent = parent;

    if (parent->keys.size() >= max_degree_) {
      Split(parent);
    }
  }
  delete old_node;
}

Node* BPlusTree::FindLeafWithKey(const Node::KeyType& key) {
  if (root_ == nullptr) {
    return nullptr;
  }
  return FindLeafWithKeyFromNode(key, root_);
}

Node* BPlusTree::FindLeafWithKeyFromNode(const Node::KeyType& key,
                                         Node* start_node) {
  assert(start_node != nullptr);
  if (start_node->is_leaf) return start_node;

  for (size_t i = 0; i < start_node->keys.size(); ++i) {
    if (key < start_node->keys[i]) {
      return FindLeafWithKeyFromNode(key, start_node->children[i]);
    }
  }
  return FindLeafWithKeyFromNode(key, start_node->children.back());
}

// don't work really
bool BPlusTree::Delete(const Node::KeyType& key) {
  Node* leaf = FindLeafWithKey(key);
  if (!leaf || std::find(leaf->keys.begin(), leaf->keys.end(), key) ==
                   leaf->keys.end()) {
    return false;
  }

  DeleteInNode(leaf, key);
  return true;
}

void BPlusTree::DeleteInNode(Node* node, const Node::KeyType& key) {
  auto iter = std::find(node->keys.begin(), node->keys.end(), key);
  assert(iter != node->keys.end());

  node->keys.erase(iter);

  if (node->is_leaf) {
    if (node->keys.size() < (max_degree_ / 2)) {
      if (node->left && node->left->parent == node->parent &&
          node->left->keys.size() > (max_degree_ / 2)) {
        BorrowFromLeft(node);
      } else if (node->right && node->right->parent == node->parent &&
                 node->right->keys.size() > (max_degree_ / 2)) {
        BorrowFromRight(node);
      } else {
        Merge(node);
      }
    }
  } else {
    UpdateKeys(node);
  }
}

void BPlusTree::Merge(Node* node) {
  Node* left_sibling = node->left;
  Node* right_sibling = node->right;
  Node* parent = node->parent;

  if (left_sibling && left_sibling->parent == parent) {
    left_sibling->keys.insert(left_sibling->keys.end(), node->keys.begin(),
                              node->keys.end());
    left_sibling->right = node->right;
    if (node->right) node->right->left = left_sibling;
    DeleteInNode(parent, node->keys.front());
    delete node;
  } else if (right_sibling && right_sibling->parent == parent) {
    node->keys.insert(node->keys.end(), right_sibling->keys.begin(),
                      right_sibling->keys.end());
    node->right = right_sibling->right;
    if (right_sibling->right) right_sibling->right->left = node;
    DeleteInNode(parent, right_sibling->keys.front());
    delete right_sibling;
  }

  if (parent == root_ && root_->keys.empty()) {
    root_ = node;
    node->parent = nullptr;
  }
}

void BPlusTree::BorrowFromLeft(Node* node) {
  Node* left_sibling = node->left;
  node->keys.insert(node->keys.begin(), left_sibling->keys.back());
  left_sibling->keys.pop_back();
  UpdateKeys(node->parent);
}

void BPlusTree::BorrowFromRight(Node* node) {
  Node* right_sibling = node->right;
  node->keys.push_back(right_sibling->keys.front());
  right_sibling->keys.erase(right_sibling->keys.begin());
  UpdateKeys(node->parent);
}

void BPlusTree::UpdateKeys(Node* node) {
  while (node) {
    for (size_t i = 1; i < node->children.size(); ++i) {
      node->keys[i - 1] = node->children[i]->keys.front();
    }
    node = node->parent;
  }
}