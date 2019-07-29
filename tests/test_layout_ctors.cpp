
#include <gtest/gtest.h>
#include <gtest/gtest-typed-test.h>
#include <experimental/mdspan>


template <class> struct TestLayoutCtors;
template <class Mapping, ptrdiff_t... DynamicSizes>
struct TestLayoutCtors<std::tuple<
  Mapping,
  std::integer_sequence<ptrdiff_t, DynamicSizes...>
>> : public ::testing::Test {
  using mapping_type = Mapping;
  using extents_type = decltype(std::declval<mapping_type>().extents());
  Mapping map = { extents_type{ DynamicSizes... } };
};

template <class Extents, ptrdiff_t... DynamicSizes>
using test_left_type = std::tuple<
  typename std::layout_left::template mapping<Extents>,
  std::integer_sequence<ptrdiff_t, DynamicSizes...>
>;

template <class Extents, ptrdiff_t... DynamicSizes>
using test_right_type = std::tuple<
  typename std::layout_right::template mapping<Extents>,
  std::integer_sequence<ptrdiff_t, DynamicSizes...>
>;

using layout_test_types =
  ::testing::Types<
    test_left_type<std::extents<10>>,
    test_right_type<std::extents<10>>,
    //----------
    test_left_type<std::extents<std::dynamic_extent>, 10>,
    test_right_type<std::extents<std::dynamic_extent>, 10>,
    //----------
    test_left_type<std::extents<std::dynamic_extent, 10>, 5>,
    test_left_type<std::extents<5, std::dynamic_extent>, 10>,
    test_left_type<std::extents<5, 10>>,
    test_right_type<std::extents<std::dynamic_extent, 10>, 5>,
    test_right_type<std::extents<5, std::dynamic_extent>, 10>,
    test_right_type<std::extents<5, 10>>
  >;

TYPED_TEST_SUITE(TestLayoutCtors, layout_test_types);

TYPED_TEST(TestLayoutCtors, default_ctor) {
  // Default constructor ensures extents() == Extents() is true.
  auto m = typename TestFixture::mapping_type();
  ASSERT_EQ(m.extents(), typename TestFixture::extents_type());
  auto m2 = typename TestFixture::mapping_type{};
  ASSERT_EQ(m2.extents(), typename TestFixture::extents_type{});
  ASSERT_EQ(m, m2);
}

template <class> struct TestLayoutCompatCtors;
template <class Mapping, ptrdiff_t... DynamicSizes, class Mapping2, ptrdiff_t... DynamicSizes2>
struct TestLayoutCompatCtors<std::tuple<
  Mapping,
  std::integer_sequence<ptrdiff_t, DynamicSizes...>,
  Mapping2,
  std::integer_sequence<ptrdiff_t, DynamicSizes2...>
>> : public ::testing::Test {
  using mapping_type1 = Mapping;
  using mapping_type2 = Mapping2;
  using extents_type1 = decltype(std::declval<mapping_type1>().extents());
  using extents_type2 = decltype(std::declval<mapping_type2>().extents());
  Mapping map1 = { extents_type1{ DynamicSizes... } };
  Mapping2 map2 = { extents_type2{ DynamicSizes2... } };
};

template <class E1, class S1, class E2, class S2>
using test_left_type_compatible = std::tuple<
  typename std::layout_left::template mapping<E1>, S1,
  typename std::layout_left::template mapping<E2>, S2
>;
template <class E1, class S1, class E2, class S2>
using test_right_type_compatible = std::tuple<
  typename std::layout_right::template mapping<E1>, S1,
  typename std::layout_right::template mapping<E2>, S2
>;
template <ptrdiff_t... Ds>
using _sizes = std::integer_sequence<ptrdiff_t, Ds...>;
template <ptrdiff_t... Ds>
using _exts = std::extents<Ds...>;

