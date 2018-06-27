#include "detail/OVF_File.hpp"
#include <iostream>

OVF_File::OVF_File( std::string filename, int format ) : 
    error_message(""), filename(filename)//, format(format)
{
    this->isOVF = false;
    this->output_to_file = "";
    this->output_to_file.reserve( int( 0x08000000 ) );  // reserve 128[MByte]
    this->n_segments = -1;

    this->ifile = NULL;
    this->n_segments = 0;
    this->n_segments_pos = 0;
    this->n_segments_as_str = "";
    this->version = "";
    this->title = "";
    this->meshunit = "";
    this->meshtype = "";
    this->valueunits = "";
    this->datatype_in = "";
    this->max = std::array<double,3>{0,0,0};
    this->min = std::array<double,3>{0,0,0};
    this->pointcount = -1;
    this->base = { std::array<double,3>{0,0,0}, std::array<double,3>{0,0,0}, std::array<double,3>{0,0,0} };
    this->stepsize = std::array<double,3>{0,0,0};

    // check if the file exists
    std::fstream file( filename );
    this->file_exists = file.is_open();
    file.close();

    // if the file exists check the version
    if ( this->file_exists ) check_version();

    // If the file has the OVF header get the number and the positions of the segments
    if ( this->isOVF )
    {
        read_n_segments_from_top_header();

        int n_seg = count_and_locate_segments();
        
        // Compare with n_segments in top header
        // if( this->n_segments != n_seg )
        //     spirit_throw( Utility::Exception_Classifier::Bad_File_Content, 
        //                     Utility::Log_Level::Error, fmt::format( "OVF Segment number "
        //                     "in header ({0}) is different from the number of segments "
        //                     "({1}) in file", this->n_segments, n_seg ) );
    }
}

void OVF_File::check_version()
{

    this->ifile = std::unique_ptr<Filter_File_Handle>( 
                        new Filter_File_Handle( this->filename, comment_tag ) ); 
    
    // Check if the file has an OVF top header
    if ( this->ifile->Read_Single( this->version, "# OOMMF OVF", false ) )
    {
        // Check the OVF version
        if( this->version != "2.0" && this->version != "2" )
        {
            // spirit_throw( Utility::Exception_Classifier::Bad_File_Content, 
            //                 Utility::Log_Level::Error,
            //                 fmt::format( "OVF {0} is not supported", this->version ) );
        } 
        this->isOVF = true;
    }
    else
    {
        this->isOVF = false;
    }
    
    this->ifile = NULL;
}

