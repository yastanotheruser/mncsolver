#include "MclWidget.hpp"
#include <string>
#include <cmath>
#include <algorithm>
#include <functional>
#include <memory>
#include <QtDebug>
#include <QMouseEvent>

void MclWidget::paintNode(const MclNode *node, QPainter &painter, 
                          const QPointF &center, const QString &rtag)
{
    QString text = QString::fromStdString(*node);
    QString vhText = QString::number(node->vh());
    QRectF rect(0, 0, nodeWidth, nodeHeight);
    rect.moveCenter(center);
    painter.drawArc(rect, 0, 16 * 360);
    painter.drawText(rect, Qt::AlignCenter, text);

    QFont pfont(painter.font());
    QFont tagFont;
    tagFont.setPointSize(8);
    painter.setFont(tagFont);
    int vhWidth = painter.fontMetrics().size(0, vhText).width();
    QRectF vhRect = rect.translated(-vhWidth - 5, 0);
    painter.drawText(vhRect, Qt::AlignLeft | Qt::AlignVCenter, vhText);
    
    if (rtag.size() > 0) {
        int rtagWidth = painter.fontMetrics().size(0, rtag).width();
        QRectF rtagRect = rect.translated(rtagWidth + 5, 0);
        painter.drawText(rtagRect, Qt::AlignRight | Qt::AlignVCenter, rtag);
    }

    painter.setFont(pfont);
}

void MclWidget::paintEdge(QPainter &painter, const QPointF &c1,
                          const QPointF &c2, const QString &tag)
{
    using std::sqrt;
    using std::pow;

    double x1 = c1.x();
    double y1 = c1.y();
    double x2 = c2.x();
    double y2 = c2.y();
    double a = nodeWidth / 2;
    double b = nodeHeight / 2;

    double ex1;
    double ey1;
    double ex2;
    double ey2;
    int slopeSgn;

    if (x1 != x2) {
        double m = (y2 - y1) / (x2 - x1);
        int sgn = x1 < x2 ? 1 : -1;
        double k = a * b / sqrt(pow(a, 2) * pow(m, 2) + pow(b, 2));
        ex1 = x1 + sgn * k;
        ey1 = m * (ex1 - x1) + y1;
        ex2 = x2 - sgn * k;
        ey2 = m * (ex2 - x2) + y2;
        slopeSgn = m > 0 ? 1 : -1;
    } else {
        int sgn = y1 < y2 ? 1 : -1;
        ex1 = x1;
        ey1 = y1 + sgn * b;
        ex2 = x2;
        ey2 = y2 - sgn * b;
        slopeSgn = 0;
    }

    QLineF line(ex1, ey1, ex2, ey2);
    painter.drawLine(line);

    if (tag.length() == 0) {
        return;
    }

    QSizeF tsize(painter.fontMetrics().size(0, tag));
    QRectF tagRect(QPointF(), tsize);

    if (slopeSgn < 0) {
        tagRect.moveBottomRight(line.center());
        tagRect.translate(-10.0, 0);
    } else {
        tagRect.moveBottomLeft(line.center());
        tagRect.translate(10.0, 0);
    }

    painter.drawText(tagRect, Qt::AlignCenter, tag);
}

MclWidget::GeometryTraverse::GeometryTraverse(const MclTree &t)
    : tree{t}, depth{0}
{
    QPointF nullPoint;
    pmap[tree.root] = nullPoint;
    QRectF rect(0, 0, nodeWidth, nodeHeight);
    rect.moveCenter(nullPoint);
    leftmost = rect;
    rightmost = rect;
}

