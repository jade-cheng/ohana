/* -------------------------------------------------------------------------
   Ohana
   Copyright (c) 2015-2017 Jade Cheng                            (\___/)
   Jade Cheng <info@jade-cheng.com>                              (='.'=)
   ------------------------------------------------------------------------- */

#ifndef JADE_GENOTYPE_MATRIX_FACTORY_HPP__
#define JADE_GENOTYPE_MATRIX_FACTORY_HPP__

#include "jade.discrete_genotype_matrix.hpp"
#include "jade.likelihood_genotype_matrix.hpp"

namespace jade
{
    ///
    /// A template for a class that creates genotype matrices based on files and
    /// their file extensions.
    ///
    template <typename TValue>
    class basic_genotype_matrix_factory
    {
    public:
        /// The value type.
        typedef TValue value_type;

        /// The genotype matrix type.
        typedef basic_genotype_matrix<value_type>
            genotype_matrix_type;

        /// The discrete genotype matrix type.
        typedef basic_discrete_genotype_matrix<value_type>
            discrete_genotype_matrix_type;

        /// The likelihood genotype matrix type.
        typedef basic_likelihood_genotype_matrix<value_type>
            likelihood_genotype_matrix_type;

        ///
        /// Creates a genotype matrix based on values from a file. This function
        /// determines what kind of genotype matrix to create based on the file
        /// extension of the specified path.
        ///
        /// \return A pointer to a new genotype matrix.
        ///
        /// \throws An exception if the path has no file extension or if the
        /// file cannot be parsed successfully.
        ///
        static genotype_matrix_type * create(
                const std::string & path) ///< The path to the file.
        {
            const auto period = path.find_last_of('.');
            if (period == std::string::npos)
                throw error() << "missing file extension for G matrix '"
                              << path << "'.";

            const auto extension = path.substr(period);
            if (extension == ".dgm")
                return new discrete_genotype_matrix_type(path);

            if (extension == ".lgm")
                return new likelihood_genotype_matrix_type(path);

            throw error() << "unsupported file extension for G matrix '"
                          << path << "'.";
        }

        ///
        /// Creates a genotype matrix based on values from a file. This function
        /// determines what kind of genotype matrix to create based on the file
        /// extension of the specified path.
        ///
        /// \return A pointer to a new genotype matrix.
        ///
        /// \throws An exception if the path has no file extension or if the
        /// file cannot be parsed successfully.
        ///
        inline static genotype_matrix_type * create(
                char const * const path) ///< The path to the file.
        {
            assert(path != nullptr);
            return create(std::string(path));
        }
    };
}

#endif // JADE_GENOTYPE_MATRIX_FACTORY_HPP__
