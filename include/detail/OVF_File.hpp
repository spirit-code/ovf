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
    // Read segment's header into member variables
    void read_segment_header( ovf_segment * segment, int idx_seg );
    // Read header and data from a given segment. Also check geometry
    template <typename T>
    void read_segment( T * vf,  const ovf_segment * segment, const int idx_seg = 0 );
    // Write segment to file (if the file exists overwrite it)
    template <typename T>
    void write_segment( const T * vf, const ovf_segment * segment, const std::string comment = "",
                        const bool append = false, int format=OVF_FORMAT_BIN );

private:
    // Check OVF version
    void check_version();
    // Read segment's header into member variables
    void read_header();
    // Check segment's geometry
    void check_geometry( const ovf_segment * segment );
    // Read segment's data
    template <typename T>
    void read_data( T * vf );
    // In case of binary data check the binary check values
    bool check_binary_values();
    // Read binary OVF data
    template <typename T>
    void read_data_bin( T * vf );
    // Read text OVF data. The delimiter, if any, will be discarded in the reading
    template <typename T>
    void read_data_txt( T * vf, const std::string& delimiter = "" );
    // Write OVF file header
    void write_top_header();
    // Write segment data binary
    template <typename T>
    void write_data_bin( const T * vf, int size );
    // Write segment data text
    template <typename T>
    void write_data_txt( const T * vf, int size, const std::string& delimiter = "" ); 
    // Increment segment count
    void increment_n_segments();
    // Read the number of segments in the file by reading the top header
    void read_n_segments_from_top_header();
    // Count the number of segments in the file. It also saves their file positions
    int count_and_locate_segments();

    // Read a variable from the comment section from the header of segment idx_seg
    template <typename T>
    void Read_Variable_from_Comment( T& var, const std::string name, const int idx_seg = 0 )
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
    template <typename T>
    void Read_String_from_Comment( T& var, std::string name,
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


    // TODO: use Filter_File_Handle instead...
    void Strings_to_File(const std::vector<std::string> text, const std::string name, int no=-1)
    {
        std::ofstream myfile;
        myfile.open(name);
        if (myfile.is_open())
        {
            if (no < 0)
                no = text.size();
            // Log(Log_Level::Debug, Log_Sender::All, "Started writing " + name);
            for (int i = 0; i < no; ++i) {
                myfile << text[i];
            }
            myfile.close();
            // Log(Log_Level::Debug, Log_Sender::All, "Finished writing " + name);
        }
        else
        {
            // Log(Log_Level::Error, Log_Sender::All, "Could not open " + name + " to write to file");
        }
    }

    void Append_String_to_File(const std::string text, const std::string name)
    {
        std::ofstream myfile;
        myfile.open(name, std::ofstream::out | std::ofstream::app);
        if (myfile.is_open())
        {
            // Log(Log_Level::Debug, Log_Sender::All, "Started writing " + name);
            myfile << text;
            myfile.close();
            // Log(Log_Level::Debug, Log_Sender::All, "Finished writing " + name);
        }
        else
        {
            // Log(Log_Level::Error, Log_Sender::All, "Could not open " + name + " to append to file");
        }
    }
};



template <typename T>
void OVF_File::read_segment( T * vf, const ovf_segment * segment, 
                                const int idx_seg )
try
{
    if ( !this->file_exists )
    {
        // spirit_throw( Exception_Classifier::File_not_Found, Log_Level::Warning, 
        //                 fmt::format( "The file \"{}\" does not exist", filename ) );
    } 
    else if ( this->n_segments == 0 )
    {
        // spirit_throw( Exception_Classifier::Bad_File_Content, Log_Level::Warning, 
        //                 fmt::format( "File \"{}\" is empty", filename ) );
    }
    else
    {
        // open the file
        this->ifile = std::unique_ptr<Filter_File_Handle>( 
                            new Filter_File_Handle( this->filename, comment_tag ) ); 
        
        // NOTE: seg_idx.max = segment_fpos.size - 2
        // if ( idx_seg >= ( this->segment_fpos.size() - 1 ) )
        //     spirit_throw( Exception_Classifier::Input_parse_failed, Log_Level::Error,
        //                     "OVF error while choosing segment - index out of bounds" );

        this->ifile->SetLimits( this->segment_fpos[idx_seg], 
                                this->segment_fpos[idx_seg+1] );
    
        read_header();
        check_geometry( segment );
        read_data( vf );

        // close the file
        this->ifile = NULL;
    }
}
catch( ... )
{

}


