#include "application.h"

int main(int argc, char *argv[])
{
    if (Application::checkInstance(argc, argv)) {
        return 0;
    }

    Application a(argc, argv);
    return a.exec();
}