void MclWidget::GeometryTraverse::operator()(const MclNode *node)
{
    using namespace std::placeholders;

    MclNode *node_ = const_cast<MclNode*>(node);
    if (node->depth > depth) {
        depth = node->depth;
    }

    if (!tree.treeContains(node) || !(node->ccount > 0)) {
        return;
    }

    MclTree::Nodes children;
    for (const auto &c : node->children) {
        if (tree.treeContains(c.get())) {
            children.push_back(c.get());
        }
    }

    int ccount = children.size();
    if (!(ccount > 0)) {
        return;
    }

    const auto &center = pmap[node_];
    double childrenWidth = nodeWidth * ccount + hMargin * (ccount - 1);
    double crx = center.x() - childrenWidth / 2;
    double cry = center.y() + nodeHeight / 2 + vMargin;
    QRectF rect(crx, cry, childrenWidth, nodeHeight);
    QPointF rcenter = rect.center();
    MclTree::Nodes leftNodes;
    MclTree::Nodes rightNodes;
    double ldelta = 0;
    double rdelta = 0;

    for (const auto &entry : rmap) {
        const auto &n = entry.first;
        if (n->depth != node->depth) {
            continue;
        }

        const auto &r = entry.second;
        auto rc = r.center();
        bool atLeft = rc.x() < rcenter.x();

        if (atLeft) {
            leftNodes.push_back(n);
        } else {
            rightNodes.push_back(n);
        }

        if (!r.intersects(rect)) {
            continue;
        }

        if (atLeft) {
            double delta = r.right() - rect.left() + hMargin;
            if (delta > ldelta) {
                ldelta = delta;
            }
        } else {
            double delta = rect.right() - r.left() + hMargin;
            if (delta > rdelta) {
                rdelta = delta;
            }
        }
    }

    auto cfunc = [this](double delta, const std::unique_ptr<MclNode> &c) {
        if (tree.treeContains(c.get())) {
            pmap.at(c.get()).rx() += delta;
        }
    };

    if (ldelta > 0) {
        auto lcomp = [this](MclNode *n1, MclNode *n2) {
            return rmap.at(n1).x() > rmap.at(n2).x();
        };
        std::sort(leftNodes.begin(), leftNodes.end(), lcomp);

        auto it = leftNodes.cbegin();
        while (true) {
            auto n = *it;
            auto r = rmap.at(n).translated(-ldelta, 0);
            rmap[n] = r;
            const auto &c = n->children;
            std::for_each(c.cbegin(), c.cend(), std::bind(cfunc, -ldelta, _1));

            if (++it == leftNodes.cend()) {
                break;
            }

            const auto &nextr = rmap.at(*it);
            if (nextr.left() < r.left() && !nextr.intersects(r)) {
                break;
            }
        }
    }

    if (rdelta > 0) {
        auto rcomp = [this](MclNode *n1, MclNode *n2) {
            return rmap.at(n1).x() < rmap.at(n2).x();
        };
        std::sort(rightNodes.begin(), rightNodes.end(), rcomp);

        auto it = rightNodes.cbegin();
        while (true) {
            auto n = *it;
            auto r = rmap.at(n).translated(rdelta, 0);
            rmap[n] = r;
            const auto &c = n->children;
            std::for_each(c.cbegin(), c.cend(), std::bind(cfunc, rdelta, _1));

            if (++it == rightNodes.cend()) {
                break;
            }

            const auto &nextr = rmap.at(*it);
            if (nextr.right() > r.right() && !nextr.intersects(r)) {
                break;
            }
        }
    }

    rmap[node_] = rect;
    double cx = rect.x() + nodeWidth / 2;
    double cy = rect.y() + nodeHeight / 2;

    for (const auto &c : children) {
        pmap[c] = QPointF(cx, cy);
        cx += nodeWidth + hMargin;
    }

    const QRectF &lrect = leftNodes.empty() ? rect : rmap.at(leftNodes.back());
    if (lrect.left() < leftmost.left()) {
        leftmost = lrect;
    }

    const QRectF &rrect = rightNodes.empty() ? rect : rmap.at(rightNodes.back());
    if (rrect.right() > rightmost.right()) {
        rightmost = rrect;
    }
}

double MclWidget::GeometryTraverse::width() const
{
    return rightmost.right() - leftmost.left();
}

double MclWidget::GeometryTraverse::height() const
{
    return (depth + 1) * nodeHeight + depth * vMargin;
}

void MclWidget::GeometryTraverse::translate(double dx, double dy)
{
    for (auto &entry : pmap) {
        entry.second.rx() += dx;
        entry.second.ry() += dy;
    }

    for (auto &entry : rmap) {
        entry.second.translate(dx, dy);
    }
}

MclNode *MclWidget::GeometryTraverse::nodeAt(double x, double y)
{
    using std::pow;

    for (const auto &entry : pmap) {
        const QPointF &c = entry.second;
        double v = pow(2 * (x - c.x()) / nodeWidth, 2) +
                   pow(2 * (y - c.y()) / nodeHeight, 2);
        if (v <= 1) {
            return const_cast<MclNode*>(entry.first);
        }
    }

    return nullptr;
}

