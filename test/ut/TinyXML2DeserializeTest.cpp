//
// Created by netcan on 2021/07/06.
//

#include <catch2/catch.hpp>
#include "UTSchema.h"
#include "DeserializeConfig.h"
#include <config-loader/ConfigLoader.h>

using namespace Catch;
using namespace CONFIG_LOADER_NS;
using namespace xml_config;

SCENARIO("deserialize xml to struct") {
    GIVEN("a flatten point config") {
        Point point;
        REQUIRE(loadXML2Obj(point, POINT_CONFIG_PATH) == Result::SUCCESS);
        REQUIRE(point.x == 1.2);
        REQUIRE(point.y == 3.4);
    }

    GIVEN("a nest rect config") {
        Rect rect;
        REQUIRE(loadXML2Obj(rect, RECT_CONFIG_PATH) == Result::SUCCESS);
        REQUIRE(rect.p1.x == 1.2);
        REQUIRE(rect.p1.y == 3.4);
        REQUIRE(rect.p2.x == 5.6);
        REQUIRE(rect.p2.y == 7.8);
        REQUIRE(rect.color == 0x12345678);
    }

    GIVEN("a nest rect config that missing p1/p2") {
        Rect rect;
        auto res = loadXML2Obj(rect, [] {
            return R"(
                <?xml version="1.0" encoding="UTF-8"?>
                <rect>
                    <color>0x12345678</color>
                </rect>
            )";
        });
        REQUIRE(res == Result::ERR_MISSING_FIELD);
    }

    GIVEN("a complex rect config") {
        SomeOfPoints someOfPoints;
        REQUIRE(loadXML2Obj(someOfPoints, SOME_OF_POINTS_CONFIG_PATH) == Result::SUCCESS);
        REQUIRE_THAT(someOfPoints.name,
                     Equals("Some of points"));
        REQUIRE(someOfPoints.points.size() == 3);
        double pointsV[] = {
                1.2, 3.4,
                5.6, 7.8,
                2.2, 3.3
        };
        for (size_t i = 0; i < someOfPoints.points.size(); ++i) {
            REQUIRE(someOfPoints.points[i].x == pointsV[i * 2]);
            REQUIRE(someOfPoints.points[i].y == pointsV[i * 2 + 1]);
        }
    }
}

///////////////////////////////////////////////////////////////////////////////
SCENARIO("deserialize xml to compound STL container") {
    GIVEN("a valid STL obj") {
        STLObj data;
        REQUIRE(loadXML2Obj(data, STLOBJ_CONFIG_PATH) == Result::SUCCESS);
        REQUIRE(data.m1[0] == 2);
        REQUIRE(data.m1[1] == 4);
        REQUIRE(data.m1[2] == 6);
        REQUIRE(data.m1.size() == 3);

        REQUIRE(data.m2["hello world"].x == 1.2);
        REQUIRE(data.m2["hello world"].y == 3.4);
        REQUIRE(data.m2.size() == 1);

        REQUIRE(data.m3.empty());

        REQUIRE(data.m4.has_value());
        REQUIRE(data.m4->x == 5.6);
        REQUIRE(data.m4->y == 7.8);

        REQUIRE(! data.m5.has_value());
        REQUIRE(data.m6.empty());

    }
}

DEFINE_SCHEMA(TestBool,
              (bool) m1);

SCENARIO("deserialize xml to bool type") {
    TestBool obj;
    GIVEN("a valid bool") {
        auto res = loadXML2Obj(obj, [] {
            return "<TestBool><m1>true</m1></TestBool>";
        });
        REQUIRE(res == Result::SUCCESS);
        REQUIRE(obj.m1);
    }
    GIVEN("a valid bool") {
        auto res = loadXML2Obj(obj, [] {
            return "<TestBool><m1>True</m1></TestBool>";
        });
        REQUIRE(res == Result::SUCCESS);
        REQUIRE(obj.m1);
    }
    GIVEN("a valid bool") {
        auto res = loadXML2Obj(obj, [] {
            return "<TestBool><m1>1</m1></TestBool>";
        });
        REQUIRE(res == Result::SUCCESS);
        REQUIRE(obj.m1);
    }
    GIVEN("a valid bool") {
        auto res = loadXML2Obj(obj, [] {
            return "<TestBool><m1>false</m1></TestBool>";
        });
        REQUIRE(res == Result::SUCCESS);
        REQUIRE(! obj.m1);
    }
    GIVEN("a invalid bool") {
        auto res = loadXML2Obj(obj, [] {
            return "<TestBool><m1>unknown</m1></TestBool>";
        });
        REQUIRE(res == Result::ERR_EXTRACTING_FIELD);
    }
}

