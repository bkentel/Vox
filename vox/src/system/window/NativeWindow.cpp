#include "common.hpp"
#include "config.hpp"
#include "NativeWindow.hpp"

namespace sys = ::vox::system;

#if defined(VOX_WINDOWS)

#include "windows/NativeWindowImpl.hpp"

struct sys::detail::opengl_context_impl {
    friend sys::detail::native_window_data;
    sys::detail::handle<HGLRC>::unique context;
};

struct sys::detail::native_window_data {
    native_window_data(unsigned width, unsigned height)
        : win(3, 2, width, height)
    {
    }

    static bool doEventsWait() {
        return detail::GlWindow::doEventsWait();
    }

    OpenGlContext acquireGl() {
        OpenGlContext result;
        result.impl_->context = win.acquireGl();
        
        return result;
    }

    void releaseGl() {
        //win.releaseGl();
    }

    void swap() const {
        win.swap();
    }

    util::Rectangle<unsigned> clientSize() const {
        RECT const r = win.getClientRect();
        return util::Rectangle<unsigned>(r.left, r.top, r.right, r.bottom);
    }

    detail::GlWindow win;
};

#endif //VOX_WINDOWS

sys::OpenGlContext::OpenGlContext()
    : impl_(
        new detail::opengl_context_impl()
    )
{
}

sys::OpenGlContext::~OpenGlContext()
{
}

sys::OpenGlContext::OpenGlContext(OpenGlContext&& other)
    : impl_(std::move(other.impl_))
{
}

sys::OpenGlContext&
sys::OpenGlContext::operator=(OpenGlContext&& rhs)
{
    std::swap(impl_, rhs.impl_);
    return *this;
}


sys::NativeWindow::NativeWindow(unsigned width, unsigned height)
    : impl_(
        new detail::native_window_data(width, height)
    )
{
}

sys::NativeWindow::~NativeWindow()
{
}

sys::OpenGlContext
sys::NativeWindow::acquireGl()
{
    return impl_->acquireGl();
}

void
sys::NativeWindow::releaseGl()
{
    impl_->releaseGl();
}

bool
sys::NativeWindow::doEventsWait()
{
    return detail::native_window_data::doEventsWait();
}

void
sys::NativeWindow::setOnClose(std::function<bool ()> callback)
{
    impl_->win.onClose = callback;
}

void
sys::NativeWindow::setOnResize(
    std::function<bool (unsigned width, unsigned height)> callback
) {
    impl_->win.onResize = callback;
}

void
sys::NativeWindow::swap() const
{
    impl_->swap();
}

vox::util::Rectangle<unsigned>
sys::NativeWindow::clientSize() const
{
    return impl_->clientSize();
}