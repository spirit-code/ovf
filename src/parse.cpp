#include <detail/parse.hpp>
#include <detail/helpers.hpp>

#include <vector>
#include <array>
#include <iostream>

#include <fmt/format.h>
#include <fmt/ostream.h>

#include <tao/pegtl.hpp>

namespace pegtl = tao::pegtl;

namespace parse
{
    // "# "
    struct prefix
        : pegtl::string< '#' >
    {};

    // "#\eol"
    struct empty_line
        : pegtl::seq< pegtl::string< '#' >, pegtl::star<pegtl::blank> >
    {};

    //
    struct version_number
        : pegtl::range< '1', '2' >
    {};

    // " OOMMF OVF "
    struct version
        : pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("OOMMF OVF"), pegtl::blank >, version_number, pegtl::until<pegtl::eol> >
    {};

    // " Segment count: "
    struct segment_count_number
        : pegtl::plus<pegtl::digit>
    {};

    // " Segment count: "
    struct segment_count
        : pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("Segment count:"), pegtl::blank >, segment_count_number, pegtl::eol >
    {};

    //////////////////////////

    struct ovf_file_header
        : pegtl::must<
            version,
            pegtl::star<pegtl::seq<empty_line, pegtl::eol>>,
            segment_count>
    {};

    //////////////////////////

    // Class template for user-defined actions that does nothing by default.
    template< typename Rule >
    struct ovf_file_action
        : pegtl::nothing< Rule >
    {};

    template<>
    struct ovf_file_action< version_number >
    {
        template< typename Input >
        static void apply( const Input& in, ovf_file & file )
        {
            file.version = std::stoi(in.string());
        }
    };

    template<>
    struct ovf_file_action< segment_count_number >
    {
        template< typename Input >
        static void apply( const Input& in, ovf_file & file )
        {
            file.n_segments = std::stoi(in.string());
        }
    };


    //////////////////////////


    namespace v2
    {
        // "# "
        struct prefix
            : pegtl::string< '#' >
        {};

        // "#\eol"
        struct empty_line
            : pegtl::seq< pegtl::string< '#' >, pegtl::star<pegtl::blank> >
        {};

        // "## " initiates comment line
        struct comment
            : pegtl::seq< pegtl::string< '#', '#' >, pegtl::star<pegtl::any> >
        {};

        struct skippable_lines
            : pegtl::star< pegtl::seq<
                pegtl::sor< pegtl::plus<empty_line>, pegtl::plus<comment> >,
                pegtl::eol
                > >
        {};

        // " OOMMF OVF "
        struct version
            : pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("OOMMF OVF"), pegtl::blank >, pegtl::range< '1', '2' >, pegtl::until<pegtl::eol> >
        {};

        // " Segment count: "
        struct segment_count_number
            : pegtl::plus<pegtl::digit>
        {};

        // " Segment count: "
        struct segment_count
            : pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("Segment count:"), pegtl::blank >, segment_count_number, pegtl::eol >
        {};

        // " Begin: "
        struct begin
            : pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("Begin:"), pegtl::blank > >
        {};

        // " End: "
        struct end
            : pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("End:"), pegtl::blank > >
        {};

        //////////////////////////////////////////////

        struct opt_plus_minus
            : pegtl::opt< pegtl::one< '+', '-' > >
        {};

        struct inf
            : pegtl::seq<
                pegtl::istring< 'i', 'n', 'f' >,
                pegtl::opt< pegtl::istring< 'i', 'n', 'i', 't', 'y' > > >
        {};

        struct nan
            : pegtl::seq<
                pegtl::istring< 'n', 'a', 'n' >,
                pegtl::opt< pegtl::one< '(' >,
                            pegtl::plus< pegtl::alnum >,
                            pegtl::one< ')' > > >
        {};

