#ifndef SRC_ALGO_LRU_H_
#define SRC_ALGO_LRU_H_

#include <iostream>
#include <list>
#include <unordered_map>

#include "common/marco.h"

namespace algo {

template <typename key_type, typename value_type>
class Lru {
 public:
  typedef std::list<std::pair<key_type, value_type>> List;
  typedef std::unordered_map<key_type, typename List::iterator> Table;

  Lru(size_t _cap) : m_capacity(_cap){};
  ~Lru() = default;

  value_type Get(key_type key);
  virtual int Put(key_type key, value_type value);

  size_t capacity() const { return m_capacity; }
  void set_capacity(size_t cap) { m_capacity = cap; }

  Table& table() { return m_table; }

  List& list() { return m_list; }

 private:
  // 容量
  size_t m_capacity;
  // 哈希表
  Table m_table;
  // 存储列表
  List m_list;

  DISALLOW_COPY_AND_ASSIGN(Lru);
};

template <typename key_type, typename value_type>
value_type Lru<key_type, value_type>::Get(key_type key) {
  auto it = m_table.find(key);
  if (it == m_table.end()) return value_type{};

  auto res = it->second->second;
  m_list.splice(m_list.begin(), m_list, it->second);
  return res;
}

template <typename key_type, typename value_type>
int Lru<key_type, value_type>::Put(key_type key, value_type value) {
  auto it = m_table.find(key);
  if (it != m_table.end()) {
    it->second->second = value;
    m_list.splice(m_list.begin(), m_list, it->second);
    it->second = m_list.begin();
    return 0;
  }

  m_list.emplace_front(std::pair<key_type, value_type>{key, value});
  m_table[key] = m_list.begin();

  if (m_list.size() > m_capacity) {
    auto i = m_list.back().first;
    m_list.erase(m_table[i]);
    m_table.erase(i);
  }
  return 0;
}

}  // namespace algo

#endif  // SRC_ALGO_LRU_H_
