#pragma once
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#pragma warning (push)
#pragma warning(disable: 26812 26439)

#pragma warning(push, 0 )
#pragma warning(disable: 6385)
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/GrContext.h"
#include "SDL.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkSurface.h"
#include "include/utils/SkRandom.h"

#include "include/gpu/gl/GrGLInterface.h"
#include "src/gpu/gl/GrGLUtil.h"

#pragma warning(pop)

#include <iostream>
#include <string>
#include <chrono>
#include <list>