        template< typename D >
        struct basic_number
            : pegtl::if_then_else<
                pegtl::one< '.' >,
                pegtl::plus< D >,
                pegtl::seq<
                    pegtl::plus< D >,
                    pegtl::opt< pegtl::one< '.' > >,
                    pegtl::star< D >
                >
            >
        {};

        struct exponent
            : pegtl::seq<
                opt_plus_minus,
                pegtl::plus< pegtl::digit > >
        {};

        struct decimal_number
            : pegtl::seq<
                basic_number< pegtl::digit >,
                pegtl::opt< pegtl::one< 'e', 'E' >, exponent > >
        {};

        struct hexadecimal_number // TODO: is this actually hexadecimal??
            : pegtl::seq<
                pegtl::one< '0' >,
                pegtl::one< 'x', 'X' >,
                basic_number< pegtl::xdigit >,
                pegtl::opt< pegtl::one< 'p', 'P' >, exponent > >
        {};

        struct float_value
            : pegtl::seq<
                opt_plus_minus,
                decimal_number >
        {};

        struct vec_float_value
            : pegtl::seq<
                opt_plus_minus,
                decimal_number >
        {};


        struct data_float
            : pegtl::seq<
                opt_plus_minus,
                decimal_number >
        {};

        struct segment_data_float
            : data_float
        {};
        struct line_data_txt
            : pegtl::plus< pegtl::pad< segment_data_float, pegtl::blank > >
        {};
        struct line_data_csv
            : pegtl::seq<
                pegtl::list< pegtl::pad<segment_data_float, pegtl::blank>, pegtl::one<','> >,
                pegtl::opt< pegtl::pad< pegtl::one<','>, pegtl::blank > >
                >
        {};

        struct bin_4_check_value
            : tao::pegtl::uint32_le::any
        {};
        struct bin_4_value
            : tao::pegtl::uint32_le::any
        {};

        struct bin_8_check_value
            : tao::pegtl::uint64_le::any
        {};
        struct bin_8_value
            : tao::pegtl::uint64_le::any
        {};

        //////////////////////////////////////////////

        // Vector3 of floating point values
        struct vector3f
            : pegtl::rep< 3, pegtl::pad< vec_float_value, pegtl::blank > >
        {};

        // This is how a line ends: either eol or the begin of a comment
        struct line_end
            : pegtl::sor<pegtl::eol, pegtl::string<'#','#'>>
        {};

        // This checks that the line end is met and moves up until eol
        struct finish_line
            : pegtl::seq<pegtl::at<line_end>, pegtl::until<pegtl::eol>>
        {};

        // Title
        struct title
            :  pegtl::until<pegtl::at< line_end >>
        {};

        // Description
        struct description
            : pegtl::until<pegtl::at< line_end >>
        {};

        // valuedim
        struct valuedim
            : pegtl::pad<pegtl::plus<pegtl::digit>, pegtl::blank>
        {};

        // valueunits
        struct valueunits
            :  pegtl::until<pegtl::at< line_end >>
        {};
        // valuelabels
        struct valuelabels
            :  pegtl::until<pegtl::at< line_end >>
        {};

        // meshunit
        struct meshunit
            : pegtl::sor<
                pegtl::pad<TAO_PEGTL_ISTRING("unspecified"), pegtl::blank>,
                pegtl::pad<pegtl::plus<pegtl::alpha>, pegtl::blank> >
        {};

        // min
        struct xmin
            : pegtl::pad<data_float, pegtl::blank>
        {};
        struct ymin
            : pegtl::pad<data_float, pegtl::blank>
        {};
        struct zmin
            : pegtl::pad<data_float, pegtl::blank>
        {};

        // max
        struct xmax
            : pegtl::pad<data_float, pegtl::blank>
        {};
        struct ymax
            : pegtl::pad<data_float, pegtl::blank>
        {};
        struct zmax
            : pegtl::pad<data_float, pegtl::blank>
        {};