template <typename T>
void OVF_File::read_data( T * vf )
{
    try
    {
        // Raw data representation
        ifile->Read_String( this->datatype_in, "# Begin: Data" );
        std::istringstream repr( this->datatype_in );
        repr >> this->datatype_in;
        if( this->datatype_in == "binary" ) 
            repr >> this->binary_length;
        else
            this->binary_length = 0;

        // auto lvl = Log_Level::Debug;

        // Log( lvl, this->sender, fmt::format( "# OVF data representation = {}", this->datatype_in ) );
        // Log( lvl, this->sender, fmt::format( "# OVF binary length       = {}", this->binary_length ) );

        // Check that representation and binary length valures are ok
        if( this->datatype_in != "text" && 
            this->datatype_in != "binary" &&
            this->datatype_in != "csv" )
        {
            // spirit_throw( Utility::Exception_Classifier::Bad_File_Content, 
            //                 Utility::Log_Level::Error, "Data representation must be "
            //                 "either \"text\", \"binary\" or \"csv\"");
        }
        
        if( this->datatype_in == "binary" && 
                this->binary_length != 4 && this->binary_length != 8  )
        {
            // spirit_throw( Exception_Classifier::Bad_File_Content, Log_Level::Error,
            //                 "Binary representation can be either \"binary 8\" or \"binary 4\"");
        }
        
        // Read the data
        if( this->datatype_in == "binary" )
            read_data_bin( vf );
        else if( this->datatype_in == "text" )
            read_data_txt( vf );
        else if( this->datatype_in == "csv" )
            read_data_txt( vf, "," );
    }
    catch (...) 
    {
        // spirit_rethrow( fmt::format("Failed to read OVF file \"{}\".", filename) );
    }
}


template <typename T>
void OVF_File::read_data_bin( T * vf )
{
    try
    {        
        // Set the input stream indicator to the end of the line describing the data block
        ifile->iss.seekg( std::ios::end );
        
        // Check if the initial check value of the binary data is valid
        // if( !check_binary_values() )
        //     spirit_throw( Exception_Classifier::Bad_File_Content, Log_Level::Error,
        //                     "The OVF initial binary value could not be read correctly");
        
        // Comparison of datum size compared to scalar type
        if ( this->binary_length == 4 )
        {
            int vectorsize = 3 * sizeof(float);
            float buffer[3];
            int index;
            for( int k=0; k<this->nodes[2]; k++ )
            {
                for( int j=0; j<this->nodes[1]; j++ )
                {
                    for( int i=0; i<this->nodes[0]; i++ )
                    {
                        index = i + j*this->nodes[0] + k*this->nodes[0]*this->nodes[1];
                        
                        ifile->myfile->read(reinterpret_cast<char *>(&buffer[0]), vectorsize);
                        
                        vf[3*index + 0] = static_cast<T>(buffer[0]);
                        vf[3*index + 1] = static_cast<T>(buffer[1]);
                        vf[3*index + 2] = static_cast<T>(buffer[2]);
                    }
                }
            }
            
            // normalize read in spins 
            // normalize_vectors( vf, nos );
        }
        else if (this->binary_length == 8)
        {
            int vectorsize = 3 * sizeof(double);
            double buffer[3];
            int index;
            for (int k = 0; k<this->nodes[2]; k++)
            {
                for (int j = 0; j<this->nodes[1]; j++)
                {
                    for (int i = 0; i<this->nodes[0]; i++)
                    {
                        index = i + j*this->nodes[0] + k*this->nodes[0] * this->nodes[1];
                        
                        ifile->myfile->read(reinterpret_cast<char *>(&buffer[0]), vectorsize);
                        
                        vf[3*index + 0] = static_cast<T>(buffer[0]);
                        vf[3*index + 1] = static_cast<T>(buffer[1]);
                        vf[3*index + 2] = static_cast<T>(buffer[2]);
                    }
                }
            }
            
            // normalize read in spins 
            // normalize_vectors( vf, nos );
        }
    }
    catch (...)
    {
        // spirit_rethrow( "Failed to read OVF binary data" );
    }
}

