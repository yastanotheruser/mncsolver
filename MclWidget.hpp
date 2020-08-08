#ifndef MCLWIDGET_HPP

#define MCLWIDGET_HPP

#include <unordered_map>
#include <QWidget>
#include <QPainter>
#include "mcl.hpp"

class MclWidget : public QWidget {
    Q_OBJECT
public:
    struct GeometryTraverse : MclTree::SequentialTraverse {
        explicit GeometryTraverse(const MclTree &t);
        ~GeometryTraverse() { }
        void operator()(const MclNode *node) override;
        double width() const;
        double height() const;
        void translate(double dx, double dy);
        MclNode *nodeAt(double x, double y);

        const MclTree &tree;
        using PMap = std::unordered_map<MclNode*, QPointF>;
        using RMap = std::unordered_map<MclNode*, QRectF>;
        PMap pmap;
        RMap rmap;
        int depth;
        QRectF leftmost;
        QRectF rightmost;
    };

    explicit MclWidget(QWidget *parent = nullptr);
    ~MclWidget();
signals:
    void treeUpdate(const MclWidget::GeometryTraverse &g);
public slots:
    void nextIteration();
    void previousIteration();
private:
    static constexpr double const nodeWidth = 80.0;
    static constexpr double const nodeHeight = 40.0;
    static constexpr double const hMargin = 60.0;
    static constexpr double const vMargin = 40.0;
    static constexpr double const treeHMargin = 50;
    static constexpr double const treeVMargin = 50;

    static void paintNode(const MclNode *node, QPainter &painter,
                          const QPointF &center, const QString &rtag = QString());
    static void paintEdge(QPainter &painter, const QPointF &c1,
                          const QPointF &c2, const QString &tag = QString());

    struct PaintTraverse : MclTree::SequentialTraverse {
        explicit PaintTraverse(QPainter &p, const MclWidget *w);
        void operator()(const MclNode *node) override;
        QPainter &painter;
        const MclWidget *widget;
        const MclTree &tree;
        const GeometryTraverse::PMap &pmap;
    };

    void doGeometryTraverse();
    void updateGeometry_();
    void updateTree();
    GeometryTraverse *gtraverse = nullptr;
    MclTree tree;
    MclTree::Nodes path;
    const MclNode *hoverNode = nullptr;
protected:
    void paintEvent(QPaintEvent *ev) override;
    void resizeEvent(QResizeEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
};

#endif