        // meshtype
        struct meshtype
            : pegtl::sor<
                pegtl::pad<TAO_PEGTL_ISTRING("rectangular"), pegtl::blank>,
                pegtl::pad<TAO_PEGTL_ISTRING("irregular"), pegtl::blank> >
        {};

        // base
        struct xbase
            : vector3f
        {};
        struct ybase
            : vector3f
        {};
        struct zbase
            : vector3f
        {};

        // stepsize
        struct xstepsize
            : pegtl::pad<data_float, pegtl::blank>
        {};
        struct ystepsize
            : pegtl::pad<data_float, pegtl::blank>
        {};
        struct zstepsize
            : pegtl::pad<data_float, pegtl::blank>
        {};

        // nodes
        struct xnodes
            : pegtl::pad<pegtl::plus<pegtl::digit>, pegtl::blank>
        {};
        struct ynodes
            : pegtl::pad<pegtl::plus<pegtl::digit>, pegtl::blank>
        {};
        struct znodes
            : pegtl::pad<pegtl::plus<pegtl::digit>, pegtl::blank>
        {};

        //////////////////////////////////////////////

        //
        struct header
            : pegtl::seq<
                begin, TAO_PEGTL_ISTRING("Header"), pegtl::eol,
                //
                skippable_lines,
                //
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("Title:"), pegtl::blank >, title, pegtl::until<pegtl::eol> >,
                //
                skippable_lines,
                //
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("Desc:"), pegtl::blank >, description, pegtl::until<pegtl::eol> >,
                //
                skippable_lines,
                //
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("valuedim:"), pegtl::blank >, valuedim, finish_line >,
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("valueunits:"), pegtl::blank >, valueunits, pegtl::until<pegtl::eol> >,
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("valuelabels:"), pegtl::blank >, valuelabels, pegtl::until<pegtl::eol> >,
                //
                skippable_lines,
                // TODO: why do I need the following `until`? -> it seems comments are not skipped properly...
                pegtl::until<pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("meshunit:"), pegtl::blank >, meshunit, finish_line >>,
                //
                skippable_lines,
                //
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("xmin:"), pegtl::blank >, xmin, finish_line >,
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("ymin:"), pegtl::blank >, ymin, finish_line >,
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("zmin:"), pegtl::blank >, zmin, finish_line >,
                //
                skippable_lines,
                //
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("xmax:"), pegtl::blank >, xmax, finish_line >,
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("ymax:"), pegtl::blank >, ymax, finish_line >,
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("zmax:"), pegtl::blank >, zmax, finish_line >,
                //
                skippable_lines,
                //
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("meshtype:"), pegtl::blank >, meshtype, finish_line >,
                //
                skippable_lines,
                //
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("xbase:"), pegtl::blank >, xbase, finish_line >,
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("ybase:"), pegtl::blank >, ybase, finish_line >,
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("zbase:"), pegtl::blank >, zbase, finish_line >,
                //
                // skippable_lines,
                // //
                // pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("xstepsize:"), pegtl::blank >, xstepsize, finish_line >,
                // pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("ystepsize:"), pegtl::blank >, ystepsize, finish_line >,
                // pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("zstepsize:"), pegtl::blank >, zstepsize, finish_line >,
                //
                skippable_lines,
                //
                pegtl::until<pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("xnodes:"), pegtl::blank >, xnodes, finish_line >>,
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("ynodes:"), pegtl::blank >, ynodes, finish_line >,
                pegtl::seq< prefix, pegtl::pad< TAO_PEGTL_ISTRING("znodes:"), pegtl::blank >, znodes, finish_line >,
                //
                skippable_lines,
                //
                pegtl::seq<end, TAO_PEGTL_ISTRING("Header"), pegtl::eol>>
        {};


