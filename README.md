# Ohana

## Introduction

`Ohana` is a suite of software for analyzing population structure and admixture history using unsupervised learning methods.  We construct statistical models to infer individual clustering from which we identify outliers for selection analyses.

This project is directed by [Dr. Rasmus Nielsen](http://www.nielsenlab.org/) at University of California Berkeley.  [Jade Cheng](http://www.jade-cheng.com/) is funded by the Bioinformatics, Computer Science Department at Aarhus University and the Natural History Museum of Denmark at Copenhagen University.

## Installation

The `Ohana` source code is available on [github](https://github.com/jade-cheng/ohana):

    $ git clone https://github.com/jade-cheng/ohana

Building `Ohana` from source requires a typical UNIX development environment, a C++11 compiler, and an installation of [OpenBLAS](http://www.openblas.net/).  Before running `make`, we specify the path to OpenBLAS in the file `configure.mk` located in the root of this source tree.  For example, Ubuntu typically installs OpenBLAS into the `/opt/OpenBLAS` directory, and the corresponding settings would be:

    $ cat ./configure.mk
    CXX            = g++
    OPENBLAS       = /opt/OpenBLAS
    INSTALL_PREFIX = /opt/ohana

Once configured, we run `make` to build the software and optionally run `make install` to copy the output files into `$INSTALL_PREFIX/bin` directory.

    $ make
    :
    $ ls ./bin/
    convert   cpax   nemeco   qpas   selscan

## Description

### convert

To facilitate different stages of the analysis, we provide several conversion subroutines. `ped2dgm` converts genotype observations from the plink format to feed into `qpas`. `bgl2lgm` converts genotype likelihoods from the beagle format to feed into `qpas`. `cov2nwk` first converts a covariance matrix to a distance matrix, then it implements the Neighbor Joining algorithm to approximate the distance matrix into a Newick tree. `nwk2svg` produces a scalar vector graphics representation of the Newick tree.  The output can be viewed with web browsers and modified with graphics editors like Inkscape.  Finally, if a tree-compatible covariance matrix is desired for `selscan`, we have `nwk2cov` to converts a Newick tree to a covariance matrix.

### qpas and cpax

Under the assumption of Hardy Weinberg Equilibrium, the likelihood of assigning an observed genotype `g` in individual `i` at locus `j` to ancestral component `k` is a function of the allelic frequency `f_kj` of the locus at `k` and the fraction of the genome of the individual `q_ik` that comes from that component.  We thus consider the likelihood of the ancestral component proportions vector `Q` and their vector of allele frequencies `F`.  In particular, if we denote `K` as the number of ancestry components, `I` as the number of individuals, and `J` as the number of polymorphic sites among the `I` individuals, then the log likelihood of observing the genotype is:

    sum_i sum_j {
      g_ij * ln[sum_k (q_ik * f_kj)] +
      (2 - g_ij) * ln[sum_k (q_ik * (1 - f_kj))]
    }

To estimate `Q` and `F`, we apply a Newton-style optimization method using quadratic programming through the active set algorithm.  `qpas` operates by solving equality-constraint quadratic problems using the Karush-Kuhn-Tucker algorithm, which is a nonlinear programming generalization of the Lagrange multiplier method.  `cpax` operates through complementarity pivoting using Lemke's algorithm.

Leveraging the block structure of the hessian matrices for `Q` and `F`, we decompose the problem into a sequence of small-matrix manipulations rather than managing one large linear system.  This allows us to update `Q`, row after row, and `F`, column after column.  The optimization task, therefore, becomes feasible.

### nemeco

We model the joint distribution of allele frequencies across all ancestral components as a multivariate Gaussian distribution.  The likelihood denotes the probability of observing the given allele frequencies.  The variances and covariances of the multivariate Gaussian are determined by a product term of two factors.  The first factor is site-specific, and the second factor records the ancestral components' variances and covariances relation.

    P(f_j | C, u_j) ~ N(u_j * (1 - u_j) * C)

We denote `C` as the covariance matrix to be inferred.  We denote the allele frequency at site `j` as `f_j` and average major allele frequency at site `j` as `u_j`.  We root the covariance matrix to avoid multiple likelihood optima and obtain `C'`.  We formulate the log likelihood analytically as the following:

    -0.5 * sum_j {
      (K - 1) * ln(2 * pi * c_j) +
      ln(det(C')) +
      (1/c_j) * (f_j')^T * (1/C') * f_j'
    }
    c_j = u_j * (1 - u_j)
    f_j' = f_j - f_j0

### nemetree

`nemetree` is a web service dedicated to delivering clean, accurate, and presentation-ready visualizations of phylogenetic trees. To find an appealing arrangement of a tree, `nemetree` takes inspiration from an electrostatic field and models tree components as like-signed charged particles with nodes constrained by the branches that connect them. `nemetree` then utilizes the Nelder-Mead algorithm to minimize the total potential energy of this system and achieve an optimal tree layout. `nemetree` allows users to specify tree structures in Newick format and adjust the model and rendering configuration through a JSON editor. `nemetree` animates the progression of the optimization and provides a method to pause and resume the process. All rendered trees may be downloaded as SVG.  What you see is what you get at http://www.jade-cheng.com/trees/

### selscan

We scan for covariance outliers by applying a likelihood model to each locus, similar to the one used genome-wide but with certain scalar factor variations. This creates a nested likelihood model.  Through a likelihood ratio test, it identifies loci in which the variance among components is larger than expected from the genome-wide estimated covariance matrix.

This program takes two input matrices, the `c matrix` and `c-scale matrix`.  These two matrices provide the minimum and maximum values of the optimization and the interpolation is used to define how to optimize multiple values at the same time using a single parameter.  In this way the same framework can be used for both optimization of additive and multiplicative models.  If `-cs` option is not supplied, by default, the programs uses a `c-scale matrix` that is 10 times of the `c matrix`.

## Workflow

A typical workflow of genetic data analysis using `Ohana` starts with structure inference with `qpas` using either genotype observations or genotype likelihoods.  For genotype observations, we first prepare the data with Plink including the `--recode12 --geno 0.0 --tab` flags.  We then convert the .ped file to a .dgm file, which is used by `qpas`.

    $ head -n 3 sample.ped | cut -f1-12
    BDV01    BDV01   0   0   0   -9   2 2   2 2   1 2   2 2   2 2   2 2
    BDV02    BDV02   0   0   0   -9   2 2   2 2   2 2   1 2   2 2   2 2
    BDV03    BDV03   0   0   0   -9   2 2   2 2   1 2   2 2   2 2   2 2

    $ convert ped2dgm ./sample.ped ./g.dgm
    $ qpas ./g.dgm -k 4 -qo q.matrix -fo f.matrix -mi 5
    seed: 2978325876

    iter   duration   log-likelihood   delta-lle
    0      0.188569   -5.871533e+06
    1      0.440765   -2.328232e+06    3.543301e+06
    2      0.474969   -2.286454e+06    4.177773e+04
    3      0.444536   -2.241289e+06    4.516513e+04
    4      0.437131   -2.210211e+06    3.107805e+04
    5      0.431438   -2.186895e+06    2.331641e+04

    Writing Q matrix to q.matrix
    Writing F matrix to f.matrix

    $ head -n 4 q.matrix
    123 4
    4.770576e-02   6.754005e-01   6.820711e-02   2.086867e-01
    4.644102e-02   8.087694e-01   2.872330e-02   1.160663e-01
    3.517502e-04   6.992593e-01   4.722621e-02   2.531627e-01

    $ head -n 4 f.matrix | cut -c1-50
    4 34429
    6.788623e-02   1.000000e-06   3.573932e-01   1.000000e-0
    1.000000e-06   1.154267e-01   4.001837e-01   3.153751e-0
    9.096221e-02   5.293618e-03   9.962799e-02   1.000000e-0

For genotype likelihoods, first prepare the data in beagle format; then convert the it to an .lgm file.

    $ head -n 4 sample.bgl | cut -c1-78
    marker allelel1 allelel2 Ind0     Ind0     Ind0     Ind1     Ind1     Ind1
    0      0        0        0.000133 0.333287 0.666580 0.333333 0.333333 0.333333
    1      0        0        0.000001 0.998406 0.001593 0.333333 0.333333 0.333333
    2      0        0        0.000053 0.999946 0.000001 0.333333 0.333333 0.333333

    $ convert bgl2lgm ./sample.bgl ./g.lgm
    $ qpas ./g.lgm -k 4 -qo ./q.matrix -fo ./f.matrix -mi 5
    seed: 2236408223

    iter   duration    log-likelihood   delta-lle
    0      0.381237    -6.925665e+06
    1      1.647531    -5.092075e+06    1.833590e+06
    2      1.572898    -5.018263e+06    7.381209e+04
    3      1.547442    -4.983535e+06    3.472794e+04
    4      1.533596    -4.952988e+06    3.054679e+04
    5      1.529023    -4.939365e+06    1.362250e+04

    Writing Q matrix to ./q.matrix
    Writing F matrix to ./f.matrix

    $ head -n 4 q.matrix
    265 4
    1.000000e-06   1.679230e-01   7.901021e-01   4.197486e-02
    1.000000e-06   1.677569e-01   8.063353e-01   2.590779e-02
    9.999980e-07   8.379532e-05   9.049327e-01   9.498254e-02

    $ head -n 4 f.matrix | cut -c1-50
    4 22654
    6.919561e-01   8.982128e-01   9.297087e-01   9.245684e-0
    7.056771e-01   9.999990e-01   7.365498e-01   2.245157e-0
    9.999990e-01   3.936945e-01   6.771335e-01   6.756550e-0

The standard output from `qpas` contains four columns: iteration number, time elapsed, log likelihood value, and the difference in log likelihood values between iterations. The `-qo` output file records the admixture inference, which can be used for interpretation directly.

    python ./tools/plot_q.py ./q.matrix ./q-bar-chart.pdf

The `-fo` output file records the allele frequency inference, which can be used to estimate component covariances with `nemeco`.

    $ nemeco ./g.lgm ./f.matrix -co c.matrix -mi 5
    iter   duration   delta-lle      log-likelihood
    1      0.002092   0.000000e+00   -1.693898e+04
    2      0.000860   2.341308e+04   6.474094e+03
    3      0.000859   8.106349e+03   1.458044e+04
    4      0.000857   0.000000e+00   1.458044e+04
    5      0.000857   0.000000e+00   1.458044e+04

    log likelihood = 14580.4
    Writing C matrix to c.matrix

    $ nemeco ./g.dgm ./f.matrix -co ./c.matrix -mi 5
    iter   duration   delta-lle      log-likelihood
    1      0.001374   0.000000e+00   -8.439035e+03
    2      0.000561   1.353884e+04   5.099802e+03
    3      0.000582   5.148315e+03   1.024812e+04
    4      0.000549   0.000000e+00   1.024812e+04
    5      0.000557   0.000000e+00   1.024812e+04

    log likelihood = 10248.1
    Writing C matrix to ./c.matrix

    $ cat c.matrix
    3 3
    1.844469e-01   7.182917e-02   9.315245e-02
    7.182917e-02   1.599473e-01   7.554783e-02
    9.315245e-02   7.554783e-02   2.084383e-01

We approximate the estimated covariance matrix into a component tree using `cov2nwk`, and produce a visualization of the tree using `nwk2svg`.  For a better visualization of the inferred Newick tree, copy the nwk file contents and visit `nemetree` at http://www.jade-cheng.com/trees/

    $ convert cov2nwk ./c.matrix ./tree.nwk

    $ cat ./tree.nwk
    (1:0.0931538,(2:0.0862588,0:0.0736885):0.0194639,3:0.113427);

    $ convert nwk2svg ./tree.nwk ./tree.svg

If the intermediate Newick file is unneeded, it is possible to pipe output through the conversion.

    $ cat ./c.matrix | convert cov2nwk | convert nwk2svg > ./tree.svg

With the inferred component covariances and allele frequencies, we scan for covariance outliers.  The standard output from `selscan` contains four columns, the scalar value when local best likelihood is reached, the local log likelihood obeying global covariances, the local optimal log likelihood, and the likelihood ratio of this locus.  The total number and order of loci match with the input genotype data.

    $ selscan ./g.dgm ./f.matrix c.matrix > lle-ratios.txt
    $ selscan ./g.lgm ./f.matrix c.matrix > lle-ratios.txt

    $ head -n 3 lle-ratios.txt
    scalar          global-lle      local-lle       lle-ratio
    +1.000000e+00   +1.418028e+00   +1.418028e+00   +0.000000e+00
    +1.000000e+00   +1.769128e+00   +1.769128e+00   +0.000000e+00
    +1.000000e+00   +1.213023e+00   +1.213023e+00   +0.000000e+00

The workflow described above is for demonstration purposes only.  For real data analysis, the structure inference using `qpas` would include many fewer loci than a selection scan using `selscan`, which would require multiple millions of loci.  What's shown above, a scan of allele frequencies produced directly from the structure analysis, would be inadequate.

## Selection study

If selection study is the goal, the first step should be to obtain a full genome dataset of high quality.  Then a subset of this data, ~100 Kbp unlinked markers, should be used for the structure analysis with `qpas`.  After that, the `-fq` and `-qi` options should be used for `qpas` to produce admixture-corrected allele frequencies for the full genome dataset.  Finally, it should be possible to proceed to the selection study with full genome allele frequencies.

    $ qpas ./g_subset.lgm -k 4 -qo ./q_subset.matrix -fo ./f_subset.matrix -mi 5
    :
    $ nemeco ./g_subset.lgm ./f_subset.matrix -mi 5 -co ./cout.matrix
    :
    $ qpas ./g_full.dgm -k 4 -qi ./q_subset.matrix -fo ./f_full.matrix -mi 5 -fq
    :
    $ selscan ./g_full.dgm ./f_full.matrix cout.matrix
    :

For faster analysis and smaller memory footprints, we recommend performing the last two steps in parallel.  For example, suppose we have the full genome dataset `HGDP.ped`, we have split it into three pieces, each containing 1/3 of the markers, and the files are named `HGDP1.ped`, `HGDP2.ped`, and `HGDP3.ped`.

We first down-sample the full dataset to perform structure analysis and then infer the component covariances.

    python ./sample-sites.py ./HGDP.dgm 5 ./HGDP_5percent.dgm
    qpas ./HGDP_5percent.dgm -e 0.0001 -k 7 -qo ./HGDP_5percent_Q.matrix -fo ./HGDP_5percent_F.matrix
    nemeco ./HGDP_5percent.dgm ./HGDP_5percent_F.matrix -e 0.0 -co ./HGDP_5percent_C.matrix

We then produce admixture-corrected allele frequencies and perform the selection scan separately, possibly in parallel.

    convert ped2dgm ./HGDP1.ped ./HGDP1.dgm
    convert ped2dgm ./HGDP2.ped ./HGDP2.dgm
    convert ped2dgm ./HGDP3.ped ./HGDP3.dgm

    qpas ./HGDP1.dgm -qi ./HGDP_5percent_Q.matrix -fo ./HGDP1_F.matrix -e 0.0001 -fq
    qpas ./HGDP2.dgm -qi ./HGDP_5percent_Q.matrix -fo ./HGDP2_F.matrix -e 0.0001 -fq
    qpas ./HGDP3.dgm -qi ./HGDP_5percent_Q.matrix -fo ./HGDP3_F.matrix -e 0.0001 -fq

    selscan ./HGDP1.dgm ./HGDP1_F.matrix ./HGDP_5percent_C.matrix > scan1.txt
    selscan ./HGDP2.dgm ./HGDP2_F.matrix ./HGDP_5percent_C.matrix > scan2.txt
    selscan ./HGDP3.dgm ./HGDP3_F.matrix ./HGDP_5percent_C.matrix > scan3.txt

    cat scan*.txt > scan_all.txt

The file `scan_all.txt` contains selection results for the full dataset with markers following the order defined in HGDP.ped.

## License
Copyright (c) 2015-2017 Jade Cheng<br>
All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.

* Neither the name of Jade Cheng nor the names of her contributors may be used to endorse or promote products derived from this software without specific prior written permission.

This software is provided by the copyright holders and contributors "as is" and any express or implied warranties, including, but not limited to, the implied warranties of merchantability and fitness for a particular purpose are disclaimed. in no event shall Jade Cheng be liable for any direct, indirect, incidental, special, exemplary, or consequential damages (including, but not limited to, procurement of substitute goods or services; loss of use, data, or profits; or business interruption) however caused and on any theory of liability, whether in contract, strict liability, or tort (including negligence or otherwise) arising in any way out of the use of this software, even if advised of the possibility of such damage.
