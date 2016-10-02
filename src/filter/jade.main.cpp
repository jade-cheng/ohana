/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2016 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "jade.args.hpp"
#include "jade.version.hpp"
#include "jade.rema.hpp"

namespace
{
    char const * const usage = R"(USAGE
  filter <command> ... [<input> <output>]

ARGUMENTS
  command  one of the following conversion types:

           rema    Reduces the number of markers in a matrix.

COMMANDS
  rema

    USAGE
      filter rema [options] <column-count> [<input> <output>]

    DESCRIPTION
      This filter reduces the number of markers in a matrix. The number of
      columns written to the output matrix is specified as a required argument.
      The filter chooses the columns to remove at random, but their relative
      order is not changed. It is possible to provide a random number seed as
      an option; if unspecified, the program uses a time-dependent default
      seed.

    OPTIONS
      --seed,-s    indicates the next argument is the random number seed; if
                   unspecified, the program uses a time-dependent default seed

    EXAMPLE
      $ filter rema 1000 in.lgm out.lgm
      $ cat in.lgm | filter rema --seed 1864 1000 > out.lgm

OPTIONS
  --help,-h       shows this help message and exits
  --version,-v    prints version information and exits

DESCRIPTION
  Filters input given to the program. Further details are provided in the
  section above describing the various commands.

  For all commands, if no arguments are given, the source data is read from
  standard input and the output data is written to standard output. Otherwise,
  the path to the input file and output file must be specified after the
  filter arguments.

EXAMPLE
  $ filter rema 1000 in.lgm out.lgm
  $ cat in.lgm | filter rema --seed 1864 1000 > out.lgm

BUGS
  Report any bugs to Jade Cheng <info@jade-cheng.com>.

Copyright (c) 2015-2016 Jade Cheng
)";

    ///
    /// Executes the program based on the specified options.
    ///
    template <typename TController>
    int execute(
            jade::args & a) ///< The command-line arguments.
    {
        //
        // Create a new controller instance, allowing it to process the
        // arguments for additional options and values.
        //
        typedef TController controller_type;
        controller_type controller (a);

        //
        // If input and output arguments were provided, read from and write to
        // the corresponding files; otherwise, read from and write to the
        // standard streams.
        //
        if (a.is_empty())
        {
            controller.execute(std::cin, std::cout);
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

            controller.execute(in, out);
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
            jade::version::write("filter", std::cout);
            return EXIT_SUCCESS;
        }

        typedef jade::rema rema_type;

        const auto command = args.pop<std::string>();

        if (command == "rema") return ::execute<rema_type>(args);

        throw jade::error() << "unsupported command '" << command << "'";
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
