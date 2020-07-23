#ifndef MCLWINDOW_HPP
#define MCLWINDOW_HPP

#include <QSharedPointer>
#include <QVBoxLayout>
#include <QWidget>
#include <QSvgWidget>
#include <QPushButton>
#include "MclWidget.hpp"

Q_DECLARE_METATYPE(QSvgWidget*);

class MclWindow : public QWidget {
    Q_OBJECT
public:
    explicit MclWindow(QWidget *parent = nullptr);
private:
    QSharedPointer<QVBoxLayout> mainBox;
    QSharedPointer<MclWidget> mcl;
    QSharedPointer<QPushButton> nextItButton;
    QSharedPointer<QPushButton> prevItButton;
    void initWindow();
    void initLayout();
    void addWidgets();
    void addInfoWidgets();
    void addButtons();
    void initSignals();
};

#endif
