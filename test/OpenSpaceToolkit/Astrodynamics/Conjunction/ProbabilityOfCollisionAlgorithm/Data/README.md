# Probability of Collision Test Data

## Alfano2009TestCases.csv

Test cases 1 to 11 from:

> Alfano, Salvatore. (2009). Satellite Conjunction Monte Carlo Analysis. AAS 09-233.

The data at TCA, the combined hard-body radii and the expected probabilities of collision (2D probability of collision integral, not the Monte Carlo truth) are taken from [NASA CARA Analysis Tools](https://github.com/nasa/CARA_Analysis_Tools) (`PcCircle_UnitTest.m` and `Pc2D_Foster_UnitTest.m`).

Loaded by `PoCTestCase::BuildAlfano2009PoCTestCases` (see `../Utility.test.hpp`).

| Column                         | Description                                                                       | Unit                 |
| ------------------------------ | --------------------------------------------------------------------------------- | -------------------- |
| `NAME`                         | Test case name (e.g. `Alfano 2009 Test Case 1`)                                   | -                    |
| `COMMENT`                      | Contextual information (e.g. `"GEO, nonlinear relative motion"`), quoted, possibly empty; shown in the test failure messages | -                    |
| `OBJ_{i}_POS_{X,Y,Z}`          | Object `i` position (ECI, taken as GCRF)                                          | m                    |
| `OBJ_{i}_VEL_{X,Y,Z}`          | Object `i` velocity (ECI, taken as GCRF)                                          | m/s                  |
| `OBJ_{i}_COV_{A}{B}`           | Object `i` position-velocity covariance (ECI, taken as GCRF), upper triangle (21 elements), with `A` and `B` in `X`, `Y`, `Z`, `VX`, `VY`, `VZ` (e.g. `COV_XVY` is the X-VY element) | m^2, m^2/s, m^2/s^2 |
| `COMBINED_HBR`                 | Combined hard-body radius                                                         | m                    |
| `EXPECTED_POC`                 | Expected probability of collision                                                 | -                    |

Notes:

- The sample covariances of Test Case 6 are not strictly symmetric in the source data: the average value is taken for the non-symmetric elements.
- Test Case 12 (identical orbits) has no expected probability of collision: it is Object 1 of Test Case 11 on both sides.

## NASACARADilutionTestCases.csv

Probability of collision analysis (dilution region) test cases 1 to 5 from [NASA CARA Analysis Tools](https://github.com/nasa/CARA_Analysis_Tools) (`DilutionMaxPc_UnitTest.m` and the Omitron CDM test cases it reads).

The data at TCA and the combined hard-body radii are taken from the CDMs, and the expected maximum probabilities of collision from the unit test. As in NASA CARA, only the position covariances are used.

NASA CARA scales both covariances by the square of its scale factor (i.e. its scale factor applies to the standard deviations), whereas the scaling factor of `ComputeProbabilityAnalysis` applies to the covariances: the expected maximum probabilities of collision are independent of this convention, the scale factors at which they occur are not.

Loaded by `PoCAnalysisTestCase::BuildNASACARADilutionPoCAnalysisTestCases` (see `../Utility.test.hpp`).

| Column                               | Description                                                                                          | Unit |
| ------------------------------------ | ---------------------------------------------------------------------------------------------------- | ---- |
| `NAME`                               | Test case name (e.g. `NASA CARA Dilution Test Case 1`)                                               | -    |
| `COMMENT`                            | Contextual information (e.g. Omitron test case, miss distance, covariance), quoted, possibly empty; shown in the test failure messages | -    |
| `TCA_UTC`                            | Time of closest approach (UTC)                                                                       | -    |
| `OBJ_{i}_POS_{X,Y,Z}`                | Object `i` position (EME2000, i.e. J2000)                                                            | m    |
| `OBJ_{i}_VEL_{X,Y,Z}`                | Object `i` velocity (EME2000, i.e. J2000)                                                            | m/s  |
| `OBJ_{i}_COV_{RR,RT,RN,TT,TN,NN}`    | Object `i` position covariance (RTN frame of the object), upper triangle                             | m^2  |
| `COMBINED_HBR`                       | Combined hard-body radius                                                                            | m    |
| `EXPECTED_REGION`                    | Expected probability region (`Robust`, `Diluted` or `Unknown`)                                       | -    |
| `EXPECTED_MAXIMUM_POC`               | Expected maximum probability of collision                                                            | -    |

Notes:

- The positions and velocities are converted from the CDM units (km, km/s) by an exact decimal shift.

## NASACARAFrisbeeMaxPcTestCases.csv

Maximum probability of collision test cases from [NASA CARA Analysis Tools](https://github.com/nasa/CARA_Analysis_Tools) (`FrisbeeMaxPc_UnitTest.m` and the sample CDMs it reads), with one row per combination of geometry, combined hard-body radius and unknown covariance:

- Frisbee example (NASA CARA tests 12-13, `FrisbeeMaxPcTestCase_Test01.cdm`): reproduces the example of Frisbee (2015), with only the Object 1 covariance known. The expected values are those of the paper, reported to 1e-6.
- Single covariance Cases 1-1 to 1-15 (NASA CARA tests 14-27, `SingleCovTestCase1-*.cdm`): previously validated against Frisbee's data, with either covariance unknown in turn. The expected values are computed by NASA CARA over an equal-area square hard body (`HBRType = 'squareEquArea'`).

The data at TCA is taken from the CDMs. As in NASA CARA, only the position covariances are used.

Loaded by `PoCTestCase::BuildNASACARAFrisbeeMaxPcTestCases` (see `../Utility.test.hpp`).

| Column                               | Description                                                                                          | Unit |
| ------------------------------------ | ---------------------------------------------------------------------------------------------------- | ---- |
| `NAME`                               | Test case name (e.g. `NASA CARA Frisbee Max Pc Test Case 1-12 \| 70.0m HBR \| No Object 1 Covariance`) | -    |
| `COMMENT`                            | Contextual information, including the ratio of the combined hard-body radius to the smaller standard deviation of the critical combined covariance (e.g. `"HBR / sigma_min = 2.81"`, read by the tests); quoted | -    |
| `TCA_UTC`                            | Time of closest approach (UTC)                                                                       | -    |
| `OBJ_{i}_POS_{X,Y,Z}`                | Object `i` position (EME2000, i.e. J2000)                                                            | m    |
| `OBJ_{i}_VEL_{X,Y,Z}`                | Object `i` velocity (EME2000, i.e. J2000)                                                            | m/s  |
| `OBJ_{i}_COV_{RR,RT,RN,TT,TN,NN}`    | Object `i` position covariance (RTN frame of the object), upper triangle; empty if unknown          | m^2  |
| `COMBINED_HBR`                       | Combined hard-body radius                                                                            | m    |
| `EXPECTED_MAXIMUM_POC`               | Expected maximum probability of collision                                                            | -    |

Notes:

- The positions and velocities are converted from the CDM units (km, km/s) by an exact decimal shift.

## OrekitRotatedEncounterFrameTestCases.csv

Test cases from [Orekit](https://gitlab.orekit.org/orekit/orekit) (`Alfano2005Test.java`, `Chan1997Test.java`, `Laas2015Test.java` and `Patera2005Test.java`: Chan test cases 01 to 12, CSM test cases 1 to 3, CDM test cases 1 to 2 and Alfano test cases 3 and 5), defined by their miss distance and combined position uncertainty in the rotated encounter frame (i.e. along the principal axes of the combined position covariance projected onto the encounter plane).

Loaded by `PoCTestCase::BuildOrekitRotatedEncounterFrameTestCases` (see `../Utility.test.hpp`), which builds a close approach yielding these rotated encounter frame parameters for each test case, and takes the name of the expected probability of collision column to use.

| Column         | Description                                                              | Unit |
| -------------- | ------------------------------------------------------------------------ | ---- |
| `NAME`         | Test case name (e.g. `Orekit Rotated Encounter Frame \| Chan test case 01`) | -    |
| `X_M`          | Miss distance along the x principal axis (xm)                            | m    |
| `Y_M`          | Miss distance along the y principal axis (ym)                            | m    |
| `SIGMA_X`      | Combined position standard deviation along the x principal axis          | m    |
| `SIGMA_Y`      | Combined position standard deviation along the y principal axis          | m    |
| `COMBINED_HBR` | Combined hard-body radius                                                | m    |
| `EXPECTED_ALFANO2005_POC` | Expected probability of collision of the Orekit Alfano 2005 method (`Alfano2005Test.java`) | -    |
| `EXPECTED_CHAN1997_POC`   | Expected probability of collision of the Orekit Chan 1997 method (`Chan1997Test.java`), an approximation (e.g. inaccurate for Alfano test cases 3 and 5) | -    |
| `EXPECTED_LAAS2015_POC`   | Expected probability of collision of the Orekit Laas 2015 method (`Laas2015Test.java`), a series method with controlled accuracy | -    |
| `EXPECTED_PATERA2005_POC` | Expected probability of collision of the Orekit Patera 2005 method (`Patera2005Test.java`) | -    |

Notes:

- The values are those of Orekit, with integer values written with a decimal point (e.g. `10.0`) so that they are loaded as real numbers.
- The Alfano 2005 expected value of CDM test case 2 (2.1652e-20) is ~5% lower than the Laas 2015 and Patera 2005 ones (2.2796e-20 and 2.2795e-20): this far-tail probability of collision (miss ~8 sigma along the y principal axis) suffers from the cancellation of the error function difference erf(a) - erf(b) of Alfano's method.
- Each Orekit test case is expanded with its swapped version (e.g. `Orekit Rotated Encounter Frame | Chan test case 01 (swapped)`), where `X_M` and `Y_M` are swapped, as well as `SIGMA_X` and `SIGMA_Y`: the probability of collision is unchanged.
- An artificial close approach is built from the rotated encounter frame parameters of each test case (using an arbitrary time, and arbitrary position and velocity vectors), as the Orekit test cases only define these parameters.
