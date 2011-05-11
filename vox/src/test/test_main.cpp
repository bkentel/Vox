#include "common.hpp"

#define BOOST_TEST_MAIN

#include <boost/test/unit_test.hpp>

using namespace boost::unit_test;

//____________________________________________________________________________//
BOOST_AUTO_TEST_SUITE( vox_test_suite )

BOOST_AUTO_TEST_CASE(test_case_main)
{
     BOOST_MESSAGE( "Testing is in progress" );
     //BOOST_CHECK( false );
}

BOOST_AUTO_TEST_SUITE_END()
//____________________________________________________________________________//
