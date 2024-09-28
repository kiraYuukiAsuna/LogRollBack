#include <fstream>
#include <iostream>
#include "Util.h"
#include "Operation.h"
#include "neuron_editing/neuron_format_converter.h"
#include "json.hpp"

inline std::vector<std::string> stringSplit(const std::string&str, char delim) {
    std::stringstream ss(str);
    std::string item;
    std::vector<std::string> elems;
    while (std::getline(ss, item, delim)) {
        elems.push_back(item);
    }
    return elems;
}

struct DateTime {
    int year;
    int month;
    int day;
    int hour;
    int minute;
    int second;
    int millisecond;
};

DateTime parseDateTime(const std::string&dateTimeStr) {
    DateTime dt;
    std::istringstream ss(dateTimeStr);
    char delimiter;

    ss >> dt.year >> delimiter >> dt.month >> delimiter >> dt.day;
    ss >> dt.hour >> delimiter >> dt.minute >> delimiter >> dt.second >> delimiter >> dt.millisecond;

    return dt;
}

class LogFileInfo {
public:
    std::string ResultPath;
    std::string ReconstructionPath;
    std::string LogPath;

    friend void to_json(nlohmann::json&json, const LogFileInfo&object) {
        json["ResultPath"] = object.ResultPath;
        json["ReconstructionPath"] = object.ReconstructionPath;
        json["LogPath"] = object.LogPath;
    }

    friend void from_json(const nlohmann::json&json, LogFileInfo&object) {
        LogFileInfo defaultObject;
        object.ResultPath = json.value("ResultPath", defaultObject.ResultPath);
        object.ReconstructionPath = json.value("ReconstructionPath", defaultObject.ReconstructionPath);
        object.LogPath = json.value("LogPath", defaultObject.LogPath);
    };
};

int main(int argc, char* argv[]) {
    std::ifstream configFile(R"(C:\Users\KiraY\Desktop\Data\mouselog\meta.json)");
    nlohmann::json config = nlohmann::json::parse(configFile);
    std::vector<LogFileInfo> logFileInfos = config;

    for (auto&logInfo: logFileInfos) {
        try {
            std::filesystem::path path = logInfo.ReconstructionPath;
            std::filesystem::path outPath = R"(C:\Users\KiraY\Desktop\Data\mouselogrollback)";
            outPath /= std::filesystem::path(logInfo.ResultPath).filename();

            Operation op;
            if(std::filesystem::exists(path)) {
                auto tree = readSWC_file(QString::fromStdString(path.string()));
                op.segments = NeuronTree__2__V_NeuronSWC_list(tree);
            }

            std::ifstream file(logInfo.LogPath);
            if (!file.is_open()) {
                std::cerr << "File not found";
                continue;
            }

            bool skip = false;

            std::string line;
            while (std::getline(file, line)) {
                if(QString::fromStdString(line).contains("...")) {
                    skip = true;
                    break;
                }
                auto timepointStr = line.substr(0, std::string{"2023/08/14 14:53:10.9571"}.size() + 1);
                DateTime dt = parseDateTime(timepointStr);

                std::string stdmsg = line.substr(std::string{"2023/08/14 14:53:10.9571 "}.size() + 1);
                QString msg = QString::fromStdString(stdmsg);

                size_t start = stdmsg.find_first_of(':') + 1;
                size_t end = stdmsg.find_first_of(',', start);
                std::string result = stdmsg.substr(start, end - start);
                auto rs = stringSplit(result, ' ');

                int deviceId = std::stoi(rs[0]);
                int userId = std::stoi(rs[1]);


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

            if(skip) {
                continue;
            }

            writeESWC_file(QString::fromStdString(outPath.string()), V_NeuronSWC_list__2__NeuronTree(op.segments));
        }catch (std::exception&e) {
            std::cerr << e.what();
        }
    }
}
