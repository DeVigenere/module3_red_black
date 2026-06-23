#pragma once
#include <memory>
#include <optional>
#include <iostream>

template <typename T>
class RedBlackTree {
    enum class Color { RED, BLACK };
    struct Node {
        T data;
        Color color;
        std::unique_ptr<Node> left;
        std::unique_ptr<Node> right;
        Node* parent;
        Node(const T& value) : data(value), color(Color::RED), left(nullptr), right(nullptr), parent(nullptr) {};
    };
    std::unique_ptr<Node> root;

    void leftRotate(Node* x) {
        auto y = x->right.get();
        x->right = std::move(y->left);
        if (x->right) {
            x->right->parent = x;
        }
        y->parent = x->parent;
        auto parent = x->parent;
        if (parent == nullptr) {
            root.reset(y);
        }
        else if (x == parent->left.get()) {
            parent->left.reset(y);
        }
        else {
            parent->right.reset(y);
        }
        y->left.reset(x);
        x->parent = y;
    }

    void rightRotate(Node* y) {
        auto x = y->left.get();
        y->left = std::move(x->right);
        if (y->left) {
            y->left->parent = y;
        }
        x->parent = y->parent;
        auto parent = y->parent;
        if (parent == nullptr) {
            root.reset(x);
        }
        else if (y == parent->left.get()) {
            parent->left.reset(x);
        }
        else {
            parent->right.reset(x);
        }
        x->right.reset(y);
        y->parent = x;
    }

    void insertFixup(Node* z) {
        while (z != root.get() && z->parent->color == Color::RED) {
            auto parent = z->parent;
            auto grandparent = parent->parent;
            if (parent == grandparent->left.get()) {
                auto uncle = grandparent->right.get();
                if (uncle && uncle->color == Color::RED) {
                    parent->color = Color::BLACK;
                    uncle->color = Color::BLACK;
                    grandparent->color = Color::RED;
                    z = grandparent;
                }
                else {
                    if (z == parent->right.get()) {
                        z = parent;
                        leftRotate(z);
                        parent = z->parent;
                    }
                    parent->color = Color::BLACK;
                    grandparent->color = Color::RED;
                    rightRotate(grandparent);
                    break;
                }
            }
            else {
                auto uncle = grandparent->left.get();
                if (uncle && uncle->color == Color::RED) {
                    parent->color = Color::BLACK;
                    uncle->color = Color::BLACK;
                    grandparent->color = Color::RED;
                    z = grandparent;
                }
                else {
                    if (z == parent->left.get()) {
                        z = parent;
                        rightRotate(z);
                        parent = z->parent;
                    }
                    parent->color = Color::BLACK;
                    grandparent->color = Color::RED;
                    leftRotate(grandparent);
                    break;
                }
            }
        }
        root->color = Color::BLACK;
    }

    void deleteFixup(Node* x, Node* parent) {
        while (x != root.get() && (x == nullptr || x->color == Color::BLACK)) {
            if (x == parent->left.get() || (x == nullptr && parent->left == nullptr)) {
                auto w = parent->right.get();
                if (w->color == Color::RED) {
                    w->color = Color::BLACK;
                    parent->color = Color::RED;
                    leftRotate(parent);
                    w = parent->right.get();
                }
                if ((w->left == nullptr || w->left->color == Color::BLACK) &&
                    (w->right == nullptr || w->right->color == Color::BLACK)) {
                    w->color = Color::RED;
                    x = parent;
                    parent = x->parent;
                }
                else {
                    if (w->right == nullptr || w->right->color == Color::BLACK) {
                        if (w->left) w->left->color = Color::BLACK;
                        w->color = Color::RED;
                        rightRotate(w);
                        w = parent->right.get();
                    }
                    w->color = parent->color;
                    parent->color = Color::BLACK;
                    if (w->right) w->right->color = Color::BLACK;
                    leftRotate(parent);
                    x = root.get();
                    parent = nullptr;
                }
            }
            else {
                auto w = parent->left.get();
                if (w->color == Color::RED) {
                    w->color = Color::BLACK;
                    parent->color = Color::RED;
                    rightRotate(parent);
                    w = parent->left.get();
                }
                if ((w->right == nullptr || w->right->color == Color::BLACK) &&
                    (w->left == nullptr || w->left->color == Color::BLACK)) {
                    w->color = Color::RED;
                    x = parent;
                    parent = x->parent;
                }
                else {
                    if (w->left == nullptr || w->left->color == Color::BLACK) {
                        if (w->right) w->right->color = Color::BLACK;
                        w->color = Color::RED;
                        leftRotate(w);
                        w = parent->left.get();
                    }
                    w->color = parent->color;
                    parent->color = Color::BLACK;
                    if (w->left) w->left->color = Color::BLACK;
                    rightRotate(parent);
                    x = root.get();
                    parent = nullptr;
                }
            }
        }
        if (x) x->color = Color::BLACK;
    }

