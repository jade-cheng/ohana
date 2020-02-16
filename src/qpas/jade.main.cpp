/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2020 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#include "jade.optimizer.hpp"
#include "jade.version.hpp"

namespace
{
    char const * const usage = R"(USAGE
  qpas [options] <g-matrix>

ARGUMENTS
  g-matrix                      the path to a genotype matrix; the format of
                                the file is determined based on the extension,
                                .dgm (discrete genotype matrix) or
                                .lgm (likelihood genotype matrix)

OPTIONS
  --epsilon,-e                  indicates the next argument is the epsilon
                                value; i.e. the minimum difference between
                                likelihood calculations per iteration; this
                                value must be greater than or equal to zero
  --help,-h                     shows this help message and exits
  --fin,-fi                     indicates the next argument is the path to the
                                initial F matrix; this option cannot be used
                                with the --fin-force option
  --fin-force,-fif              indicates the next argument is the path to a
                                portion of the initial F matrix; this option
                                cannot be used with the --fin option
  --fixed-f,-ff                 indicates the optimizer should not optimize the
                                specified F matrix
  --fixed-q,-fq                 indicates the optimizer should not optimize the
                                specified Q matrix
  --fout,-fo                    indicates the next argument is the path to the
                                computed F matrix
  --force,-fg                   indicates the next argument is the path to a
                                file identifying an assignment of components
                                for each individual and a range of Q values for
                                each component
  --frequency-bounds,-frb       indicates the algorithm applies bounds between
                                1 / (2n + 1) and 1 - (1 / (2n + 1)) for allele
                                frequencies, where n is the number of
                                individuals; without this flag, this bounds
                                are set to 0 and 1
  --ksize,-k                    indicates the next argument is the number of
                                components; this value must be at least one
  --max-iterations,-mi          indicates the next argument is the maximum
                                number of iterations to execute the algorithm;
                                this value must be greater than or equal to
                                zero
  --max-time,-mt                indicates the next argument is the maximum time
                                in seconds to execute the algorithm; this value
                                must be greater than or equal to zero
  --qin,-qi                     indicates the next argument is the path to the
                                initial Q matrix
  --qout,-qo                    indicates the next argument is the path to the
                                computed Q matrix
  --seed,-s                     indicates the next argument is the seed for the
                                random number generator

  At least one of --ksize, --qin, --fin, or --force must be specified in order
  to determine the number of components (K).

DESCRIPTION
  Under the assumption of Hardy Weinberg Equilibrium, the likelihood of
  assigning an observed genotype g in individual i at locus j to component k
  is a function of the allelic frequency f_kj of the locus at k and the
  fraction of the genome of the individual q_ik that comes from that
  component.  We thus consider the likelihood of the ancestral component
  proportions vector Q and their vector of allele frequencies F.  In
  particular, if we denote K as the number of ancestry components, I as the
  number of individuals, and J as the number of polymorphic sites among the I
  individuals, then the probability of observing the genotype is:

  sum_i sum_j {
    g_ij * ln[sum_k (q_ik * f_kj)] +
    (2 - g_ij) * ln[sum_k (q_ik * (1 - f_kj))]
  }

  To estimate Q and F, we follow Newton's method.  In general, we can
  approximate a function F with its second order Taylor expansion F_T.  To
  solve this inequality- and equality-constraint quadratic optimization
  problem, first we derive the first and second differentials for lnP1(Q, F)
  with respect to values in Q and F, separately.  Then we incorporate the
  active set algorithm [Murty 1988].

  [Command-Line Output]

  Unless the --quiet option is specified, the program writes tabular
  information to standard output.  Each iteration, the program writes a row
  with the following information:

  1. iteration Number
  2. seconds expired during the iteration
  3. log-likelihood after the iteration
  4. delta log-likelihood from the previous iteration

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
     row sums to 1.0.

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

  [Forced-Grouping File Format]

  A forced-grouping file is an ASCII file in the following format.  All values
  are separated by whitespace, and lines beginning with '#' are treated as
  comments and ignored.  The first value indicates the number of individuals
  (N), the second value indicates the number of components (K), the next N
  values represent the population index for each individual, and the remaining
  values represent matrices of ranges for the Q matrix.  Each matrix is
  [2K x 1] in which the first K values indicate the minimum Q values, and the
  last K values indicate the maximum Q values.  For example, the following
  represents a forged-grouping of 15 individuals in 3 components:

  # 'I' Individuals and 'K' Components
  15 3

  # Component Assignments per Individual
  0 1 2 0 0 0 1 1 1 0 0 1 2 2 2

  # Population 0
  6 1
  0.4  0.0  0.0
  1.0  1.0  1.0

  # Population 1
  6 1
  0 0  0.4  0.0
  1.0  1.0  0.1

  # Population 2
  6 1
  0.0  0.0  0.4
  1.0  0.1  1.0

EXAMPLES
  $ qpas -k 4 -qo ./qout.matrix -fo ./fout.matrix -mi 5 ./g.dgm
  seed: 3964111000

  0    0.027389    -3.119424845029e+06
  1    0.864429    -2.334222638280e+06    7.852022067495e+05
  2    0.837323    -2.295676472449e+06    3.854616583094e+04
  3    0.745620    -2.260783781893e+06    3.489269055576e+04
  4    0.703584    -2.231310220386e+06    2.947356150691e+04
  5    0.693284    -2.205170273510e+06    2.613994687624e+04

  Writing Q matrix to ./qout.matrix
  Writing F matrix to ./fout.matrix

BUGS
  Report any bugs to Jade Cheng <info@jade-cheng.com>.

Copyright (c) 2015-2020 Jade Cheng
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
            jade::version::write("qpas", std::cout);
            return EXIT_SUCCESS;
        }

        typedef double                            value_type;
        typedef jade::basic_settings<value_type>  settings_type;
        typedef jade::basic_optimizer<value_type> optimizer_type;

        settings_type settings (args);
        optimizer_type::execute(
                settings,
                settings.get_q(),
                settings.get_f());

        return EXIT_SUCCESS;
    }
    catch (const std::exception & e)
    {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
