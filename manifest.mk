
DEBUG_CONVERT = tmp/debug/src/convert/jade.main.o

tmp/debug/src/convert/jade.main.o: src/convert/jade.main.cpp src/lib/jade.args.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/convert/jade.bgl2lgm.hpp src/lib/jade.bgl_reader.hpp src/convert/jade.cov2nwk.hpp src/lib/jade.neighbor_joining.hpp src/lib/jade.matrix.hpp src/lib/jade.openblas.hpp src/lib/jade.verification.hpp src/lib/jade.genotype_matrix.hpp src/convert/jade.nwk2cov.hpp src/lib/jade.rerooted_tree.hpp src/lib/jade.tree_path.hpp src/lib/jade.newick.hpp src/lib/jade.scanner.hpp src/convert/jade.nwk2svg.hpp src/lib/jade.svg_tree.hpp src/lib/jade.simplex.hpp src/lib/jade.vec2.hpp src/convert/jade.ped2dgm.hpp src/lib/jade.ped_reader.hpp src/lib/jade.version.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Isrc/convert)

DEBUG_CPAX = tmp/debug/src/cpax/jade.main.o

tmp/debug/src/cpax/jade.main.o: src/cpax/jade.main.cpp src/cpax/jade.optimizer.hpp src/cpax/jade.improver.hpp src/cpax/jade.forced_grouping.hpp src/lib/jade.randomizer.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.openblas.hpp src/lib/jade.lemke.hpp src/lib/jade.verification.hpp src/lib/jade.genotype_matrix.hpp src/cpax/jade.settings.hpp src/lib/jade.genotype_matrix_factory.hpp src/lib/jade.discrete_genotype_matrix.hpp src/lib/jade.genotype.hpp src/lib/jade.likelihood_genotype_matrix.hpp src/cpax/jade.options.hpp src/lib/jade.args.hpp src/lib/jade.stopwatch.hpp src/lib/jade.version.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Isrc/cpax)

DEBUG_NEMECO = tmp/debug/src/nemeco/jade.main.o

tmp/debug/src/nemeco/jade.main.o: src/nemeco/jade.main.cpp src/nemeco/jade.optimizer.hpp src/nemeco/jade.controller_factory.hpp src/nemeco/jade.tree_controller.hpp src/nemeco/jade.controller.hpp src/lib/jade.likelihood.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.openblas.hpp src/nemeco/jade.settings.hpp src/lib/jade.genotype_matrix_factory.hpp src/lib/jade.discrete_genotype_matrix.hpp src/lib/jade.genotype.hpp src/lib/jade.verification.hpp src/lib/jade.genotype_matrix.hpp src/lib/jade.likelihood_genotype_matrix.hpp src/nemeco/jade.options.hpp src/lib/jade.args.hpp src/lib/jade.simplex.hpp src/lib/jade.stopwatch.hpp src/lib/jade.rerooted_tree.hpp src/lib/jade.tree_path.hpp src/lib/jade.newick.hpp src/lib/jade.scanner.hpp src/nemeco/jade.treeless_controller.hpp src/lib/jade.version.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Isrc/nemeco)

DEBUG_QPAS = tmp/debug/src/qpas/jade.main.o

tmp/debug/src/qpas/jade.main.o: src/qpas/jade.main.cpp src/qpas/jade.optimizer.hpp src/qpas/jade.improver.hpp src/qpas/jade.forced_grouping.hpp src/lib/jade.randomizer.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.openblas.hpp src/qpas/jade.qpas.hpp src/lib/jade.verification.hpp src/lib/jade.genotype_matrix.hpp src/qpas/jade.settings.hpp src/lib/jade.genotype_matrix_factory.hpp src/lib/jade.discrete_genotype_matrix.hpp src/lib/jade.genotype.hpp src/lib/jade.likelihood_genotype_matrix.hpp src/qpas/jade.options.hpp src/lib/jade.args.hpp src/lib/jade.stopwatch.hpp src/lib/jade.version.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Isrc/qpas)

DEBUG_SELSCAN = tmp/debug/src/selscan/jade.main.o

tmp/debug/src/selscan/jade.main.o: src/selscan/jade.main.cpp src/selscan/jade.selscan.hpp src/lib/jade.args.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.genotype_matrix_factory.hpp src/lib/jade.discrete_genotype_matrix.hpp src/lib/jade.genotype.hpp src/lib/jade.verification.hpp src/lib/jade.genotype_matrix.hpp src/lib/jade.matrix.hpp src/lib/jade.openblas.hpp src/lib/jade.likelihood_genotype_matrix.hpp src/lib/jade.version.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Isrc/selscan)

