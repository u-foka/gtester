#include "globalprogress.h"

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
