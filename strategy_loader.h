#include "strategy.h"
#include <memory>

#pragma once

std::shared_ptr<GlobalStrategy> createGlobalStrategy(const std::vector<std::string>& args) {
    if (args[0] == "") {
        return nullptr;
    } else if (args[0] == "") {
        return nullptr;
    } else {
        return nullptr;
    }
}