./bin/debug/convert: $(DEBUG_CONVERT)
	@ $(call .link,$@,$^,$(DEBUG_LDFLAGS))
./bin/debug/cpax: $(DEBUG_CPAX)
	@ $(call .link,$@,$^,$(DEBUG_LDFLAGS))
./bin/debug/nemeco: $(DEBUG_NEMECO)
	@ $(call .link,$@,$^,$(DEBUG_LDFLAGS))
./bin/debug/qpas: $(DEBUG_QPAS)
	@ $(call .link,$@,$^,$(DEBUG_LDFLAGS))
./bin/debug/selscan: $(DEBUG_SELSCAN)
	@ $(call .link,$@,$^,$(DEBUG_LDFLAGS))

.debug: \
	./bin/debug/convert \
	./bin/debug/cpax \
	./bin/debug/nemeco \
	./bin/debug/qpas \
	./bin/debug/selscan

DEBUG_TEST_CONVERT = tmp/debug/test/convert/test.main.o

tmp/debug/test/convert/test.main.o: test/convert/test.main.cpp test/convert/test.main.hpp test/test.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/convert -Itest/convert)

DEBUG_TEST_CPAX = tmp/debug/test/cpax/test.main.o

tmp/debug/test/cpax/test.main.o: test/cpax/test.main.cpp test/cpax/test.main.hpp test/test.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/cpax -Itest/cpax)

DEBUG_TEST_LIB = tmp/debug/test/lib/test.args.o tmp/debug/test/lib/test.discrete_genotype_matrix.o tmp/debug/test/lib/test.error.o tmp/debug/test/lib/test.lemke.o tmp/debug/test/lib/test.likelihood_genotype_matrix.o tmp/debug/test/lib/test.main.o tmp/debug/test/lib/test.matrix.o tmp/debug/test/lib/test.neighbor_joining.o tmp/debug/test/lib/test.newick.o tmp/debug/test/lib/test.scanner.o tmp/debug/test/lib/test.simplex.o tmp/debug/test/lib/test.stopwatch.o tmp/debug/test/lib/test.svg_tree.o tmp/debug/test/lib/test.vec2.o

tmp/debug/test/lib/test.args.o: test/lib/test.args.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.args.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/debug/test/lib/test.discrete_genotype_matrix.o: test/lib/test.discrete_genotype_matrix.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.discrete_genotype_matrix.hpp src/lib/jade.genotype.hpp src/lib/jade.system.hpp src/lib/jade.verification.hpp src/lib/jade.genotype_matrix.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.openblas.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/debug/test/lib/test.error.o: test/lib/test.error.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/debug/test/lib/test.lemke.o: test/lib/test.lemke.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.lemke.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.openblas.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/debug/test/lib/test.likelihood_genotype_matrix.o: test/lib/test.likelihood_genotype_matrix.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.likelihood_genotype_matrix.hpp src/lib/jade.genotype_matrix.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.openblas.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/debug/test/lib/test.main.o: test/lib/test.main.cpp test/lib/test.main.hpp test/test.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/debug/test/lib/test.matrix.o: test/lib/test.matrix.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.openblas.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/debug/test/lib/test.neighbor_joining.o: test/lib/test.neighbor_joining.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.neighbor_joining.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.openblas.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/debug/test/lib/test.newick.o: test/lib/test.newick.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.newick.hpp src/lib/jade.scanner.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/debug/test/lib/test.scanner.o: test/lib/test.scanner.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.scanner.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/debug/test/lib/test.simplex.o: test/lib/test.simplex.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.simplex.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/debug/test/lib/test.stopwatch.o: test/lib/test.stopwatch.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.stopwatch.hpp src/lib/jade.system.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/debug/test/lib/test.svg_tree.o: test/lib/test.svg_tree.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.svg_tree.hpp src/lib/jade.newick.hpp src/lib/jade.scanner.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.simplex.hpp src/lib/jade.vec2.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/debug/test/lib/test.vec2.o: test/lib/test.vec2.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.vec2.hpp src/lib/jade.system.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)

DEBUG_TEST_NEMECO = tmp/debug/test/nemeco/test.main.o

