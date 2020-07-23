#include "mcl.hpp"
#include <sstream>

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

MclSolver::MclSolver()
{
    root = new MclNode(3, 3, 0);
    current = nullptr;
    uniq.insert(root);
    open.insert(root);
}

bool MclSolver::isTarget(const MclNode *node)
{
    return node->m == 0 && node->c == 0 && node->l == 1;
}

bool MclSolver::next()
{
    if (current != nullptr && isTarget(current)) {
        return false;
    }

    auto first = open.cbegin();
    current = *first;
    open.erase(first);
    path.push_back(current);

    if (isTarget(current)) {
        return true;
    }

    current->iterate();
    for (const auto &child : current->children) {
        if (uniq.insert(child.get()).second) {
            open.insert(child.get());
        }
    }

    return true;
}

bool MclSolver::previous()
{
    if (path.empty()) {
        return false;
    }

    const auto &prev = path.back();
    for (const auto &c : prev->children) {
        const auto cptr = c.get();
        if (treeContains(cptr)) {
            uniq.erase(cptr);
            open.erase(cptr);
        }
    }

    prev->uniterate();
    path.pop_back();
    open.insert(prev);
    current = !path.empty() ? path.back() : nullptr;
    return true;
}

bool MclSolver::treeContains(const MclNode *node) const
{
    MclNode *n = const_cast<MclNode*>(node);
    auto it = uniq.find(n);
    return it != uniq.end() && *it == n;
}
