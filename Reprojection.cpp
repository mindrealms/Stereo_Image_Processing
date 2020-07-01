#include "Reprojection.h"
#include <QStringList>
#include <QProcess>

Reprojection::Reprojection() {

    QString path = "/Users/marky/Desktop/BVC/dmr_project/depth_map_reproj";
    QString  command("python");
    QStringList params = QStringList() << "reprojection_script.py";

    QProcess *process = new QProcess();
    process->startDetached(command, params, path);
    process->waitForFinished();
    process->close();
}