template <typename T>
void OVF_File::read_data_txt( T * vf, const std::string& delimiter )
{
    try
    { 
        int nos = this->nodes[0] * this->nodes[1] * this->nodes[2];

        for (int i=0; i<nos; i++)
        {
            this->ifile->GetLine( delimiter );

            this->ifile->iss >> vf[3*i + 0];
            this->ifile->iss >> vf[3*i + 1];
            this->ifile->iss >> vf[3*i + 2];
        }
    }
    catch (...)
    {
        // spirit_rethrow( "Failed to check OVF initial binary value" );
    }
}


template <typename T>
void OVF_File::write_segment( const T * vf, const ovf_segment * segment,
                                const std::string comment, const bool append, int format )
{
    try
    {
        this->output_to_file.reserve( int( 0x08000000 ) );  // reserve 128[MByte]

        // If we are not appending or the file does not exists we need to write the top header
        // and to turn the file_exists attribute to true so we can append more segments
        if ( !append || !this->file_exists )
        {
            write_top_header();
            read_n_segments_from_top_header();  // finds the file position of n_segments
            this->file_exists = true;
            this->isOVF  = true;
        }

        this->output_to_file += fmt::format( empty_line );
        this->output_to_file += fmt::format( "# Begin: Segment\n" );
        this->output_to_file += fmt::format( "# Begin: Header\n" );
        this->output_to_file += fmt::format( empty_line );

        this->output_to_file += fmt::format( "# Title:\n");
        this->output_to_file += fmt::format( empty_line );

        this->output_to_file += fmt::format( "# Desc: {}\n", comment );
        this->output_to_file += fmt::format( empty_line );

        // The value dimension is always 3 since we are writting Vector3-data
        this->output_to_file += fmt::format( "# valuedim: {} ##Value dimension\n", 3 );
        this->output_to_file += fmt::format( "# valueunits: None None None\n" );
        this->output_to_file +=
            fmt::format("# valuelabels: spin_x_component spin_y_component "
                        "spin_z_component \n");
        this->output_to_file += fmt::format( empty_line );

        this->output_to_file += fmt::format( "## Fundamental mesh measurement unit. "
                                                "Treated as a label:\n" );
        this->output_to_file += fmt::format( "# meshunit: unspecified\n" );
        this->output_to_file += fmt::format( empty_line );

        this->output_to_file += fmt::format( "# xmin: {}\n", segment->bounds_min[0] );
        this->output_to_file += fmt::format( "# ymin: {}\n", segment->bounds_min[1] );
        this->output_to_file += fmt::format( "# zmin: {}\n", segment->bounds_min[2] );
        this->output_to_file += fmt::format( "# xmax: {}\n", segment->bounds_max[0] );
        this->output_to_file += fmt::format( "# ymax: {}\n", segment->bounds_max[1] );
        this->output_to_file += fmt::format( "# zmax: {}\n", segment->bounds_max[2] );
        this->output_to_file += fmt::format( empty_line );

        // TODO: Spirit does not support irregular geometry yet. Write ONLY rectangular mesh
        this->output_to_file += fmt::format( "# meshtype: rectangular\n" );

        // Bravais Lattice
        this->output_to_file += fmt::format( "# xbase: {} {} {}\n", 
                                                segment->bravais_vectors[0][0], 
                                                segment->bravais_vectors[0][1],
                                                segment->bravais_vectors[0][2] );
        this->output_to_file += fmt::format( "# ybase: {} {} {}\n",
                                                segment->bravais_vectors[1][0], 
                                                segment->bravais_vectors[1][1],
                                                segment->bravais_vectors[1][2] );
        this->output_to_file += fmt::format( "# zbase: {} {} {}\n",
                                                segment->bravais_vectors[2][0], 
                                                segment->bravais_vectors[2][1],
                                                segment->bravais_vectors[2][2] );

        this->output_to_file += fmt::format( "# xstepsize: {}\n", 
                                    segment->lattice_constant * segment->bravais_vectors[0][0] );
        this->output_to_file += fmt::format( "# ystepsize: {}\n", 
                                    segment->lattice_constant * segment->bravais_vectors[1][1] );
        this->output_to_file += fmt::format( "# zstepsize: {}\n", 
                                    segment->lattice_constant * segment->bravais_vectors[2][2] );

        this->output_to_file += fmt::format( "# xnodes: {}\n", segment->n_cells[0] );
        this->output_to_file += fmt::format( "# ynodes: {}\n", segment->n_cells[1] );
        this->output_to_file += fmt::format( "# znodes: {}\n", segment->n_cells[2] );
        this->output_to_file += fmt::format( empty_line );

        this->output_to_file += fmt::format( "# End: Header\n" );
        this->output_to_file += fmt::format( empty_line );

        std::string datatype_out = "";
        if ( format == OVF_FORMAT_BIN || format == OVF_FORMAT_BIN8 )
            datatype_out = "Binary 8";
        else if ( format == OVF_FORMAT_BIN4 ) 
            datatype_out = "Binary 4";
        else if( format == OVF_FORMAT_TEXT ) 
            datatype_out = "Text";
        else if( format == OVF_FORMAT_CSV ) 
            datatype_out = "CSV";

        // Data
        this->output_to_file += fmt::format( "# Begin: Data {}\n", datatype_out );

        int size = segment->n_cells[0]*segment->n_cells[1]*segment->n_cells[2];
        
        if ( format == OVF_FORMAT_BIN || format == OVF_FORMAT_BIN8 || format == OVF_FORMAT_BIN4 )
            write_data_bin( vf, size );
        else if ( format == OVF_FORMAT_TEXT )
            write_data_txt( vf, size );
        else if ( format == OVF_FORMAT_CSV )
            write_data_txt( vf, size, "," );

        this->output_to_file += fmt::format( "# End: Data {}\n", datatype_out );
        this->output_to_file += fmt::format( "# End: Segment\n" );

        // Append the #End keywords
        Append_String_to_File( this->output_to_file, this->filename );

        // reset output string buffer
        this->output_to_file = "";  

        // Increment the n_segments after succesfully appending the segment body to the file
        increment_n_segments();
    }
    catch( ... )
    {
        // spirit_rethrow( fmt::format("Failed to write OVF file \"{}\".", this->filename) );
    }
}

