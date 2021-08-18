#include <catch.hpp>

#include <ovf.h>

#include <iostream>

TEST_CASE( "NonExistent", "[nonexistent]" )
{
    auto file = ovf_open("nonexistent.ovf");
    REQUIRE( file->found == false );
    REQUIRE( file->is_ovf == false );
    REQUIRE( file->n_segments == 0 );
    ovf_segment segment;
    int success = ovf_read_segment_header(file, 0, &segment);
    REQUIRE( success == OVF_ERROR );
    ovf_close(file);
}

TEST_CASE( "Write", "[write]" )
{
    const char * testfile = "testfile_cpp.ovf";

    SECTION( "write" )
    {
        // segment header
        auto segment = ovf_segment_create();
        segment->title = const_cast<char *>("ovf test title - write");
        segment->comment = const_cast<char *>("test write");
        segment->valuedim = 3;
        segment->n_cells[0] = 2;
        segment->n_cells[1] = 2;
        segment->n_cells[2] = 1;
        segment->N = 4;

        // data
        std::vector<double> field(3*segment->N, 1);
        field[0] = 3;
        field[3] = 2;
        field[6] = 1;
        field[9] = 0;

        // open
        auto file = ovf_open(testfile);

        // write
        int success = ovf_write_segment_8(file, segment, field.data(), OVF_FORMAT_TEXT);
        if( OVF_OK != success )
            std::cerr << ovf_latest_message(file) << std::endl;
        REQUIRE( success == OVF_OK );

        // close
        ovf_close(file);
    }

    SECTION( "append" )
    {
        // segment header
        auto segment = ovf_segment_create();
        segment->title = const_cast<char *>("ovf test title - append");
        segment->comment = const_cast<char *>("test append");
        segment->valuedim = 3;
        segment->n_cells[0] = 2;
        segment->n_cells[1] = 2;
        segment->n_cells[2] = 1;
        segment->N = 4;

        // data
        std::vector<double> field(3*segment->N, 1);
        field[0] = 6;
        field[3] = 4;
        field[6] = 2;
        field[9] = 0;

        // open
        auto file = ovf_open(testfile);

        // write
        int success = ovf_append_segment_8(file, segment, field.data(), OVF_FORMAT_CSV);
        if( OVF_OK != success )
            std::cerr << ovf_latest_message(file) << std::endl;
        REQUIRE( success == OVF_OK );

        // close
        ovf_close(file);
    }

    SECTION( "append irregular" )
    {
        // segment header
        auto segment = ovf_segment_create();
        segment->title = const_cast<char *>("ovf test title - append irregular mesh");
        segment->comment = const_cast<char *>("an irregular mesh has different keywords than a rectangular one");
        segment->valuedim = 3;
        segment->meshtype = const_cast<char *>("irregular");
        segment->pointcount = 4;

        // data
        std::vector<double> field(3*segment->pointcount, 1);
        field[0] = 6;
        field[3] = 4;
        field[6] = 2;
        field[9] = 0;

        // open
        auto file = ovf_open(testfile);

        // write
        int success = ovf_append_segment_8(file, segment, field.data(), OVF_FORMAT_CSV);
        if( OVF_OK != success )
            std::cerr << ovf_latest_message(file) << std::endl;
        REQUIRE( success == OVF_OK );

        // close
        ovf_close(file);
    }
}

TEST_CASE( "Read", "[read]" )
{
    const char * testfile = "testfile_cpp.ovf";

    SECTION( "first segment" )
    {
        // open
        auto file = ovf_open(testfile);
        REQUIRE( file->found == true );
        REQUIRE( file->is_ovf == true );
        REQUIRE( file->n_segments == 3 );
        int index = 0;

        // segment header
        auto segment = ovf_segment_create();

        // read header
        int success = ovf_read_segment_header(file, index, segment);
        if( OVF_OK != success )
            std::cerr << ovf_latest_message(file) << std::endl;
        REQUIRE( success == OVF_OK );
        REQUIRE( segment->N == 4 );
        REQUIRE( std::string(segment->meshtype) == "rectangular" );

        // data
        std::vector<float> field(3*segment->N);

        // read data
        success = ovf_read_segment_data_4(file, index, segment, field.data());
        if( OVF_OK != success )
            std::cerr << ovf_latest_message(file) << std::endl;
        REQUIRE( success == OVF_OK );
        REQUIRE( field[0] == 3 );
        REQUIRE( field[1] == 1 );
        REQUIRE( field[3] == 2 );
        REQUIRE( field[6] == 1 );
        REQUIRE( field[9] == 0 );

        // close
        ovf_close(file);
    }

    SECTION( "second segment" )
    {
        // open
        auto file = ovf_open(testfile);
        REQUIRE( file->n_segments == 3 );
        int index = 1;

        // segment header
        auto segment = ovf_segment_create();

        // read header
        int success = ovf_read_segment_header(file, index, segment);
        if( OVF_OK != success )
            std::cerr << ovf_latest_message(file) << std::endl;
        REQUIRE( success == OVF_OK );
        REQUIRE( segment->N == 4 );
        REQUIRE( std::string(segment->meshtype) == "rectangular" );

        // data
        std::vector<double> field(3*segment->N);

        // read data
        success = ovf_read_segment_data_8(file, index, segment, field.data());
        if( OVF_OK != success )
            std::cerr << ovf_latest_message(file) << std::endl;
        REQUIRE( success == OVF_OK );
        REQUIRE( field[0] == 6 );
        REQUIRE( field[1] == 1 );
        REQUIRE( field[3] == 4 );
        REQUIRE( field[6] == 2 );
        REQUIRE( field[9] == 0 );

        // close
        ovf_close(file);
    }

    SECTION( "third segment" )
    {
        // open
        auto file = ovf_open(testfile);
        REQUIRE( file->n_segments == 3 );
        int index = 2;

        // segment header
        auto segment = ovf_segment_create();

        // read header
        int success = ovf_read_segment_header(file, index, segment);
        if( OVF_OK != success )
            std::cerr << ovf_latest_message(file) << std::endl;
        REQUIRE( success == OVF_OK );
        REQUIRE( segment->N == 4 );
        REQUIRE( std::string(segment->meshtype) == "irregular" );
        REQUIRE( segment->pointcount == 4 );

        // data
        std::vector<double> field(3*segment->N);

        // read data
        success = ovf_read_segment_data_8(file, index, segment, field.data());
        if( OVF_OK != success )
            std::cerr << ovf_latest_message(file) << std::endl;
        REQUIRE( success == OVF_OK );
        REQUIRE( field[0] == 6 );
        REQUIRE( field[1] == 1 );
        REQUIRE( field[3] == 4 );
        REQUIRE( field[6] == 2 );
        REQUIRE( field[9] == 0 );

        // close
        ovf_close(file);
    }
}

