#include "json/status.hh"
#include "testrunner/testrunner.h"

TEST(StatusOr_ConvertsToBool) {
  auto status_unassigned = json::StatusOr<uint64_t>{json::Status::Ok};
  ASSERT_FALSE(status_unassigned);

  auto status_assigned = json::StatusOr<uint64_t>{42};
  ASSERT_TRUE(status_assigned);
}

TEST(StatusOr_CanDereferenceValue) {
  auto status_int = json::StatusOr<uint64_t>{42};
  EXPECT_EQ(*status_int, 42);

  struct AB {
    uint32_t a{};
    uint32_t b{};
  };

  auto status_ab = json::StatusOr<AB>{{10, 20}};
  EXPECT_EQ(status_ab->a, 10)
  EXPECT_EQ(status_ab->b, 20)
}
