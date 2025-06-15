//
//  styleManager.cpp
//  EVA_01
//
//  Created by Jan Everaert on 15/06/2025.
//

#include "styleManager.hpp"

StyleManager& StyleManager::getInstance() {
    static StyleManager instance;
    return instance;
}
