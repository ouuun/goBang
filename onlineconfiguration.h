#ifndef ONLINECONFIGURATION_H
#define ONLINECONFIGURATION_H

#include "game.h"

#include <QButtonGroup>
#include <QValidator>
#include <QWidget>

namespace Ui {
class OnlineConfiguration;
}

class OnlineConfiguration : public QWidget
{
    Q_OBJECT

public:
    explicit OnlineConfiguration(QWidget *parent = nullptr);
    ~OnlineConfiguration();

public:
    signals:void gameModeSelected(Game::MODE,QString,QString);

private:
    void init();
    bool validateIp(QString ip);
    bool validatePort(QString port);

    Ui::OnlineConfiguration *ui;
    QButtonGroup *buttonGroup;
};


#endif // ONLINECONFIGURATION_H
