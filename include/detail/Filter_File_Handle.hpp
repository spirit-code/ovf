#pragma once
#ifndef IO_FILTERFILEHANDLE_H
#define IO_FILTERFILEHANDLE_H

#include <memory>
#include <string>
#include <istream>
#include <fstream>
#include <sstream>
#include <algorithm>

#include <fmt/format.h>
#include <fmt/ostream.h>

class Filter_File_Handle
{
private:
    std::size_t found;
    std::string line;
    const std::string comment_tag;
    std::string dump;
    // Beggining and end of file stream indicator 
    std::ios::pos_type position_file_beg;
    std::ios::pos_type position_file_end;
    // Start and stop of file stream indicator
    std::ios::pos_type position_start;
    std::ios::pos_type position_stop;
    int n_lines;
    int n_comment_lines;
    std::ifstream myfile;

public:
    std::string filename;
    std::istringstream iss;
    
    // Constructs a Filter_File_Handle with string filename
    Filter_File_Handle( const std::string& filename, const std::string comment_tag = "#" );
    // Destructor
    ~Filter_File_Handle();
    
    // Get the position of the file stream indicator
    std::ios::pos_type GetPosition( std::ios::seekdir dir = std::ios::cur );
    // Set limits in the file stream indicator
    void SetLimits( const std::ios::pos_type beg, const std::ios::pos_type end );
    // Reset the limits of the file stream indicator 
    void ResetLimits();
    // Reads next line of file into the handle (false -> end-of-file)
    bool GetLine_Handle( const std::string str_to_remove = "" );
    // Reads the next line of file into the handle and into the iss
    bool GetLine( const std::string str_to_remove = "" );
    // Reset the file stream to the start of the file
    void ResetStream();
    // Tries to find s in the current file and if found outputs the line into internal iss
    bool Find(const std::string& s);
    // Tries to find s in the current line and if found outputs into internal iss
    bool Find_in_Line(const std::string & s);
    // Removes a set of chars from a string
    void Remove_Chars_From_String(std::string &str, const char* charsToRemove);
    // Removes comments from a string
    bool Remove_Comments_From_String( std::string &str );
    // Read a string (separeated by whitespaces) into var. Capitalization is ignored.
    void Read_String( std::string& var, std::string keyword, bool log_notfound = true );
    // Count the words of a string
    int Count_Words( const std::string& str );
    // Returns the number of lines which are not starting with a comment
    int Get_N_Non_Comment_Lines();

    template <typename T>
    void read( T * var, std::size_t n )
    {
        myfile.read( var, n );
    };

    // Reads a single variable into var, with optional logging in case of failure.
    //
    //// NOTE: Capitalization is ignored (expected).
    //
    template <typename T>
    bool Read_Single( T & var, std::string name, bool log_notfound = true )
    {
        try
        {
            std::transform( name.begin(), name.end(), name.begin(), ::tolower );

            if (Find(name))
            {
                iss >> var;
                return true;
            }
            // else if (log_notfound)
            //     Log( Utility::Log_Level::Warning, Utility::Log_Sender::IO, "Keyword '" + name + 
            //         "' not found. Using Default: " + fmt::format( "{}", var ) );
        }
        catch (...)
        {
            // spirit_handle_exception_core(fmt::format("Failed to read single variable \"{}\".", name));
        }
        return false;
    };

    // Require a single field. In case that it is not found an execption is thrown. 
    //
    //// NOTE: Capitalization is ignored (expected).
    //
    template <typename T>
    void Require_Single( T& var, std::string name )
    {
        std::transform( name.begin(), name.end(), name.begin(), ::tolower );
        
        if( !Read_Single( var, name, false ) )
        {
            // spirit_throw(Utility::Exception_Classifier::Bad_File_Content, Utility::Log_Level::Error,
            //     fmt::format("Required keyword \"{}\" not found.", name));
        }
    }

    // Reads a 3-component object, with optional logging in case of failure
    template <typename T>
    void Read_3Vector( T & var, std::string name, bool log_notfound = true )
    {
        try
        {
            std::transform( name.begin(), name.end(), name.begin(), ::tolower );
            
            if (Find(name))
                iss >> var[0] >> var[1] >> var[2];
            // else if (log_notfound)
            //     Log( Utility::Log_Level::Warning, Utility::Log_Sender::IO, "Keyword '" + name + 
            //         "' not found. Using Default: (" + fmt::format( "{}", var[0] ) + " " + 
            //         fmt::format( "{}", var[1] ) + " " + fmt::format( "{}", var[2] ) + ")" );

        }
        catch (...)
        {
            // spirit_handle_exception_core(fmt::format("Failed to read 3Vector \"{}\".", name));
        }
    };
};//end class FilterFileHandle

#endif