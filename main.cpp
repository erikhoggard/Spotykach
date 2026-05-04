#include <daisy.h>
#include "app.h"

static spotykach::Application app;

extern "C" int main(void)
{
    app.Init();
    app.Loop();
}
