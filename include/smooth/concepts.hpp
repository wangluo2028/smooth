#ifndef SMOOTH__CONCEPTS_HPP_
#define SMOOTH__CONCEPTS_HPP_

#include <Eigen/Core>

#include <concepts>
#include <random>

#include "common.hpp"


namespace smooth
{

/**
 * @brief Storage concept: requires operator[] to access one of N ints
 *
 * @tparam S storage type
 * @tparam Scalar scalar type
 * @tparam N number of scalars in storage
 */
template<typename S>
concept StorageLike = requires {
  typename S::Scalar;
  {S::SizeAtCompileTime}->std::convertible_to<uint32_t>;
} &&
requires(const S & s, int i) {
  {s[i]}->std::convertible_to<typename S::Scalar>;
};

// Storage that can be mapped
template<typename S>
concept MappableStorageLike = StorageLike<S>&&
  requires(const S & s) {
  {s.data()}->std::same_as<const typename S::Scalar *>;
};

// Storage that can be both mapped and modified
template<typename S>
concept ModifiableStorageLike =
  MappableStorageLike<S>&&
  requires(S & s, int i) {
  {s.data()}->std::same_as<typename S::Scalar *>;
  {s[i]}->std::convertible_to<typename S::Scalar &>;
};

/**
 * @brief Storage that is not modifiable
 */
template<typename S>
concept ConstStorageLike = StorageLike<S>&& !ModifiableStorageLike<S>;


template<typename G>
concept LieGroupLike =
// typedefs
  requires {
  typename G::Scalar;
  typename G::Group;
  typename G::Tangent;
  typename G::TangentMap;
  typename G::Vector;
  typename G::MatrixGroup;
} &&
// static constants
requires {
  {G::lie_size}->std::same_as<const uint32_t &>;      // size of representation
  {G::lie_dim}->std::same_as<const uint32_t &>;       // side of square matrix group
  {G::lie_dof}->std::same_as<const uint32_t &>;       // degrees of freedom (tangent space dimension)
  {G::lie_actdim}->std::same_as<const uint32_t &>;   // dimension of vector space on which group acts
} &&
std::is_same_v<typename G::Tangent::Scalar, typename G::Scalar>&&
std::is_base_of_v<Eigen::MatrixBase<typename G::Tangent>, typename G::Tangent>&&
std::is_same_v<typename G::TangentMap::Scalar, typename G::Scalar>&&
std::is_base_of_v<Eigen::MatrixBase<typename G::TangentMap>, typename G::TangentMap>&&
std::is_same_v<typename G::Vector::Scalar, typename G::Scalar>&&
std::is_base_of_v<Eigen::MatrixBase<typename G::Vector>, typename G::Vector>&&
std::is_same_v<typename G::MatrixGroup::Scalar, typename G::Scalar>&&
std::is_base_of_v<Eigen::MatrixBase<typename G::MatrixGroup>, typename G::MatrixGroup>&&
(G::Tangent::RowsAtCompileTime == G::lie_dof) &&
(G::Tangent::ColsAtCompileTime == 1) &&
(G::TangentMap::RowsAtCompileTime == G::lie_dof) &&
(G::TangentMap::ColsAtCompileTime == G::lie_dof) &&
(G::Vector::RowsAtCompileTime == G::lie_actdim) &&
(G::Vector::ColsAtCompileTime == 1) &&
(G::MatrixGroup::RowsAtCompileTime == G::lie_dim) &&
(G::MatrixGroup::ColsAtCompileTime == G::lie_dim) &&
// member methods
requires(const G & g1, const G & g2, const typename G::Vector & v)
{
  {g1.template cast<double>()};
  {g1.template cast<float>()};
  {g1.inverse()}->std::same_as<typename G::Group>;
  {g1 * v}->std::same_as<typename G::Vector>;
  {g1 * g2}->std::same_as<typename G::Group>;
  {g1.log()}->std::same_as<typename G::Tangent>;
  {g1.matrix_group()}->std::same_as<typename G::MatrixGroup>;
  {g1.Ad()}->std::same_as<typename G::TangentMap>;
} &&
// static methods
requires(const typename G::Tangent & a, std::default_random_engine & rng)
{
  {G::Identity()}->std::same_as<typename G::Group>;
  {G::Random(rng)}->std::same_as<typename G::Group>;
  {G::exp(a)}->std::same_as<typename G::Group>;
  {G::ad(a)}->std::same_as<typename G::TangentMap>;
  {G::hat(a)}->std::same_as<typename G::MatrixGroup>;
};

template<typename G>
concept ModifiableLieGroupLike = LieGroupLike<G>&&
// std::is_constructible_v<G> &&
// member methods
  requires(G & t, std::default_random_engine & rng)
{
  {t.setIdentity()}->std::same_as<void>;
  {t.setRandom(rng)}->std::same_as<void>;
  {t.data()}->std::same_as<typename G::Scalar *>;
};


template<typename T>
struct is_eigen_vec : public std::false_type {};

template<typename Scalar, int rows>
struct is_eigen_vec<Eigen::Matrix<Scalar, rows, 1>> : public std::true_type {};


template<typename G>
concept EnLike =
requires {
  typename G::Scalar;
} &&
(is_eigen_vec<G>::value);

} // namespace smooth

#endif  // SMOOTH__CONCEPTS_HPP_
