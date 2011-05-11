#pragma once
#ifndef VOX_WINDOW_WINDOWS_NATIVE_WINDOW_IMPL_HPP
#define VOX_WINDOW_WINDOWS_NATIVE_WINDOW_IMPL_HPP

#include "../NativeWindow.hpp"
#include "system.hpp"
#include <Richedit.h>

namespace vox {
	namespace system {
		namespace detail {
			class WindowClass;
			class Window;
			class GlWindow;
			class RichText;

			template <typename handle_t>
			struct deleter_t;
			
            typedef ::std::wstring string_t;

			////////////////////////////////////////////////////////////////////
			// Convienience typedefs and functions for handles
			////////////////////////////////////////////////////////////////////
            template <typename T>
			struct handle {
                //unique_ptr for a handle of type T
				typedef typename ::std::unique_ptr<T,
					deleter_t<T>
				>											unique;
				//shared_ptr for a handle of type T
                typedef typename ::std::shared_ptr<
					typename std::remove_pointer<T>::type
				>											shared;
                //make a shared_ptr from a raw handle
				static shared make_shared(T handle) {
					return shared(handle, deleter_t<T>());
				}
                //make a shared_ptr from unique_ptr
   				static shared make_shared(unique handle) {
					return shared(std::move(handle));
				}
			};

			////////////////////////////////////////////////////////////////////
			// RAII wrapper for window classes
			////////////////////////////////////////////////////////////////////
            class WindowClass : private boost::noncopyable {
            public:
                explicit WindowClass(string_t name, ATOM atom = 0);
                
                WindowClass(WindowClass&& other);
                WindowClass& operator=(WindowClass&& right);

                ~WindowClass();

                LPCWSTR         classId() const;
                string_t const& name()    const { return name_; }
                ATOM            atom()    const { return atom_; }
            private:
                string_t name_;
                ATOM     atom_;
            };

			////////////////////////////////////////////////////////////////////
			// Custom deleters for windows handles
			////////////////////////////////////////////////////////////////////
			void delete_handle(HGLRC handle);
            template <> struct deleter_t<HGLRC> {
				typedef HGLRC pointer;
				void operator()(HGLRC handle) const {
                    delete_handle(handle);
				}
			};

			void delete_handle(HWND handle);
            template <> struct deleter_t<HWND> {
				typedef HWND pointer;
				void operator()(HWND handle) const {
                    delete_handle(handle);
				}
			};

			//return the HINSTANCE for the application
			HINSTANCE getInstance();

			//return the RECT required to contain a client size of (width, height)
			RECT adjustWindowRect(unsigned width, unsigned height, DWORD style, DWORD exStyle = 0, bool menu = false);

			WindowClass           createGlWindowClass(WNDPROC proc);
			handle<HGLRC>::unique createLegacyGlContext(HDC dc);
			handle<HGLRC>::unique createGlContext(HDC dc, int const* params);

			////////////////////////////////////////////////////////////////////
			// Window creation paramaters
			////////////////////////////////////////////////////////////////////
			struct WindowParams {
                explicit WindowParams(WindowClass const& windowClass);

				handle<HWND>::unique createWindow() const;

				WindowParams& clientSize(unsigned width, unsigned height);
				WindowParams& thisPointer(Window* window);
				WindowParams& style(DWORD flags);
				WindowParams& childId(unsigned id);
				WindowParams& parent(Window const& window);

                LPCWSTR classId() const {
                    return windowClassAtom ?
                        MAKEINTATOM(windowClassAtom) :
                        windowClass.c_str();
                }

				DWORD		dwExStyle;
				string_t	windowClass;
                ATOM        windowClassAtom;
				string_t	windowName;
				DWORD		dwStyle;
				int			x;
				int			y;
				int			nWidth;
				int			nHeight;
				HWND		hWndParent;
				HMENU		hMenu;
				HINSTANCE	hInstance;
				LPVOID		lpParam;
			};

			////////////////////////////////////////////////////////////////////
			// Basic native window
			////////////////////////////////////////////////////////////////////
			class Window : private boost::noncopyable {
			public:
				static WindowParams defaultParams();

				explicit Window(WindowParams const& params);
				virtual ~Window() {}

				HDC		deviceContext()	const;
				HWND	window()		const { return window_.get(); }
				LPCWSTR	windowClass()	const { return createParams_.classId(); }

				RECT getClientRect() const;
				void setClientSize(unsigned width, unsigned height);
				void setPixelFormat(PIXELFORMATDESCRIPTOR const& format) const;

				virtual LRESULT windowProc(UINT msg, WPARAM wParam, LPARAM lParam);

				void show() const;
				virtual bool close();

				void resize(unsigned width, unsigned height) const;
				void resize(RECT const& rect) const;

				bool doEvents(unsigned count) const;
				static bool doEventsWait();

                std::unique_ptr<Window> clone() const {
                    return std::unique_ptr<Window>(
                        new Window(createParams_)
                    );
                }
			public:
				std::function<bool ()> onClose;
				std::function<bool (unsigned width, unsigned height)> onResize;
				std::function<bool (bool active)> onActivate;
				std::function<bool (NMHDR const*)> onNotify;
				std::function<bool (unsigned)> onKeyDown;
			protected:
				static LRESULT CALLBACK staticWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
			private:
				WindowParams         createParams_;
				handle<HWND>::unique window_;
			};
		
			////////////////////////////////////////////////////////////////////
			// Native window with support for opengl
			////////////////////////////////////////////////////////////////////
			class GlWindow : public Window {
			public:
				static WindowParams defaultParams();
            public:
				GlWindow(unsigned major, unsigned minor, unsigned width, unsigned height);

				using Window::setPixelFormat;
                void setPixelFormat() const;

				virtual bool close();

				void swap() const;
				
                handle<HGLRC>::unique acquireGl();
                void releaseGl(handle<HGLRC>::unique handle);

                //HGLRC renderingContext() const { return glContext_.get(); }
                HDC   deviceContext() const { return glDc_; }
			private:
				unsigned			major_;
				unsigned			minor_;
				
                //handle<HGLRC>::unique glContext_;
                HDC                   glDc_;
			};

			struct CharFormat {
				CharFormat();
				
				void enable(DWORD mask, bool enable);

				void enableEffect(DWORD effect, bool enable);
				void autoBackcolor(bool enable);
				void setBackcolor(unsigned r, unsigned g, unsigned b);

				CHARFORMAT2W format;
			};

			class RichText : public Window {
			public:
				std::function<bool ()> onMouseEvent;
				std::function<bool ()> onPaint;
				std::function<bool (unsigned, unsigned)> onScroll;

				virtual LRESULT windowProc(UINT msg, WPARAM wParam, LPARAM lParam);

				static LRESULT CALLBACK staticWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

				static WindowParams defaultParams();

				RichText(Window& parent, unsigned childId);

				CHARFORMAT2W getCharFormat(bool default = true);

				void setCharFormat(CHARFORMAT2W const& format, unsigned target) const;

				void setSelection(int begin, int end) const;

				POINT getScrollPos() const;

				void setScrollPos(const POINT& p) const;

				void replaceSelection(std::wstring const& text) const;

				void append(std::wstring const& text) const;

				void applyFormat(CharFormat const& format);
			private:
				WNDPROC defaultProc_;
			};

		} //detail
	} //system
} //vox


#endif //VOX_WINDOW_WINDOWS_NATIVE_WINDOW_IMPL_HPP
