#pragma once

#include <vector>
#include <list>
#include <deque>
#include <set>
#include <map>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <functional>
#include <numeric>
#include <random>
#include <chrono>

namespace LGE {

template<typename T>
class VectorUtils {
public:
    static void removeDuplicates(std::vector<T>& vec) {
        std::sort(vec.begin(), vec.end());
        auto last = std::unique(vec.begin(), vec.end());
        vec.erase(last, vec.end());
    }

    static void shuffle(std::vector<T>& vec) {
        static std::mt19937 rng(static_cast<unsigned int>(
            std::chrono::steady_clock::now().time_since_epoch().count()
        ));
        std::shuffle(vec.begin(), vec.end(), rng);
    }

    static T& getRandomElement(std::vector<T>& vec) {
        static std::mt19937 rng(static_cast<unsigned int>(
            std::chrono::steady_clock::now().time_since_epoch().count()
        ));
        std::uniform_int_distribution<size_t> dist(0, vec.size() - 1);
        return vec[dist(rng)];
    }

    static const T& getRandomElement(const std::vector<T>& vec) {
        static std::mt19937 rng(static_cast<unsigned int>(
            std::chrono::steady_clock::now().time_since_epoch().count()
        ));
        std::uniform_int_distribution<size_t> dist(0, vec.size() - 1);
        return vec[dist(rng)];
    }

    static void reverse(std::vector<T>& vec) {
        std::reverse(vec.begin(), vec.end());
    }

    static bool contains(const std::vector<T>& vec, const T& value) {
        return std::find(vec.begin(), vec.end(), value) != vec.end();
    }

    static size_t count(const std::vector<T>& vec, const T& value) {
        return std::count(vec.begin(), vec.end(), value);
    }

    static void fill(std::vector<T>& vec, const T& value) {
        std::fill(vec.begin(), vec.end(), value);
    }

    static void resizeAndFill(std::vector<T>& vec, size_t newSize, const T& value) {
        vec.resize(newSize);
        std::fill(vec.begin(), vec.end(), value);
    }

    static std::vector<T> slice(const std::vector<T>& vec, size_t start, size_t end) {
        if (start >= vec.size()) return {};
        if (end > vec.size()) end = vec.size();
        return std::vector<T>(vec.begin() + start, vec.begin() + end);
    }

    static std::vector<T> take(const std::vector<T>& vec, size_t n) {
        if (n >= vec.size()) return vec;
        return std::vector<T>(vec.begin(), vec.begin() + n);
    }

    static std::vector<T> drop(const std::vector<T>& vec, size_t n) {
        if (n >= vec.size()) return {};
        return std::vector<T>(vec.begin() + n, vec.end());
    }

    static std::vector<T> concat(const std::vector<T>& a, const std::vector<T>& b) {
        std::vector<T> result;
        result.reserve(a.size() + b.size());
        result.insert(result.end(), a.begin(), a.end());
        result.insert(result.end(), b.begin(), b.end());
        return result;
    }

    static void append(std::vector<T>& target, const std::vector<T>& source) {
        target.insert(target.end(), source.begin(), source.end());
    }

    static T sum(const std::vector<T>& vec) {
        return std::accumulate(vec.begin(), vec.end(), T{});
    }

    static T product(const std::vector<T>& vec) {
        if (vec.empty()) return T{1};
        return std::accumulate(vec.begin(), vec.end(), T{1}, std::multiplies<T>());
    }

    static T min(const std::vector<T>& vec) {
        return *std::min_element(vec.begin(), vec.end());
    }

    static T max(const std::vector<T>& vec) {
        return *std::max_element(vec.begin(), vec.end());
    }

    static void sort(std::vector<T>& vec) {
        std::sort(vec.begin(), vec.end());
    }

    template<typename Compare>
    static void sort(std::vector<T>& vec, Compare comp) {
        std::sort(vec.begin(), vec.end(), comp);
    }

