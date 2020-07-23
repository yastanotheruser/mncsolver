#include "LabelRow.hpp"
#include <QtDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QSvgRenderer>

LabelRow::LabelRow(const QList<QVariant> &items, const QFont &font,
                   QWidget *parent)
    : QWidget(parent)
{
    int fontHeight = QFontMetrics(font).height();
    for (auto &item : items) {
        QWidget *widget;
        if (qstrcmp(item.typeName(), "int") == 0) {
            widget = new QLabel(QString(item.toInt(), ' '));
        } else if (item.canConvert<QString>()) {
            widget = new QLabel(item.toString());
        } else if (item.canConvert<QPixmap>()) {
            QLabel *label = new QLabel();
            label->setPixmap(item.value<QPixmap>());
            resizePixmapLabel(label, fontHeight);
            widget = label;
        } else if (item.canConvert<QSvgWidget*>()) {
            QSvgWidget *svg = item.value<QSvgWidget*>();
            resizeSvg(svg, fontHeight);
            widget = svg;
        } else {
            qWarning() << "Skipping LabelRow item, invalid type found\n";
            continue;
        }

        widgets.append(QSharedPointer<QWidget>(widget));
    }

    QWidget::setFont(font);
    initLayout();
}

void LabelRow::changeEvent(QEvent *ev)
{
    if (ev->type() == QEvent::FontChange) {
        int fontHeight = fontMetrics().height();
        for (auto &w : widgets) {
            if (auto label = dynamic_cast<QLabel*>(w.get())) {
                resizePixmapLabel(label, fontHeight);
            } else if (auto svg = dynamic_cast<QSvgWidget*>(w.get())) {
                resizeSvg(svg, fontHeight);
            }
        }
    }
}

void LabelRow::resizePixmapLabel(QLabel *label, int height)
{
    const QPixmap *pixmap = label->pixmap();
    if (pixmap != nullptr) {
        label->setPixmap(pixmap->scaledToHeight(height, Qt::SmoothTransformation));
    }
}

void LabelRow::resizeSvg(QSvgWidget *svg, int height)
{
    QSize size = svg->renderer()->defaultSize();
    size.scale(height, height, Qt::KeepAspectRatioByExpanding);
    svg->setFixedSize(size);
}

void LabelRow::initLayout()
{
    QHBoxLayout *layout = new QHBoxLayout();
    for (const auto &w : widgets) {
        layout->addWidget(w.get());
    }

    layout->addStretch();
    setLayout(layout);
}