    Node* findNode(const T& value) const {
        auto current = root.get();
        while (current) {
            if (value < current->data) {
                current = current->left.get();
            }
            else if (value > current->data) {
                current = current->right.get();
            }
            else {
                return current;
            }
        }
        return nullptr;
    }

    Node* minimum(Node* node) const {
        while (node->left) {
            node = node->left.get();
        }
        return node;
    }

    void transplant(Node* u, std::unique_ptr<Node>&& v) {
        auto parent = u->parent;
        if (parent == nullptr) {
            root = std::move(v);
            if (root) root->parent = nullptr;
        }
        else if (u == parent->left.get()) {
            parent->left = std::move(v);
            if (parent->left) parent->left->parent = parent;
        }
        else {
            parent->right = std::move(v);
            if (parent->right) parent->right->parent = parent;
        }
    }

    std::unique_ptr<Node> extractNode(Node* node) {
        auto parent = node->parent;
        std::unique_ptr<Node> result;
        if (parent == nullptr) {
            result = std::move(root);
        }
        else if (node == parent->left.get()) {
            result = std::move(parent->left);
        }
        else {
            result = std::move(parent->right);
        }
        return result;
    }

    void inorderTraversal(Node* node, std::ostream& os) const {
        if (node) {
            inorderTraversal(node->left.get(), os);
            os << node->data << " ";
            inorderTraversal(node->right.get(), os);
        }
    }

    void setReplacementChild(Node* z, std::unique_ptr<Node>&& child, Node*& x, Node*& xParent) {
        x = child.get();
        xParent = z->parent;
        auto zPtr = extractNode(z);
        transplant(z, std::move(child));
        if (x) x->parent = xParent;
    }

public:
    RedBlackTree() : root(nullptr) {}

    void insert(const T& value) {
        auto z = std::make_unique<Node>(value);
        Node* zPtr = z.get();
        Node* y = nullptr;
        auto x = root.get();

        while (x) {
            y = x;
            if (zPtr->data < x->data) {
                x = x->left.get();
            }
            else {
                x = x->right.get();
            }
        }

        zPtr->parent = y;
        if (y == nullptr) {
            root = std::move(z);
        }
        else if (zPtr->data < y->data) {
            y->left = std::move(z);
            y->left->parent = y;
        }
        else {
            y->right = std::move(z);
            y->right->parent = y;
        }

        insertFixup(zPtr);
    }

    void remove(const T& value) {
        auto z = findNode(value);
        if (!z) return;

        Node* y = z;
        Color yOriginalColor = y->color;
        Node* x = nullptr;
        Node* xParent = nullptr;

        if (z->left == nullptr) {
            setReplacementChild(z, std::move(z->right), x, xParent);
        }
        else if (z->right == nullptr) {
            setReplacementChild(z, std::move(z->left), x, xParent);
        }
        else {
            y = minimum(z->right.get());
            yOriginalColor = y->color;
            x = y->right.get();

            if (y->parent == z) {
                xParent = y;
                if (x) x->parent = y;
            }
            else {
                xParent = y->parent;
                auto yPtr = extractNode(y);
                transplant(y, std::move(yPtr->right));
                y->right = std::move(z->right);
                if (y->right) y->right->parent = y;
            }

            auto zPtr = extractNode(z);
            transplant(z, std::move(zPtr));
            y->left = std::move(z->left);
            if (y->left) y->left->parent = y;
            y->color = z->color;
        }

        if (yOriginalColor == Color::BLACK) {
            deleteFixup(x ? x : root.get(), xParent ? xParent : root.get());
        }
    }

    void print() const {
        inorderTraversal(root.get(), std::cout);
        std::cout << std::endl;
    }

    std::optional<T> find(const T& value) const {
        auto node = findNode(value);
        if (node) {
            return node->data;
        }
        return std::nullopt;
    }
};