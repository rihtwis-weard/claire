#pragma once

namespace claire {

  template <typename R, typename... T>
  class Visitor;

  template <typename R, typename T>
  class Visitor<R, T> {
  public:
    virtual R operator()(T const *visitable) = 0;
  };

  template <typename R, typename T, typename... Rest>
  class Visitor<R, T, Rest...> : public Visitor<R, Rest...> {
  public:
    using Visitor<R, Rest...>::operator();

    virtual R operator()(T const *visitable) = 0;
  };

} // namespace claire
