#include "common.hpp"
#include <boost/test/unit_test.hpp>
#include "../NativeWindowImpl.hpp"

using namespace boost::unit_test;
namespace detail = ::vox::system::detail;

static LRESULT CALLBACK wndProc(HWND, UINT, WPARAM, LPARAM) {
	return 0;
}

//____________________________________________________________________________//
BOOST_AUTO_TEST_CASE(WindowClass)
{
	
    {   //System window class
        std::wstring const className(L"Edit");
        detail::WindowClass testClass(className.c_str());
        
        BOOST_CHECK(className == testClass.name()); //TODO should use equal
        BOOST_CHECK_EQUAL(0, testClass.atom());
    }
    
    {   //Custom, opengl supporting window class
        detail::string_t const glClassName(L"VoxGlWindow");
        detail::WindowClass testClass = detail::createGlWindowClass(wndProc);
        
        //trying to recreate the class should not fail
        BOOST_CHECK_NO_THROW(
            detail::createGlWindowClass(wndProc)
        );

        BOOST_CHECK(testClass.name() == glClassName);

        {   //Test by getting info via the class name
            WNDCLASSEX classInfo;
            {
                auto const result = ::GetClassInfoExW(
                    detail::getInstance(), testClass.name().c_str(), &classInfo
                );

                BOOST_CHECK(result != FALSE);
            }
            
            BOOST_CHECK(
                detail::string_t(classInfo.lpszClassName) == testClass.name()
            );
        }
    }   
}

//____________________________________________________________________________//
BOOST_AUTO_TEST_CASE(Window)
{   
    detail::Window win(detail::Window::defaultParams());
}
/*____________________________________________________________________________//
BOOST_AUTO_TEST_CASE( WindowParams )
{
	detail::window_class_t testClass(
		new detail::WindowClass(L"test class", wndProc)
	);
	
	detail::WindowParams testParams(testClass);

	BOOST_CHECK(testParams.dwExStyle			== 0);
	BOOST_CHECK(testParams.windowClass			== testClass);
	BOOST_CHECK(testParams.windowName.size()	== 0);
	BOOST_CHECK(testParams.dwStyle				== 0);
	BOOST_CHECK(testParams.x					== CW_USEDEFAULT);
	BOOST_CHECK(testParams.y					== CW_USEDEFAULT);
	BOOST_CHECK(testParams.nWidth				== CW_USEDEFAULT);
	BOOST_CHECK(testParams.nHeight				== CW_USEDEFAULT);
	BOOST_CHECK(testParams.hWndParent			== 0);
	BOOST_CHECK(testParams.hMenu				== 0);
	BOOST_CHECK(testParams.hInstance			== detail::getInstance());
	BOOST_CHECK(testParams.lpParam				== nullptr);

	testParams.clientSize(100, 100);
	BOOST_CHECK(testParams.nWidth >= 100);
	BOOST_CHECK(testParams.nHeight >= 100);

	testParams.thisPointer((detail::Window*)100);
	BOOST_CHECK((int)testParams.lpParam == 100);

	testParams.style(100);
	BOOST_CHECK((int)testParams.dwStyle == 100);

	testParams.childId(100);
	BOOST_CHECK((int)testParams.hMenu == 100);
}

//____________________________________________________________________________*/