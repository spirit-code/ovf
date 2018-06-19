#pragma once
#ifndef DETAIL_OVFFILE_H
#define DETAIL_OVFFILE_H

#include "ovf.h"
#include "Filter_File_Handle.hpp"

#include <string>
#include <fstream>
#include <cctype>
#include <array>

#include <fmt/format.h>
#include <fmt/ostream.h>


// Test values for 4bit and 8bit binary data
static const uint32_t test_hex_4b = 0x4996B438;
static const uint64_t test_hex_8b = 0x42DC12218377DE40;

// Comment tag in OVF file header
static const std::string comment_tag = "##";

// ?
static const std::string empty_line = "#\n";

// The number of zero-padding for segment count.
// This is needed so that, when appending, the file does not need to be overwritten
static const int n_segments_str_digits = 6; // can store 1M modes


struct OVF_File
{
    std::string error_message;
    
    std::unique_ptr<Filter_File_Handle> ifile;

    // Whether the file could be opened
    bool file_exists;
    // Whether the opened file contains an OVF header
    bool isOVF;

    // The file name (without path?)
    std::string filename;

    // VF_FileFormat format;

    // Output attributes
    std::string output_to_file;
    std::string datatype_out;

    // Vector-field geometry
    ovf_geometry geometry;

    // Input attributes 
    std::string version;
    std::string title;
    std::string meshunit;
    std::string meshtype;
    std::string valueunits;
    std::string datatype_in;
    int binary_length;
    std::array<double,3> max;
    std::array<double,3> min;
    int valuedim; // number of data columns
    // Irregular mesh
    int pointcount;
    // Rectangular mesh
    std::array<std::array<double,3>,3> base;
    std::array<double,3> stepsize;
    std::array<int,3> nodes;

    // Number of segments specified by the header
    int n_segments;
    std::string n_segments_as_str;
    // ?
    std::ios::pos_type n_segments_pos;
    // Positions of the beggining of each segment in the input file 
    std::vector<std::ios::pos_type> segment_fpos;

    // Constructor
    OVF_File( std::string filename, int format = 0 );

    // Check if the file already exists
    bool exists();
    // Check if the file is in OVF format
    bool is_OVF();
    // Get the number of segments in the file
    int get_n_segments();
    // Read header and data from a given segment. Also check geometry
    void read_segment( double * vf, ovf_geometry * geometry, const int idx_seg = 0 );
    // Write segment to file (if the file exists overwrite it)
    void write_segment( const double * vf, const ovf_geometry * geometry,
                        const std::string comment = "", const bool append = false );

private:
    // Check OVF version
    void check_version();
    // Read segment's header
    void read_header();
    // Check segment's geometry
    void check_geometry( const ovf_geometry * geometry );
    // Read segment's data
    void read_data( double * vf, ovf_geometry * geometry );
    // In case of binary data check the binary check values
    bool check_binary_values();
    // Read binary OVF data
    void read_data_bin( double * vf, ovf_geometry * geometry );
    // Read text OVF data. The delimiter, if any, will be discarded in the reading
    void read_data_txt( double * vf, ovf_geometry * geometry, 
                        const std::string& delimiter = "" );
    // Write OVF file header
    void write_top_header();
    // Write segment data binary
    void write_data_bin( const double * vf, int size );
    // Write segment data text
    void write_data_txt( const double * vf, int size, const std::string& delimiter = "" ); 
    // Increment segment count
    void increment_n_segments();
    // Read the number of segments in the file by reading the top header
    void read_n_segments_from_top_header();
    // Count the number of segments in the file. It also saves their file positions
    int count_and_locate_segments();

    // Read a variable from the comment section from the header of segment idx_seg
    template <typename T> void Read_Variable_from_Comment( T& var, const std::string name,
                                                            const int idx_seg = 0 )
    {
        try
        {
            // NOTE: seg_idx.max = segment_fpos.size - 2
            // if ( idx_seg >= ( this->segment_fpos.size() - 1 ) )
            //     spirit_throw( Utility::Exception_Classifier::Input_parse_failed, 
            //                     Utility::Log_Level::Error,
            //                     "OVF error while choosing segment to read - "
            //                     "index out of bounds" );

            this->ifile->SetLimits( this->segment_fpos[idx_seg], 
                                    this->segment_fpos[idx_seg+1] );

            this->ifile->Read_Single( var, name );

            // Log( Utility::Log_Level::Debug, this->sender, fmt::format( "{}{}", name, var ) );
        }
        catch (...)
        {
            // spirit_handle_exception_core(fmt::format( "Failed to read variable \"{}\" "
            //                                             "from comment", name ));
        }
    }

    // Read a Vector3 from the comment section from the header of segment idx_seg
    template <typename T> void Read_String_from_Comment( T& var, std::string name,
                                                            const int idx_seg = 0 )
    {
        try
        {
            // NOTE: seg_idx.max = segment_fpos.size - 2
            // if ( idx_seg >= ( this->segment_fpos.size() - 1 ) )
            //     spirit_throw( Utility::Exception_Classifier::Input_parse_failed, 
            //                     Utility::Log_Level::Error,
            //                     "OVF error while choosing segment to read - "
            //                     "index out of bounds" );

            this->ifile->SetLimits( this->segment_fpos[idx_seg], 
                                    this->segment_fpos[idx_seg+1] );

            this->ifile->Read_String( var, name, false );

            // Log( Utility::Log_Level::Debug, this->sender, fmt::format( "{}{}", name, var ) );
        }
        catch (...)
        {
            // spirit_handle_exception_core(fmt::format( "Failed to read string \"{}\" "
            //                                             "from comment", name ));
        }
    }
};

#endif