        //
        struct segment
            : pegtl::seq<
                pegtl::star<pegtl::seq<empty_line, pegtl::eol>>,
                pegtl::seq< begin, TAO_PEGTL_ISTRING("Segment"), pegtl::eol>,
                // pegtl::star<pegtl::seq<empty_line, pegtl::eol>>,
                // header,
                // pegtl::star<pegtl::seq<empty_line, pegtl::eol>>,
                // pegtl::sor<data_text, data_csv, data_binary_8, data_binary_4>,
                // pegtl::star<pegtl::seq<empty_line, pegtl::eol>>,
                pegtl::until<pegtl::seq<end, TAO_PEGTL_ISTRING("Segment")>>, pegtl::eol >
        {};


        // Class template for user-defined actions that does nothing by default.
        template< typename Rule >
        struct ovf_segment_action
            : pegtl::nothing< Rule >
        {};

        template<>
        struct ovf_segment_action< segment >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & file )
            {
                file._state->file_contents.push_back(in.string());
            }
        };


        //////////////////////////////////////////////

        //
        struct segment_header
            : pegtl::seq<
                pegtl::star<pegtl::seq<empty_line, pegtl::eol>>,
                pegtl::seq< begin, TAO_PEGTL_ISTRING("Segment"), pegtl::eol>,
                pegtl::star<pegtl::seq<empty_line, pegtl::eol>>,
                header,
                pegtl::star<pegtl::seq<empty_line, pegtl::eol>>,
                // pegtl::sor<data_text, data_csv, data_binary_8, data_binary_4>,
                // pegtl::star<pegtl::seq<empty_line, pegtl::eol>>,
                pegtl::until<pegtl::seq<end, TAO_PEGTL_ISTRING("Segment")>>, pegtl::eol >
        {};

        // Class template for user-defined actions that does nothing by default.
        template< typename Rule >
        struct ovf_segment_header_action
            : pegtl::nothing< Rule >
        {};

        template<>
        struct ovf_segment_header_action< segment_header >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & file, ovf_segment & segment )
            {
                // file._state->file_contents.push_back(in.string());
                segment.N = segment.n_cells[0] * segment.n_cells[1] * segment.n_cells[2];
            }
        };


        template<>
        struct ovf_segment_header_action< vec_float_value >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & file, ovf_segment & segment )
            {
                file._state->tmp_vec3[file._state->tmp_idx] = std::stod(in.string());
                ++file._state->tmp_idx;
            }
        };


        ////////////////////////////////////////////


        template<>
        struct ovf_segment_header_action< title >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.title = strdup(in.string().c_str());
            }
        };

        template<>
        struct ovf_segment_header_action< description >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.comment = strdup(in.string().c_str());
            }
        };

        template<>
        struct ovf_segment_header_action< valuedim >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.valuedim = std::stoi(in.string());
            }
        };

        template<>
        struct ovf_segment_header_action< valueunits >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.valueunits = strdup(in.string().c_str());
            }
        };

        template<>
        struct ovf_segment_header_action< valuelabels >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.valuelabels = strdup(in.string().c_str());
            }
        };

        template<>
        struct ovf_segment_header_action< xmin >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.bounds_min[0] = std::stod(in.string());
            }
        };
        template<>
        struct ovf_segment_header_action< ymin >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.bounds_min[1] = std::stod(in.string());
            }
        };
        template<>
        struct ovf_segment_header_action< zmin >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.bounds_min[2] = std::stod(in.string());
            }
        };

        template<>
        struct ovf_segment_header_action< xmax >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.bounds_max[0] = std::stod(in.string());
            }
        };
        template<>
        struct ovf_segment_header_action< ymax >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.bounds_max[1] = std::stod(in.string());
            }
        };
        template<>
        struct ovf_segment_header_action< zmax >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.bounds_max[2] = std::stod(in.string());
            }
        };

        template<>
        struct ovf_segment_header_action< xstepsize >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.step_size[0] = std::stod(in.string());
            }
        };
        template<>
        struct ovf_segment_header_action< ystepsize >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.step_size[1] = std::stod(in.string());
            }
        };
        template<>
        struct ovf_segment_header_action< zstepsize >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.step_size[2] = std::stod(in.string());
            }
        };

        // TODO
        template<>
        struct ovf_segment_header_action< xbase >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                for(int dim=0; dim<3; ++dim)
                    segment.bravais_vectors[0][dim] = f._state->tmp_vec3[dim];
                f._state->tmp_idx = 0;
            }
        };
        template<>
        struct ovf_segment_header_action< ybase >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                for(int dim=0; dim<3; ++dim)
                    segment.bravais_vectors[1][dim] = f._state->tmp_vec3[dim];
                f._state->tmp_idx = 0;
            }
        };
        template<>
        struct ovf_segment_header_action< zbase >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                for(int dim=0; dim<3; ++dim)
                    segment.bravais_vectors[2][dim] = f._state->tmp_vec3[dim];
                f._state->tmp_idx = 0;
            }
        };

        template<>
        struct ovf_segment_header_action< meshunit >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.meshunits = strdup(in.string().c_str());
            }
        };

        template<>
        struct ovf_segment_header_action< meshtype >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.meshtype = strdup(in.string().c_str());
            }
        };

        template<>
        struct ovf_segment_header_action< xnodes >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.n_cells[0] = std::stoi(in.string());
            }
        };
        template<>
        struct ovf_segment_header_action< ynodes >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.n_cells[1] = std::stoi(in.string());
            }
        };
        template<>
        struct ovf_segment_header_action< znodes >
        {
            template< typename Input >
            static void apply( const Input& in, ovf_file & f, ovf_segment & segment )
            {
                segment.n_cells[2] = std::stoi(in.string());
            }
        };

        ////////////////////////////////////////////////////////////////////////////////////////////

        struct data_text
            : pegtl::seq<
                begin, TAO_PEGTL_ISTRING("Data Text"), pegtl::eol,
                pegtl::plus< line_data_txt, pegtl::eol >,
                end, TAO_PEGTL_ISTRING("Data Text"), pegtl::eol
                >
        {};

        struct data_csv
            : pegtl::seq<
                begin, TAO_PEGTL_ISTRING("Data CSV"), pegtl::eol,
                pegtl::plus< line_data_csv, pegtl::eol >,
                end, TAO_PEGTL_ISTRING("Data CSV"), pegtl::eol
                >
        {};

        struct data_binary_4
            : pegtl::seq<
                begin, TAO_PEGTL_ISTRING("Data Binary 4"), pegtl::eol,
                // TODO
                // pegtl::plus< line_data_bin_4, pegtl::eol >,
                // pegtl::until<pegtl::seq<end, TAO_PEGTL_ISTRING("Data Binary 4")>>, pegtl::eol
                bin_4_check_value,
                // data_block_4,// pegtl::eol,
                // pegtl::until< pegtl::eol, pegtl::plus<bin_4_value> >,
                // pegtl::plus<pegtl::if_then_else< pegtl::not_at<pegtl::eol>, bin_4_value, pegtl::eol >>,
                // pegtl::seq< pegtl::star< pegtl::not_at< pegtl::eol >, pegtl::not_at< pegtl::eof >, bin_4_value >, pegtl::eol >,
                pegtl::until< pegtl::eol, bin_4_value >,
                end, TAO_PEGTL_ISTRING("Data Binary 4"), pegtl::eol
                >
        {};

        struct data_binary_8
            : pegtl::seq<
                begin, TAO_PEGTL_ISTRING("Data Binary 8"), pegtl::eol,
                // TODO
                // pegtl::plus< line_data_bin_8, pegtl::eol >,
                // end, TAO_PEGTL_ISTRING("Data Binary 8"), pegtl::eol
                bin_8_check_value,
                pegtl::until< pegtl::eol, bin_8_value >,
                end, TAO_PEGTL_ISTRING("Data Binary 8"), pegtl::eol
                >
        {};

        struct segment_data
            : pegtl::seq<
                pegtl::star<pegtl::seq<empty_line, pegtl::eol>>,
                begin, TAO_PEGTL_ISTRING("Segment"), pegtl::eol,
                pegtl::star<pegtl::seq<empty_line, pegtl::eol>>,
                header,
                pegtl::star<pegtl::seq<empty_line, pegtl::eol>>,
                pegtl::sor< data_text, data_csv, data_binary_4, data_binary_8 >,
                pegtl::star<pegtl::seq<empty_line, pegtl::eol>>,
                pegtl::until<pegtl::seq<end, TAO_PEGTL_ISTRING("Segment")>>, pegtl::eol >
        {};

        ////////////////////////////////////

        // Class template for user-defined actions that does nothing by default.
        template< typename Rule >
        struct ovf_segment_data_action
            : pegtl::nothing< Rule >
        {};

        // template<>
        // struct ovf_segment_data_action< data_text >
        // {
        //     template< typename Input, typename scalar >
        //     static void apply( const Input& in, ovf_file & f, const ovf_segment & segment, scalar * data )
        //     {
        //         std::cerr << "\nsegment data action triggered\n" << std::endl;
        //     }
        // };

        template<>
        struct ovf_segment_data_action< line_data_txt >
        {
            template< typename Input, typename scalar >
            static void apply( const Input& in, ovf_file & f, const ovf_segment & segment, scalar * data )
            {
                f._state->current_column = 0;
                ++f._state->current_line;
            }
        };

        template<>
        struct ovf_segment_data_action< line_data_csv >
        {
            template< typename Input, typename scalar >
            static void apply( const Input& in, ovf_file & f, const ovf_segment & segment, scalar * data )
            {
                f._state->current_column = 0;
                ++f._state->current_line;
            }
        };

        template<>
        struct ovf_segment_data_action< segment_data_float >
        {
            template< typename Input, typename scalar >
            static void apply( const Input& in, ovf_file & f, const ovf_segment & segment, scalar * data )
            {
                int row = f._state->current_line;
                int col = f._state->current_column;

                int n_cols = segment.valuedim;

                double value = std::stod(in.string());

                int idx = col + row*n_cols;

                data[idx] = value;
                ++f._state->current_column;
            }
        };


        template<>
        struct ovf_segment_data_action< bin_4_check_value >
        {
            template< typename Input, typename scalar >
            static void apply( const Input& in, ovf_file & f, const ovf_segment & segment, scalar * data )
            {
                std::string bytes = in.string();
                uint32_t hex_4b = endian::from_little_32(reinterpret_cast<const uint8_t *>( bytes.c_str() ));

                if ( hex_4b != check::val_4b )
                    throw tao::pegtl::parse_error( "the expected binary check value could not be parsed!", in );
            }
        };

        template<>
        struct ovf_segment_data_action< bin_4_value >
        {
            template< typename Input, typename scalar >
            static void apply( const Input& in, ovf_file & f, const ovf_segment & segment, scalar * data )
            {
                std::string bytes = in.string();
                uint32_t ivalue = endian::from_little_32(reinterpret_cast<const uint8_t *>( bytes.c_str() ));
                float value = *reinterpret_cast<const float *>( &ivalue );

                int row = f._state->current_line;
                int col = f._state->current_column;

                int n_cols = segment.valuedim;

                int idx = col + row*n_cols;

                data[idx] = value;
                ++f._state->current_column;
            }
        };

        template<>
        struct ovf_segment_data_action< bin_8_check_value >
        {
            template< typename Input, typename scalar >
            static void apply( const Input& in, ovf_file & f, const ovf_segment & segment, scalar * data )
            {
                std::string bytes = in.string();
                uint64_t hex_8b = endian::from_little_64(reinterpret_cast<const uint8_t *>( bytes.c_str() ));

                if ( hex_8b != check::val_8b )
                    throw tao::pegtl::parse_error( "the expected binary check value could not be parsed!", in );
            }
        };

        template<>
        struct ovf_segment_data_action< bin_8_value >
        {
            template< typename Input, typename scalar >
            static void apply( const Input& in, ovf_file & f, const ovf_segment & segment, scalar * data )
            {
                std::string bytes = in.string();
                uint64_t ivalue = endian::from_little_64(reinterpret_cast<const uint8_t *>( bytes.c_str() ));
                double value = *reinterpret_cast<const double *>( &ivalue );

                int row = f._state->current_line;
                int col = f._state->current_column;

                int n_cols = segment.valuedim;

                int idx = col + row*n_cols;

                data[idx] = value;
                ++f._state->current_column;
            }
        };

        template<>
        struct ovf_segment_data_action< data_binary_4 >
        {
            template< typename Input, typename scalar >
            static void apply( const Input& in, ovf_file & f, const ovf_segment & segment, scalar * data )
            {
                static_assert(
                    !std::is_floating_point<scalar>::value ||
                    (std::is_floating_point<scalar>::value && std::numeric_limits<scalar>::is_iec559),
                    "Portable binary only supports IEEE 754 standardized floating point" );
            }
        };

        template<>
        struct ovf_segment_data_action< data_binary_8 >
        {
            template< typename Input, typename scalar >
            static void apply( const Input& in, ovf_file & f, const ovf_segment & segment, scalar * data )
            {
                static_assert(
                    !std::is_floating_point<scalar>::value ||
                    (std::is_floating_point<scalar>::value && std::numeric_limits<scalar>::is_iec559),
                    "Portable binary only supports IEEE 754 standardized floating point" );
            }
        };
    }; // namespace v2

    namespace v1
    {
        struct file
            : pegtl::star<pegtl::any>
        {};

        // Class template for user-defined actions that does nothing by default.
        template< typename Rule >
        struct file_action
            : pegtl::nothing< Rule >
        {};
    };

} // namespace parse


