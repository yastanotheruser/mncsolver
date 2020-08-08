#include "MclWindow.hpp"
#include <QtDebug>
#include <QHBoxLayout>
#include <QLabel>
#include "LabelRow.hpp"
#include "mcl.hpp"

MclWindow::MclWindow(QWidget *parent) : QWidget(parent)
{
    initWindow();
}

void MclWindow::mclUpdated(const MclWidget::GeometryTraverse &g)
{
    const QPointF &cc = g.pmap.at(g.tree.current);
    mclScroll->ensureVisible(cc.x(), cc.y());
    nextItButton->setDisabled(MclTree::isTarget(g.tree.current));
    prevItButton->setDisabled(g.tree.current == g.tree.root);
}
 
void MclWindow::initWindow()
{
    initLayout();
    addWidgets();
    initSignals();
}

void MclWindow::initLayout()
{
    mainBox = new QHBoxLayout();
    mainBox->setAlignment(Qt::AlignLeft);
    setLayout(mainBox);
}

void MclWindow::addWidgets()
{
    addInfoWidgets();
    QVBoxLayout *rightLayout = new QVBoxLayout();
    QHBoxLayout *hbox = new QHBoxLayout();
    nextItButton = new QPushButton("Siguiente ➡️");
    prevItButton = new QPushButton("⬅️ Anterior");
    nextItButton->setFocusPolicy(Qt::ClickFocus);
    prevItButton->setFocusPolicy(Qt::ClickFocus);
    hbox->addWidget(prevItButton);
    hbox->addWidget(nextItButton);
    rightLayout->addLayout(hbox);

    mclScroll = new QScrollArea();
    mcl = new MclWidget();
    mclScroll->setWidget(mcl);
    rightLayout->addWidget(mclScroll, 1);
    mainBox->addLayout(rightLayout);
}

void MclWindow::addInfoWidgets()
{
    QVBoxLayout *infoLayout = new QVBoxLayout();
    infoLayout->setSpacing(0);

    infoLayout->addWidget(new LabelRow({
        "Espacio de estados:",
        QVariant::fromValue(new QSvgWidget("latex/latex1.svg"))
    }), 0, Qt::AlignHCenter);

    infoLayout->addWidget(new LabelRow({
        "Estado inicial:",
        QVariant::fromValue(new QSvgWidget("latex/latex2.svg"))
    }), 0, Qt::AlignHCenter);

    infoLayout->addWidget(new LabelRow({
        "Estado objetivo:",
        QVariant::fromValue(new QSvgWidget("latex/latex3.svg"))
    }), 0, Qt::AlignHCenter);

    infoLayout->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex4.svg")),
        ": Cantidad de misioneros al lado izquierdo del río",
    }), 0, Qt::AlignHCenter);

    infoLayout->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex5.svg")),
        ": Cantidad de caníbales al lado izquierdo del río",
    }), 0, Qt::AlignHCenter);

    infoLayout->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex6.svg")),
        ": Posición de la lancha",
    }), 0, Qt::AlignHCenter);

    infoLayout->addSpacing(20);
    infoLayout->addWidget(new QLabel("Operadores"), 0, Qt::AlignHCenter);
    infoLayout->addSpacing(10);

    infoLayout->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex7.svg"))
    }), 0, Qt::AlignHCenter);

    infoLayout->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex8.svg"))
    }), 0, Qt::AlignHCenter);

    infoLayout->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex9.svg"))
    }), 0, Qt::AlignHCenter);

    infoLayout->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex10.svg"))
    }), 0, Qt::AlignHCenter);

    infoLayout->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex11.svg"))
    }), 0, Qt::AlignHCenter);

    infoLayout->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex12.svg"))
    }), 0, Qt::AlignHCenter);

    infoLayout->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex13.svg"))
    }), 0, Qt::AlignHCenter);

    infoLayout->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex14.svg"))
    }), 0, Qt::AlignHCenter);

    infoLayout->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex15.svg"))
    }), 0, Qt::AlignHCenter);

    infoLayout->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex16.svg"))
    }), 0, Qt::AlignHCenter);

    infoLayout->addStretch();
    mainBox->addLayout(infoLayout);
}

void MclWindow::initSignals()
{
    connect(nextItButton, SIGNAL(clicked()), mcl,
            SLOT(nextIteration()));
    connect(prevItButton, SIGNAL(clicked()), mcl,
            SLOT(previousIteration()));
    connect(mcl, SIGNAL(treeUpdate(const MclWidget::GeometryTraverse&)),
            this, SLOT(mclUpdated(const MclWidget::GeometryTraverse&)));
}