MclWidget::PaintTraverse::PaintTraverse(QPainter &p, const MclWidget *w)
    : painter{p}, widget{w}, tree{w->tree}, pmap{w->gtraverse->pmap}
{
}

void MclWidget::PaintTraverse::operator()(const MclNode *node)
{
    if (!tree.treeContains(node)) {
        return;
    }

    MclNode *node_ = const_cast<MclNode*>(node);
    const MclNode *hn = widget->hoverNode;
    bool isDummyChildren = false;

    if (hn != nullptr && hn->children.size() > 0) {
        for (const auto &c : hn->children) {
            auto it = tree.uniq.find(c.get());
            if (*it == node && node != c.get()) {
                isDummyChildren = true;
                break;
            }
        }
    }

    const auto &path = widget->path;
    QString color;
    if (node == hn) {
        color = "darkblue";
    } else if (node == tree.current) {
        color = "darkred";
    } else if (isDummyChildren) {
        color = "darkblue";
    } else if (std::find(path.cbegin(), path.cend(), node_) != path.cend()) {
        color = "green";
    }

    QPen ppen = painter.pen();
    if (!color.isEmpty()) {
        QPen pen = ppen;
        pen.setColor(QColor(color));
        painter.setPen(pen);
    }

    const auto &c = node->children;
    QString rtag;
    if (c.size() > 0) {
        auto p = [this](const std::unique_ptr<MclNode> &c) {
            return !tree.treeContains(c.get());
        };
        int dummyChildren = std::count_if(c.cbegin(), c.cend(), p);
        if (dummyChildren > 0) {
            QTextStream(&rtag) << "+" << dummyChildren;
        }
    }

    paintNode(node, painter, pmap.at(node_), rtag);
    if (!color.isEmpty()) {
        painter.setPen(ppen);
    }

    if (node->parent != nullptr) {
        QString tag('A' + node->op);
        paintEdge(painter, pmap.at(node_), pmap.at(node->parent), tag);
    }
}

MclWidget::MclWidget(QWidget *parent) : QWidget(parent)
{
    setMouseTracking(true);
}

MclWidget::~MclWidget()
{
    if (gtraverse != nullptr) {
        delete gtraverse;
    }
}

void MclWidget::nextIteration()
{
    if (tree.next()) {
        updateTree();
    }
}

void MclWidget::previousIteration()
{
    if (tree.previous()) {
        updateTree();
    }
}

void MclWidget::doGeometryTraverse()
{
    if (gtraverse != nullptr) {
        delete gtraverse;
    }

    gtraverse = new GeometryTraverse(tree);
    tree.traverse(*gtraverse);
    updateGeometry_();
}

void MclWidget::updateGeometry_()
{
    double minWidth = 0;
    double minHeight = 0;
    
    if (parentWidget() != nullptr) {
        QSize ps = parentWidget()->size();
        minWidth = ps.width();
        minHeight = ps.height();
    }

    double tw = gtraverse->width();
    double th = gtraverse->height(); double w = std::max(minWidth, tw + 2 * treeHMargin);
    double h = std::max(minHeight, th + 2 * treeVMargin);
    double treeRectLeft = (w - tw) / 2;
    double dx = treeRectLeft - gtraverse->leftmost.left();
    double dy = treeVMargin + nodeHeight / 2;
    gtraverse->translate(dx, dy);
    setFixedSize(w, h);
}

void MclWidget::updateTree()
{
    doGeometryTraverse();
    path = tree.pathBetween(tree.root, tree.current);
    repaint();
    emit treeUpdate(*gtraverse);
}

void MclWidget::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    QPen pen(painter.pen());
    pen.setWidth(2);
    painter.setPen(pen);
    PaintTraverse ptraverse(painter, this);
    tree.traverse(ptraverse);
}

void MclWidget::resizeEvent(QResizeEvent *ev)
{
    updateTree();
}

void MclWidget::mouseMoveEvent(QMouseEvent *ev)
{
    const MclNode *prev = hoverNode;
    hoverNode = gtraverse->nodeAt(ev->x(), ev->y());
    if (hoverNode != prev) {
        repaint();
    }
}