namespace parse
{
    int initial(ovf_file & file)
    try
    {
        pegtl::file_input<> in( file.file_name );
        bool success = pegtl::parse< ovf_file_header, ovf_file_action >( in, file );
        if( success )
        {
            success = false;
            if( file.version == 2 )
            {
                success = pegtl::parse< pegtl::until<pegtl::until<pegtl::at< pegtl::seq<v2::begin, TAO_PEGTL_ISTRING("Segment"), pegtl::eol >>>> >( in, file );
                success = pegtl::parse< pegtl::plus<v2::segment>, v2::ovf_segment_action >( in, file );
            }
            else if( file.version == 1 )
            {
                // TODO...
                file._state->message_latest = fmt::format(
                    "libovf initial: OVF version \'{}\' in file \'{}\' is not supported...",
                    file.file_name, file.version);
            }
            else
            {
                file._state->message_latest = fmt::format(
                    "libovf initial: OVF version \'{}\' in file \'{}\' is not supported...",
                    file.file_name, file.version);
                return OVF_INVALID;
            }

            if( success )
            {
                int n_located = file._state->file_contents.size();
                if( file.n_segments != n_located )
                {
                    file._state->message_latest = fmt::format(
                        "libovf initial: n_segments specified in header ({}) is different from the number"
                        " of segments ({}) found in the file \'{}\'...",
                        file.n_segments, n_located, file.file_name);
                    return OVF_INVALID;
                }

                file.is_ovf = true;
                return OVF_OK;
            }
            else
            {
                file._state->message_latest = "libovf initial: no success in parsing...";
                return OVF_INVALID;
            }
        }
        else
        {
            file._state->message_latest = "libovf initial: no success in parsing...";
            return OVF_INVALID;
        }
    }
    catch( pegtl::parse_error err )
    {
        file._state->message_latest = fmt::format(
            "libovf initial: pegtl parse error \'{}\'", + err.what());
        return OVF_ERROR;
    }
    catch( std::exception ex )
    {
        file._state->message_latest = fmt::format(
            "libovf initial: std::exception \'{}\'", + ex.what());
        return OVF_ERROR;
    }
    catch( ... )
    {
        file._state->message_latest = "libovf initial: unknown exception";
        return OVF_ERROR;
    }

