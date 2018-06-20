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


/* all header info on a segment */
struct ovf_segment {
    char *title;
    int valuedim;
    char *valueunits;
    char *valuelabels;

    /* the geometrical information on the vector field */
    char *meshtype;
    char *meshunits;

    int n_cells[3];
    int N;

    float bounds_min[3];
    float bounds_max[3];

    float lattice_constant;
    float bravais_vectors[3][3];

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

/* read the geometry info from a segment header */
DLLEXPORT int ovf_read_segment_header(struct ovf_file *, int index, struct ovf_segment *segment);

/* This function checks the segment in the file against the passed segment and,
    if the dimensions fit, will read the data into the passed array. */
DLLEXPORT int ovf_read_segment_data_4(struct ovf_file *, int index, const struct ovf_segment *segment, float *data);
// DLLEXPORT int ovf_read_segment_data_8(struct ovf_file *, int index, const struct ovf_segment *segment, double *data);

/* write a segment to the file, overwriting all contents. The new header will have segment count = 1 */
DLLEXPORT int ovf_write_segment(struct ovf_file *, long codepoint);
/* append a segment to the file. The segment count will be incremented */
DLLEXPORT int ovf_append_segment(struct ovf_file *, long codepoint);

/* close the file and clean up resources */
DLLEXPORT int ovf_close(struct ovf_file *);

#undef DLLEXPORT
#endif