#include "onlineconfiguration.h"
#include "ui_onlineconfiguration.h"

#include <qmessagebox.h>

OnlineConfiguration::OnlineConfiguration(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::OnlineConfiguration)
{
    ui->setupUi(this);
    init();
}

void OnlineConfiguration::init(){

    //设置 当选中 以主机开始游戏时 禁止输入ip
    connect(ui->isHost,&QRadioButton::clicked,ui->formLayout,[&]{
        ui->ip->setEnabled(false);
    });

    //设置 当选中 以从机开始游戏时 允许输入ip
    connect(ui->isSlave,&QRadioButton::clicked,ui->formLayout,[&]{
        ui->ip->setEnabled(true);
    });

    //点击确定按钮时对填入参数进行校验
    connect(ui->ok,&QPushButton::clicked,this,[&]{
        QString ip = ui->ip->text();
        QString port = ui->port->text();

        bool isHost = ui->isHost->isChecked();

        //当选中 主机选项  不校验ip
        if(!isHost && !validateIp(ip)) {
            QMessageBox::information(this, "提示", "ip输入错误！");
            return;
        }
        if(!validatePort(port)){
            QMessageBox::information(this, "提示", "port输入错误！");
            return;
        }
        //发出信号参数分别为 选中的模式，ip，port
        if (isHost) emit gameModeSelected(Game::Host,0,port);
        else emit gameModeSelected(Game::Slave,ip,port);
    });

    //设置 按钮组 实现按钮互斥
    buttonGroup = new QButtonGroup;
    buttonGroup->addButton(ui->isHost);
    buttonGroup->addButton(ui->isSlave);


    ui->isHost->click();

}

bool OnlineConfiguration::validateIp(QString ip)
{
    QRegularExpression regex("^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$");
    QRegularExpressionMatch match = regex.match(ip);
    return match.hasMatch();
}

bool OnlineConfiguration::validatePort(QString input)
{
    bool ok;
    int port = input.toInt(&ok);

    if (ok && port >= 0 && port <= 65535) return true;
    return false;
}

OnlineConfiguration::~OnlineConfiguration()
{
    delete ui;
    delete buttonGroup;
}
