#ifndef DEFAULTMENU_H
#define DEFAULTMENU_H

#include <QMenuBar>

class DefaultMenu : public QMenuBar
{
    Q_OBJECT
public:
    explicit DefaultMenu(QWidget *parent = 0);

signals:

public slots:

private slots:
    void ShowAboutQtBox();

private:

};

#endif // DEFAULTMENU_H
