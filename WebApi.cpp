#include "WebApi.h"

WebApi::WebApi() {
    BrainTellHostAddress = "http://114.117.165.134:26000";
}

WebApi::WebApi(const std::string &host) : BrainTellHostAddress(host) {

}

std::tuple<bool,ImageResolutionInfo> WebApi::getImageResolution(const std::string &imageName) {
    httplib::Client client(BrainTellHostAddress);
    std::string api = "/dynamic/image/getimagelist";
    std::string body;

    json jObject;
    json userObject;
    json imageObject;

    userObject["name"] = "zackzhy";
    userObject["passwd"] = "123456";

    imageObject["name"] = imageName;
    imageObject["detail"] = "";

    jObject["user"] = userObject;
    jObject["Image"] = imageObject;

    body = jObject.dump();

    httplib::Result result = client.Post(api, body, "application/json");

    std::string maxResString;
    std::string subMaxResString;

    if (result.error() == httplib::Error::Success && result->status == 200) {
        auto resultBody = result->body;
        json resultJson = json::parse(resultBody);

//            std::cout<< resultJson.dump(4);

        if (resultJson.is_array()) {
            for (auto &obj: resultJson) {
                std::string name;
                std::string detail;

                if (obj.is_object()) {
                    if (obj.contains("name")) {
                        name = obj["name"];
                    }
                }

                if (name != imageName) {
                    continue;
                } else {
                    if (obj.contains("detail")) {
                        detail = obj["detail"];

                        json detailObj = json::parse(detail);

                        if (detailObj.is_array()) {
                            for (auto idx = 0; idx < detailObj.size(); idx++) {
                                if (idx == 0) {
                                    maxResString = detailObj[0];
                                } else if (idx == 1) {
                                    subMaxResString = detailObj[1];
                                } else {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        if (!maxResString.empty() && !subMaxResString.empty()) {
            std::cout << imageName << ":\n";
            std::cout << maxResString << "\n";
            std::cout << subMaxResString << "\n";

            std::regex pattern(R"(\((\d+)x(\d+)x(\d+)\))");

           ImageResolutionInfo info{};

            std::smatch maxResMatches;
            if (std::regex_search(maxResString, maxResMatches, pattern)) {
                if (maxResMatches.size() == 4) {
                    info.maxRes.y = std::stoi(maxResMatches[1]);
                    info.maxRes.x = std::stoi(maxResMatches[2]);
                    info.maxRes.z = std::stoi(maxResMatches[3]);
                    std::cout << "max -> x: " << info.maxRes.x << ", y: " << info.maxRes.y << ", z: " << info.maxRes.z
                              << std::endl;
                } else {
                    std::cout << "Max Resolution extraction error!\n";
                }
            } else {
                std::cout << "Pattern not found!" << std::endl;
                return {false, {}};
            }
            std::smatch subResMatches;
            if (std::regex_search(subMaxResString, subResMatches, pattern)) {
                if (subResMatches.size() == 4) {
                    info.subRes.y = std::stoi(subResMatches[1]);
                    info.subRes.x = std::stoi(subResMatches[2]);
                    info.subRes.z = std::stoi(subResMatches[3]);
                    std::cout << "sub -> x: " << info.subRes.x << ", y: " << info.subRes.y << ", z: " << info.subRes.z
                              << std::endl;
                } else {
                    std::cout << "Sub Resolution extraction error!\n";
                }
            } else {
                std::cout << "Pattern not found!" << std::endl;
                return {false, {}};
            }

            return {true, info};
        }
    }
    return {false, {}};
}

