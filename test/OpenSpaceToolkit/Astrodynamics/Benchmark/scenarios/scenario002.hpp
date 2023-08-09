#if !defined SCENARIO002_HPP_INCLUDED
#define SCENARIO002_HPP_INCLUDED

int scenario002(int const i) {
  if (i <= 1) {
    return 1;
  }
  return scenario002(i - 2) + scenario002(i - 1) + scenario002(i-3);
}

#endif // SCENARIO002_HPP_INCLUDED
