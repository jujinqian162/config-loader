//
// Created by netcan on 2021/07/09.
//

#ifdef HAS_JSONCPP
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_all.hpp>
#include <json/json.h>
#include "DeserializeConfig.h"
#include <config-loader/ConfigLoader.h>

using CONFIG_LOADER_NS::getFileContent;
using namespace Catch;
using namespace Catch::Matchers;

static void checkPoint(const Json::Value& root, double x, double y) {
    REQUIRE(root.isObject());
    REQUIRE(root["x"] == x);
    REQUIRE(root["y"] == y);
}

using namespace json_config;
SCENARIO("load a json config") {
    Json::CharReaderBuilder builder;
    std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    GIVEN("a point config") {
        Json::Value root;
        auto pointCfg = getFileContent(POINT_CONFIG_PATH);
        REQUIRE(reader->parse(pointCfg.data(), pointCfg.data() + pointCfg.size(),
                              &root , nullptr));
        checkPoint(root, 1.2, 3.4);
    }

    GIVEN("a rect config") {
        Json::Value root;
        auto rectCfg = getFileContent(RECT_CONFIG_PATH);
        REQUIRE(reader->parse(rectCfg.data(), rectCfg.data() + rectCfg.size(),
                              &root , nullptr));

        checkPoint(root["p1"], 1.2, 3.4);
        checkPoint(root["p2"], 5.6, 7.8);
        REQUIRE(root["color"] == 12345678);
    }

    GIVEN("a someOfPoints config") {
        Json::Value root;
        auto someOfPointsCfg = getFileContent(SOME_OF_POINTS_CONFIG_PATH);
        REQUIRE(reader->parse(someOfPointsCfg.data(), someOfPointsCfg.data() + someOfPointsCfg.size(),
                              &root , nullptr));
        REQUIRE_THAT(root["name"].asString(), Equals("Some of points"));

        double pointsV[] = {
                1.2, 3.4,
                5.6, 7.8,
                2.2, 3.3
        };

        size_t pointIdx = 0;
        for (auto&& point = root["point"].begin()
                ; point != root["point"].end()
                ; ++point) {
            checkPoint(*point, pointsV[pointIdx], pointsV[pointIdx + 1]);
            pointIdx += 2;
        }
    }
}

SCENARIO("test Json::Value as string") {
    GIVEN("a int") {
        Json::Value v = 2;
        REQUIRE_THAT(v.asString(), Equals("2"));
    }
    GIVEN("a double") {
        Json::Value v = 1.234;
        REQUIRE_THAT(v.asString(), Equals("1.234"));
    }
    GIVEN("a string") {
        Json::Value v = "hello world";
        REQUIRE_THAT(v.asString(), Equals("hello world"));
    }
    GIVEN("a nullptr") {
        Json::Value v = Json::nullValue;
        REQUIRE_THAT(v.asString(), Equals(""));
    }

}

#endif
