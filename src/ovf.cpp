#include "ovf.h"
#include <detail/OVF_File.hpp>
#include <fmt/format.h>


struct ovf_file_handle
{
    /* messages, e.g. in case a function returned OVF_ERROR.
        message_out will be filled and returned by ovf_latest_message, while message_latest
        will be filled by other functions and cleared by ovf_latest_message. */
    std::string message_out, message_latest;
    /* the actual OVF file */
    OVF_File file;
};


struct ovf_file * ovf_open(const char *filename)
try
{
    // TODO: allow different file formats
    int format = 0;

    struct ovf_file * ovf_file_ptr = new ovf_file{ false, false, 0, nullptr };
    ovf_file_ptr->_file_handle = new ovf_file_handle{ "", "", OVF_File(filename, format) };

    OVF_File& file = ovf_file_ptr->_file_handle->file;
    ovf_file_ptr->found      = file.exists();
    ovf_file_ptr->is_ovf     = file.is_OVF();
    ovf_file_ptr->n_segments = file.get_n_segments();

    return ovf_file_ptr;
}
catch ( ... )
{
    return nullptr;
}

int ovf_read_segment_header(struct ovf_file *ovf_file_ptr, int index, struct ovf_segment *segment)
try
{
    OVF_File& file = ovf_file_ptr->_file_handle->file;

    if (!file.exists())
    {
        ovf_file_ptr->_file_handle->message_latest = "libovf ovf_read_segment_header: file does not exist...";
        return OVF_ERROR;
    }

    if (!file.is_OVF())
    {
        ovf_file_ptr->_file_handle->message_latest = "libovf ovf_read_segment_header: file is not ovf...";
        return OVF_ERROR;
    }

    if (index >= file.get_n_segments())
    {
        ovf_file_ptr->_file_handle->message_latest =
            fmt::format("libovf ovf_read_segment_header: index ({}) >= n_segments ({})...", index, file.get_n_segments());
        return OVF_ERROR;
    }

    file.read_segment_header( segment, index );

    return OVF_OK;
}
catch ( ... )
{
    return OVF_ERROR;
}

int ovf_read_segment_data_4(struct ovf_file *ovf_file_ptr, int index, const struct ovf_segment *segment, float *data)
try
{
    OVF_File& file = ovf_file_ptr->_file_handle->file;

    if (!file.exists())
    {
        ovf_file_ptr->_file_handle->message_latest = "libovf ovf_read_segment_data_4: file does not exist...";
        return OVF_ERROR;
    }

    if (!file.is_OVF())
    {
        ovf_file_ptr->_file_handle->message_latest = "libovf ovf_read_segment_data_4: file is not ovf...";
        return OVF_ERROR;
    }

    if (index >= file.get_n_segments())
    {
        ovf_file_ptr->_file_handle->message_latest =
            fmt::format("libovf ovf_read_segment_data_4: index ({}) >= n_segments ({})...", index, file.get_n_segments());
        return OVF_ERROR;
    }

    // TODO: parse binary length correctly
    // if (file.binary_length != 4)
    // {
    //     ovf_file_ptr->_file_handle->message_latest = "libovf ovf_read_segment_data_4: file does not have binary length 4...";
    //     return OVF_ERROR;
    // }

    file.read_segment(data, segment, index);

    return OVF_OK;
}
catch ( ... )
{
    return OVF_ERROR;
}

int ovf_read_segment_data_8(struct ovf_file *ovf_file_ptr, int index, const struct ovf_segment *segment, double *data)
try
{
    OVF_File& file = ovf_file_ptr->_file_handle->file;

    if (!file.exists())
    {
        ovf_file_ptr->_file_handle->message_latest = "libovf ovf_read_segment_8: file does not exist...";
        return OVF_ERROR;
    }

    if (!file.is_OVF())
    {
        ovf_file_ptr->_file_handle->message_latest = "libovf ovf_read_segment_8: file is not ovf...";
        return OVF_ERROR;
    }

    if (index >= file.get_n_segments())
    {
        ovf_file_ptr->_file_handle->message_latest =
            fmt::format("libovf ovf_read_segment_8: index ({}) >= n_segments ({})...", index, file.get_n_segments());
        return OVF_ERROR;
    }

    // TODO: parse binary length correctly
    // if (file.binary_length != 8)
    // {
    //     ovf_file_ptr->_file_handle->message_latest = "libovf ovf_read_segment_8: file does not have binary length 4...";
    //     return OVF_ERROR;
    // }

    file.read_segment(data, segment, index);

    return OVF_OK;
}
catch ( ... )
{
    return OVF_ERROR;
}

