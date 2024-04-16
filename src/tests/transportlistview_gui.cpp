// SPDX-FileCopyrightText: 2024 Laurent Montel <montel@kde.org>
// SPDX-License-Identifier: LGPL-2.0-or-later

#include "widgets/transportlistview.h"
#include <QApplication>
#include <QVBoxLayout>
#include <QWidget>

class TransportListView : public QWidget
{
public:
    explicit TransportListView(QWidget *parent = nullptr);
    ~TransportListView() override = default;
};

TransportListView::TransportListView(QWidget *parent)
    : QWidget(parent)
{
    auto mainLayout = new QVBoxLayout(this);
    auto treeview = new MailTransport::TransportListView(this);
    mainLayout->addWidget(treeview);
}

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    auto w = new TransportListView;
    w->show();
    return app.exec();
}
