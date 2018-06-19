#pragma once
#ifndef LIBOVF_H
#define LIBOVF_H

// Platform-specific definition of DLLEXPORT
#ifdef _WIN32
    #ifdef __cplusplus
        #define DLLEXPORT extern "C" __declspec(dllexport)
    #else
        #define DLLEXPORT __declspec(dllexport)
    #endif
#else
    #ifdef __cplusplus
        #define DLLEXPORT extern "C"
    #else
        #define DLLEXPORT
    #endif
#endif


/* return codes */
#define OVF_OK          -1
#define OVF_ERROR       -2
#define OVF_INVALID     -3

/* the geometrical information on the vector field */
struct ovf_geometry {
    int n_cells[3];
    int N;

    float bounds_min[3];
    float bounds_max[3];

    float lattice_constant;
    float bravais_vectors[3][3];

    /* then some "private" internal fields */
};

/* all header info on a segment */
struct ovf_segment {
    char *title;
    char *meshunits;
    int valuedim;
    char *valueunits;
    char *valuelabels;
    char *meshtype;

    /* then some "private" internal fields */
};

/* opaque handle which holds the file pointer */
struct ovf_file_handle;

/* the main struct which keeps the info on the main header of a file */
struct ovf_file {
    /* file could be found */
    bool found;
    /* file contains an ovf header */
    bool is_ovf;
    /* number of segments the file should contain */
    int n_segments;

    /* then some "private" internal fields */
    struct ovf_file_handle *_file_handle;
};

/* opening a file will fill the struct and prepare everything for read/write */
DLLEXPORT struct ovf_file * ovf_open(const char *filename);
/* reading a segment will write the data, if present, into the buffer.
    The ordering of the data will depend on the specification of the geometry.
    It will also check the file against the expected geometry.
    The ovf_file will be filled with the information from the segment header. */
DLLEXPORT int  ovf_read_segment_4(struct ovf_file *, int index, const struct ovf_geometry *expected, struct ovf_segment *segment, float *data);
DLLEXPORT int  ovf_read_segment_8(struct ovf_file *, int index, const struct ovf_geometry *expected, struct ovf_segment *segment, double *data);
/* write a segment to the file, overwriting all contents. The new header will have segment count = 1 */
DLLEXPORT int  ovf_write_segment(struct ovf_file *, long codepoint);
/* append a segment to the file. The segment count will be incremented */
DLLEXPORT int  ovf_append_segment(struct ovf_file *, long codepoint);
/* close the file and clean up resources */
DLLEXPORT int ovf_close(struct ovf_file *);

#undef DLLEXPORT
#endif