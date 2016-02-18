#ifndef ag_h
#define ag_h

#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cstddef>

#ifdef _WIN32
namespace _win32
{
#include <windows.h>
}
#undef DELETE
#endif

#ifdef __linux__
namespace _x11
{
#include <X11/X.h>
#include <X11/Xlib.h>
}
#endif

#include <GL/gl.h>

#ifdef __linux__
namespace _x11
{
#include <GL/glx.h>
}
#endif

#include <string>

/* Constants */

#include "keys.hpp"

/* Headers */

#include "pointer.hpp"

#ifdef _WIN32
#include "win32/win32.h"
#else
#include "linux/linux.h"
#endif

/* Sources */

#include "pointer.cpp"

#ifdef _WIN32
#include "win32/win32.c"
#else
#include "linux/linux.c"
#endif

#endif /* ag_h */
