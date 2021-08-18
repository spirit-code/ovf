#pragma once
#ifndef LIBOVF_DETAIL_PEGTL_DEFINES_H
#define LIBOVF_DETAIL_PEGTL_DEFINES_H

#include <array>
#include <vector>
#include <string>
#include <ios>

struct parser_state
{
    // For the segment strings
    std::vector<std::string> file_contents{};

    // for reading data blocks
    int current_column = 0;
    int current_line = 0;

    std::string keyword="", value="";

    // Whether certain keywords were found in parsing
    bool found_title        = false;
    bool found_meshunit     = false;
    bool found_valuedim     = false;
    bool found_valueunits   = false;
    bool found_valuelabels  = false;
    bool found_xmin         = false;
    bool found_ymin         = false;
    bool found_zmin         = false;
    bool found_xmax         = false;
    bool found_ymax         = false;
    bool found_zmax         = false;
    bool found_meshtype     = false;
    bool found_xbase        = false;
    bool found_ybase        = false;
    bool found_zbase        = false;
    bool found_xstepsize    = false;
    bool found_ystepsize    = false;
    bool found_zstepsize    = false;
    bool found_xnodes       = false;
    bool found_ynodes       = false;
    bool found_znodes       = false;
    bool found_pointcount   = false;

    /*
    messages, e.g. in case a function returned OVF_ERROR.
    message_out will be filled and returned by ovf_latest_message, while message_latest
    will be filled by other functions and cleared by ovf_latest_message.
    */
    std::string message_out="", message_latest="";

    int max_data_index=0;
    int tmp_idx=0;
    std::array<double, 3> tmp_vec3 = std::array<double, 3>{0,0,0};

    std::ios::pos_type n_segments_pos = 0;
};

#endif