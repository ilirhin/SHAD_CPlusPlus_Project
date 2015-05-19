#include <QApplication>
#include <iostream>

#include "viewer.h"
#include "client.h"

void runApplication(int argc, char *argv[], Notifier* notifier) {
    QApplication app(argc, argv);
    Field field(notifier);
    app.exec();
}

void runViewer(Notifier* notifier) {
    Viewer viewer(ActionManager(), notifier);
    viewer.run(8800)
}

int main(int argc, char *argv[]) {
    Notifier notifier;
    std::thread viewer_runner(runViewer, &notifier);
    std::thread application_runner(runApplication, argc, argv, &notifier);
    viewer_runner.join();
    application_runner.join();
    
    return 0;
}