TEST_CASE( "Atomistic Write", "[write]" )
{
    const char * testfile = "testfile_cpp.aovf";

    SECTION( "write" )
    {
        // segment header
        auto segment = ovf_segment_create();
        segment->title = const_cast<char *>("ovf test title - write");
        segment->comment = const_cast<char *>("test write");
        segment->valuedim = 3;
        segment->n_cells[0] = 2;
        segment->n_cells[1] = 1;
        segment->n_cells[2] = 1;

        // Fill in atomistic values
        segment->bravaisa[0] = 1;
        segment->bravaisa[1] = 0;
        segment->bravaisa[2] = 0;

        segment->bravaisb[0] = 0;
        segment->bravaisb[1] = 1;
        segment->bravaisb[2] = 0;

        segment->bravaisc[0] = 0;
        segment->bravaisc[1] = 0;
        segment->bravaisc[2] = 1;

        segment->ncellpoints = 2;
        float basis[6] = {
                            0,0.1,0.2,
                            2,3,4
                        };

        segment->basis = (float *) malloc( segment->ncellpoints * 3 * sizeof(float) );
        for( int ib=0; ib < segment->ncellpoints; ib++)
        {
            for(int i=0; i<3; i++)
            {
                segment->basis[ib*3 + i] = basis[ib*3 + i];
            }
        }

        segment->N = 4;
        segment->meshtype = const_cast<char *>("lattice");

        // data
        std::vector<double> field(3*segment->N, 1);
        field[0] = 3;
        field[3] = 2;
        field[6] = 1;
        field[9] = 0;

        // open
        auto file = ovf_open(testfile);

        file->ovf_extension_format = OVF_EXTENSION_FORMAT_AOVF; // Set the flag for the atomistic extension

        // write
        int success = ovf_write_segment_8(file, segment, field.data(), OVF_FORMAT_TEXT);
        // if( OVF_OK != success )
        //     std::cerr << ovf_latest_message(file) << std::endl;
        REQUIRE( success == OVF_OK );

        // close
        ovf_close(file);
    }
}

TEST_CASE( "Atomistic Read", "[read]" )
{
    const char * testfile = "testfile_cpp.aovf";

    SECTION( "first segment" )
    {
        // open
        auto file = ovf_open(testfile);
        std::cerr << ovf_latest_message(file) << std::endl;
        REQUIRE( file->found == true );
        REQUIRE( file->is_ovf == true );
        REQUIRE( file->n_segments == 1 );
        int index = 0;

        // segment header
        auto segment = ovf_segment_create();

        // read header
        int success = ovf_read_segment_header(file, index, segment);
        if( OVF_OK != success )
            std::cerr << ovf_latest_message(file) << std::endl;
        REQUIRE( success == OVF_OK );
        REQUIRE( segment->N == 4 );

        REQUIRE( segment->ncellpoints == 2);

        REQUIRE( segment->basis[0] == 0);
        REQUIRE( segment->basis[1] == 0.1f);
        REQUIRE( segment->basis[2] == 0.2f);
        REQUIRE( segment->basis[3] == 2);
        REQUIRE( segment->basis[4] == 3);
        REQUIRE( segment->basis[5] == 4);

        REQUIRE( segment->bravaisa[0] == 1);
        REQUIRE( segment->bravaisa[1] == 0);
        REQUIRE( segment->bravaisa[2] == 0);

        REQUIRE( segment->bravaisb[0] == 0);
        REQUIRE( segment->bravaisb[1] == 1);
        REQUIRE( segment->bravaisb[2] == 0);

        REQUIRE( segment->bravaisc[0] == 0);
        REQUIRE( segment->bravaisc[1] == 0);
        REQUIRE( segment->bravaisc[2] == 1);


        REQUIRE( std::string(segment->meshtype) == "lattice" );

        // data
        std::vector<float> field(3*segment->N);

        // read data
        success = ovf_read_segment_data_4(file, index, segment, field.data());
        if( OVF_OK != success )
            std::cerr << ovf_latest_message(file) << std::endl;
        REQUIRE( success == OVF_OK );
        REQUIRE( field[0] == 3 );
        REQUIRE( field[1] == 1 );
        REQUIRE( field[3] == 2 );
        REQUIRE( field[6] == 1 );
        REQUIRE( field[9] == 0 );


        // close
        ovf_close(file);
    }
}