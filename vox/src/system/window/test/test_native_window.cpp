#include "common.hpp"
#include <boost/test/unit_test.hpp>

#include "../NativeWindow.hpp"

//____________________________________________________________________________//
BOOST_AUTO_TEST_CASE(NativeWindow) {
    vox::system::NativeWindow win(640, 480);
    win.acquireGl();

    bool closed = false;
    win.setOnClose([&closed] {
        return (closed = true);
    });

    while (!closed) {
        win.doEventsWait();
    }
}