tmp/debug/test/nemeco/test.main.o: test/nemeco/test.main.cpp test/nemeco/test.main.hpp test/test.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/nemeco -Itest/nemeco)

DEBUG_TEST_QPAS = tmp/debug/test/qpas/test.main.o

tmp/debug/test/qpas/test.main.o: test/qpas/test.main.cpp test/qpas/test.main.hpp test/test.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/qpas -Itest/qpas)

DEBUG_TEST_SELSCAN = tmp/debug/test/selscan/test.main.o

tmp/debug/test/selscan/test.main.o: test/selscan/test.main.cpp test/selscan/test.main.hpp test/test.hpp
	@ $(call .compile,$<,$@,$(DEBUG_CXXFLAGS) -Isrc/lib -Itest -Isrc/selscan -Itest/selscan)

./bin/debug/test-convert: $(DEBUG_TEST_CONVERT)
	@ $(call .link,$@,$^,$(DEBUG_LDFLAGS))
./bin/debug/test-cpax: $(DEBUG_TEST_CPAX)
	@ $(call .link,$@,$^,$(DEBUG_LDFLAGS))
./bin/debug/test-lib: $(DEBUG_TEST_LIB)
	@ $(call .link,$@,$^,$(DEBUG_LDFLAGS))
./bin/debug/test-nemeco: $(DEBUG_TEST_NEMECO)
	@ $(call .link,$@,$^,$(DEBUG_LDFLAGS))
./bin/debug/test-qpas: $(DEBUG_TEST_QPAS)
	@ $(call .link,$@,$^,$(DEBUG_LDFLAGS))
./bin/debug/test-selscan: $(DEBUG_TEST_SELSCAN)
	@ $(call .link,$@,$^,$(DEBUG_LDFLAGS))

.test-debug: \
	./bin/debug/test-convert \
	./bin/debug/test-cpax \
	./bin/debug/test-lib \
	./bin/debug/test-nemeco \
	./bin/debug/test-qpas \
	./bin/debug/test-selscan

	@ ./bin/debug/test-convert
	@ ./bin/debug/test-cpax
	@ ./bin/debug/test-lib
	@ ./bin/debug/test-nemeco
	@ ./bin/debug/test-qpas
	@ ./bin/debug/test-selscan


RELEASE_CONVERT = tmp/release/src/convert/jade.main.o

tmp/release/src/convert/jade.main.o: src/convert/jade.main.cpp src/lib/jade.args.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/convert/jade.bgl2lgm.hpp src/lib/jade.bgl_reader.hpp src/convert/jade.cov2nwk.hpp src/lib/jade.neighbor_joining.hpp src/lib/jade.matrix.hpp src/lib/jade.openblas.hpp src/lib/jade.verification.hpp src/lib/jade.genotype_matrix.hpp src/convert/jade.nwk2cov.hpp src/lib/jade.rerooted_tree.hpp src/lib/jade.tree_path.hpp src/lib/jade.newick.hpp src/lib/jade.scanner.hpp src/convert/jade.nwk2svg.hpp src/lib/jade.svg_tree.hpp src/lib/jade.simplex.hpp src/lib/jade.vec2.hpp src/convert/jade.ped2dgm.hpp src/lib/jade.ped_reader.hpp src/lib/jade.version.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Isrc/convert)

RELEASE_CPAX = tmp/release/src/cpax/jade.main.o

tmp/release/src/cpax/jade.main.o: src/cpax/jade.main.cpp src/cpax/jade.optimizer.hpp src/cpax/jade.improver.hpp src/cpax/jade.forced_grouping.hpp src/lib/jade.randomizer.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.openblas.hpp src/lib/jade.lemke.hpp src/lib/jade.verification.hpp src/lib/jade.genotype_matrix.hpp src/cpax/jade.settings.hpp src/lib/jade.genotype_matrix_factory.hpp src/lib/jade.discrete_genotype_matrix.hpp src/lib/jade.genotype.hpp src/lib/jade.likelihood_genotype_matrix.hpp src/cpax/jade.options.hpp src/lib/jade.args.hpp src/lib/jade.stopwatch.hpp src/lib/jade.version.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Isrc/cpax)

RELEASE_NEMECO = tmp/release/src/nemeco/jade.main.o

