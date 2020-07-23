#ifndef MCLWIDGET_HPP
#define MCLWIDGET_HPP

#include <QPair>
#include <QWidget>
#include <QPainter>
#include "mcl.hpp"

class MclWidget : public QWidget {
    Q_OBJECT
public slots:
    void nextIteration();
    void previousIteration();
private:
    static constexpr QRectF const nodeRect = QRectF(0, 0, 80.0, 40.0);
    static constexpr double const hMargin = 40.0;
    static constexpr double const vMargin = 20.0;
    static void paintNode(const MclNode *node, QPainter &painter,
                          const QPointF &center, bool highlight = false);
    static void paintEdge(QPainter &painter, const QPointF &c1,
                          const QPointF &c2, bool highlight = false);
    void paintTree(QPainter &painter) const;
    MclSolver solver;
protected:
    void paintEvent(QPaintEvent *ev) override;
};

#endif
