#include <fstream>
#include <iostream>
#include <QCoreApplication>
#include <QDebug>

#include "Operation.h"
#include "neuron_editing/neuron_format_converter.h"

int main(int argc, char* argv[]) {
    std::filesystem::path path = "12939_P065_T01_(1)_S019_1dAFTsur_PL.L_OL.L_R0919_OMZ_20240229_YW.swc";
    std::filesystem::path outPath = "after_12939_P065_T01_(1)_S019_1dAFTsur_PL.L_OL.L_R0919_OMZ_20240229_YW.swc";

    Operation op;
    auto tree = readSWC_file(QString::fromStdString(path.string()));
    op.segments = NeuronTree__2__V_NeuronSWC_list(tree);

    std::ifstream file("12939_P065_T01_(1)_S019_1dAFTsur_PL.L_OL.L_R0919_OMZ_20240229_YW.txt");
    if (!file.is_open()) {
        std::cerr << "File not found";
        return 1;
    }

    std::string line;
    while (std::getline(file, line)) {
        QString msg = QString::fromStdString(line.substr(std::string{"2023/08/14 14:53:10.9571  "}.size()));
        if (msg.startsWith("/drawline_norm:") || msg.startsWith("/drawline_undo:") || msg.
            startsWith("/drawline_redo:")) {
            op.addseg(msg.right(msg.size() - QString("/drawline_norm:").size()));
        }
        else if (msg.startsWith("/delline_norm:") || msg.startsWith("/delline_undo:") || msg.startsWith(
                     "/delline_redo:")) {
            op.delseg(msg.right(msg.size() - QString("/delline_norm:").size()));
        }
        else if (msg.startsWith("/addmarker_norm:") || msg.startsWith("/addmarker_undo:") || msg.startsWith(
                     "/addmarker_redo:")) {
            op.addmarkers(msg.right(msg.size() - QString("/addmarker_norm:").size()));
        }
        else if (msg.startsWith("/delmarker_norm:") || msg.startsWith("/delmarker_undo:") || msg.startsWith(
                     "/delmarker_redo:")) {
            op.delmarkers(msg.right(msg.size() - QString("/delmarker_norm:").size()));
        }
        else if (msg.startsWith("/retypemarker_norm:") || msg.startsWith("/retypemarker_undo:") || msg.startsWith(
                     "/retypemarker_redo:")) {
            op.retypemarker(msg.right(msg.size() - QString("/retypemarker_norm:").size()));
        }
        else if (msg.startsWith("/connectline_norm:") || msg.startsWith("/connectline_undo:") || msg.startsWith(
                     "/connectline_redo:")) {
            op.connectseg(msg.right(msg.size() - QString("/connectline_norm:").size()));
        }
        else if (msg.startsWith("/retypeline_norm:") || msg.startsWith("/retypeline_undo:") || msg.startsWith(
                     "/retypeline_redo:")) {
            op.retypesegment(msg.right(msg.size() - QString("/retypeline_norm:").size()));
        }
        else if (msg.startsWith("/splitline_norm:") || msg.startsWith("/splitline_undo:") || msg.startsWith(
                     "/splitline_redo:")) {
            op.splitseg(msg.right(msg.size() - QString("/splitline_norm:").size()));
        }
        else if (msg.startsWith("/drawmanylines_norm:") || msg.startsWith("/drawmanylines_undo:")) {
            op.addmanysegs(msg.right(msg.size() - QString("/drawmanylines_norm:").size()));
        }

        // QString log;
        // // QString::number按照第二个参数提供的转换进制将数字类型转换为QString
        // log=QDateTime::currentDateTime().toString("yyyy/MM/dd hh:mm:ss.zzz")+QString::number(myServer->processedmsgcnt+myServer->savedmsgcnt)+" "+msg+"\n";
        // logfile->write(log.toStdString().c_str(),log.toStdString().size());
    }

    writeESWC_file(QString::fromStdString(outPath.string()), V_NeuronSWC_list__2__NeuronTree(op.segments));
}