tmp/release/src/nemeco/jade.main.o: src/nemeco/jade.main.cpp src/nemeco/jade.optimizer.hpp src/nemeco/jade.controller_factory.hpp src/nemeco/jade.tree_controller.hpp src/nemeco/jade.controller.hpp src/lib/jade.likelihood.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.openblas.hpp src/nemeco/jade.settings.hpp src/lib/jade.genotype_matrix_factory.hpp src/lib/jade.discrete_genotype_matrix.hpp src/lib/jade.genotype.hpp src/lib/jade.verification.hpp src/lib/jade.genotype_matrix.hpp src/lib/jade.likelihood_genotype_matrix.hpp src/nemeco/jade.options.hpp src/lib/jade.args.hpp src/lib/jade.simplex.hpp src/lib/jade.stopwatch.hpp src/lib/jade.rerooted_tree.hpp src/lib/jade.tree_path.hpp src/lib/jade.newick.hpp src/lib/jade.scanner.hpp src/nemeco/jade.treeless_controller.hpp src/lib/jade.version.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Isrc/nemeco)

RELEASE_QPAS = tmp/release/src/qpas/jade.main.o

tmp/release/src/qpas/jade.main.o: src/qpas/jade.main.cpp src/qpas/jade.optimizer.hpp src/qpas/jade.improver.hpp src/qpas/jade.forced_grouping.hpp src/lib/jade.randomizer.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.openblas.hpp src/qpas/jade.qpas.hpp src/lib/jade.verification.hpp src/lib/jade.genotype_matrix.hpp src/qpas/jade.settings.hpp src/lib/jade.genotype_matrix_factory.hpp src/lib/jade.discrete_genotype_matrix.hpp src/lib/jade.genotype.hpp src/lib/jade.likelihood_genotype_matrix.hpp src/qpas/jade.options.hpp src/lib/jade.args.hpp src/lib/jade.stopwatch.hpp src/lib/jade.version.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Isrc/qpas)

RELEASE_SELSCAN = tmp/release/src/selscan/jade.main.o

tmp/release/src/selscan/jade.main.o: src/selscan/jade.main.cpp src/selscan/jade.selscan.hpp src/lib/jade.args.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.genotype_matrix_factory.hpp src/lib/jade.discrete_genotype_matrix.hpp src/lib/jade.genotype.hpp src/lib/jade.verification.hpp src/lib/jade.genotype_matrix.hpp src/lib/jade.matrix.hpp src/lib/jade.openblas.hpp src/lib/jade.likelihood_genotype_matrix.hpp src/lib/jade.version.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Isrc/selscan)

./bin/convert: $(RELEASE_CONVERT)
	@ $(call .link,$@,$^,$(RELEASE_LDFLAGS))
./bin/cpax: $(RELEASE_CPAX)
	@ $(call .link,$@,$^,$(RELEASE_LDFLAGS))
./bin/nemeco: $(RELEASE_NEMECO)
	@ $(call .link,$@,$^,$(RELEASE_LDFLAGS))
./bin/qpas: $(RELEASE_QPAS)
	@ $(call .link,$@,$^,$(RELEASE_LDFLAGS))
./bin/selscan: $(RELEASE_SELSCAN)
	@ $(call .link,$@,$^,$(RELEASE_LDFLAGS))

.release: \
	./bin/convert \
	./bin/cpax \
	./bin/nemeco \
	./bin/qpas \
	./bin/selscan

RELEASE_TEST_CONVERT = tmp/release/test/convert/test.main.o

tmp/release/test/convert/test.main.o: test/convert/test.main.cpp test/convert/test.main.hpp test/test.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/convert -Itest/convert)

RELEASE_TEST_CPAX = tmp/release/test/cpax/test.main.o

tmp/release/test/cpax/test.main.o: test/cpax/test.main.cpp test/cpax/test.main.hpp test/test.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/cpax -Itest/cpax)

RELEASE_TEST_LIB = tmp/release/test/lib/test.args.o tmp/release/test/lib/test.discrete_genotype_matrix.o tmp/release/test/lib/test.error.o tmp/release/test/lib/test.lemke.o tmp/release/test/lib/test.likelihood_genotype_matrix.o tmp/release/test/lib/test.main.o tmp/release/test/lib/test.matrix.o tmp/release/test/lib/test.neighbor_joining.o tmp/release/test/lib/test.newick.o tmp/release/test/lib/test.scanner.o tmp/release/test/lib/test.simplex.o tmp/release/test/lib/test.stopwatch.o tmp/release/test/lib/test.svg_tree.o tmp/release/test/lib/test.vec2.o

