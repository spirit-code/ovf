#include "ovf.h"
#include <detail/OVF_File.hpp>


struct ovf_file_handle
{
    OVF_File file;
};


struct ovf_file * ovf_open(const char *filename)
try
{
    // TODO: allow different file formats
    int format = 0;

    auto ovf_file_ptr = new ovf_file{};
    ovf_file_ptr->_file_handle = new ovf_file_handle{ OVF_File(filename, format) };

    auto& file = ovf_file_ptr->_file_handle->file;
    ovf_file_ptr->found      = file.exists();
    ovf_file_ptr->is_ovf     = file.is_OVF();
    ovf_file_ptr->n_segments = file.get_n_segments();

    return ovf_file_ptr;
}
catch ( ... )
{
    return nullptr;
}

int  ovf_read_segment_4(struct ovf_file *, int index, const struct ovf_geometry *expected, struct ovf_segment *segment, float *data)
try
{
    return OVF_OK;
}
catch ( ... )
{
    return OVF_ERROR;
}

int  ovf_read_segment_8(struct ovf_file *, int index, const struct ovf_geometry *expected, struct ovf_segment *segment, double *data)
try
{
    return OVF_OK;
}
catch ( ... )
{
    return OVF_ERROR;
}

int  ovf_write_segment(struct ovf_file *, long codepoint)
try
{
    return OVF_OK;
}
catch ( ... )
{
    return OVF_ERROR;
}

int  ovf_append_segment(struct ovf_file *, long codepoint)
try
{
    return OVF_OK;
}
catch ( ... )
{
    return OVF_ERROR;
}

int ovf_close(struct ovf_file *)
try
{
    return OVF_OK;
}
catch ( ... )
{
    return OVF_ERROR;
}