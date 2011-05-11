#include "common.hpp"
#include "config.hpp"

#if defined(VOX_WINDOWS)

#include "NativeWindowImpl.hpp"
#include "../NativeWindow.hpp"

namespace vsys		= ::vox::system;
namespace detail	= ::vox::system::detail;

#define LOG_PRINT(x, y)

#define LOG_TRACE(fmt, params) ::std::wcout << (::boost::wformat(fmt) % params) << ::std::endl

#define CHECK_API_FAILURE(condition, api)				 \
if (!(condition)) {										 \
	::SetLastError(0);									 \
} else {												 \
	BOOST_THROW_EXCEPTION(throw_windows_exception(api)); \
}														 \

vsys::error::system_error
throw_windows_exception(char const* api)
{
    long const errorCode = ::GetLastError();

    LPWSTR msgPtr = nullptr;
    auto const result = FormatMessageW(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM     |
        FORMAT_MESSAGE_IGNORE_INSERTS, //__in      DWORD dwFlags,
        nullptr,                       //__in_opt  LPCVOID lpSource,
        errorCode,                     //__in      DWORD dwMessageId,
        0,                             //__in      DWORD dwLanguageId,
        (LPWSTR)&msgPtr,               //__out     LPTSTR lpBuffer,
        0,                             //__in      DWORD nSize,
        nullptr                        //__in_opt  va_list *Arguments
    );
    assert(result != 0);

    struct deleter_t {
        typedef HLOCAL pointer;
        void operator()(pointer p) const { ::LocalFree(p); }
    };

    std::unique_ptr<HLOCAL, deleter_t> errorDescription(
        reinterpret_cast<HLOCAL>(msgPtr)
    );

    std::wstring const errorString(
        reinterpret_cast<LPWSTR>(errorDescription.get()),
        result
    );

    vsys::error::system_error exception;

    exception << vsys::error::system_error::error_code(errorCode)
              << vsys::error::system_error::error_description(errorString)
              << vsys::error::system_error::api_function(api)
              ;

    return exception;
}

detail::WindowClass::WindowClass(string_t name, ATOM atom)
    : name_(std::move(name))
    , atom_(atom)
{
    LOG_TRACE(L"WindowClass(\"%1%\", %2%)", name_ % (void*)atom_);
}
                
detail::WindowClass::WindowClass(WindowClass&& other)
    : name_(std::move(other.name_))
    , atom_(other.atom_)
{
    LOG_TRACE(L"WindowClass(\"%1%\", %2%) [move constructed]", name_ % (void*)atom_);
    other.atom_ = 0;
}

detail::WindowClass&
detail::WindowClass::operator=(WindowClass&& right)
{
    LOG_TRACE( L"WindowClass(\"%1%\", %2%) = WindowClass(\"%1%\", %2%)",
               name_ % (void*)atom_ % right.name_ % (void*)right.atom_ );

    name_.swap(std::move(right.name_));
    std::swap(atom_, right.atom_);

    return *this;
}

detail::WindowClass::~WindowClass()
{
    LOG_TRACE(L"~WindowClass(\"%1%\", %2%)", name_ % (void*)atom_);

    if (atom_) {
		auto const module = ::GetModuleHandleW(nullptr);
		assert(module != nullptr);

		auto const result = ::UnregisterClassW(MAKEINTATOM(atom_), module);
        assert(result != FALSE);

        LOG_TRACE(L"~WindowClass(\"%1%\", %2%) unregistered", name_ % (void*)atom_);
    }
}

LPCWSTR
detail::WindowClass::classId() const
{
    return atom_ ? MAKEINTATOM(atom_) : name_.c_str();
}

void
detail::delete_handle(HGLRC handle)
{
    LOG_TRACE(L"Deleter[HGLRC](handle=%1%)", handle);

    if (handle == ::wglGetCurrentContext()) {
        LOG_TRACE(L"Deleter[HGLRC](handle=%1%) making not current", handle);
        ::wglMakeCurrent(::wglGetCurrentDC(), 0);
    }

    auto const result = ::wglDeleteContext(handle);
    assert(result != FALSE); //TODO
}

