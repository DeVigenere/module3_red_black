#include "gtest/gtest.h"
#include "RedBlackTree.h"
#include <vector>
#include <algorithm>
#include <random>
#include <chrono>
#include <set>

std::vector<int> generateRandomNumbers(int count, int min = 1, int max = 100000) {
    std::vector<int> numbers(count);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min, max);

    for (int i = 0; i < count; ++i) {
        numbers[i] = dis(gen);
    }
    return numbers;
}

TEST(RedBlackTreeTest, Insert1000Elements) {
    RedBlackTree<int> tree;
    std::vector<int> numbers = generateRandomNumbers(1000);
    for (int num : numbers) {
        tree.insert(num);
    }
    int foundCount = 0;
    for (int num : numbers) {
        auto result = tree.find(num);
        EXPECT_TRUE(result.has_value());
        if (result.has_value()) {
            EXPECT_EQ(result.value(), num);
            foundCount++;
        }
    }
    EXPECT_EQ(foundCount, 1000);
}

TEST(RedBlackTreeTest, CheckOrder) {
    RedBlackTree<int> tree;
    std::vector<int> numbers = generateRandomNumbers(1000);
    for (int num : numbers) {
        tree.insert(num);
    }
    std::set<int> sortedNumbers(numbers.begin(), numbers.end());
    for (int num : sortedNumbers) {
        auto result = tree.find(num);
        EXPECT_TRUE(result.has_value());
        if (result.has_value()) {
            EXPECT_EQ(result.value(), num);
        }
    }
    int count = 0;
    for (int num : sortedNumbers) {
        if (tree.find(num).has_value()) {
            count++;
        }
    }
    EXPECT_EQ(count, sortedNumbers.size());
}