    int read_segment_header(ovf_file & file, int index, ovf_segment & segment)
    try
    {
        pegtl::memory_input<> in( file._state->file_contents[index], "" );
        bool success = false;

        if( file.version == 2 )
        {
            success = pegtl::parse< pegtl::plus<v2::segment_header>, v2::ovf_segment_header_action >( in, file, segment );
        }
        else if( file.version == 1 )
        {
            // TODO...
            file._state->message_latest = fmt::format(
                "libovf read_segment_header: OVF version \'{}\' in file \'{}\' is not supported...",
                file.file_name, file.version);
            return OVF_INVALID;
        }
        else
        {
            file._state->message_latest = fmt::format(
                "libovf read_segment_header: OVF version \'{}\' in file \'{}\' is not supported...",
                file.file_name, file.version);
            return OVF_INVALID;
        }

        if( success )
            return OVF_OK;
        else
        {
            file._state->message_latest = "libovf read_segment_header: no success in parsing";
            std::cerr << file._state->file_contents[index] << std::endl;
            return OVF_INVALID;
        }
    }
    catch( pegtl::parse_error err )
    {
        file._state->message_latest = fmt::format(
            "libovf read_segment_header: pegtl parse error \'{}\'", + err.what());
        return OVF_ERROR;
    }
    catch( std::exception ex )
    {
        file._state->message_latest = fmt::format(
            "libovf read_segment_header: std::exception \'{}\'", + ex.what());
        return OVF_ERROR;
    }
    catch( ... )
    {
        file._state->message_latest = "libovf read_segment_header: unknown exception";
        return OVF_ERROR;
    }

