// smooth: Lie Theory for Robotics
// https://github.com/pettni/smooth
//
// Licensed under the MIT License <http://opensource.org/licenses/MIT>.
//
// Copyright (c) 2021 Petter Nilsson
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <gtest/gtest.h>

#include "smooth/compat/ros.hpp"

template<smooth::NativeLieGroup G>
void test(G)
{}

TEST(Ros, LieGroup)
{
  geometry_msgs::msg::Quaternion msg3;
  geometry_msgs::msg::Pose msg4;
  geometry_msgs::msg::Transform msg5;

  test(smooth::Map<geometry_msgs::msg::Quaternion>(msg3));
  test(smooth::Map<const geometry_msgs::msg::Quaternion>(msg3));
  test(smooth::Map<geometry_msgs::msg::Pose>(msg4));
  test(smooth::Map<const geometry_msgs::msg::Pose>(msg4));
  test(smooth::Map<geometry_msgs::msg::Transform>(msg5));
  test(smooth::Map<const geometry_msgs::msg::Transform>(msg5));
}

TEST(Ros, Pose)
{
  geometry_msgs::msg::Pose p;
  p.position.x    = 3;
  p.position.y    = 5;
  p.position.z    = 4;
  p.orientation.w = 0;
  p.orientation.z = 1;
  p.orientation.x = 0;
  p.orientation.y = 0;

  smooth::Map<geometry_msgs::msg::Pose> m(p);
  ASSERT_TRUE(m.r3().isApprox(Eigen::Vector3d(3, 5, 4)));

  smooth::SE3d g;
  g.setRandom();
  m = g;

  ASSERT_TRUE(m.isApprox(g));

  smooth::Map<const geometry_msgs::msg::Pose> m_const(p);
  ASSERT_TRUE(m_const.isApprox(g));

  ASSERT_DOUBLE_EQ(p.position.x, m.r3().x());
  ASSERT_DOUBLE_EQ(p.position.y, m.r3().y());
  ASSERT_DOUBLE_EQ(p.position.z, m.r3().z());
  ASSERT_DOUBLE_EQ(p.orientation.x, m.so3().quat().x());
  ASSERT_DOUBLE_EQ(p.orientation.y, m.so3().quat().y());
  ASSERT_DOUBLE_EQ(p.orientation.z, m.so3().quat().z());
  ASSERT_DOUBLE_EQ(p.orientation.w, m.so3().quat().w());
}

TEST(Ros, Ops)
{
  using Vec6 = Eigen::Matrix<double, 6, 1>;

  for (auto i = 0u; i != 10; ++i) {
    geometry_msgs::msg::Pose p;
    smooth::Map<geometry_msgs::msg::Pose> m(p);
    smooth::SE3d g = smooth::SE3d::Random();

    m = g;

    Vec6 a             = Vec6::Random();
    smooth::SE3d grand = smooth::SE3d::Random();

    {
      auto x = smooth::rplus(m, a);
      auto y = smooth::rplus(g, a);
      static_assert(std::is_same_v<decltype(x), smooth::SE3d>);
      static_assert(std::is_same_v<decltype(y), smooth::SE3d>);
      ASSERT_TRUE(x.isApprox(y));
    }

    {
      auto x = smooth::log(m);
      auto y = smooth::log(g);
      static_assert(std::is_same_v<decltype(x), Vec6>);
      static_assert(std::is_same_v<decltype(y), Vec6>);
      ASSERT_TRUE(x.isApprox(y));
    }

    {
      auto x = smooth::lminus(m, grand);
      auto y = smooth::lminus(g, grand);
      static_assert(std::is_same_v<decltype(x), Vec6>);
      static_assert(std::is_same_v<decltype(y), Vec6>);
      ASSERT_TRUE(x.isApprox(y));
    }
  }
}