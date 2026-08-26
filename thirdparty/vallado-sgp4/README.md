# Vallado SGP4

Reference implementation of SGP4, by David Vallado, as distributed with *Fundamentals of
Astrodynamics and Applications*.

| | |
|---|---|
| Upstream | <https://celestrak.org/software/vallado-sw.php> (`cpp` distribution) |
| Version | `SGP4 Version 2020-07-13` (see `SGP4Version` in `SGP4.h`) |
| Files | `SGP4.h`, `SGP4.cpp` |
| Modifications | None. The files are verbatim upstream. |

## Why it is vendored rather than installed

The two files are self-contained: they depend on nothing outside the C++ standard library, and
upstream ships no build system, no release tags, and no package. Vendoring is what upstream's
distribution shape supports, and it pins the propagation math to an exact known version — which
matters here, because SGP4's output is only meaningful relative to the implementation that
produced the mean elements it is fed.

This replaced a build-time dependency on [dnwrnr/sgp4](https://github.com/dnwrnr/sgp4), a
re-implementation that only accepted elements as TLE text and so capped their precision at the
text format's.

## Compiler warnings

Upstream's code is compiled with `-w` (see `CMakeLists.txt`): it is held to upstream's warning
standards, not this project's. Leaving the warnings enabled would mean either a permanently noisy
build or a local patch to be re-applied on every update.

## Updating

Replace both files with the new upstream versions, update the version row above, and re-run the
`SGP4FullPrecision` and `SGP4` test suites. Any local change would have to be recorded in the
Modifications row and re-applied on the next update, so prefer working around upstream behaviour
in `src/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Model/SGP4/SGP4FullPrecision.cpp` instead.

## License

Upstream states no explicit license. The code is published as companion code to the book and is
in wide redistributed use, including in other open-source astrodynamics libraries. It is included
here on that basis and is not covered by this project's Apache-2.0 license.