    static bool allOf(const std::vector<T>& vec, std::function<bool(const T&)> pred) {
        return std::all_of(vec.begin(), vec.end(), pred);
    }

    static bool anyOf(const std::vector<T>& vec, std::function<bool(const T&)> pred) {
        return std::any_of(vec.begin(), vec.end(), pred);
    }

    static bool noneOf(const std::vector<T>& vec, std::function<bool(const T&)> pred) {
        return std::none_of(vec.begin(), vec.end(), pred);
    }

    template<typename U>
    static std::vector<U> transform(const std::vector<T>& vec, std::function<U(const T&)> func) {
        std::vector<U> result;
        result.reserve(vec.size());
        for (const auto& item : vec) {
            result.push_back(func(item));
        }
        return result;
    }

    static std::vector<T> filter(const std::vector<T>& vec, std::function<bool(const T&)> pred) {
        std::vector<T> result;
        for (const auto& item : vec) {
            if (pred(item)) {
                result.push_back(item);
            }
        }
        return result;
    }

    static ptrdiff_t findIndex(const std::vector<T>& vec, const T& value) {
        auto it = std::find(vec.begin(), vec.end(), value);
        if (it == vec.end()) return -1;
        return std::distance(vec.begin(), it);
    }

    static ptrdiff_t findIndexIf(const std::vector<T>& vec, std::function<bool(const T&)> pred) {
        auto it = std::find_if(vec.begin(), vec.end(), pred);
        if (it == vec.end()) return -1;
        return std::distance(vec.begin(), it);
    }

    static void eraseAtIndex(std::vector<T>& vec, size_t index) {
        if (index < vec.size()) {
            vec.erase(vec.begin() + index);
        }
    }

    static void insertAtIndex(std::vector<T>& vec, size_t index, const T& value) {
        if (index <= vec.size()) {
            vec.insert(vec.begin() + index, value);
        }
    }

    static void moveElement(std::vector<T>& vec, size_t from, size_t to) {
        if (from == to || from >= vec.size() || to >= vec.size()) return;
        T item = std::move(vec[from]);
        vec.erase(vec.begin() + from);
        vec.insert(vec.begin() + to, std::move(item));
    }

    static void swapElements(std::vector<T>& vec, size_t i, size_t j) {
        if (i < vec.size() && j < vec.size()) {
            std::swap(vec[i], vec[j]);
        }
    }

    static void rotateLeft(std::vector<T>& vec, size_t n) {
        if (vec.empty()) return;
        n = n % vec.size();
        std::rotate(vec.begin(), vec.begin() + n, vec.end());
    }

    static void rotateRight(std::vector<T>& vec, size_t n) {
        if (vec.empty()) return;
        n = n % vec.size();
        std::rotate(vec.rbegin(), vec.rbegin() + n, vec.rend());
    }

    static bool isSorted(const std::vector<T>& vec) {
        return std::is_sorted(vec.begin(), vec.end());
    }

    static size_t uniqueCount(const std::vector<T>& vec) {
        std::vector<T> copy = vec;
        removeDuplicates(copy);
        return copy.size();
    }

    static std::vector<std::pair<T, size_t>> frequency(const std::vector<T>& vec) {
        std::unordered_map<T, size_t> counts;
        for (const auto& item : vec) {
            counts[item]++;
        }
        std::vector<std::pair<T, size_t>> result;
        result.reserve(counts.size());
        for (const auto& [key, count] : counts) {
            result.emplace_back(key, count);
        }
        return result;
    }
};

template<typename T>
class ListUtils {
public:
    static void removeDuplicates(std::list<T>& lst) {
        lst.sort();
        lst.unique();
    }

    static void reverse(std::list<T>& lst) {
        lst.reverse();
    }

    static bool contains(const std::list<T>& lst, const T& value) {
        return std::find(lst.begin(), lst.end(), value) != lst.end();
    }

