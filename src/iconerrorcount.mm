#include "iconerrorcount.h"

#include <AppKit/NSDockTile.h>
#include <AppKit/NSApplication.h>
#include <Foundation/NSString.h>

void IconErrorCount::SetErrorlabel(const QString &text)
{
#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5
    const char *utf8String = text.toUtf8().constData();
    NSString *cocoaString = [[NSString alloc] initWithUTF8String:utf8String];
    [[NSApp dockTile] setBadgeLabel:cocoaString];
    [cocoaString release];
#else
    Q_UNUSED(text)
#endif
}