DEFINE_SCHEMA(TestInt8T,
              (uint8_t) m1,
              (int8_t) m2);
SCENARIO("deserialize xml to int8_t type") {
    TestInt8T obj;
    GIVEN("a valid uint8_t") {
        auto res = loadXML2Obj(obj, [] {
            return "<TestBool><m1>48</m1><m2>0</m2></TestBool>";
        });
        REQUIRE(res == Result::SUCCESS);
        REQUIRE(obj.m1 == 48);
        REQUIRE(obj.m2 == 0); // isn't '0' (aka 48)
    }
}

DEFINE_SCHEMA(TestInt,
              (int) number);

SCENARIO("deserialize xml to a number") {
    TestInt obj;
    GIVEN("a HEX number") {
        auto res = loadXML2Obj(obj, [] {
            return "<TestInt><number>0X12</number></TestInt>";
        });
        REQUIRE(res == Result::SUCCESS);
        REQUIRE(obj.number == 0x12);
    }
    GIVEN("a hex number") {
        auto res = loadXML2Obj(obj, [] {
            return "<TestInt><number>0x12</number></TestInt>";
        });
        REQUIRE(res == Result::SUCCESS);
        REQUIRE(obj.number == 0x12);
    }

    GIVEN("a hex number") {
        auto res = loadXML2Obj(obj, [] {
            return "<TestInt><number>0x</number></TestInt>";
        });
        REQUIRE(res == Result::ERR_EXTRACTING_FIELD);
    }

}

SCENARIO("deserialize xml to sum type(std::variant)") {
    TestVariant obj;
    GIVEN("a string") {
        auto res = loadXML2Obj(obj, [] {
            return R"(
                <TestVariant>
                    <sumType>hello world!</sumType>
                </TestVariant>
            )";
        });
        REQUIRE(res == Result::SUCCESS);
        REQUIRE(obj.sumType.index() == 2);
        REQUIRE(! obj.sumType.valueless_by_exception());
        REQUIRE_THAT(std::get<2>(obj.sumType),
                     Equals("hello world!"));
    }

    GIVEN("a int") {
        auto res = loadXML2Obj(obj, [] {
            return R"(
                 <TestVariant>
                     <sumType>987654</sumType>
                 </TestVariant>
                )";
        });
        REQUIRE(res == Result::SUCCESS);
        REQUIRE(obj.sumType.index() == 1);
        REQUIRE(! obj.sumType.valueless_by_exception());
        REQUIRE(std::get<1>(obj.sumType) == 987654);
    }

    GIVEN("a point") {
        auto res = loadXML2Obj(obj, [] {
            return R"(
                 <TestVariant>
                     <sumType>
                        <x>1.2</x><y>3.4</y>
                    </sumType>
                 </TestVariant>
            )";
        });
        REQUIRE(res == Result::SUCCESS);
        REQUIRE(obj.sumType.index() == 0);
        REQUIRE(! obj.sumType.valueless_by_exception());
        auto&& [x, y] = std::get<0>(obj.sumType);
        REQUIRE(x == 1.2);
        REQUIRE(y == 3.4);
    }

    GIVEN("a missing field") {
        auto res = loadXML2Obj(obj, [] {
            return R"(
                 <TestVariant>
                 </TestVariant>
            )";
        });
        REQUIRE(res == Result::ERR_MISSING_FIELD);
    }

    GIVEN("a invalid object") {
        auto res = loadXML2Obj(obj, [] {
            return R"(
                 <TestVariant>
                     <sumType>
                        <x>1.2</x>
                    </sumType>
                 </TestVariant>
            )";
        });
        REQUIRE(res == Result::ERR_TYPE);
    }

    GIVEN("a empty object") {
        auto res = loadXML2Obj(obj, [] {
            return R"(
                 <TestVariant>
                     <sumType>
                    </sumType>
                 </TestVariant>
            )";
        });
        REQUIRE(res == Result::ERR_TYPE);
    }
}

SCENARIO("deserialize xml to tree type") {
    TestTree obj;
    GIVEN("a tree") {
        REQUIRE(loadXML2Obj(obj, TREE_CONFIG_PATH) == Result::SUCCESS);
        REQUIRE_THAT(obj.name, Equals("hello"));
        REQUIRE(obj.children.size() == 3);
        REQUIRE_THAT(obj.children[0]->name, Equals("world"));
        REQUIRE_THAT(obj.children[1]->name, Equals("first"));
        REQUIRE_THAT(obj.children[2]->name, Equals("second"));
        REQUIRE(obj.children[2]->children.size() == 1);
        REQUIRE_THAT(obj.children[2]->children[0]->name, Equals("leaf"));
    }

}
