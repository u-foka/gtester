#include "globalprogress.h"

#if MAC_OS_X_VERSION_MAX_ALLOWED >= MAC_OS_X_VERSION_10_5

#import <AppKit/NSDockTile.h>
#import <AppKit/NSApplication.h>
#import <AppKit/NSImageView.h>
#import <AppKit/NSCIImageRep.h>
#import <AppKit/NSBezierPath.h>
#import <AppKit/NSColor.h>
#import <Foundation/NSString.h>

@interface ApplicationProgressView : NSView {
    int min;
    int max;
    int value;
}

+ (ApplicationProgressView *)sharedProgressView;

- (void)setRangeMin:(int)v1 max:(int)v2;
- (void)setValue:(int)v;
- (void)updateBadge;

@end

static ApplicationProgressView *sharedProgressView = nil;

@implementation ApplicationProgressView

+ (ApplicationProgressView *)sharedProgressView
{
    if (sharedProgressView == nil) {
        sharedProgressView = [[ApplicationProgressView alloc] init];
    }
    return sharedProgressView;
}

- (void)setRangeMin:(int)v1 max:(int)v2
{
    min = v1;
    max = v2;
    [self updateBadge];
}

- (void)setValue:(int)v
{
    value = v;
    [self updateBadge];
}

- (void)updateBadge
{
    [[NSApp dockTile] display];
}

- (void)drawRect:(NSRect)rect
{
    Q_UNUSED(rect)
    NSRect boundary = [self bounds];
    [[NSApp applicationIconImage] drawInRect:boundary
                                     fromRect:NSZeroRect
                                    operation:NSCompositeCopy
                                     fraction:1.0];
    NSRect progressBoundary = boundary;
    progressBoundary.size.height *= 0.13;
    progressBoundary.size.width *= 0.8;
    progressBoundary.origin.x = (NSWidth(boundary) - NSWidth(progressBoundary))/2.;
    progressBoundary.origin.y = NSHeight(boundary)*0.13;

    double range = max - min;
    double percent = 0.50;
    if (range != 0)
        percent = (value - min) / range;
    if (percent > 1)
        percent = 1;
    else if (percent < 0)
        percent = 0;

    NSRect currentProgress = progressBoundary;
    currentProgress.size.width *= percent;
    [[NSColor blackColor] setFill];
    [NSBezierPath fillRect:progressBoundary];
    [[NSColor lightGrayColor] setFill];
    [NSBezierPath fillRect:currentProgress];
    [[NSColor blackColor] setStroke];
    [NSBezierPath strokeRect:progressBoundary];
}

@end

void GlobalProgress::ErrorLabel(const QString &errors)
{
    const char *utf8String = errors.toUtf8().constData();
    NSString *cocoaString = [[NSString alloc] initWithUTF8String:utf8String];
    [[NSApp dockTile] setBadgeLabel:cocoaString];
    [cocoaString release];
}

void GlobalProgress::Progress(int progress, int max)
{
    [[ApplicationProgressView sharedProgressView] setRangeMin:0 max:max];
    [[ApplicationProgressView sharedProgressView] setValue:progress];
}

void GlobalProgress::ShowProgress(bool show)
{
    if (show) {
        [[NSApp dockTile] setContentView:[ApplicationProgressView sharedProgressView]];
    } else {
        [[NSApp dockTile] setContentView:nil];
    }
    [[NSApp dockTile] display];
}

#else

void GlobalProgress::ErrorLabel(const QString &errors)
{
    Q_UNUSED(errors);
}

void GlobalProgress::Progress(int progress, int max)
{
    Q_UNUSED(progress);
    Q_UNUSED(max);
}

void GlobalProgress::ShowProgress(bool show)
{
    Q_UNUSED(show);
}

#endif