template <template <class, class, class, class> class _test_case_type>
using compatible_layout_test_types =
  ::testing::Types<
    _test_case_type<_exts<std::dynamic_extent>, _sizes<10>, _exts<10>, _sizes<>>,
    //--------------------
    _test_case_type<_exts<std::dynamic_extent, 10>, _sizes<5>, _exts<5, std::dynamic_extent>, _sizes<10>>,
    _test_case_type<_exts<std::dynamic_extent, std::dynamic_extent>, _sizes<5, 10>, _exts<5, std::dynamic_extent>, _sizes<10>>,
    _test_case_type<_exts<std::dynamic_extent, std::dynamic_extent>, _sizes<5, 10>, _exts<std::dynamic_extent, 10>, _sizes<5>>,
    _test_case_type<_exts<std::dynamic_extent, std::dynamic_extent>, _sizes<5, 10>, _exts<5, 10>, _sizes<>>,
    _test_case_type<_exts<5, 10>, _sizes<>, _exts<5, std::dynamic_extent>, _sizes<10>>,
    _test_case_type<_exts<5, 10>, _sizes<>, _exts<std::dynamic_extent, 10>, _sizes<5>>,
    //--------------------
    _test_case_type<_exts<std::dynamic_extent, std::dynamic_extent, 15>, _sizes<5, 10>, _exts<5, std::dynamic_extent, 15>, _sizes<10>>,
    _test_case_type<_exts<5, 10, 15>, _sizes<>, _exts<5, std::dynamic_extent, 15>, _sizes<10>>,
    _test_case_type<_exts<5, 10, 15>, _sizes<>, _exts<std::dynamic_extent, std::dynamic_extent, std::dynamic_extent>, _sizes<5, 10, 15>>
  >;

using left_compatible_test_types = compatible_layout_test_types<test_left_type_compatible>;
using right_compatible_test_types = compatible_layout_test_types<test_right_type_compatible>;

template <class T> struct TestLayoutLeftCompatCtors : TestLayoutCompatCtors<T> { };
template <class T> struct TestLayoutRightCompatCtors : TestLayoutCompatCtors<T> { };

TYPED_TEST_SUITE(TestLayoutLeftCompatCtors, left_compatible_test_types);
TYPED_TEST_SUITE(TestLayoutRightCompatCtors, right_compatible_test_types);

TYPED_TEST(TestLayoutLeftCompatCtors, compatible_construct_1) {
  // The compatible mapping constructor ensures extents() == other.extents() is true.
  auto m1 = typename TestFixture::mapping_type1(this->map2);
  ASSERT_EQ(m1.extents(), this->map2.extents());
}

TYPED_TEST(TestLayoutRightCompatCtors, compatible_construct_1) {
  // The compatible mapping constructor ensures extents() == other.extents() is true.
  auto m1 = typename TestFixture::mapping_type1(this->map2);
  ASSERT_EQ(m1.extents(), this->map2.extents());
}

TYPED_TEST(TestLayoutLeftCompatCtors, compatible_construct_2) {
  // The compatible mapping constructor ensures extents() == other.extents() is true.
  auto m2 = typename TestFixture::mapping_type2(this->map1);
  ASSERT_EQ(m2.extents(), this->map1.extents());
}

TYPED_TEST(TestLayoutRightCompatCtors, compatible_construct_2) {
  // The compatible mapping constructor ensures extents() == other.extents() is true.
  auto m2 = typename TestFixture::mapping_type2(this->map1);
  ASSERT_EQ(m2.extents(), this->map1.extents());
}

TYPED_TEST(TestLayoutLeftCompatCtors, compatible_assign_1) {
  this->map1 = this->map2;
  ASSERT_EQ(this->map1.extents(), this->map2.extents());
}

TYPED_TEST(TestLayoutRightCompatCtors, compatible_assign_1) {
  this->map1 = this->map2;
  ASSERT_EQ(this->map1.extents(), this->map2.extents());
}

TYPED_TEST(TestLayoutLeftCompatCtors, compatible_assign_2) {
  this->map2 = this->map1;
  ASSERT_EQ(this->map1.extents(), this->map2.extents());
}

TYPED_TEST(TestLayoutRightCompatCtors, compatible_assign_2) {
  this->map2 = this->map1;
  ASSERT_EQ(this->map1.extents(), this->map2.extents());
}