    static size_t count(const std::list<T>& lst, const T& value) {
        return std::count(lst.begin(), lst.end(), value);
    }

    static void sort(std::list<T>& lst) {
        lst.sort();
    }

    template<typename Compare>
    static void sort(std::list<T>& lst, Compare comp) {
        lst.sort(comp);
    }

    static typename std::list<T>::iterator nth(std::list<T>& lst, size_t n) {
        auto it = lst.begin();
        std::advance(it, n);
        return it;
    }

    static typename std::list<T>::const_iterator nth(const std::list<T>& lst, size_t n) {
        auto it = lst.begin();
        std::advance(it, n);
        return it;
    }

    static T& get(std::list<T>& lst, size_t n) {
        return *nth(lst, n);
    }

    static const T& get(const std::list<T>& lst, size_t n) {
        return *nth(lst, n);
    }

    static void eraseAtIndex(std::list<T>& lst, size_t n) {
        lst.erase(nth(lst, n));
    }

    static void insertAtIndex(std::list<T>& lst, size_t n, const T& value) {
        auto it = nth(lst, n);
        lst.insert(it, value);
    }

    static std::vector<T> toVector(const std::list<T>& lst) {
        return std::vector<T>(lst.begin(), lst.end());
    }

    static std::list<T> fromVector(const std::vector<T>& vec) {
        return std::list<T>(vec.begin(), vec.end());
    }

    static T sum(const std::list<T>& lst) {
        return std::accumulate(lst.begin(), lst.end(), T{});
    }

    static T product(const std::list<T>& lst) {
        if (lst.empty()) return T{1};
        return std::accumulate(lst.begin(), lst.end(), T{1}, std::multiplies<T>());
    }

    static T min(const std::list<T>& lst) {
        return *std::min_element(lst.begin(), lst.end());
    }

    static T max(const std::list<T>& lst) {
        return *std::max_element(lst.begin(), lst.end());
    }

    static bool allOf(const std::list<T>& lst, std::function<bool(const T&)> pred) {
        return std::all_of(lst.begin(), lst.end(), pred);
    }

    static bool anyOf(const std::list<T>& lst, std::function<bool(const T&)> pred) {
        return std::any_of(lst.begin(), lst.end(), pred);
    }

    static bool noneOf(const std::list<T>& lst, std::function<bool(const T&)> pred) {
        return std::none_of(lst.begin(), lst.end(), pred);
    }

    template<typename U>
    static std::list<U> transform(const std::list<T>& lst, std::function<U(const T&)> func) {
        std::list<U> result;
        for (const auto& item : lst) {
            result.push_back(func(item));
        }
        return result;
    }

    static std::list<T> filter(const std::list<T>& lst, std::function<bool(const T&)> pred) {
        std::list<T> result;
        for (const auto& item : lst) {
            if (pred(item)) {
                result.push_back(item);
            }
        }
        return result;
    }
};

template<typename K, typename V>
class MapUtils {
public:
    static bool containsKey(const std::map<K, V>& map, const K& key) {
        return map.find(key) != map.end();
    }

    static bool containsValue(const std::map<K, V>& map, const V& value) {
        return std::find_if(map.begin(), map.end(),
            [&value](const auto& pair) { return pair.second == value; }) != map.end();
    }

    static std::vector<K> keys(const std::map<K, V>& map) {
        std::vector<K> result;
        result.reserve(map.size());
        for (const auto& pair : map) {
            result.push_back(pair.first);
        }
        return result;
    }

    static std::vector<V> values(const std::map<K, V>& map) {
        std::vector<V> result;
        result.reserve(map.size());
        for (const auto& pair : map) {
            result.push_back(pair.second);
        }
        return result;
    }

    static std::map<V, K> invert(const std::map<K, V>& map) {
        std::map<V, K> result;
        for (const auto& pair : map) {
            result[pair.second] = pair.first;
        }
        return result;
    }

