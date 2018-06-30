#include <catch.hpp>

#include <ovf.h>

TEST_CASE( "NonExistent", "[nonexistent]" )
{
    auto file = ovf_open("nonexistent.ovf");
    ovf_segment segment;
    int success = ovf_read_segment_header(file, 0, &segment);
    REQUIRE( success == OVF_ERROR );
    ovf_close(file);
}

TEST_CASE( "Write", "[write]" )
{
    const char * testfile = "test/testfile_cpp.ovf";

    SECTION( "write" )
    {
        auto file = ovf_open(testfile);
        ovf_close(file);
    }
    SECTION( "append")
    {
        auto file = ovf_open(testfile);
        ovf_close(file);
    }
}

TEST_CASE( "Read", "[read]")
{
    const char * testfile = "test/testfile_cpp.ovf";

    SECTION( "first segment" )
    {
        auto file = ovf_open(testfile);
        ovf_close(file);
    }
    SECTION( "second segment")
    {
        auto file = ovf_open(testfile);
        ovf_close(file);
    }
}