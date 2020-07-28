#ifndef TREE_HPP
#define TREE_HPP

#include <memory>
#include <string>
#include <vector>
#include <deque>
#include <functional>
#include <set>
#include <unordered_set>

class MclNode {
public:
    explicit MclNode(int _m, int _c, int _l, MclNode *p, int d, int i);
    explicit MclNode(int _m, int _c, int _l);
    operator std::string() const;
    constexpr int vh() const { return 6 - 2 * m - 2 * c - 1000 * (m != c); }
    int iterate();
    void uniterate();

    int m;
    int c;
    int l;
    MclNode *parent;
    int depth;
    int index;
    int ccount;
    std::vector<std::unique_ptr<MclNode>> children;
private:
    void addChild(int _m, int _c, int _l);
};

class MclTree {
private:
    static struct {
        std::size_t operator()(const MclNode *node) const
        {
            return (node->m << 3) | (node->c << 1) | node->l;
        }
    } nodeHash;

    static struct {
        bool operator()(const MclNode *n1, const MclNode *n2) const
        {
            return n1->m == n2->m && n1->c == n2->c && n1->l == n2->l;
        }
    } nodeEqual;

    static struct {
        bool operator()(const MclNode *n1, const MclNode *n2) const
        {
            int vh1 = n1->vh();
            int vh2 = n2->vh();
            if (vh1 != vh2) {
                return vh1 > vh2;
            }

            int d1 = n1->depth;
            int d2 = n2->depth;
            if (d1 != d2) {
                return d1 < d2;
            }

            return n1->index < n2->index;
        }
    } openCompare;

public:
    template<typename... Ts>
    struct Traverse {
        virtual ~Traverse() { }
        virtual void operator()(Ts... args) = 0;
    };

    using Nodes = std::deque<MclNode*>;
    using SequentialTraverse = Traverse<const MclNode*>;
    using LevelTraverse = Traverse<const Nodes&, int>;

    static bool isTarget(const MclNode *node);
    explicit MclTree();
    bool next();
    bool previous();
    bool treeContains(const MclNode *node) const;
    Nodes pathBetween(MclNode *a, MclNode *b) const;
    void traverse(SequentialTraverse &func) const;
    void traverse(LevelTraverse &func) const;

    MclNode *root;
    MclNode *current;
    std::unordered_set<MclNode*, decltype(nodeHash), decltype(nodeEqual)> uniq;
    std::set<MclNode*, decltype(openCompare)> open;
    Nodes closed;
};

#endif
