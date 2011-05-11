#pragma once
#ifndef VOX_INCLUDE_SYSTEM_HPP
#define VOX_INCLUDE_SYSTEM_HPP

#include "config.hpp"

#if defined(VOX_WINDOWS)
#	define NTDDI_VERSION	0x06000000
#	define WINVER			0x0600
#	define _WIN32_WINNT		0x0600
#	define _WIN32_WINDOWS	0x0600
#
#	define WIN32_LEAN_AND_MEAN
#	define STRICT
#	define NOMINMAX
#	define NOIME
#	define NOMCX
#	define NOSERVICE
#
#	include <Windows.h>
#endif

#endif //VOX_INCLUDE_SYSTEM_HPP