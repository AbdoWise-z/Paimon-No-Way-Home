//
// Created by xAbdoMo on 4/20/2024.
//

#ifndef GFX_LAB_GLOBALS_H
#define GFX_LAB_GLOBALS_H

#include "imgui.h"

#ifndef EXTERN
#define EXTERN extern
#endif

namespace our {
    EXTERN bool SUPPRESS_SHADER_ERRORS;
    EXTERN ImFont* AppFont;
    EXTERN std::string level_path;
}

#endif //GFX_LAB_GLOBALS_H