void
detail::delete_handle(HWND handle)
{
    LOG_TRACE(L"Deleter[HWND](handle=%1%)", handle);

    auto const result = ::DestroyWindow(handle);
    //assert(result != FALSE); //TODO
}

detail::WindowClass
detail::createGlWindowClass(WNDPROC proc)
{
	auto const module			= getInstance();
	auto const icon				= LoadIconW(nullptr, IDI_APPLICATION);
	auto const cursor			= LoadCursorW(nullptr, IDC_ARROW);
	wchar_t const className[]	= L"VoxGlWindow";

	WNDCLASSEX const wndClass = {
		sizeof(WNDCLASSEX),	//UINT      cbSize
		CS_OWNDC,			//UINT      style;
		proc,				//WNDPROC   lpfnWndProc;
		0,					//int       cbClsExtra;
		0,					//int       cbWndExtra;
		module,				//HINSTANCE hInstance;
		icon,				//HICON     hIcon;
		cursor,				//HCURSOR   hCursor;
		nullptr,			//HBRUSH    hbrBackground;
		nullptr,			//LPCTSTR   lpszMenuName;
		className,			//LPCTSTR   lpszClassName;
		nullptr				//HICON     hIconSm;
	};

    ATOM const result = ::RegisterClassExW(&wndClass);
    if (result == 0) {
        auto const errcode = ::GetLastError();
        if (errcode == ERROR_CLASS_ALREADY_EXISTS) {
            LOG_TRACE(L"[WARNING] Window class \"%1%\" already exists", className);
        }

        CHECK_API_FAILURE(true, "RegisterClassExW");
    } else {
        LOG_TRACE(L"Registered Window class name=\"%1%\" atom=%2%", className % (void*)result);
    }

	return WindowClass(className, result);
}

detail::handle<HGLRC>::unique
detail::createLegacyGlContext(HDC dc)
{
	handle<HGLRC>::unique result(::wglCreateContext(dc));
	CHECK_API_FAILURE(result == 0, "wglCreateContext");

    LOG_TRACE(L"Created legacy opengl context=%1% for dc=%2%", result.get() % dc);

	return result;
}

detail::handle<HGLRC>::unique
detail::createGlContext(HDC dc, int const* params)
{
	assert(::wglCreateContextAttribsARB);

	handle<HGLRC>::unique result(
		::wglCreateContextAttribsARB(dc, nullptr, params)
	);
	CHECK_API_FAILURE(result == nullptr, "wglCreateContextAttribsARB");

    LOG_TRACE(L"Created modern opengl context=%1% for dc=%2%", result.get() % dc);

	return result;
}

namespace {
	template <typename T>
	T* setWindowLong(HWND window, int index, T* data) {
		::SetLastError(0);
		
		auto result = SetWindowLongPtrW(
			window, index, reinterpret_cast<LONG>(data)
		);
		
		assert(result != 0 || ::GetLastError() == 0);

		return reinterpret_cast<T*>(result);
	}
}

detail::WindowParams::WindowParams(WindowClass const& windowClass)
	: dwExStyle(0)
	, windowClass(windowClass.name())
    , windowClassAtom(windowClass.atom())
	, windowName()
	, dwStyle(0)
	, x(CW_USEDEFAULT), y(CW_USEDEFAULT)
	, nWidth(CW_USEDEFAULT), nHeight(CW_USEDEFAULT)
	, hWndParent(nullptr)
	, hMenu(nullptr)
	, hInstance(getInstance())
	, lpParam(nullptr)
{
}

detail::handle<HWND>::unique
detail::WindowParams::createWindow() const
{
	handle<HWND>::unique result(
        CreateWindowExW(
		    dwExStyle,
            classId(),
            windowName.c_str(),
            dwStyle,
		    x, y,
            nWidth, nHeight,
		    hWndParent,
            hMenu,
            hInstance,
            lpParam
	    )
    );
	CHECK_API_FAILURE(result == nullptr, "CreateWindowExW");

    LOG_TRACE( L"Created window=%1% with class=%2%; name=\"%3%\"",
               result.get() % (void*)classId() % windowName );

	return result;
}