template <typename T>
void OVF_File::write_data_bin( const T * vf, int size )
{
    // float test value
    const float ref_4b = *reinterpret_cast<const float *>( &test_hex_4b );
    
    // double test value
    const double ref_8b = *reinterpret_cast<const double *>( &test_hex_8b );
    
    if( true )//format == VF_FileFormat::OVF_BIN8 )
    {
        this->output_to_file += std::string( reinterpret_cast<const char *>(&ref_8b),
            sizeof(double) );
        
        // in case that scalar is 4bytes long
        if (sizeof(double) == sizeof(float))
        {
            double buffer[3];
            for (unsigned int i=0; i<size; i++)
            {
                buffer[0] = static_cast<double>(vf[3*i + 0]);
                buffer[1] = static_cast<double>(vf[3*i + 1]);
                buffer[2] = static_cast<double>(vf[3*i + 2]);
                this->output_to_file += std::string( reinterpret_cast<char *>(buffer), 
                    sizeof(buffer) );
            }
        } 
        else
        {
            for (unsigned int i=0; i<size; i++)
                this->output_to_file += 
                    std::string( reinterpret_cast<const char *>(&vf[i]), 3*sizeof(double) );
        }
    }
    else if( true )//format == VF_FileFormat::OVF_BIN4 )
    {
        this->output_to_file += std::string( reinterpret_cast<const char *>(&ref_4b),
            sizeof(float) );
        
        // in case that scalar is 8bytes long
        if (sizeof(double) == sizeof(double))
        {
            float buffer[3];
            for (unsigned int i=0; i<size; i++)
            {
                buffer[0] = static_cast<float>(vf[3*i + 0]);
                buffer[1] = static_cast<float>(vf[3*i + 1]);
                buffer[2] = static_cast<float>(vf[3*i + 2]);
                this->output_to_file += std::string( reinterpret_cast<char *>(buffer), 
                    sizeof(buffer) );
            }
        } 
        else
        {
            for (unsigned int i=0; i<size; i++)
                this->output_to_file += 
                    std::string( reinterpret_cast<const char *>(&vf[i]), 3*sizeof(float) );
        }
    }
}

template <typename T>
void OVF_File::write_data_txt( const T * vf, int size, const std::string& delimiter )
{
    for (int iatom = 0; iatom < size; ++iatom)
    {
        this->output_to_file += fmt::format( "{:22.12f}{} {:22.12f}{} {:22.12f}{}\n", 
                                                vf[3*iatom + 0], delimiter, 
                                                vf[3*iatom + 1], delimiter,
                                                vf[3*iatom + 2], delimiter );
    }
}

#endif