#include "setfocus.h"

#import <Cocoa/Cocoa.h>
#include <QWidget>

void SetFocus::To(QWidget *widget) {
  [NSApp activateIgnoringOtherApps:YES];
  widget->activateWindow();
  widget->raise();
}
