#include "MclWidget.hpp"
#include <string>
#include <cmath>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <iterator>
#include <QtDebug>

void MclWidget::paintNode(const MclNode *node, QPainter &painter, 
                          const QPointF &center, bool highlight)
{
    static double const nodeWidth = nodeRect.width();
    static double const nodeHeight = nodeRect.height();

    QPen ppen(painter.pen());
    if (highlight) {
        QPen hpen(ppen);
        hpen.setColor(QColor("darkred"));
        painter.setPen(hpen);
    }

    QString text = QString::fromStdString(*node);
    QString vhText = QString::number(node->vh());
    QRectF rect = nodeRect;
    rect.moveCenter(center);
    painter.drawArc(rect, 0, 16 * 360);
    painter.drawText(rect, Qt::AlignCenter, text);

    QFont pfont(painter.font());
    QFont vhFont;
    vhFont.setPointSize(8);
    painter.setFont(vhFont);
    int vhWidth = painter.fontMetrics().size(0, vhText).width();
    QRectF vhRect = rect.translated(-vhWidth - 5, 0);
    painter.drawText(vhRect, Qt::AlignLeft | Qt::AlignVCenter, vhText);
    painter.setFont(pfont);

    if (highlight) {
        painter.setPen(ppen);
    }
}

void MclWidget::paintEdge(QPainter &painter, const QPointF &c1,
                          const QPointF &c2, bool highlight)
{
    using std::sqrt;
    using std::pow;

    double x1 = c1.x();
    double y1 = c1.y();
    double x2 = c2.x();
    double y2 = c2.y();
    double a = nodeRect.width() / 2;
    double b = nodeRect.height() / 2;

    if (x1 == x2) {
        int sgn = y1 < y2 ? 1 : -1;
        painter.drawLine(QPointF(x1, y1 + sgn * b), QPointF(x2, y2 - sgn * b));
        return;
    }

    double m = (y2 - y1) / (x2 - x1);
    int sgn = x1 < x2 ? 1 : -1;
    double k = a * b / sqrt(pow(a, 2) * pow(m, 2) + pow(b, 2));
    double ex1 = x1 + sgn * k;
    double ey1 = m * (ex1 - x1) + y1;
    double ex2 = x2 - sgn * k;
    double ey2 = m * (ex2 - x2) + y2;

    QPen ppen(painter.pen());
    if (highlight) {
        QPen hpen(ppen);
        hpen.setColor(QColor("darkred"));
        painter.setPen(hpen);
    }
    painter.drawLine(QPointF(ex1, ey1), QPointF(ex2, ey2));
    if (highlight) {
        painter.setPen(ppen);
    }
}

void MclWidget::nextIteration()
{
    if (solver.next()) {
        repaint();
    }
}

void MclWidget::previousIteration()
{
    if (solver.previous()) {
        repaint();
    }
}

void MclWidget::paintTree(QPainter &painter) const
{
    static int count = 1;
    static double const nodeWidth = nodeRect.width();
    static double const nodeHeight = nodeRect.height();
    static double const dx = nodeWidth + hMargin;
    static double const dy = nodeHeight + vMargin;

    QSize size = this->size();
    double width = size.width();
    double cy = 10 + nodeHeight / 2;
    MclNode *root = solver.root;
    MclNode *current = solver.current;
    std::vector<MclNode*> nodes;
    std::unordered_map<MclNode*, QPointF> pmap;
    nodes.push_back(root);

    while (!nodes.empty()) {
        int ncount = nodes.size();
        double childrenRectWidth = nodeWidth * ncount + hMargin * (ncount - 1);
        double cx = (width - childrenRectWidth + nodeWidth) / 2;
 
        for (const auto &n : nodes) {
            QPointF center(cx, cy);
            paintNode(n, painter, center, n == current);
            if (n->parent != nullptr) {
                paintEdge(painter, center, pmap[n->parent]);
            }
            pmap[n] = center;
            cx += dx;
        }

        cy += dy;
        decltype(nodes) parents;
        parents.swap(nodes);

        for (const auto &p : parents) {
            if (!(p->ccount > 0)) {
                continue;
            }

            for (const auto &c : p->children) {
                if (solver.treeContains(c.get())) {
                    nodes.push_back(c.get());
                }
            }
        }
    }

    count++;
}

void MclWidget::paintEvent(QPaintEvent *ev)
{
    QPainter painter(this);
    QPen pen(painter.pen());
    pen.setWidth(2);
    painter.setPen(pen);
    paintTree(painter);
}
