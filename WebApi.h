#pragma once

#include "httplib.h"
#include "json.hpp"
#include <regex>


using namespace nlohmann;

struct Vec3 {
    float x, y, z;
};

struct ImageResolutionInfo {
    Vec3 maxRes{1, 1, 1};
    Vec3 subRes{1, 1, 1};
};

class WebApi {
public:
    WebApi();

    WebApi(const std::string &host);

    std::tuple<bool, ImageResolutionInfo> getImageResolution(const std::string &imageName);

private:
    std::string BrainTellHostAddress;
};