void OVF_File::read_header()
{
    try
    {
        ifile->Read_String( this->title, "# Title:" );
        ifile->Read_Single( this->meshunit, "# meshunit:" );
        ifile->Require_Single( this->valuedim, "# valuedim:" );
        ifile->Read_String( this->valueunits, "# valueunits:" );
        ifile->Read_String( this->valueunits, "# valuelabels:" );
        
        ifile->Read_Single( this->min[0], "# xmin:" );
        ifile->Read_Single( this->min[1], "# ymin:" );
        ifile->Read_Single( this->min[2], "# zmin:" );
        ifile->Read_Single( this->max[0], "# xmax:" );
        ifile->Read_Single( this->max[1], "# ymax:" );
        ifile->Read_Single( this->max[2], "# zmax:" );
        
        ifile->Require_Single( this->meshtype, "# meshtype:" );
        
        if( this->meshtype != "rectangular" && this->meshtype != "irregular" )
        {
            // spirit_throw(Utility::Exception_Classifier::Bad_File_Content, Utility::Log_Level::Error,
            //     "Mesh type must be either \"rectangular\" or \"irregular\"");
        }
        
        // Emit Header to Log
        // auto lvl = Log_Level::Debug;
        // Log( lvl, this->sender, fmt::format( "# OVF title               = {}", this->title ) );
        // Log( lvl, this->sender, fmt::format( "# OVF values dimensions   = {}", this->valuedim ) );
        // Log( lvl, this->sender, fmt::format( "# OVF meshunit            = {}", this->meshunit ) );
        // Log( lvl, this->sender, fmt::format( "# OVF xmin                = {}", this->min[0] ) );
        // Log( lvl, this->sender, fmt::format( "# OVF ymin                = {}", this->min[1] ) );
        // Log( lvl, this->sender, fmt::format( "# OVF zmin                = {}", this->min[2] ) );
        // Log( lvl, this->sender, fmt::format( "# OVF xmax                = {}", this->max[0] ) );
        // Log( lvl, this->sender, fmt::format( "# OVF ymax                = {}", this->max[1] ) );
        // Log( lvl, this->sender, fmt::format( "# OVF zmax                = {}", this->max[2] ) );
        
        // For different mesh types
        if( this->meshtype == "rectangular" )
        {
            ifile->Read_3Vector( this->base[0], "# xbase:", true );
            ifile->Read_3Vector( this->base[1], "# ybase:", true );
            ifile->Read_3Vector( this->base[2], "# zbase:", true );
            
            ifile->Require_Single( this->stepsize[0], "# xstepsize:" );
            ifile->Require_Single( this->stepsize[1], "# ystepsize:" );
            ifile->Require_Single( this->stepsize[2], "# zstepsize:" );
            
            ifile->Require_Single( this->nodes[0], "# xnodes:" );
            ifile->Require_Single( this->nodes[1], "# ynodes:" );
            ifile->Require_Single( this->nodes[2], "# znodes:" );
            
            // Write to Log
            // Log( lvl, this->sender, fmt::format( "# OVF meshtype <{}>", this->meshtype ) );
            // Log( lvl, this->sender, fmt::format( "# xbase      = {:.8}", this->base[0] ) );
            // Log( lvl, this->sender, fmt::format( "# ybase      = {:.8}", this->base[1] ) );
            // Log( lvl, this->sender, fmt::format( "# zbase      = {:.8}", this->base[2] ) );
            // Log( lvl, this->sender, fmt::format( "# xstepsize  = {:.8f}", this->stepsize[0] ) );
            // Log( lvl, this->sender, fmt::format( "# ystepsize  = {:.8f}", this->stepsize[1] ) );
            // Log( lvl, this->sender, fmt::format( "# zstepsize  = {:.8f}", this->stepsize[2] ) );
            // Log( lvl, this->sender, fmt::format( "# xnodes     = {}", this->nodes[0] ) );
            // Log( lvl, this->sender, fmt::format( "# ynodes     = {}", this->nodes[1] ) );
            // Log( lvl, this->sender, fmt::format( "# znodes     = {}", this->nodes[2] ) );
        }
        
        // Check mesh type
        if ( this->meshtype == "irregular" )
        {
            ifile->Require_Single( this->pointcount, "# pointcount:" );
            
            // Write to Log
            // Log( lvl, this->sender, fmt::format( "# OVF meshtype <{}>", this->meshtype ) );
            // Log( lvl, this->sender, fmt::format( "# OVF point count = {}", this->pointcount ) );
        }
    }
    catch (...) 
    {
        // spirit_rethrow( fmt::format("Failed to read OVF file \"{}\".", this->filename) );
    }
}
    
void OVF_File::check_geometry( const ovf_segment * segment )
{
    try
    {
        // Check that nos is smaller or equal to the nos of the current image
        int nos = this->nodes[0] * this->nodes[1] * this->nodes[2];
        // if ( nos > geometry->nos )
        //     spirit_throw(Utility::Exception_Classifier::Bad_File_Content, 
        //         Utility::Log_Level::Error,"NOS of the OVF file is greater than the NOS in the "
        //         "current image");
        
        // Check if the geometry of the ovf file is the same with the one of the current image
        if( this->nodes[0] != segment->n_cells[0] ||
            this->nodes[1] != segment->n_cells[1] ||
            this->nodes[2] != segment->n_cells[2] )
        {
            // Log(Log_Level::Warning, this->sender, fmt::format("The geometry of the OVF file "
            //     "does not much the geometry of the current image") );
        }
    }
    catch (...) 
    {
        // spirit_rethrow( fmt::format("Failed to read OVF file \"{}\".", this->filename) );
    }
}



bool OVF_File::check_binary_values()
{
    try
    {
        // create initial check values for the binary data (see OVF specification)
        const double ref_8b = *reinterpret_cast<const double *>( &test_hex_8b );
        double read_8byte = 0;
        
        const float ref_4b = *reinterpret_cast<const float *>( &test_hex_4b );
        float read_4byte = 0;
        
        // check the validity of the initial check value read with the reference one
        if ( this->binary_length == 4 )
        {    
            ifile->myfile->read( reinterpret_cast<char *>( &read_4byte ), sizeof(float) );
            if ( read_4byte != ref_4b ) 
            {
                // spirit_throw( Exception_Classifier::Bad_File_Content, Log_Level::Error,
                //                 "OVF initial check value of binary data is inconsistent" );
            }
        }
        else if ( this->binary_length == 8 )
        {
            ifile->myfile->read( reinterpret_cast<char *>( &read_8byte ), sizeof(double) );
            if ( read_8byte != ref_8b )
            {
                // spirit_throw( Exception_Classifier::Bad_File_Content, Log_Level::Error,
                //                 "OVF initial check value of binary data is inconsistent" );
            }
        }
        
        return true;
    }
    catch (...)
    {
        // spirit_rethrow( "Failed to check OVF initial binary value" );
        return false;
    }


}

