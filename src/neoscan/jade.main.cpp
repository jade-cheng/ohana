/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "jade.neoscan.hpp"
#include "jade.version.hpp"

namespace
{
    char const * const usage = R"(USAGE
  neoscan [options] <g-matrix> <q-matrix> <f-matrix> <year-matrix>

ARGUMENTS
  g-matrix       path to the [I x J] G matrix
  q-matrix       path to the [I x K] Q matrix
  f-matrix       path to the [K x J] F matrix
  year-matrix    path to the [I x 1] matrix for years

OPTIONS
  --help,-h      shows this help message and exits

DESCRIPTION
  Performs a selection scan between ancient and modern data. The program can
  take advantage of the dating for each individual ancient sample. The program
  prints for each marker the delta when local optima is reached, the global
  likelihood, the optimal local likelihood, and the likelihood ratio.

  The program requires a <year-matrix>, which contains for each individual a
  row representing the number of years going backward in time. For instance,
  modern samples would be zero, and a millennium would be a positive value,
  e.g. 1000.

  [Notation]

  K := Number of Components
     This value must be greater than or equal to one.

  I := Number of Individuals
     This value must be greater than or equal to one.

  J := Number of Markers
     This value must be greater than or equal to one.

  G := [I x J] Discrete or Likelihood Genotype Matrix
       dgm consists of integer values ranging from 0 to 3, inclusive.
         0 := major-major allele
         1 := major-minor allele
         2 := minor-minor allele
         3 := missing allele information
       lgm with three floating-point value matrices in the following order.
         n x m matrix, values 0.0 to 1.0 for minor-minor
         n x m matrix, values 0.0 to 1.0 for major-minor
         n x m matrix, values 0.0 to 1.0 for major-major

  F := [K x J] Frequency Matrix
     This matrix consists of floating-point values ranging from 0 to 1.

  Q := [I x K] Proportion Matrix
     This matrix consists of floating-point values ranging from 0 to 1.  Each
     row sums to 1.0, and each row contains more than one distinct value.

  [Matrix File Format]

  A matrix file is an ASCII file in the following format.  All values are
  separated by white-space.  The first value indicates the number of rows, the
  second value indicates the number of columns, and then the remaining values
  represent the values of the matrix in row-major order.  For example, the
  following represents a matrix with 2 rows and 4 columns:

  2 4
  1.0  2.0  3.0  4.0
  5.0  6.0  7.0  8.0

  In the case of the likelihood genotype matrix, three individual matrices
  exist within the file representing minor-minor, major-minor, and major-major
  allele frequencies. For example, the following represents a matrix with
  2 rows (individuals) and 4 columns (markers):

  2 4
  0.1  0.2  0.3  0.4
  0.5  0.6  0.7  0.8
  2 4
  0.9  0.8  0.7  0.6
  0.5  0.4  0.3  0.2
  2 4
  0.1  0.9  0.2  0.8
  0.3  0.7  0.4  0.6

EXAMPLE
  $ neoscan g.dgm q.matrix f.matrix years.json
  d       global-lle      local-lle       lle-ratio
  -1.00   +1.418028e+00   +1.418028e+00   +0.000000e+00
  +0.00   +1.769128e+00   +1.769128e+00   +0.000000e+00
  +1.00   +1.213023e+00   +1.213023e+00   +0.000000e+00

BUGS
  Report any bugs to Jade Cheng <info@jade-cheng.com>.

Copyright (c) 2015-2017 Jade Cheng
)";
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
            jade::version::write("neoscan", std::cout);
            return EXIT_SUCCESS;
        }

        typedef double                          value_type;
        typedef jade::basic_neoscan<value_type> neoscan_type;

        neoscan_type::run(args);

        return EXIT_SUCCESS;
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
