#if !defined SCENARIO001_HPP_INCLUDED
#define SCENARIO001_HPP_INCLUDED

int scenario001(int const i) {
  if (i <= 1) {
    return 1;
  }
  return scenario001(i - 2) + scenario001(i - 1) + scenario001(i-3);
}

#endif // SCENARIO001_HPP_INCLUDED
