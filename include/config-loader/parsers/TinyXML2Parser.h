//
// Created by netcan on 2021/07/10.
//

#ifndef CONFIG_LOADER_TINYXML2PARSER_H
#define CONFIG_LOADER_TINYXML2PARSER_H
#include <config-loader/ConfigLoaderNS.h>
#include <config-loader/Result.h>
#include <config-loader/parsers/UnsupportedParser.h>
#include <optional>
#include <string>
#include <string_view>

#ifdef HAS_TINYXML2
#include <tinyxml2.h>
#endif

CONFIG_LOADER_NS_BEGIN

#ifdef HAS_TINYXML2
struct TinyXML2Parser {
    Result parse(std::string_view content) {
        return doc.Parse(content.data()) == tinyxml2::XML_SUCCESS
               ? Result::SUCCESS
               : Result::ERR_ILL_FORMED;
    }
    struct ElemType;

    ElemType toRootElemType() {
        return ElemType{doc.FirstChildElement()};
    }

    struct ElemType {
        explicit ElemType(const tinyxml2::XMLElement* elem): elem(elem) {}
        bool isValid() const { return elem != nullptr; }
        ElemType toChildElem(const char* fieldName) const {
            return ElemType{elem->FirstChildElement(fieldName)};
        }
        std::optional<std::string> getValueText() const {
            if (auto valueText = elem->GetText()) {
                return valueText;
            }
            return std::nullopt;
        }
        const char* getKeyName() const {
            if (auto attr = elem->FirstAttribute()) {
                return attr->Value();
            }
            return nullptr;
        }
        template<typename F>
        Result forEachElement(F&& f) const {
            for (auto item = elem->FirstChildElement()
                    ; item
                    ; item = item->NextSiblingElement()) {
                CFL_EXPECT_SUCC(f(ElemType{item}));
            }
            return Result::SUCCESS;
        }
    private:
        const tinyxml2::XMLElement* elem;
    };

private:
    tinyxml2::XMLDocument doc;
};
#else
using TinyXML2Parser = UnsupportedParser;
#endif

CONFIG_LOADER_NS_END

#endif //CONFIG_LOADER_TINYXML2PARSER_H
