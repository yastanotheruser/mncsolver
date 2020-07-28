#include "mcl.hpp"
#include <sstream>
#include <algorithm>

MclNode::MclNode(int _m, int _c, int _l, MclNode *p, int d, int i)
    : m{_m}, c{_c}, l{_l}, parent{p}, depth{d}, index{i}, ccount{-1}
{
}

MclNode::MclNode(int _m, int _c, int _l)
    : m{_m}, c{_c}, l{_l}, parent{nullptr}, depth{0}, index{-1}, ccount{-1}
{
}

int MclNode::iterate()
{
    if (ccount != -1) {
        return -1;
    }

    ccount = 0;
    if (l == 0) {
        if (m >= 1) {
            addChild(m - 1, c, 1);
        }

        if (m >= 2) {
            addChild(m - 2, c, 1);
        }

        if (c >= 1) {
            addChild(m, c - 1, 1);
        }

        if (c >= 2) {
            addChild(m, c - 2, 1);
        }

        if (m >= 1 && c >= 1) {
            addChild(m - 1, c - 1, 1);
        }
   } else if (l == 1) {
        if (m < 3) {
            addChild(m + 1, c, 0);
        }

        if (m < 2) {
            addChild(m + 2, c, 0);
        }

        if (c < 3) {
            addChild(m, c + 1, 0);
        }

        if (c < 2) {
            addChild(m, c + 2, 0);
        }

        if (m < 3 && c < 3) {
            addChild(m + 1, c + 1, 0);
        }
    }

    return ccount;
}

void MclNode::uniterate()
{
    if (ccount == -1) {
        return;
    }

    children.clear();
    ccount = -1;
}

MclNode::operator std::string() const
{
    std::ostringstream os;
    os << "(" << m << ", " << c << ", " << l << ")";
    return os.str();
};

void MclNode::addChild(int _m, int _c, int _l)
{
    MclNode *child = new MclNode(_m, _c, _l, this, depth + 1, ccount++);
    children.push_back(std::unique_ptr<MclNode>(child));
}

MclTree::MclTree()
{
    root = new MclNode(3, 3, 0);
    current = root;
    uniq.insert(root);
    closed.push_back(root);
}

bool MclTree::isTarget(const MclNode *node)
{
    return node->m == 0 && node->c == 0 && node->l == 1;
}

bool MclTree::next()
{
    if (isTarget(current)) {
        return false;
    }

    current->iterate();
    for (const auto &child : current->children) {
        if (uniq.insert(child.get()).second) {
            open.insert(child.get());
        }
    }

    auto first = open.cbegin();
    current = *first;
    open.erase(first);
    closed.push_back(current);
    return true;
}

bool MclTree::previous()
{
    if (current == root) {
        return false;
    }

    const auto &prev = closed.back();
    closed.pop_back();
    open.insert(prev);
    current = closed.back();

    for (const auto &c : current->children) {
        const auto cptr = c.get();
        if (treeContains(cptr)) {
            uniq.erase(cptr);
            open.erase(cptr);
        }
    }

    current->uniterate();
    return true;
}

bool MclTree::treeContains(const MclNode *node) const
{
    MclNode *n = const_cast<MclNode*>(node);
    auto it = uniq.find(n);
    return it != uniq.end() && *it == n;
}

MclTree::Nodes MclTree::pathBetween(MclNode *a, MclNode *b) const
{
    if (a == b) {
        return {a};
    }

    Nodes bAsc;
    for (MclNode *n = b; n != nullptr; n = n->parent) {
        bAsc.push_front(n);
        if (n == a) {
            return bAsc;
        }
    }

    Nodes path;
    Nodes::const_iterator it;
    for (MclNode *n = a; n != nullptr; n = n->parent) {
        it = std::find(bAsc.cbegin(), bAsc.cend(), n);
        if (it != bAsc.cend()) {
            break;
        }
        path.push_back(n);
    }

    if (it == bAsc.cend()) {
        return Nodes();
    }

    path.insert(path.cend(), it, bAsc.cend());
    return path;
}

void MclTree::traverse(MclTree::SequentialTraverse &func) const
{
    MclTree::Nodes nodes = {root};
    while (!nodes.empty()) {
        decltype(nodes) current;
        current.swap(nodes);

        for (const auto &n : current) {
            const auto &c = n->children;
            std::for_each(c.cbegin(), c.cend(),
                          [&nodes](const auto &n) { nodes.push_back(n.get()); });
            func(n);
        }
    }
}

void MclTree::traverse(MclTree::LevelTraverse &func) const
{
    MclTree::Nodes nodes = {root};
    int depth = 0;

    while (!nodes.empty()) {
        func(nodes, depth++);
        decltype(nodes) parents;
        parents.swap(nodes);

        for (const auto &p : parents) {
            if (!(p->ccount > 0)) {
                continue;
            }

            for (const auto &c : p->children) {
                if (treeContains(c.get())) {
                    nodes.push_back(c.get());
                }
            }
        }
    }
}