detail::WindowParams&
detail::WindowParams::clientSize(unsigned width, unsigned height)
{
	auto const clientRect = adjustWindowRect(width, height, dwStyle, dwExStyle, false);

	nWidth	= std::abs(clientRect.right - clientRect.left);
	nHeight	= std::abs(clientRect.bottom - clientRect.top);
					
	LOG_TRACE( L"WindowParams::clientSize(%1%, %2%) -> (%3%, %4%)",
               width % height % nWidth % nHeight );
    
    return *this;
}

detail::WindowParams&
detail::WindowParams::thisPointer(Window* window)
{
	lpParam = reinterpret_cast<LPVOID>(window);
	
    LOG_TRACE(L"WindowParams::thisPointer(%1%)", window);

    return *this;
}

detail::WindowParams&
detail::WindowParams::style(DWORD flags)
{
	dwStyle = flags;
	
    LOG_TRACE(L"WindowParams::style(%1%)", flags);

    return *this;
}

detail::WindowParams&
detail::WindowParams::childId(unsigned id)
{
	hMenu = (HMENU)id;
	
    LOG_TRACE(L"WindowParams::childId(%1%)", hMenu);
    
    return *this;
}

detail::WindowParams
detail::RichText::defaultParams()
{
	static HMODULE mod = LoadLibraryW(L"Msftedit.dll");
	assert(mod && "Failed to load [Msftedit.dll]");

    static WindowClass windowClass(MSFTEDIT_CLASS);

	WindowParams params(windowClass);
	params.style(WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_READONLY);

	return params;
}

detail::WindowParams
detail::Window::defaultParams()
{
    static WindowClass defaultClass(
        createGlWindowClass(&Window::staticWindowProc)
    );

	WindowParams params(defaultClass);
	params.dwStyle = WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_OVERLAPPEDWINDOW;

	return params;
}

void
detail::Window::show() const
{
    ::ShowWindow(window(), SW_SHOWNORMAL);
    LOG_TRACE(L"Window[%1%]::show", window());
}

void
detail::Window::resize(unsigned width, unsigned height) const
{
	LOG_TRACE(L"Window[%1%]::resize(%1%, %2%)", window() % width % height);
    
    RECT newRect = {0, 0, width, height};
	::AdjustWindowRect(&newRect, createParams_.dwStyle, FALSE);

	auto const newWidth		= std::abs(newRect.right - newRect.left);
	auto const newHeight	= std::abs(newRect.bottom - newRect.top);

	auto const result =	::SetWindowPos(
		window(), nullptr,
		0, 0,
		newWidth, newHeight,
		SWP_NOZORDER | SWP_NOREPOSITION | SWP_NOMOVE
	);
	CHECK_API_FAILURE(result == 0, "SetWindowPos");
}

void
detail::Window::resize(RECT const& rect) const
{
    LOG_TRACE(L"Window[%1%]::resize(rect)", window());

	unsigned const width = std::abs(rect.right - rect.left);
	unsigned const height = std::abs(rect.bottom - rect.top);

	resize(width, height);
}

detail::CharFormat::CharFormat()
{
	::memset(&format, 0, sizeof(format));
	format.cbSize = sizeof(format);
}
				
void
detail::CharFormat::enable(DWORD mask, bool enable)
{
	if (enable) {
		format.dwMask |= mask;
	} else {
		format.dwMask &= ~mask;
	}
}

void
detail::CharFormat::enableEffect(DWORD effect, bool enable)
{
	if (enable) {
		format.dwEffects |= effect;
	} else {
		format.dwEffects &= ~effect;
	}
}

void
detail::CharFormat::autoBackcolor(bool enable)
{
	enableEffect(CFE_AUTOBACKCOLOR, enable);
}

void
detail::CharFormat::setBackcolor(unsigned r, unsigned g, unsigned b)
{
	format.crBackColor = RGB(r, g, b);
	enable(CFM_BACKCOLOR, true);
}

detail::RichText::RichText(Window& parent, unsigned childId)
	: Window(
		defaultParams().parent(parent).childId(childId)
	)
{
	auto const prevData = setWindowLong(window(), GWLP_USERDATA, this);
	assert(prevData == 0);

	defaultProc_ = setWindowLong(window(), GWL_WNDPROC, &Window::staticWindowProc);

	auto const result = ::SendMessageW(
		window(), EM_SETEVENTMASK,
		(WPARAM)0,
		(LPARAM)ENM_SELCHANGE
	);
	assert(result == 0);
}

