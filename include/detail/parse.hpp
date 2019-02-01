#pragma once
#ifndef PARSE_H
#define PARSE_H

#include "ovf.h"
#include "Filter_File_Handle.hpp"

#include <string>
#include <vector>
#include <array>
#include <fstream>

struct parser_state
{
    // For the segment strings
    std::vector<std::string> file_contents{};

    // for reading data blocks
    int current_column = 0;
    int current_line = 0;

    /*
    messages, e.g. in case a function returned OVF_ERROR.
    message_out will be filled and returned by ovf_latest_message, while message_latest
    will be filled by other functions and cleared by ovf_latest_message.
    */
    std::string message_out="", message_latest="";

    int tmp_idx=0;
    std::array<double, 3> tmp_vec3 = std::array<double, 3>{0,0,0};
};

namespace parse
{
    // Test values for 4bit and 8bit binary data
    static const uint32_t test_hex_4b = 0x4996B438;
    static const uint64_t test_hex_8b = 0x42DC12218377DE40;

    /*
    The number of zero-padding for segment count.
    This is needed so that, when appending, the file does not need to be overwritten.
    6: can store 1M segments
    */
    static const int n_segments_str_digits = 6;

    /*
    Read the overall file header and locate and count segments in the file
    (the segments are stored in memory!)
    */
    int initial(ovf_file & file);

    //
    int get_segments(ovf_file & file);

    // Reads in the header info into a given segment
    int read_segment_header(ovf_file & file, int index, ovf_segment & segment);

    // Reads the data of a segment into a given data array (float)
    int read_segment_data(ovf_file & file, int index, const ovf_segment & segment, float * data);

    // Reads the data of a segment into a given data array (double)
    int read_segment_data(ovf_file & file, int index, const ovf_segment & segment, double * data);
}

#endif