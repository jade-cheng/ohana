/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "jade.selscan.hpp"
#include "jade.version.hpp"

namespace
{
    char const * const usage = R"(USAGE
  selscan [options] <g-matrix> <f-matrix> <c-matrix>

ARGUMENTS
  g-matrix     path to the [I x J] G matrix
  f-matrix     path to the [K x J] F matrix
  c-matrix     path to the [K-1 x K-1] global C matrix

OPTIONS
  --steps,-s (100)   the number of steps to interpolate between C matrices
  --help,-h          shows this help message and exits
  --c-scale,-cs      indicates the next argument is the path to a [K-1 x K-1] C
                     matrix that provides scaling information; each step
                     linearly interpolates between the global C matrix and this
                     matrix

DESCRIPTION
  Performs a selection scan to identify covariance outliers and prints for each
  marker the step number when local optima is reached, the global likelihood,
  the optimal local likelihood, and the likelihood ratio. By default, the
  program linearly interpolates between the global C matrix and 10 times its
  values, but it is possible to specify a scaling matrix using the --c-scale
  option.

  There are 'I' individuals, 'K' populations, and 'J' markers. The sizes of
  the matrices evaluated by this program are:

  G matrix    [I x J]     the path to a genotype matrix; the format of the file
                          is determined based on the extension,
                            .dgm (discrete genotype matrix) or
                            .lgm (likelihood genotype matrix)
  F matrix    [K x J]     floating-point values ranging from 0.0 to 1.0
  C matrix    [K-1 x K-1] floating-point values; the matrix is symmetric and
                          positive semidefinite

  [Notation]

  K := Number of Populations
     This value must be greater than or equal to two.

  I := Number of Individuals
     This value must be greater than or equal to two.

  J := Number of Markers
     This value must be greater than or equal to two.

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

  C := [K-1 x K-1] Rooted Covariance Matrix
     This matrix consists of floating-point values.  It is a symmetric and
     positive semidefinite matrix.

EXAMPLE
  $ selscan g.dgm f.matrix c.matrix
  step    global-lle      local-lle       lle-ratio
  0       +1.418028e+00   +1.418028e+00   +0.000000e+00
  2       +1.769128e+00   +1.769128e+00   +0.000000e+00
  0       +1.213023e+00   +1.213023e+00   +0.000000e+00

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
            jade::version::write("selscan", std::cout);
            return EXIT_SUCCESS;
        }

        typedef double                          value_type;
        typedef jade::basic_selscan<value_type> selscan_type;

        selscan_type selscan (args);
        selscan.execute();

        return EXIT_SUCCESS;
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
