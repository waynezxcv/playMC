

#include <iostream>
#include "Application.hpp"
#include "MasterRender.hpp"
#include "World.hpp"
#include "Camera.hpp"

int main(int argc, char *argv[]) {
    
    GLL::Application<GLL::World> app;
    app.start();

    return 0;
}