    static void merge(std::map<K, V>& target, const std::map<K, V>& source, bool overwrite = true) {
        for (const auto& pair : source) {
            if (overwrite || target.find(pair.first) == target.end()) {
                target[pair.first] = pair.second;
            }
        }
    }

    static V getOrDefault(const std::map<K, V>& map, const K& key, const V& defaultValue) {
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second;
        }
        return defaultValue;
    }

    static V getOrInsert(std::map<K, V>& map, const K& key, const V& defaultValue) {
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second;
        }
        map[key] = defaultValue;
        return defaultValue;
    }

    static std::vector<std::pair<K, V>> toVector(const std::map<K, V>& map) {
        return std::vector<std::pair<K, V>>(map.begin(), map.end());
    }

    static size_t countIf(const std::map<K, V>& map, std::function<bool(const K&, const V&)> pred) {
        size_t count = 0;
        for (const auto& pair : map) {
            if (pred(pair.first, pair.second)) {
                count++;
            }
        }
        return count;
    }

    static std::map<K, V> filter(const std::map<K, V>& map, std::function<bool(const K&, const V&)> pred) {
        std::map<K, V> result;
        for (const auto& pair : map) {
            if (pred(pair.first, pair.second)) {
                result.insert(pair);
            }
        }
        return result;
    }
};

template<typename K, typename V>
class UnorderedMapUtils {
public:
    static bool containsKey(const std::unordered_map<K, V>& map, const K& key) {
        return map.find(key) != map.end();
    }

    static bool containsValue(const std::unordered_map<K, V>& map, const V& value) {
        return std::find_if(map.begin(), map.end(),
            [&value](const auto& pair) { return pair.second == value; }) != map.end();
    }

    static std::vector<K> keys(const std::unordered_map<K, V>& map) {
        std::vector<K> result;
        result.reserve(map.size());
        for (const auto& pair : map) {
            result.push_back(pair.first);
        }
        return result;
    }

    static std::vector<V> values(const std::unordered_map<K, V>& map) {
        std::vector<V> result;
        result.reserve(map.size());
        for (const auto& pair : map) {
            result.push_back(pair.second);
        }
        return result;
    }

    static void merge(std::unordered_map<K, V>& target, const std::unordered_map<K, V>& source, bool overwrite = true) {
        for (const auto& pair : source) {
            if (overwrite || target.find(pair.first) == target.end()) {
                target[pair.first] = pair.second;
            }
        }
    }

    static V getOrDefault(const std::unordered_map<K, V>& map, const K& key, const V& defaultValue) {
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second;
        }
        return defaultValue;
    }

    static V getOrInsert(std::unordered_map<K, V>& map, const K& key, const V& defaultValue) {
        auto it = map.find(key);
        if (it != map.end()) {
            return it->second;
        }
        map[key] = defaultValue;
        return defaultValue;
    }

    static std::vector<std::pair<K, V>> toVector(const std::unordered_map<K, V>& map) {
        return std::vector<std::pair<K, V>>(map.begin(), map.end());
    }

    static size_t countIf(const std::unordered_map<K, V>& map, std::function<bool(const K&, const V&)> pred) {
        size_t count = 0;
        for (const auto& pair : map) {
            if (pred(pair.first, pair.second)) {
                count++;
            }
        }
        return count;
    }

    static std::unordered_map<K, V> filter(const std::unordered_map<K, V>& map, std::function<bool(const K&, const V&)> pred) {
        std::unordered_map<K, V> result;
        for (const auto& pair : map) {
            if (pred(pair.first, pair.second)) {
                result.insert(pair);
            }
        }
        return result;
    }
};

template<typename T>
class SetUtils {
public:
    static bool contains(const std::set<T>& set, const T& value) {
        return set.find(value) != set.end();
    }

    static std::set<T> unionSet(const std::set<T>& a, const std::set<T>& b) {
        std::set<T> result;
        std::set_union(a.begin(), a.end(), b.begin(), b.end(),
            std::inserter(result, result.begin()));
        return result;
    }

