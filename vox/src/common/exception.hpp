#pragma once
#ifndef VOX_CONFIG_EXCEPTION_HPP
#define VOX_CONFIG_EXCEPTION_HPP

#include <exception>
#include <stdexcept>
#include <boost/exception/all.hpp>

#define VOX_DEFINE_EXCEPTION_INFO(name, type) typedef ::boost::error_info<struct tag_##name, type> name

namespace vox {
	struct exception
		: virtual ::std::exception
		, virtual ::boost::exception
	{
        virtual char const* what() const {
            return "vox::exception";
        }
		
        virtual ~exception() throw() {
        }
	};
} //namespace vox

#endif //VOX_CONFIG_EXCEPTION_HPP
