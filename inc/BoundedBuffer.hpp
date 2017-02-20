#ifndef __BOUNDED_BUFFER_HPP__
#define __BOUNDED_BUFFER_HPP__

#include <memory>
#include <list>

template <typename T> class BoundedBuffer {
  class Node {
  public:
    T &getData() { return data; }
    void setData(const T &newData) { data = newData; }

    std::shared_ptr<Node> prev, next;

  private:
    T data;
  };

  class CyclicallyLinkedList {
  public:
    std::shared_ptr<Node> head, tail;

    CyclicallyLinkedList(unsigned size) : size_(size) {
      std::shared_ptr<Node> prev_node;
      for (unsigned i = 0; i < size; ++i) {
        if (i == 0) {
          head.reset(new Node());
          prev_node = head;
        } else if (i == size - 1) {
          tail.reset(new Node());

          tail.get()->prev = prev_node;
          prev_node.get()->next = tail;

          tail.get()->next = head;
          head.get()->prev = tail;

          prev_node = tail;
        } else {
          std::shared_ptr<Node> next_node(new Node());

          next_node.get()->prev = prev_node;
          prev_node.get()->next = next_node;

          prev_node = next_node;
        }
      }
    }

    // Because the list consists of cyclically linked shared pointers, in order
    // for the memory to be deleted, one of the pointers must be reset, thus
    // disposing of the reference counts for atleast one of the nodes in the
    // list, triggering object destruction
    ~CyclicallyLinkedList() {
      // TODO: Test that this works using valgrind
      delete head.get();
    }

    unsigned size() const { return size_; }

  private:
    unsigned size_;
  };

  class CyclicallyLinkedListForwardIterator {
  public:
    CyclicallyLinkedListForwardIterator(
        const std::shared_ptr<CyclicallyLinkedList> &list)
        : traversalCount_(0) {
      list_ = list;
      currentNode_ = list.get()->head;
    }

    T &operator*() { return currentNode_.get()->getData(); }

    bool operator==(const CyclicallyLinkedListForwardIterator &it) const {
      return traversalCount_ == it.traversalCount_;
    }

    bool operator!=(const CyclicallyLinkedListForwardIterator &it) const {
      return !operator==(it);
    }

    void operator++() {
      currentNode_ = currentNode_.get()->next;
      traversalCount_++;
    }

  private:
    unsigned traversalCount_;

    std::shared_ptr<CyclicallyLinkedList> list_;
    std::shared_ptr<Node> currentNode_;

    friend class BoundedBuffer;
  };

public:
  BoundedBuffer(unsigned size) : size_(size) {
    buffer_.reset(new CyclicallyLinkedList(size));
  }

  unsigned size() const { return buffer_.get()->size(); }

  void push(const T &newData) {
    auto buffer = buffer_.get();

    buffer->tail.get()->setData(newData);
    buffer->head = buffer->tail;
    buffer->tail = buffer->head.get()->prev;
  }

  T &peek() const { return buffer_.get()->head.get()->getData(); }

  CyclicallyLinkedListForwardIterator begin() const {
    return CyclicallyLinkedListForwardIterator(buffer_);
  }

  CyclicallyLinkedListForwardIterator end() const {
    auto it = CyclicallyLinkedListForwardIterator(buffer_);
    it.traversalCount_ = buffer_.get()->size();

    return it;
  }

private:
  unsigned size_;

  std::shared_ptr<CyclicallyLinkedList> buffer_;
};

#endif // __BOUNDED_BUFFER_HPP__