POINT
detail::RichText::getScrollPos() const
{
	POINT result;

	::SendMessageW(
		window(), EM_GETSCROLLPOS,
		(WPARAM)0,
		(LPARAM)&result
	);

	return result;
}

void
detail::RichText::setScrollPos(const POINT& p) const
{
	::SendMessageW(
		window(), EM_SETSCROLLPOS,
		(WPARAM)0,
		(LPARAM)&p
	);
}

void
detail::RichText::applyFormat(CharFormat const& format)
{
	setCharFormat(format.format, SCF_SELECTION);
}

LRESULT
detail::RichText::windowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg >= WM_MOUSEFIRST && msg <= WM_MOUSELAST) {
		if (onMouseEvent && onMouseEvent() == false)
			return 0;
	} 

	if (msg == WM_PAINT) {
		if (onPaint && onPaint() == false)
			return 0;
	} else if (msg == WM_VSCROLL) {
		if (onScroll && onScroll(HIWORD(wParam), LOWORD(wParam)) == false)
			return 0;
	}

	return defaultProc_(window(), msg, wParam, lParam);
}

CHARFORMAT2W
detail::RichText::getCharFormat(bool default)
{
	CHARFORMAT2W format;
	format.cbSize = sizeof(CHARFORMAT2W);

	::SendMessageW(
		window(), EM_GETCHARFORMAT,
		(WPARAM)(default ? SCF_DEFAULT : SCF_SELECTION),
		(LPARAM)&format
	);

	return format;
}

void
detail::RichText::setCharFormat(CHARFORMAT2W const& format, unsigned target) const
{
	auto const result = ::SendMessageW(
		window(), EM_SETCHARFORMAT,
		(WPARAM)target,
		(LPARAM)&format
	);
	
	CHECK_API_FAILURE(result == 0, "SendMessageW");
}

void
detail::RichText::setSelection(int begin, int end) const
{
	CHARRANGE const range = { begin, end };
	::SendMessageW(
		window(), EM_EXSETSEL,
		(WPARAM)0,
		(LPARAM)&range
	);
}

void
detail::RichText::replaceSelection(std::wstring const& text) const
{
	::SendMessageW(
		window(), EM_REPLACESEL,
		(WPARAM)FALSE,
		(LPARAM)text.c_str()
	);
}

void
detail::RichText::append(std::wstring const& text) const
{
	setSelection(-1, -1);
	replaceSelection(text);
}

detail::WindowParams
detail::GlWindow::defaultParams()
{
    return Window::defaultParams();
}

HINSTANCE
detail::getInstance()
{
	auto result = GetModuleHandleW(nullptr);

	CHECK_API_FAILURE(result == nullptr, "GetModuleHandleW");

	return result;
}

RECT
detail::adjustWindowRect(unsigned width, unsigned height, DWORD style, DWORD exStyle, bool menu)
{
	RECT rect = {0, 0, width, height};
	auto const result = ::AdjustWindowRectEx(&rect, style, menu ? TRUE : FALSE, exStyle);
	
	CHECK_API_FAILURE(result == 0, "AdjustWindowRectEx");
	
	return rect;
}

//Top level window procedure used to forward messages to the individual windows
LRESULT CALLBACK
detail::Window::staticWindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (msg == WM_NCCREATE) {
		auto const createInfo = reinterpret_cast<CREATESTRUCT const*>(lParam);
		assert(createInfo != nullptr);
		
		auto const window = reinterpret_cast<Window*>(createInfo->lpCreateParams);
		if (window == nullptr) {
			assert(0); //TODO throw an exception
		}

		auto const previous = setWindowLong(hWnd, GWLP_USERDATA, window);
		assert(previous == nullptr);

		assert(window->window_ == nullptr);
        window->window_.reset(hWnd);

		LOG_PRINT(L"Set this pointer for Window[%1%]", hWnd);
        LOG_TRACE(L"Window[%1%] -> %2%", hWnd % window);
	}

	auto const windowPtr = ::GetWindowLongPtrW(hWnd, GWLP_USERDATA);
	auto const window = reinterpret_cast<detail::Window*>(windowPtr);

	if (window != nullptr) {
		return window->windowProc(msg, wParam, lParam);
	}
	
	LOG_PRINT(L"Warning! No this pointer for Window[%1%]; Message[%2%] defaulted!", hWnd % msg);
    LOG_TRACE(L"[WARNING] Message[%1%] defaulted for Window[%2%]", msg % hWnd);

	return ::DefWindowProcW(hWnd, msg, wParam, lParam);
}

