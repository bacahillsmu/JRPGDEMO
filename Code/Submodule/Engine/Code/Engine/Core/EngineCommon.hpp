#pragma once
#pragma warning(error: 4172) // returning address of local variable or temporary
#pragma warning(error: 4715) // not all control paths return a value


#include "Engine/Renderer/RenderContext.hpp"
#include "Engine/Renderer/DebugRender.hpp"
#include "Engine/Renderer/TextureView.hpp"

#include "Engine/Core/Image.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Core/VertexUtils.hpp"

#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/Vec2.hpp"

//#include "Engine/Core/NamedStrings.hpp"

#include <vector>

// ------------------------------------------------------------------------------------------------
#define UNUSED(x) (void)(x);
constexpr bool NATIVE_IS_BIG_ENDIAN = false;
//#define STATIC();

class NamedStrings;
//class NamedProperties;

typedef NamedStrings EventArgs;
//typedef NamedProperties EventArgs;


const extern Vec2 ALIGNED_CENTER;
const extern Vec2 ALIGNED_TOP_LEFT;
const extern Vec2 ALIGNED_TOP_RIGHT;
const extern Vec2 ALIGNED_BOTTOM_LEFT;
const extern Vec2 ALIGNED_BOTTOM_RIGHT;
const extern Vec2 ALIGNED_CENTER_BOTTOM;
const extern Vec2 ALIGNED_CENTER_RIGHT;
const extern Vec2 ALIGNED_CENTER_LEFT;
const extern Vec2 ALIGNED_CENTER_TOP;

extern NamedStrings* g_gameConfigBlackboard;
//extern NamedProperties g_gameConfigBlackboard;

constexpr float SOME_SMALL_NUMBER = 0.000000000000000001f;
constexpr float SOME_LARGE_NUMBER = 100000000000000000.0f;

template<typename T>
void DELETE_POINTER(T*& ptr)
{
	delete ptr;
	ptr = nullptr;
}