void OVF_File::write_top_header()
{
    this->output_to_file += fmt::format( "# OOMMF OVF 2.0\n" );
    this->output_to_file += fmt::format( empty_line );
    
    // initialize n_segments to zero
    this->n_segments = 0;
    // convert n_segments to string
    std::string n_segments_str = std::to_string( this->n_segments );
    // calculate padding's length
    int padding_length = n_segments_str_digits - n_segments_str.length(); 
    // create padding string 
    std::string padding( padding_length, '0' );
    // write padding plus n_segments
    this->output_to_file += fmt::format( "# Segment count: {}\n", padding + n_segments_str );
    
    Strings_to_File( {this->output_to_file}, this->filename );
    this->output_to_file = "";  // reset output string buffer
}


void OVF_File::increment_n_segments()
{
    try
    {
        std::fstream file( this->filename ); 
    
        // update n_segments
        this->n_segments++;
        
        // convert updated n_segment into padded string
        std::string new_n_str = std::to_string( this->n_segments );
        std::string::size_type new_n_len = new_n_str.length();

        std::string::size_type padding_len = n_segments_str_digits - new_n_len;
        std::string padding( padding_len, '0' ); 

        // n_segments_pos is the end of the line that contains '#segment count' (after '\n')
        std::ios::off_type offset = n_segments_str_digits + 1;
        
        // go to the beginning '#segment count' value position
        file.seekg( this->n_segments_pos );
        file.seekg( (-1)*offset, std::ios::cur );

        // replace n_segments value in the stream
        file << ( padding + new_n_str );

        file.close();
    }
    catch( ... )
    {
        // spirit_rethrow( fmt::format("Failed to increment n_segments in OVF file \"{}\".", 
        //                 this->filename) );
    }
}

void OVF_File::read_n_segments_from_top_header()
{
    try
    {
        this->ifile = std::unique_ptr<Filter_File_Handle>( 
                            new Filter_File_Handle( this->filename, comment_tag ) ); 
        
        // get the number of segments from the initial keyword
        ifile->Require_Single( this->n_segments, "# segment count:" ); 

        // get the number of segment as string
        ifile->Read_String( this->n_segments_as_str, "# segment count:" );

        // save the file position indicator in case we have to increment n_segment
        this->n_segments_pos = this->ifile->GetPosition();

        // TODO: what will happen if the n_segments does not have padding?
        
        // close the file
        this->ifile = NULL;
    }
    catch( ... )
    {
        // spirit_rethrow( fmt::format("Failed to read OVF file \"{}\".", this->filename) );
    }

}

int OVF_File::count_and_locate_segments()
{
    try
    {
        this->ifile = std::unique_ptr<Filter_File_Handle>( 
                            new Filter_File_Handle( this->filename, comment_tag ) ); 

        // get the number of segments from the occurrences of "# Begin: Segment"
        int n_begin_segment = 0;
        
        std::ios::pos_type end = this->ifile->GetPosition( std::ios::end ); 
        
        // NOTE: the keyword to find must be lower case since the Filter File Handle 
        // converts the content of the input file to lower case automatically
        while( ifile->Find( "# begin: segment" ) )
        {
            std::ios::pos_type pos = this->ifile->GetPosition(); 
            this->segment_fpos.push_back( pos );
            ifile->SetLimits( pos, end );

            ++n_begin_segment;
        }
        
        // find the very last keyword of the file
        this->segment_fpos.push_back( end );

        // reset limits
        ifile->ResetLimits();
        
        // close the file
        this->ifile = NULL;

        return n_begin_segment;
    }
    catch( ... )
    {
        // spirit_rethrow( fmt::format("Failed to read OVF file \"{}\".", this->filename) );
        return 0;
    }
}

// Public methods ------------------------------------------------------------------------------

bool OVF_File::exists()
{
    return this->file_exists;
}

bool OVF_File::is_OVF()
{
    return this->isOVF;
}

int OVF_File::get_n_segments()
{
    return this->n_segments;
}


void OVF_File::read_segment_header( ovf_segment * segment, int idx_seg )
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

        segment->n_cells[0] = this->nodes[0];
        segment->n_cells[1] = this->nodes[1];
        segment->n_cells[2] = this->nodes[2];

        segment->valuedim = this->valuedim;

        segment->title = new char[this->title.length() + 1];
        strcpy(segment->title, this->title.c_str());
        segment->meshunits = new char[this->meshunit.length() + 1];
        strcpy(segment->meshunits, this->meshunit.c_str());
        segment->valueunits = new char[this->valueunits.length() + 1];
        strcpy(segment->valueunits, this->valueunits.c_str());

        // close the file
        this->ifile = NULL;
    }
}
catch( ... )
{

}