    static std::set<T> intersection(const std::set<T>& a, const std::set<T>& b) {
        std::set<T> result;
        std::set_intersection(a.begin(), a.end(), b.begin(), b.end(),
            std::inserter(result, result.begin()));
        return result;
    }

    static std::set<T> difference(const std::set<T>& a, const std::set<T>& b) {
        std::set<T> result;
        std::set_difference(a.begin(), a.end(), b.begin(), b.end(),
            std::inserter(result, result.begin()));
        return result;
    }

    static std::set<T> symmetricDifference(const std::set<T>& a, const std::set<T>& b) {
        std::set<T> result;
        std::set_symmetric_difference(a.begin(), a.end(), b.begin(), b.end(),
            std::inserter(result, result.begin()));
        return result;
    }

    static bool isSubset(const std::set<T>& subset, const std::set<T>& superset) {
        return std::includes(superset.begin(), superset.end(),
            subset.begin(), subset.end());
    }

    static bool isDisjoint(const std::set<T>& a, const std::set<T>& b) {
        return intersection(a, b).empty();
    }

    static std::vector<T> toVector(const std::set<T>& set) {
        return std::vector<T>(set.begin(), set.end());
    }

    static std::set<T> fromVector(const std::vector<T>& vec) {
        return std::set<T>(vec.begin(), vec.end());
    }

    static void insertAll(std::set<T>& set, const std::vector<T>& vec) {
        set.insert(vec.begin(), vec.end());
    }

    static void eraseAll(std::set<T>& set, const std::vector<T>& vec) {
        for (const auto& item : vec) {
            set.erase(item);
        }
    }
};

template<typename T>
class UnorderedSetUtils {
public:
    static bool contains(const std::unordered_set<T>& set, const T& value) {
        return set.find(value) != set.end();
    }

    static std::unordered_set<T> unionSet(const std::unordered_set<T>& a, const std::unordered_set<T>& b) {
        std::unordered_set<T> result = a;
        result.insert(b.begin(), b.end());
        return result;
    }

    static std::unordered_set<T> intersection(const std::unordered_set<T>& a, const std::unordered_set<T>& b) {
        std::unordered_set<T> result;
        for (const auto& item : a) {
            if (b.find(item) != b.end()) {
                result.insert(item);
            }
        }
        return result;
    }

    static std::unordered_set<T> difference(const std::unordered_set<T>& a, const std::unordered_set<T>& b) {
        std::unordered_set<T> result;
        for (const auto& item : a) {
            if (b.find(item) == b.end()) {
                result.insert(item);
            }
        }
        return result;
    }

    static bool isSubset(const std::unordered_set<T>& subset, const std::unordered_set<T>& superset) {
        for (const auto& item : subset) {
            if (superset.find(item) == superset.end()) {
                return false;
            }
        }
        return true;
    }

    static bool isDisjoint(const std::unordered_set<T>& a, const std::unordered_set<T>& b) {
        for (const auto& item : a) {
            if (b.find(item) != b.end()) {
                return false;
            }
        }
        return true;
    }

    static std::vector<T> toVector(const std::unordered_set<T>& set) {
        return std::vector<T>(set.begin(), set.end());
    }

    static std::unordered_set<T> fromVector(const std::vector<T>& vec) {
        return std::unordered_set<T>(vec.begin(), vec.end());
    }

    static void insertAll(std::unordered_set<T>& set, const std::vector<T>& vec) {
        set.insert(vec.begin(), vec.end());
    }

    static void eraseAll(std::unordered_set<T>& set, const std::vector<T>& vec) {
        for (const auto& item : vec) {
            set.erase(item);
        }
    }
};

template<typename T>
class StackUtils {
public:
    static std::vector<T> toVector(std::stack<T> stack) {
        std::vector<T> result;
        while (!stack.empty()) {
            result.push_back(stack.top());
            stack.pop();
        }
        std::reverse(result.begin(), result.end());
        return result;
    }

