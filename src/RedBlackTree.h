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
        std::shared_ptr<Node> left;
        std::shared_ptr<Node> right;
        std::weak_ptr<Node> parent;
        Node(const T& value) : data(value), color(Color::RED), left(nullptr), right(nullptr) {};
    };
    std::shared_ptr<Node> root;
    std::shared_ptr<Node> nil;

    void initialization_nil() {
        nil = std::make_shared<Node>(T());
        nil->color = Color::BLACK;
        nil->left = nullptr;
        nil->right = nullptr;
    }

    void leftRotate(std::shared_ptr<Node> x) {
        auto y = x->right;
        x->right = y->left;
        if (y->left != nil) {
            y->left->parent = x;
        }
        y->parent = x->parent;
        auto parent = x->parent.lock();
        if (parent == nullptr) {
            root = y;
        }
        else if (x == parent->left) {
            parent->left = y;
        }
        else {
            parent->right = y;
        }
        y->left = x;
        x->parent = y;
    }

    void rightRotate(std::shared_ptr<Node> y) {
        auto x = y->left;
        y->left = x->right;
        if (x->right != nil) {
            x->right->parent = y;
        }
        x->parent = y->parent;
        auto parent = y->parent.lock();
        if (parent == nullptr) {
            root = x;
        }
        else if (y == parent->left) {
            parent->left = x;
        }
        else {
            parent->right = x;
        }
        x->right = y;
        y->parent = x;
    }

    void insertFixup(std::shared_ptr<Node> z) {
        auto parent = z->parent.lock();
        while (parent && parent->color == Color::RED) {
            auto grandparent = parent->parent.lock();
            if (parent == grandparent->left) {
                auto uncle = grandparent->right;
                if (uncle->color == Color::RED) {
                    parent->color = Color::BLACK;
                    uncle->color = Color::BLACK;
                    grandparent->color = Color::RED;
                    z = grandparent;
                    parent = z->parent.lock();
                }
                else {
                    if (z == parent->right) {
                        z = parent;
                        leftRotate(z);
                        parent = z->parent.lock();
                        grandparent = parent->parent.lock();
                    }
                    parent->color = Color::BLACK;
                    grandparent->color = Color::RED;
                    rightRotate(grandparent);
                    break;
                }
            }
            else {
                auto uncle = grandparent->left;
                if (uncle->color == Color::RED) {
                    parent->color = Color::BLACK;
                    uncle->color = Color::BLACK;
                    grandparent->color = Color::RED;
                    z = grandparent;
                    parent = z->parent.lock();
                }
                else {
                    if (z == parent->left) {
                        z = parent;
                        rightRotate(z);
                        parent = z->parent.lock();
                        grandparent = parent->parent.lock();
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

    void deleteFixup(std::shared_ptr<Node> x) {
        while (x != root && x->color == Color::BLACK) {
            auto parent = x->parent.lock();
            if (x == parent->left) {
                auto w = parent->right;
                if (w->color == Color::RED) {
                    w->color = Color::BLACK;
                    parent->color = Color::RED;
                    leftRotate(parent);
                    w = parent->right;
                }
                if (w->left->color == Color::BLACK && w->right->color == Color::BLACK) {
                    w->color = Color::RED;
                    x = parent;
                }
                else {
                    if (w->right->color == Color::BLACK) {
                        w->left->color = Color::BLACK;
                        w->color = Color::RED;
                        rightRotate(w);
                        w = parent->right;
                    }
                    w->color = parent->color;
                    parent->color = Color::BLACK;
                    w->right->color = Color::BLACK;
                    leftRotate(parent);
                    x = root;
                    break;
                }
            }
            else {
                auto w = parent->left;
                if (w->color == Color::RED) {
                    w->color = Color::BLACK;
                    parent->color = Color::RED;
                    rightRotate(parent);
                    w = parent->left;
                }
                if (w->right->color == Color::BLACK && w->left->color == Color::BLACK) {
                    w->color = Color::RED;
                    x = parent;
                }
                else {
                    if (w->left->color == Color::BLACK) {
                        w->right->color = Color::BLACK;
                        w->color = Color::RED;
                        leftRotate(w);
                        w = parent->left;
                    }
                    w->color = parent->color;
                    parent->color = Color::BLACK;
                    w->left->color = Color::BLACK;
                    rightRotate(parent);
                    x = root;
                    break;
                }
            }
        }
        x->color = Color::BLACK;
    }

    std::shared_ptr<Node> findNode(const T& value) const {
        auto current = root;
        while (current != nil && current != nullptr) {
            if (value < current->data) {
                current = current->left;
            }
            else if (value > current->data) {
                current = current->right;
            }
            else {
                return current;
            }
        }
        return nullptr;
    }

    std::shared_ptr<Node> minimum(std::shared_ptr<Node> node) const {
        while (node->left != nil) {
            node = node->left;
        }
        return node;
    }

    void transplant(std::shared_ptr<Node> u, std::shared_ptr<Node> v) {
        auto parent = u->parent.lock();
        if (parent == nullptr) {
            root = v;
        }
        else if (u == parent->left) {
            parent->left = v;
        }
        else {
            parent->right = v;
        }
        v->parent = parent;
    }

    void inorderTraversal(std::shared_ptr<Node> node, std::ostream& os) const {
        if (node != nil && node != nullptr) {
            inorderTraversal(node->left, os);
            os << node->data << " ";
            inorderTraversal(node->right, os);
        }
    }

public:
    RedBlackTree() {
        initialization_nil();
        root = nil;
    }

    void remove(const T& value) {
        auto z = findNode(value);
        if (!z || z == nil) return;
        auto y = z;
        auto yOriginalColor = y->color;
        std::shared_ptr<Node> x;
        if (z->left == nil) {
            x = z->right;
            transplant(z, z->right);
        }
        else if (z->right == nil) {
            x = z->left;
            transplant(z, z->left);
        }
        else {
            y = minimum(z->right);
            yOriginalColor = y->color;
            x = y->right;
            auto parent = y->parent.lock();
            if (parent == z) {
                x->parent = y;
            }
            else {
                transplant(y, y->right);
                y->right = z->right;
                y->right->parent = y;
            }
            transplant(z, y);
            y->left = z->left;
            y->left->parent = y;
            y->color = z->color;
        }
        if (yOriginalColor == Color::BLACK) {
            deleteFixup(x);
        }
    }

    void insert(const T& value) {
        auto z = std::make_shared<Node>(value);
        z->left = nil;
        z->right = nil;

        std::shared_ptr<Node> y = nullptr;
        auto x = root;

        while (x != nil && x != nullptr) {
            y = x;
            if (z->data < x->data) {
                x = x->left;
            }
            else {
                x = x->right;
            }
        }

        z->parent = y;
        if (y == nullptr) {
            root = z;
        }
        else if (z->data < y->data) {
            y->left = z;
        }
        else {
            y->right = z;
        }

        insertFixup(z);
    }

    void print() const {
        inorderTraversal(root, std::cout);
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