detail::WindowParams&
detail::WindowParams::parent(Window const& window)
{
	hWndParent = window.window();
    return *this;
}

detail::Window::Window(WindowParams const& params)
	: createParams_(params)
	, window_()
{
	window_ = createParams_.thisPointer(this).createWindow();
    LOG_TRACE(L"Window[%1%]::Window(WindowParams)", window());
}

bool
detail::Window::close()
{
    LOG_TRACE(L"Window[%1%]::close", window());

	::SendMessageW(window(), WM_CLOSE, 0, 0);

	if (::IsWindow(window())) {
		return false;
	}

	window_		= 0;

	return true;
}

bool
detail::GlWindow::close()
{
    LOG_TRACE(L"GlWindow[%1%]::close", window());
	
    return Window::close();
}

void
detail::Window::setPixelFormat(PIXELFORMATDESCRIPTOR const& format) const
{
    auto const dc = deviceContext();

	auto const pixelFormat = ::ChoosePixelFormat(dc, &format);
	CHECK_API_FAILURE(pixelFormat == 0, "ChoosePixelFormat");

	auto const result = ::SetPixelFormat(dc, pixelFormat, &format);
	CHECK_API_FAILURE(result == FALSE, "SetPixelFormat");

    LOG_TRACE(L"Window[%1%]::setPixelFormat dc=%2% format=%3%", window() % dc % pixelFormat);
}

namespace {
	template <UINT>
	LRESULT handleMessage(detail::Window& win, WPARAM wParam, LPARAM lParam);

	template<>
	LRESULT handleMessage<WM_KEYDOWN>(detail::Window& win, WPARAM wParam, LPARAM lParam)
	{
		if (!win.onKeyDown) {
			return ::DefWindowProcW(win.window(), WM_KEYDOWN, wParam, lParam);
		}

		win.onKeyDown(wParam);

		return 0;
	}

	template<>
	LRESULT handleMessage<WM_NOTIFY>(detail::Window& win, WPARAM wParam, LPARAM lParam)
	{
		if (!win.onNotify) {
			return ::DefWindowProcW(win.window(), WM_NOTIFY, wParam, lParam);
		}

		win.onNotify((NMHDR const*)lParam);

		return 0;
	}

	template<>
	LRESULT handleMessage<WM_CLOSE>(detail::Window& win, WPARAM wParam, LPARAM lParam)
	{
		//if there is no handler set, or if the handler returns true (close),
        //call the default window proc => close the window
        if (!win.onClose || win.onClose()) {
			return ::DefWindowProcW(win.window(), WM_CLOSE, wParam, lParam);
		}

		//otherwise, indicate that the message has been handled
        return 0;
	}

	template<>
	LRESULT handleMessage<WM_SIZE>(detail::Window& win, WPARAM wParam, LPARAM lParam)
	{
		if (!win.onResize) {
			return ::DefWindowProcW(win.window(), WM_SIZE, wParam, lParam);
		}

		auto const type		= wParam;
		auto const width	= LOWORD(lParam);
		auto const height	= HIWORD(lParam);

		win.onResize(width, height);

		return 0;
	}

	template<>
	LRESULT handleMessage<WM_ACTIVATE>(detail::Window& win, WPARAM wParam, LPARAM lParam)
	{
		if (!win.onActivate) {
			return ::DefWindowProcW(win.window(), WM_ACTIVATE, wParam, lParam);
		}

		auto const minFlag	= HIWORD(wParam);
		auto const state	= LOWORD(wParam);
		auto const hWnd		= reinterpret_cast<HWND>(lParam);

		switch (state) {
		case WA_INACTIVE :
			win.onActivate(false);
			break;
		case WA_ACTIVE :
		case WA_CLICKACTIVE :
			win.onActivate(true);
			break;
		default :
			assert(0);
		}

		LOG_PRINT(L"Activate [minimize=%1%, state=%2%, hwnd=%3%]", minFlag % state % hWnd);

		return 0;
	}
}

