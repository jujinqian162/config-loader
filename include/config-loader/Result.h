//
// Created by netcan on 2021/07/05.
//

#ifndef CONFIG_LOADER_RESULT_H
#define CONFIG_LOADER_RESULT_H
#include <config-loader/ConfigLoaderNS.h>

CONFIG_LOADER_NS_BEGIN

enum class Result {
    SUCCESS,
    ERR_EMPTY_CONTENT,
    ERR_LOAD_CONTENT,
};

CONFIG_LOADER_NS_END

#endif //CONFIG_LOADER_RESULT_H
