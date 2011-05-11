#ifndef VOX_COMMON_CONFIG_HPP
#define VOX_COMMON_CONFIG_HPP

#if defined( _DEBUG )
#	define VOX_DEBUG
#endif

#if defined( _WIN32 )
#	define VOX_WINDOWS
#endif

#if defined ( _MSC_VER )
#	define VOX_MSVC
#	if _MSC_VER < 1600
#		error "no c++0x support"
#	else
#		define VOX_CPP_0X
#	endif
#endif

#endif //VOX_COMMON_CONFIG_HPP
