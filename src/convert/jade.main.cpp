/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "jade.args.hpp"
#include "jade.bgl2lgm.hpp"
#include "jade.cov2nwk.hpp"
#include "jade.nwk2cov.hpp"
#include "jade.nwk2svg.hpp"
#include "jade.ped2dgm.hpp"
#include "jade.version.hpp"

namespace
{
char const * const usage = R"(USAGE
  convert <command> [<input> <output>]

ARGUMENTS
  command  one of the following conversion types:
           bgl2lgm
           cov2nwk
           nwk2cov
           nwk2svg
           ped2dgm

COMMANDS
  bgl2lgm  Converts a beagle file to an lgm matrix.  Each line of the beagle
           file is split into a number of fields.  The first three fields are
           ignored.  The remaining fields are split into individuals, each of
           which must contain three genotype likelihoods.

  cov2nwk  Converts a covariance matrix to a Newick-formatted tree.  First the
           covariance matrix is converted into a distance matrix, which is then
           approximated into a Newick-formatted tree using the Neighbor Joining
           algorithm.

  nwk2cov  Converts a Newick-formatted tree to a covariance matrix.  Each tree
           structure is mapped to a covariance matrix unambiguously.

  nwk2svg  Converts a Newick-formatted tree to an SVG representation.  To find
           an appealing arrangement of a tree, this converter takes inspiration
           from an electrostatic field and models tree components as like-signed
           charged particles with nodes constrained by the branches that connect
           them. It then utilizes the Nelder-Mead algorithm to minimize the
           total potential energy of this system and achieve an optimal tree
           layout.

  ped2dgm  Converts a plink ped file to a dgm matrix. Each line of the ped file
           is split into a number of fields, separated by tabs. The first six
           fields are ignored, but the remaining fields must consist of two
           symbols separated by a space. Each pair of symbols is considered a
           pair of a column.

  For all commands, if no arguments are given, the source file is read from
  standard input and the output file is written to standard output. Otherwise,
  the path to the input file and output file must be specified after the
  conversion type argument.

OPTIONS
  --help,-h    shows this help message and exits

DESCRIPTION
  Converts files.

EXAMPLE
  $ convert nwk2svg foo.nwk foo.svg
  $ cat bar.bgl | convert bgl2lgm > bar.lgm

BUGS
  Report any bugs to Jade Cheng <info@jade-cheng.com>.

Copyright (c) 2015-2017 Jade Cheng
)";

    ///
    /// Executes the program based on the specified options.
    ///
    template <typename TController>
    int execute(
            jade::args & a) ///< The command-line arguments.
    {
        //
        // If arguments were provided, read from and write to files;
        // otherwise, read from and write to standard streams.
        //
        if (a.is_empty())
        {
            TController::execute(std::cin, std::cout);
        }
        else
        {
            const auto src = a.pop<std::string>();
            const auto dst = a.pop<std::string>();
            a.validate_empty();

            std::ifstream in (src);
            if (!in.good())
                throw jade::error() << "failed to open '" << src << "'";

            std::ofstream out (dst);
            if (!out.good())
                throw jade::error() << "failed to create '" << dst << "'";

            TController::execute(in, out);
        }

        return EXIT_SUCCESS;
    }
}

///
/// The main entry point of the program.
/// \param argc The argument count.
/// \param argv The argument values.
/// \return     EXIT_SUCCESS or EXIT_FAILURE.
///
int main(const int argc, const char * argv[])
{
    try
    {
        jade::args args (argc, argv);

        if (args.read_flag("--help", "-h"))
        {
            std::cout << ::usage;
            return EXIT_SUCCESS;
        }

        if (args.read_flag("--version", "-v"))
        {
            jade::version::write("convert", std::cout);
            return EXIT_SUCCESS;
        }

        typedef double                          value_type;
        typedef jade::basic_bgl2lgm<value_type> bgl2lgm_type;
        typedef jade::basic_cov2nwk<value_type> cov2nwk_type;
        typedef jade::basic_nwk2cov<value_type> nwk2cov_type;
        typedef jade::basic_nwk2svg<value_type> nwk2svg_type;
        typedef jade::basic_ped2dgm<value_type> ped2dgm_type;

        const auto command = args.pop<std::string>();

        if (command == "bgl2lgm") return ::execute<bgl2lgm_type>(args);
        if (command == "cov2nwk") return ::execute<cov2nwk_type>(args);
        if (command == "nwk2cov") return ::execute<nwk2cov_type>(args);
        if (command == "nwk2svg") return ::execute<nwk2svg_type>(args);
        if (command == "ped2dgm") return ::execute<ped2dgm_type>(args);

        throw jade::error() << "unsupported command '" << command << "'";
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
