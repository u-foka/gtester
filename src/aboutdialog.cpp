#include "aboutdialog.h"
#include "ui_aboutdialog.h"

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    _ui(new Ui::AboutDialog)
{
    _ui->setupUi(this);
    setFixedSize(geometry().width(), geometry().height());
}

AboutDialog::~AboutDialog()
{
    delete _ui;
}
