#include "setfocus.h"

#include <QWidget>

void SetFocus::To(QWidget *widget)
{
    widget->activateWindow();
    widget->raise();
}
