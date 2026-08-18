# vallado-sgp4

The reference SGP4 implementation published by David Vallado as the companion code to
AIAA 2006-6753, *Revisiting Spacetrack Report #3*. This is the library's SGP4 — every TLE
and every mean element set is propagated through these two files.

| | |
|---|---|
| Source | https://celestrak.org/publications/AIAA/2006-6753/AIAA-2006-6753.zip |
| Path in archive | `sgp4/cpp/SGP4/SGP4/SGP4.{h,cpp}` |
| Version string | `SGP4 Version 2020-07-13` (`SGP4Version` in `SGP4.h`) |
| Terms | Distributed by CelesTrak as companion code to the paper; the archive ships no license file |

## Why it is here

It replaced [dnwrnr/sgp4], which had been installed as a system library. That one can only be
reached through 69 columns of text: its `Tle` constructs from line strings, its element
doubles are private with no setters, and its `SGP4` takes only a `Tle`.

The TLE format rounds eccentricity to `1e-7` and the angles to `1e-4` deg. That makes the
propagator a staircase rather than a smooth function of its elements, which is fatal to any
finite-difference Jacobian taken on a near-circular orbit: for `e < ~1e-4` the relative
finite-difference step falls below the eccentricity quantum, the eccentricity written into
the string does not change at all, and the `ex`/`ey` Jacobian columns collapse onto a single
direction. `TLESolver` then drives the orbit onto a hyperbola and throws `"Algorithm error."`

Vallado's `sgp4init()` takes the eight elements as doubles, so `MeanElements` fits against
continuous elements and serialises a TLE once, at the end. Two further things fell out of
the switch: `orbit::model::SGP4` no longer has to launder Alpha-5 satellite numbers past a
parser that rejects them, and there is one fewer system dependency to install and pin.

[dnwrnr/sgp4]: https://github.com/dnwrnr/sgp4

## Vendored, not installed

These two files depend on nothing but `<math.h>`, `<stdio.h>`, `<string.h>` and
`<iostream>`, so they are compiled straight into the library rather than installed by the
Dockerfile and found by CMake. There is no version to pin anywhere else.

Before the switch, `MeanElements.test.cpp` held this implementation against the outgoing
`libsgp4` at 10 µm. Measured then over 201 samples spanning each window, the two agreed to:

| Orbit | Window | Max Δposition | Max Δvelocity |
|---|---|---|---|
| ISS, near-Earth | ±1 d | 0.28 µm | 3.2e-10 m/s |
| ISS, near-Earth | ±7 d | 2.0 µm | 2.3e-09 m/s |
| Sun-synchronous LEO, near-circular | ±3 d | 6.5 µm | 4.8e-09 m/s |
| Geosynchronous, deep space | ±1 d | 0.16 µm | 1.2e-11 m/s |
| Molniya (e = 0.75), deep space resonance | ±1 d | 52 µm | 3.3e-09 m/s |

That is the size of the numerical step the switch cost — two implementations of the same
paper, agreeing to well under a millimetre. The reference-value tests in `SGP4.test.cpp`,
which predate the switch, now hold this implementation to those same expectations.

## Changes from upstream

Verbatim except for three points, so that a re-sync is a plain diff against the archive:

1. Line endings converted from CRLF to LF.
2. Three degree signs in comments, written in a non-UTF-8 code page (byte `0xF8`), spelled
   out as `deg`.
3. The two file-scope debug globals, `help` and `dbgfile`, given internal linkage — at
   external linkage the shared library would export a symbol named `help`. Marked
   `PATCH (Open Space Toolkit)` in the source.

The translation unit is compiled with warnings off (see `CMakeLists.txt`); it is upstream's
code and is not held to this project's warning settings. `ostk-format-cpp` does not touch
`thirdparty/`.
