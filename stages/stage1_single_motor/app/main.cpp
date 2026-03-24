#include "cli_app.hpp"

int main(int argc, char** argv) {
    qrc::CliApp app;
    return app.run(argc, argv);
}

