#include <QApplication>
#include <iostream>
#include <string>
#include <thread>

#include "viewer.h"
#include "client.h"

void runApplication(int argc, char *argv[], Notifier* notifier) {
    QApplication app(argc, argv);
    Field field(notifier);
    app.exec();
}

void runViewer(Notifier* notifier, int port) {
    Viewer viewer(ActionManager(), notifier);
    viewer.run(port);
}

int main(int argc, char *argv[]) {
    assert(argc == 2);
    int port = std::atoi(argv[1]);
    Notifier notifier;
    std::thread viewer_runner(runViewer, &notifier, port);
    std::thread application_runner(runApplication, argc, argv, &notifier);
    viewer_runner.join();
    application_runner.join();
    
    return 0;
}