LRESULT
detail::Window::windowProc(UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_NOTIFY :	return handleMessage<WM_NOTIFY>(*this, wParam, lParam);
	case WM_CLOSE :		return handleMessage<WM_CLOSE>(*this, wParam, lParam);
	case WM_SIZE :		return handleMessage<WM_SIZE>(*this, wParam, lParam);
	case WM_ACTIVATE :	return handleMessage<WM_ACTIVATE>(*this, wParam, lParam);
	case WM_KEYDOWN :	return handleMessage<WM_KEYDOWN>(*this, wParam, lParam);
	default :
		break;
	}

	return ::DefWindowProcW(window(), msg, wParam, lParam);
}

bool
detail::Window::doEvents(unsigned count) const
{
	auto messageAvailable = [this](MSG& msg) -> bool {
		if (GetQueueStatus(QS_ALLINPUT)) {
			auto const result = PeekMessageW(&msg, this->window(), 0, 0, PM_REMOVE);
			return result != 0;
		}
		
		return false;
	};

	MSG msg = {};

	for (unsigned i = 0; messageAvailable(msg) && i < count; ++i) {
		TranslateMessage(&msg); 
		DispatchMessageW(&msg);
	}

	return false;
}

bool
detail::Window::doEventsWait()
{
	MSG msg = {0};

	auto const result = ::GetMessageW(&msg, nullptr, 0, 0);
	if (result == 0) {
		return false;
	} else if (result < 0) {
		return false;
	}
	
	TranslateMessage(&msg); 
	DispatchMessageW(&msg);

	return true;
}

RECT
detail::Window::getClientRect() const
{
	RECT winRect;
	::GetWindowRect(window(), &winRect);

	RECT rect;
	auto const result = ::GetClientRect(window(), &rect);
	CHECK_API_FAILURE(result == 0, "GetClientRect");

	return rect;
}

void
detail::Window::setClientSize(unsigned width, unsigned height)
{
}


namespace {
	//get the maximum number of pixel formats
	unsigned getMaxPixelFormats(HDC dc)
	{
		int const	attributes = WGL_NUMBER_PIXEL_FORMATS_ARB;
		int			maxFormats = 0;

		auto const result = ::wglGetPixelFormatAttribivARB(
			dc, 0, 0, 1, &attributes, &maxFormats
		);
		CHECK_API_FAILURE(result == FALSE, "wglGetPixelFormatAttribivARB");

        LOG_TRACE(L"dc=[%1%] has a max of %2% pixel formats", dc % maxFormats);
		
		return maxFormats;
	}

	//return the indicies of all matching pixel formats
	std::vector<int> getPixelFormats(HDC dc)
	{
		int const pixelAttribs[] = {
			WGL_DRAW_TO_WINDOW_ARB,	GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB,	GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB,	GL_TRUE,
			WGL_ACCELERATION_ARB,	WGL_FULL_ACCELERATION_ARB,
			WGL_PIXEL_TYPE_ARB,		WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB,		32,
			WGL_DEPTH_BITS_ARB,		24,
			WGL_STENCIL_BITS_ARB,	8,
			0,						//End
		};
					
		unsigned const		max			= getMaxPixelFormats(dc);
		UINT				numFormats	= 0;
		std::vector<int>	formats(max);
					
		auto const result = ::wglChoosePixelFormatARB(
			dc, pixelAttribs, nullptr, max, &formats[0], &numFormats
		);
		CHECK_API_FAILURE(result == FALSE, "wglChoosePixelFormatARB");

        formats.resize(numFormats);

        LOG_TRACE(L"got %1% pixel formats for dc=[%2%]", numFormats % dc);

		return formats;
	}

	//fill a PIXELFORMATDESCRIPTOR with info for format
	PIXELFORMATDESCRIPTOR describePixelFormat(int format, HDC dc)
	{
		PIXELFORMATDESCRIPTOR pfd = {0};

		auto const result = ::DescribePixelFormat(
			dc, format, sizeof(PIXELFORMATDESCRIPTOR), &pfd
		);
		CHECK_API_FAILURE(result == 0, "DescribePixelFormat");

        LOG_TRACE(L"DescribePixelFormat(format=%1%, dc=%2%)", format % dc);

		return pfd;
	}
} //anon


