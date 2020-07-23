#ifndef LABELROW_HPP
#define LABELROW_HPP

#include <QList>
#include <QSharedPointer>
#include <QVariant>
#include <QEvent>
#include <QWidget>
#include <QLabel>
#include <QSvgWidget>

class LabelRow : public QWidget {
public:
    explicit LabelRow(const QList<QVariant> &items, const QFont &font = QFont(),
                      QWidget *parent = nullptr);
protected:
    void changeEvent(QEvent *ev) override;
private:
    static void resizePixmapLabel(QLabel *label, int height);
    static void resizeSvg(QSvgWidget *svg, int height);
    void initLayout();
    QList<QSharedPointer<QWidget>> widgets;
};

#endif
