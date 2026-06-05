#include "EntryPoint.h"

#include <iostream>

int main(int argc, char** argv)
{
    auto* application = Walrus::CreateApplication({ argc, argv });
    if (!application)
    {
        std::cerr << "Walrus: CreateApplication returned null." << std::endl;
        return 1;
    }

    application->Run();
    delete application;

    return 0;
}