void
detail::GlWindow::setPixelFormat() const
{
	auto const dc		= deviceContext();
	auto const formats	= getPixelFormats(dc);

	if (formats.size() == 0) {
		assert(0); //TODO
	}

	auto const format		= formats[0];
	auto const descriptor	= describePixelFormat(format, dc);
	
	auto const result = ::SetPixelFormat(dc, format, &descriptor);
	CHECK_API_FAILURE(result == FALSE, "SetPixelFormat");

    LOG_TRACE( L"GlWindow[%1%]::setPixelFormat for dc=%2% to format[0]=%3% of %4% formats",
               window() % dc % format % formats.size() );
}

namespace {
    void initGlew() {
        using namespace detail;

        LOG_TRACE(L"Initializing glew... %1%", "");

        static bool initialized = false;
        if (initialized) {
            return;
        }

        Window win(Window::defaultParams());

        PIXELFORMATDESCRIPTOR pfd = {0};
	    pfd.nSize		= sizeof(PIXELFORMATDESCRIPTOR);
	    pfd.nVersion	= 1;
	    pfd.dwFlags		= PFD_SUPPORT_OPENGL      |
                          PFD_GENERIC_ACCELERATED |
                          PFD_DOUBLEBUFFER        |
                          PFD_DRAW_TO_WINDOW;
	    pfd.iPixelType	= PFD_TYPE_RGBA;
	    pfd.cColorBits	= 32;
	    pfd.cDepthBits	= 32;
	    pfd.iLayerType	= PFD_MAIN_PLANE;

        win.setPixelFormat(pfd);

        auto const dc = win.deviceContext();
        auto const rc = createLegacyGlContext(win.deviceContext());
        {
            auto const result = ::wglMakeCurrent(dc, rc.get());
            assert(result != FALSE); //TODO
        }
		{
            auto const result = ::glewInit();
            assert(result == GLEW_OK); //TODO
		}

        initialized = true;
        LOG_TRACE(L"Initialized glew! %1%", "");
    };

} //namespace anon

detail::GlWindow::GlWindow(unsigned major, unsigned minor, unsigned width, unsigned height)
	: Window(
		GlWindow::defaultParams()
			.clientSize(width, height)
	)
	, major_(major)
	, minor_(minor)
    , glDc_(0)
{
	LOG_TRACE( L"GlWindow[%1%]::GlWindow(major=%2%, minor=%3%, width=%4%, height=%5%)",
               window() % major % minor % width % height );

    initGlew();
	show();
}

detail::handle<HGLRC>::unique
detail::GlWindow::acquireGl()
{
    LOG_TRACE(L"GlWindow[%1%]::acquireGl", window());

    glDc_ = Window::deviceContext();

    setPixelFormat();
						
	//create the final rendering context for this window
	int const contextAttribs[] = {
		WGL_CONTEXT_MAJOR_VERSION_ARB,	major_,
		WGL_CONTEXT_MINOR_VERSION_ARB,	minor_,
		WGL_CONTEXT_FLAGS_ARB,			WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,
		0
	};
    
    auto handle = detail::createGlContext(deviceContext(), contextAttribs);
    
    auto const result = ::wglMakeCurrent(deviceContext(), handle.get());
    CHECK_API_FAILURE(result == FALSE, "wglMakeCurrent");

    return handle;
}

void
detail::GlWindow::releaseGl(handle<HGLRC>::unique handle)
{
    //::wglMakeCurrent(deviceContext(), 0);
}

void
detail::GlWindow::swap() const
{
	auto const result = ::SwapBuffers(deviceContext());
	CHECK_API_FAILURE(result == FALSE, "SwapBuffers");
}

HDC
detail::Window::deviceContext() const
{
    auto const dc = ::GetDC(window());

    LOG_TRACE(L"Window[%1%]::deviceContext -> %2%", window() % dc);

    return dc;
}

#endif // defined(VOX_WINDOWS)
