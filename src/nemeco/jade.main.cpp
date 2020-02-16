/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "jade.optimizer.hpp"
#include "jade.version.hpp"

namespace
{
    char const * const usage = R"(USAGE
  nemeco [options] <g-matrix> <f-matrix>

ARGUMENTS
  g-matrix                      the path to a genotype matrix
  f-matrix                      the path to the frequency matrix

OPTIONS
  --ain,-ai                     indicates the next argument is the path to the
                                admixture graph input file; this option cannot
                                be specified with the --cin or --tin options
  --cin,-ci                     indicates the next argument is the path to the
                                initial covariance matrix; this option cannot
                                be specified with the --ain or --tin options
  --cout,-co                    indicates the next argument is the path to the
                                [k-1 x k-1] output covariance matrix
  --epsilon,-e                  indicates the next argument is the epsilon
                                value; i.e. the minimum difference between
                                likelihood; this value must be greater than or
                                equal to zero
  --help,-h                     shows this help message
  --max-iterations,-mi          indicates the next argument is the maximum
                                number of iterations to execute the algorithm;
                                this value must be greater than zero
  --max-time,-mt                indicates the next argument is the maximum time
                                in seconds to execute the algorithm; this value
                                must be greater than or equal to zero
  --tin, -ti                    indicates the next argument is the path to the
                                file that defines the input tree structure; the
                                file is in Newick format; this option cannot
                                be specified with the --ain or --cin options
  --tout, -to                   indicates the next argument is the path to the
                                output file containing the tree structure; the
                                file is in Newick format; this option cannot be
                                specified without the --tin option

DESCRIPTION
  Models the joint distribution of the allele frequencies as a variant of a
  multivariate Gaussian and infers its covariance matrix using the Nelder-Mead
  optimization method.

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

  C := [K-1 x K-1] Rooted Covariance Matrix
     This matrix consists of floating-point values.  It is a symmetric and
     positive semidefinite matrix.

  [Admixture Graph Input File Format]

  An Admixture Graph Input (AGI) file is an ASCII file that specifies parameter
  names for branch lengths, parameter names for admixture proportions, a number
  of components, and expressions that are evaluated to compose the rooted
  covariance matrix.  At least one branch length parameter and at least one
  proportion parameter must be defined, but not all parameters must be used in
  the expressions.  For example, the following represents an admixture graph
  with three components, one admixture event, and rooted at component A:

    #         f/ \
    #         /   \g
    #        /\    \
    #       / d\   /\
    #     a/    \ /e \
    #     / <-p  |b   \c
    #    A       B     C
    #
    # Branch length parameters, range: [0, inf)
    a b c d e f g

    # Admixture proportion parameters, range: [0, 1]
    p

    # K value
    3

    # Matrix entries, total number should be: K*(K-1)/2
    # They map to a C matrix, e.g. K=3 it maps to:
    #   0 1
    #   1 2
    (1 - p) * (b + e + g + f + a) + p * (b + d + a)
    p * a + (1 - p) * (g + f + a)
    c + g + f + a

EXAMPLES
  $ nemeco -mi 3 -e 0 -co ./cout.matrix g.matrix f.matrix
  iter    duration    delta-log       log-likelihood
  1       0.005124    0.000000e+00    -6.190334e+04
  2       0.000044    0.000000e+00    -6.190334e+04
  3       0.000042    0.000000e+00    -6.190334e+04

  log likelihood = -61903.3
  Writing C matrix to ./cout.matrix

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
            jade::version::write("nemeco", std::cout);
            return EXIT_SUCCESS;
        }

        typedef double                            value_type;
        typedef jade::basic_settings<value_type>  settings_type;
        typedef jade::basic_optimizer<value_type> optimizer_type;

        const settings_type settings (args);
        optimizer_type::execute(settings);

        return EXIT_SUCCESS;
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
