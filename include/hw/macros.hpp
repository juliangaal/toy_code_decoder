//
// Created by julian on 5/24/19.
//

#ifndef HWCPP_MACROS_HPP
#define HWCPP_MACROS_HPP

// adapted from http://bastian.rieck.me/blog/posts/2017/simple_unit_tests/
#include <stdexcept>
#include <string>

#define ASSERT_THROW( condition )                             		\
{                                                                   \
  if( !( condition ) )                                              \
  {                                                                 \
    throw std::runtime_error(   std::string( __FILE__ )             \
                              + std::string( ":" )                  \
                              + std::to_string( __LINE__ )          \
                              + std::string( " in " )               \
                              + std::string( __PRETTY_FUNCTION__ )  \
    );                                                              \
  }                                                                 \
}

#define ASSERT_EQUAL( x, y )                                  		\
{                                                                   \
  if( ( x ) != ( y ) )                                              \
  {                                                                 \
    throw std::runtime_error(   std::string( __FILE__ )             \
                              + std::string( ":" )                  \
                              + std::to_string( __LINE__ )          \
                              + std::string( " in " )               \
                              + std::string( __PRETTY_FUNCTION__ )  \
                              + std::string( ": Got " )                 \
                              + std::to_string( ( x ) )             \
                              + std::string( ", Wanted " )               \
                              + std::to_string( ( y ) )             \
    );                                                              \
  }                                                                 \
}

#endif //HWCPP_MACROS_HPP
