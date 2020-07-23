#include "MclWindow.hpp"
#include <QtDebug>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include "LabelRow.hpp"

MclWindow::MclWindow(QWidget *parent) : QWidget(parent)
{
    initWindow();
}

void MclWindow::initWindow()
{
    initLayout();
    addWidgets();
    initSignals();
}

void MclWindow::initLayout()
{
    mainBox = QSharedPointer<QVBoxLayout>(new QVBoxLayout());
    mainBox->setAlignment(Qt::AlignTop);
    setLayout(mainBox.get());
}

void MclWindow::addWidgets()
{
    addInfoWidgets();
    addButtons();
    mcl = QSharedPointer<MclWidget>(new MclWidget());
    QHBoxLayout *layout = new QHBoxLayout();
    layout->addWidget(mcl.get(), 1);
    QScrollArea *scrollArea = new QScrollArea();
    scrollArea->setLayout(layout);
    mainBox->addWidget(scrollArea, 1);
}

void MclWindow::addInfoWidgets()
{
    mainBox->addWidget(new LabelRow({
        "Espacio de estados:",
        QVariant::fromValue(new QSvgWidget("latex/latex1.svg")),
        4,
        "Estado inicial:",
        QVariant::fromValue(new QSvgWidget("latex/latex2.svg")),
        4,
        "Estado objetivo:",
        QVariant::fromValue(new QSvgWidget("latex/latex3.svg"))
    }), 0, Qt::AlignHCenter);

    mainBox->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex4.svg")),
        ": Cantidad de misioneros al lado izquierdo del río",
    }), 0, Qt::AlignHCenter);

    mainBox->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex5.svg")),
        ": Cantidad de caníbales al lado izquierdo del río",
    }), 0, Qt::AlignHCenter);

    mainBox->addWidget(new LabelRow({
        QVariant::fromValue(new QSvgWidget("latex/latex6.svg")),
        ": Posición de la lancha",
    }), 0, Qt::AlignHCenter);
}

void MclWindow::addButtons()
{
    QHBoxLayout *hbox = new QHBoxLayout();
    nextItButton = QSharedPointer<QPushButton>(new QPushButton("Siguiente ➡️"));
    prevItButton = QSharedPointer<QPushButton>(new QPushButton("⬅️ Anterior"));
    nextItButton->setFocusPolicy(Qt::ClickFocus);
    prevItButton->setFocusPolicy(Qt::ClickFocus);
    hbox->addWidget(prevItButton.get());
    hbox->addWidget(nextItButton.get());
    mainBox->addLayout(hbox);
}

void MclWindow::initSignals()
{
    connect(nextItButton.get(), SIGNAL(clicked()), mcl.get(),
            SLOT(nextIteration()));
    connect(prevItButton.get(), SIGNAL(clicked()), mcl.get(),
            SLOT(previousIteration()));
}
