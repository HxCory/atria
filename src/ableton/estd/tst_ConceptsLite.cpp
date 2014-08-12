// Copyright: 2014, Ableton AG, Berlin. All rights reserved.

#include <ableton/estd/ConceptsLite.hpp>
#include <gtest/gtest.h>

namespace ableton {
namespace estd {

//!
// This is a example concept kinda compatible with C++1y concepts
// lite. The problem with this mechanism is that it rises compilation
// errors when the concept is not met, making it non suitable for
// SFINAE-friendly concepts.
//
template <typename T>
constexpr bool Example_concept()
{
  using namespace std;

  return Valid<decltype(
    expressions(
      // Dereferenceable
      *declval<T>(),
      // Has value type
      declval<Value_type<T>>(),
      // Can forward dereferenced to value type
      forward<Value_type<T>>( *declval<T>() ),
      // Has an action() method that may return void
      (declval<T>().action(), canBeVoid)
      ))>();
}

//!
// Models the concept:
//   Example_concept
//
struct ExampleModel
{
  typedef int value_type;
  int operator*() { return 0; };
  void action() {};
};

//!
// Example non-model, missing action
//
struct ExampleNonModel
{
  typedef int value_type;
  std::string operator*() { return 0; };
  void action() {};
};

TEST(SimpleConcept, CanBeEvaluatedLikeAConceptLite)
{
  EXPECT_TRUE(Example_concept<ExampleModel>());
  // This is expected to fail to compile
  //   EXPECT_FALSE(Example_concept<ExampleNonModel>());
}

TEST(SimpleConcept, CanBeUsedWithConceptAssert)
{
  ABL_ASSERT_CONCEPT(Example_concept, ExampleModel);
  // This is expected to fail to compile
  //   ABL_ASSERT_CONCEPT(Example_concept, ExampleNonModel);
}

//!
// Same concept as `Example_concept`, but modeled in a SFINAE-friendly
// way, which is preferred.  This syntax has the advantage that
// `declval` is required less in the specification.  On the other
// hand, it is a bit more "magical"
//
template <typename _>
struct Example_concept_two : Concept<Example_concept_two<_>>
{
  template <typename T>
  auto requires(T&& x) -> decltype(
    expressions(
      // Dereferenceable
      *x,
      // Has value type
      std::declval<Value_type<T>>(),
      // Can forward dereferenced to value type
      std::forward<Value_type<T>>( *x ),
      // Has an action() method that may return void
      (x.action(), canBeVoid)
      ));
};

TEST(ConceptSpec, CanBeEvaluatedLikeAConceptLite)
{
  EXPECT_TRUE(Example_concept_two<ExampleModel>());
  EXPECT_FALSE(Example_concept_two<ExampleNonModel>());
}

TEST(ConceptSpec, CanBeUsedWithConceptAssert)
{
  ABL_ASSERT_CONCEPT(Example_concept_two, ExampleModel);
  ABL_ASSERT_NOT_CONCEPT(Example_concept_two, ExampleNonModel);
}

TEST(ConceptSpec, CanUseSatisfiesWithConceptSpec)
{
  EXPECT_TRUE(satisfies<Example_concept_two<void>(ExampleModel)>());
  EXPECT_FALSE(satisfies<Example_concept_two<void>(ExampleNonModel)>());
}

TEST(ConceptSpec, CanUseCheckWithConceptSpec)
{
  EXPECT_TRUE(check<Example_concept_two<void>(ExampleModel)>());
  // This should fail to compiler, rising an error in the line of
  // the specification that is not met.
  //   EXPECT_FALSE(check<Example_concept_two<void>(ExampleNonModel)>());
}

//!
// Finally, an example of specifying a SFINAE-friendly concept in two
// steps, which migth seem clearer to some.
//
struct Example_concept_spec
{
  template <typename T>
  auto requires(T&& x) -> decltype(
    expressions(
      // Dereferenceable
      *x,
      // Has value type
      std::declval<Value_type<T>>(),
      // Can forward dereferenced to value type
      std::forward<Value_type<T>>( *x ),
      // Has an action() method that may return void
      (x.action(), canBeVoid)
      ));
};

template <typename T>
using Example_concept_three = Concept<Example_concept_spec(T)>;

TEST(ConceptSpec, CanBeDefinedInTwoSteps)
{
  EXPECT_TRUE(Example_concept_three<ExampleModel>());
  EXPECT_FALSE(Example_concept_three<ExampleNonModel>());
}

} // namespace estd
} // namespace ableton