    static std::stack<T> fromVector(const std::vector<T>& vec) {
        std::stack<T> result;
        for (const auto& item : vec) {
            result.push(item);
        }
        return result;
    }

    static void pushAll(std::stack<T>& stack, const std::vector<T>& vec) {
        for (const auto& item : vec) {
            stack.push(item);
        }
    }

    static std::vector<T> popAll(std::stack<T>& stack) {
        std::vector<T> result;
        while (!stack.empty()) {
            result.push_back(stack.top());
            stack.pop();
        }
        return result;
    }

    static void reverse(std::stack<T>& stack) {
        std::vector<T> vec = toVector(stack);
        std::reverse(vec.begin(), vec.end());
        stack = fromVector(vec);
    }

    static T& getNthFromTop(std::stack<T>& stack, size_t n) {
        std::vector<T> vec = toVector(stack);
        return vec[vec.size() - 1 - n];
    }

    static const T& getNthFromTop(const std::stack<T>& stack, size_t n) {
        std::vector<T> vec = toVector(stack);
        return vec[vec.size() - 1 - n];
    }
};

template<typename T>
class QueueUtils {
public:
    static std::vector<T> toVector(std::queue<T> queue) {
        std::vector<T> result;
        while (!queue.empty()) {
            result.push_back(queue.front());
            queue.pop();
        }
        return result;
    }

    static std::queue<T> fromVector(const std::vector<T>& vec) {
        std::queue<T> result;
        for (const auto& item : vec) {
            result.push(item);
        }
        return result;
    }

    static void pushAll(std::queue<T>& queue, const std::vector<T>& vec) {
        for (const auto& item : vec) {
            queue.push(item);
        }
    }

    static std::vector<T> popAll(std::queue<T>& queue) {
        std::vector<T> result;
        while (!queue.empty()) {
            result.push_back(queue.front());
            queue.pop();
        }
        return result;
    }

    static void reverse(std::queue<T>& queue) {
        std::vector<T> vec = toVector(queue);
        std::reverse(vec.begin(), vec.end());
        queue = fromVector(vec);
    }

    static T& getNth(std::queue<T>& queue, size_t n) {
        std::vector<T> vec = toVector(queue);
        return vec[n];
    }

    static const T& getNth(const std::queue<T>& queue, size_t n) {
        std::vector<T> vec = toVector(queue);
        return vec[n];
    }
};

template<typename T, typename Container = std::vector<T>, typename Compare = std::less<typename Container::value_type>>
class PriorityQueueUtils {
public:
    using PriorityQueueType = std::priority_queue<T, Container, Compare>;

    static std::vector<T> toVector(PriorityQueueType pq) {
        std::vector<T> result;
        while (!pq.empty()) {
            result.push_back(pq.top());
            pq.pop();
        }
        return result;
    }

    static PriorityQueueType fromVector(const std::vector<T>& vec) {
        PriorityQueueType pq;
        for (const auto& item : vec) {
            pq.push(item);
        }
        return pq;
    }

    static void pushAll(PriorityQueueType& pq, const std::vector<T>& vec) {
        for (const auto& item : vec) {
            pq.push(item);
        }
    }

    static std::vector<T> popAll(PriorityQueueType& pq) {
        std::vector<T> result;
        while (!pq.empty()) {
            result.push_back(pq.top());
            pq.pop();
        }
        return result;
    }

    static T& getNthHighest(PriorityQueueType& pq, size_t n) {
        std::vector<T> vec = toVector(pq);
        return vec[n];
    }

    static const T& getNthHighest(const PriorityQueueType& pq, size_t n) {
        std::vector<T> vec = toVector(pq);
        return vec[n];
    }

    static void merge(PriorityQueueType& target, PriorityQueueType& source) {
        while (!source.empty()) {
            target.push(source.top());
            source.pop();
        }
    }
};

}
