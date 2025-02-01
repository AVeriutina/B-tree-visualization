#include "BPlusTree.h"

int main() {
  BPlusTree tree(3);
  assert(tree.Insert(6));
  assert(tree.Insert(5));
  assert(tree.Insert(7));
  assert(tree.Insert(588));
  assert(tree.Insert(45));
}