tmp/release/test/lib/test.args.o: test/lib/test.args.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.args.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/release/test/lib/test.discrete_genotype_matrix.o: test/lib/test.discrete_genotype_matrix.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.discrete_genotype_matrix.hpp src/lib/jade.genotype.hpp src/lib/jade.system.hpp src/lib/jade.verification.hpp src/lib/jade.genotype_matrix.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.openblas.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/release/test/lib/test.error.o: test/lib/test.error.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/release/test/lib/test.lemke.o: test/lib/test.lemke.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.lemke.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.openblas.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/release/test/lib/test.likelihood_genotype_matrix.o: test/lib/test.likelihood_genotype_matrix.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.likelihood_genotype_matrix.hpp src/lib/jade.genotype_matrix.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.openblas.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/release/test/lib/test.main.o: test/lib/test.main.cpp test/lib/test.main.hpp test/test.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/release/test/lib/test.matrix.o: test/lib/test.matrix.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.openblas.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/release/test/lib/test.neighbor_joining.o: test/lib/test.neighbor_joining.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.neighbor_joining.hpp src/lib/jade.matrix.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.openblas.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/release/test/lib/test.newick.o: test/lib/test.newick.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.newick.hpp src/lib/jade.scanner.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/release/test/lib/test.scanner.o: test/lib/test.scanner.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.scanner.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/release/test/lib/test.simplex.o: test/lib/test.simplex.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.simplex.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/release/test/lib/test.stopwatch.o: test/lib/test.stopwatch.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.stopwatch.hpp src/lib/jade.system.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/release/test/lib/test.svg_tree.o: test/lib/test.svg_tree.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.svg_tree.hpp src/lib/jade.newick.hpp src/lib/jade.scanner.hpp src/lib/jade.error.hpp src/lib/jade.assert.hpp src/lib/jade.system.hpp src/lib/jade.simplex.hpp src/lib/jade.vec2.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)
tmp/release/test/lib/test.vec2.o: test/lib/test.vec2.cpp test/lib/test.main.hpp test/test.hpp src/lib/jade.vec2.hpp src/lib/jade.system.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/lib -Itest/lib)

RELEASE_TEST_NEMECO = tmp/release/test/nemeco/test.main.o

tmp/release/test/nemeco/test.main.o: test/nemeco/test.main.cpp test/nemeco/test.main.hpp test/test.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/nemeco -Itest/nemeco)

RELEASE_TEST_QPAS = tmp/release/test/qpas/test.main.o

tmp/release/test/qpas/test.main.o: test/qpas/test.main.cpp test/qpas/test.main.hpp test/test.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/qpas -Itest/qpas)

RELEASE_TEST_SELSCAN = tmp/release/test/selscan/test.main.o

tmp/release/test/selscan/test.main.o: test/selscan/test.main.cpp test/selscan/test.main.hpp test/test.hpp
	@ $(call .compile,$<,$@,$(RELEASE_CXXFLAGS) -Isrc/lib -Itest -Isrc/selscan -Itest/selscan)

./bin/release/test-convert: $(RELEASE_TEST_CONVERT)
	@ $(call .link,$@,$^,$(RELEASE_LDFLAGS))
./bin/release/test-cpax: $(RELEASE_TEST_CPAX)
	@ $(call .link,$@,$^,$(RELEASE_LDFLAGS))
./bin/release/test-lib: $(RELEASE_TEST_LIB)
	@ $(call .link,$@,$^,$(RELEASE_LDFLAGS))
./bin/release/test-nemeco: $(RELEASE_TEST_NEMECO)
	@ $(call .link,$@,$^,$(RELEASE_LDFLAGS))
./bin/release/test-qpas: $(RELEASE_TEST_QPAS)
	@ $(call .link,$@,$^,$(RELEASE_LDFLAGS))
./bin/release/test-selscan: $(RELEASE_TEST_SELSCAN)
	@ $(call .link,$@,$^,$(RELEASE_LDFLAGS))

.test-release: \
	./bin/release/test-convert \
	./bin/release/test-cpax \
	./bin/release/test-lib \
	./bin/release/test-nemeco \
	./bin/release/test-qpas \
	./bin/release/test-selscan

	@ ./bin/release/test-convert
	@ ./bin/release/test-cpax
	@ ./bin/release/test-lib
	@ ./bin/release/test-nemeco
	@ ./bin/release/test-qpas
	@ ./bin/release/test-selscan