    template<typename scalar>
    int read_segment_data_template(ovf_file & file, int index, const ovf_segment & segment, scalar * data)
    try
    {
        pegtl::memory_input<> in( file._state->file_contents[index], "" );
        int retcode = OVF_ERROR;
        bool success = false;

        if( file.version == 2 )
        {
            success = pegtl::parse< v2::segment_data, v2::ovf_segment_data_action >( in, file, segment, data );
            file._state->current_line = 0;
            file._state->current_column = 0;
        }
        else if( file.version == 1 )
        {
            // TODO...
            file._state->message_latest = fmt::format(
                "libovf read_segment_data: OVF version \'{}\' in file \'{}\' is not supported...",
                file.file_name, file.version);
            return OVF_INVALID;
        }
        else
        {
            file._state->message_latest = fmt::format(
                "libovf read_segment_data: OVF version \'{}\' in file \'{}\' is not supported...",
                file.file_name, file.version);
            return OVF_INVALID;
        }

        if( success )
        {
            return OVF_OK;
        }
        else
        {
            file._state->message_latest = "libovf read_segment_data: no success in parsing";
            return OVF_INVALID;
        }
    }
    catch( pegtl::parse_error err )
    {
        file._state->message_latest = fmt::format(
            "libovf read_segment_data: pegtl parse error \'{}\'", + err.what());
        return OVF_ERROR;
    }
    catch( std::exception ex )
    {
        file._state->message_latest = fmt::format(
            "libovf read_segment_data: std::exception \'{}\'", + ex.what());
        return OVF_ERROR;
    }
    catch( ... )
    {
        file._state->message_latest = "libovf read_segment_data: unknown exception";
        return OVF_ERROR;
    }

    int read_segment_data(ovf_file & file, int index, const ovf_segment & segment, float * data)
    {
        return read_segment_data_template(file, index, segment, data);
    }

    int read_segment_data(ovf_file & file, int index, const ovf_segment & segment, double * data)
    {
        return read_segment_data_template(file, index, segment, data);
    }
}