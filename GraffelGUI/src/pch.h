#pragma once
#define NOMINMAX
#include <windows.h>
#include <GL/gl.h>
#pragma warning (push)
#pragma warning(disable: 26812 26439 26451 26495 6011 6385 4244 6386 6297 4267)

#include "SDL.h"
#include "include/gpu/GrBackendSurface.h"
#include "include/gpu/gl/GrGLInterface.h"
#include "include/gpu/GrContext.h"
#include "src/gpu/GrCaps.h"
#include "src/gpu/GrContextPriv.h"
#include "src/gpu/gl/GrGLDefines.h"
#include "src/gpu/gl/GrGLUtil.h"
#include "include/core/SkCanvas.h"
#include "include/core/SkFont.h"
#include "include/core/SkSurface.h"
#include "include/core/SkGraphics.h"
#include "include/core/SkRefCnt.h"
#include "include/utils/SkRandom.h"
#include "src/core/SkMathPriv.h"
#include "src/core/SkAutoMalloc.h"
#include "src/core/SkUtils.h"
#include "src/image/SkImage_Base.h"
#include "include/effects/SkGradientShader.h"
#include "tools/sk_app/GLWindowContext.h"


#pragma warning(pop)

#include <iostream>
#include <string>
#include <chrono>
#include <list>

