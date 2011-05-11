#pragma once
#ifndef VOX_SYSTEM_WINDOW_NATIVE_WINDOW_HPP
#define VOX_SYSTEM_WINDOW_NATIVE_WINDOW_HPP

#include <functional>
#include <memory>
#include <boost/noncopyable.hpp>

#include "exception.hpp"
#include "../../util/rect.hpp"

namespace vox {
	namespace system {
		namespace error {
			struct system_error : virtual vox::exception {
                VOX_DEFINE_EXCEPTION_INFO(api_function,      ::std::string);
                VOX_DEFINE_EXCEPTION_INFO(error_code,        long);
                VOX_DEFINE_EXCEPTION_INFO(error_description, ::std::wstring);

                virtual char const* what() const { return "System Error"; }
            };
		} //namespace error

		namespace detail {
			//opaque class for the system specific implementation of NativeWindow
			struct native_window_data;
            struct opengl_context_impl;
		} //namespace detail

        class NativeWindow;

        class OpenGlContext : private boost::noncopyable {
            friend detail::native_window_data;
        public:
            OpenGlContext();
            ~OpenGlContext();

            OpenGlContext(OpenGlContext&& other);
            OpenGlContext& operator=(OpenGlContext&& rhs);
        private:
            std::unique_ptr<detail::opengl_context_impl> impl_;
        };

		////////////////////////////////////////////////////////////////////////
		// Interface for the management of system specific windowing functions
		////////////////////////////////////////////////////////////////////////
		class NativeWindow : private boost::noncopyable {
		public:
			NativeWindow(unsigned width, unsigned height);
			~NativeWindow();

			void resize(unsigned width, unsigned height);
			bool close();

			util::Rectangle<unsigned> clientSize() const;

			//process up to count messages from the system in a non-blocking fashion
			//bool doEvents(unsigned count) const;
			//block until a message from the system is received
			static bool doEventsWait();

			void swap() const;
			OpenGlContext acquireGl();
            void releaseGl();
		public:
            void setOnClose(std::function<bool ()> callback);
            void setOnResize(std::function<bool (unsigned width, unsigned height)> callback);

			//called when the window is closed
			std::function<bool ()> onClose;
			//called when the window is resized
			std::function<bool (unsigned width, unsigned height)> onResize;
			//called when a window gains/loses focus
			std::function<bool (bool active)> onActivate;

			std::function<bool (unsigned dx, unsigned dy)> onMouseMove;
			std::function<bool (unsigned key)> onKeyDown;
		private:
			std::unique_ptr<detail::native_window_data> impl_;
		};
	} //system
} //vox

#endif //VOX_SYSTEM_WINDOW_NATIVE_WINDOW_HPP