int ovf_write_segment_4(struct ovf_file *ovf_file_ptr, const struct ovf_segment *segment, float *data, int format)
try
{
    OVF_File& file = ovf_file_ptr->_file_handle->file;

    if (format != OVF_FORMAT_BIN && format != OVF_FORMAT_TEXT && format != OVF_FORMAT_CSV )
    {
        ovf_file_ptr->_file_handle->message_latest =
            fmt::format("libovf ovf_write_segment_4: invalid format \'{}\'...", format);
        return OVF_ERROR;
    }

    file.write_segment(data, segment, "libovf test comment", false, format);

    return OVF_OK;
}
catch ( ... )
{
    return OVF_ERROR;
}

int ovf_write_segment_8(struct ovf_file *ovf_file_ptr, const struct ovf_segment *segment, double *data, int format)
try
{
    OVF_File& file = ovf_file_ptr->_file_handle->file;

    if (format != OVF_FORMAT_BIN && format != OVF_FORMAT_TEXT && format != OVF_FORMAT_CSV )
    {
        ovf_file_ptr->_file_handle->message_latest =
            fmt::format("libovf ovf_write_segment_8: invalid format \'{}\'...", format);
        return OVF_ERROR;
    }

    file.write_segment(data, segment, "libovf test comment", false, format);

    return OVF_OK;
}
catch ( ... )
{
    return OVF_ERROR;
}

int ovf_append_segment_4(struct ovf_file *ovf_file_ptr, const struct ovf_segment *segment, float *data, int format)
try
{
    OVF_File& file = ovf_file_ptr->_file_handle->file;

    if (!file.exists())
    {
        ovf_file_ptr->_file_handle->message_latest = "libovf ovf_append_segment_4: file does not exist...";
        return OVF_ERROR;
    }

    if (!file.is_OVF())
    {
        ovf_file_ptr->_file_handle->message_latest = "libovf ovf_append_segment_4: file is not ovf...";
        return OVF_ERROR;
    }

    if (format != OVF_FORMAT_BIN && format != OVF_FORMAT_TEXT && format != OVF_FORMAT_CSV )
    {
        ovf_file_ptr->_file_handle->message_latest =
            fmt::format("libovf ovf_append_segment_4: invalid format \'{}\'...", format);
        return OVF_ERROR;
    }

    file.write_segment(data, segment, "libovf test comment", true, format);

    return OVF_OK;
}
catch ( ... )
{
    return OVF_ERROR;
}

int ovf_append_segment_8(struct ovf_file *ovf_file_ptr, const struct ovf_segment *segment, double *data, int format)
try
{
    OVF_File& file = ovf_file_ptr->_file_handle->file;

    if (!file.exists())
    {
        ovf_file_ptr->_file_handle->message_latest = "libovf ovf_append_segment_8: file does not exist...";
        return OVF_ERROR;
    }

    if (!file.is_OVF())
    {
        ovf_file_ptr->_file_handle->message_latest = "libovf ovf_append_segment_8: file is not ovf...";
        return OVF_ERROR;
    }

    if (format != OVF_FORMAT_BIN && format != OVF_FORMAT_TEXT && format != OVF_FORMAT_CSV )
    {
        ovf_file_ptr->_file_handle->message_latest =
            fmt::format("libovf ovf_append_segment_8: invalid format \'{}\'...", format);
        return OVF_ERROR;
    }

    file.write_segment(data, segment, "libovf test comment", true, format);

    return OVF_OK;
}
catch ( ... )
{
    return OVF_ERROR;
}

const char * ovf_latest_message(struct ovf_file *ovf_file_ptr)
try
{
    ovf_file_ptr->_file_handle->message_out = ovf_file_ptr->_file_handle->message_latest;
    ovf_file_ptr->_file_handle->message_latest = "";
    return ovf_file_ptr->_file_handle->message_out.c_str();
}
catch( ... )
{
    return "";
}

int ovf_close(struct ovf_file *ovf_file_ptr)
try
{
    delete(ovf_file_ptr->_file_handle);
    delete(ovf_file_ptr);
    return OVF_OK;
}
catch ( ... )
{
    return OVF_ERROR;
}