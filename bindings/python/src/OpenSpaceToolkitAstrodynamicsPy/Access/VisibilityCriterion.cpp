// VisibilityCriterion.cpp

#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <OpenSpaceToolkit/Core/Container/Map.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Interval.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Position.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Spherical/AER.hpp>
#include <OpenSpaceToolkit/Physics/Environment.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Access/VisibilityCriterion.hpp>

void OpenSpaceToolkitAstrodynamicsPy_Access_VisibilityCriterion(pybind11::module& aModule)
{
    using namespace pybind11;

    using ostk::core::container::Map;
    using ostk::core::type::Real;

    using ostk::mathematics::object::Interval;

    using ostk::physics::coordinate::spherical::AER;
    using ostk::physics::Environment;
    using ostk::physics::unit::Angle;

    using ostk::astrodynamics::access::VisibilityCriterion;

    class_<VisibilityCriterion> visibilityCriterionClass(
        aModule,
        "VisibilityCriterion",
        R"doc(
            A class representing a visibility criterion for accesses between objects.
        )doc"
    );

    class_<VisibilityCriterion::AERInterval>(
        visibilityCriterionClass,
        "AERInterval",
        R"doc(
            An AER interval visibility criterion.
        )doc"
    )
        .def(
            init<const Interval<Real>&, const Interval<Real>&, const Interval<Real>&>(),
            arg("azimuth_interval"),
            arg("elevation_interval"),
            arg("range_interval"),
            R"doc(
                Constructs an AER interval.

                Args:
                    azimuth_interval (RealInterval): Azimuth interval in degrees.
                    elevation_interval (RealInterval): Elevation interval in degrees.
                    range_interval (RealInterval): Range interval in meters.
            )doc"
        )
        .def_readonly(
            "azimuth",
            &VisibilityCriterion::AERInterval::azimuth,
            R"doc(
                Azimuth interval in radians.

                :type: RealInterval
            )doc"
        )
        .def_readonly(
            "elevation",
            &VisibilityCriterion::AERInterval::elevation,
            R"doc(
                Elevation interval in radians.

                :type: RealInterval
            )doc"
        )
        .def_readonly(
            "range",
            &VisibilityCriterion::AERInterval::range,
            R"doc(
                Range interval in meters.

                :type: RealInterval
            )doc"
        )
        .def(
            "is_satisfied",
            overload_cast<const AER&>(&VisibilityCriterion::AERInterval::isSatisfied, const_),
            arg("aer"),
            R"doc(
                Checks if the given AER satisfies the criterion.

                Args:
                    aer (AER): The Azimuth, Elevation, and Range to check.

                Returns:
                    bool: True if the criterion is satisfied, False otherwise.
            )doc"
        )
        .def(
            "is_satisfied",
            overload_cast<const Real&, const Real&, const Real&>(
                &VisibilityCriterion::AERInterval::isSatisfied, const_
            ),
            arg("azimuth"),
            arg("elevation"),
            arg("range"),
            R"doc(
                Checks if the given Azimuth, Elevation, and Range values satisfy the criterion.

                Args:
                    azimuth (float): Azimuth in radians.
                    elevation (float): Elevation in radians.
                    range (float): Range in meters.

                Returns:
                    bool: True if the criterion is satisfied, False otherwise.
            )doc"
        );

    class_<VisibilityCriterion::AERMask>(
        visibilityCriterionClass,
        "AERMask",
        R"doc(
            An AER mask visibility criterion.
        )doc"
    )
        .def(
            init<const Map<Real, Real>&, const Interval<Real>&>(),
            arg("azimuth_elevation_mask"),
            arg("range_interval"),
            R"doc(
                Constructs an AER mask.

                Args:
                    azimuth_elevation_mask (dict): A map of azimuth angles (degrees) to elevation angles (degrees).
                    range_interval (RealInterval): Range interval in meters.
            )doc"
        )
        .def_readonly(
            "azimuth_elevation_mask",
            &VisibilityCriterion::AERMask::azimuthElevationMask,
            R"doc(
                A map of azimuth angles to elevation angles in radians defining the mask.

                :type: dict
            )doc"
        )
        .def_readonly(
            "range",
            &VisibilityCriterion::AERMask::range,
            R"doc(
                Range interval in meters.

                :type: RealInterval
            )doc"
        )
        .def(
            "is_satisfied",
            overload_cast<const AER&>(&VisibilityCriterion::AERMask::isSatisfied, const_),
            arg("aer"),
            R"doc(
                Checks if the given AER satisfies the criterion.

                Args:
                    aer (AER): The Azimuth, Elevation, and Range to check.

                Returns:
                    bool: True if the criterion is satisfied, False otherwise.
            )doc"
        )
        .def(
            "is_satisfied",
            overload_cast<const Real&, const Real&, const Real&>(&VisibilityCriterion::AERMask::isSatisfied, const_),
            arg("azimuth"),
            arg("elevation"),
            arg("range"),
            R"doc(
                Checks if the given Azimuth, Elevation, and Range values satisfy the criterion.

                Args:
                    azimuth (float): Azimuth in radians.
                    elevation (float): Elevation in radians.
                    range (float): Range in meters.

                Returns:
                    bool: True if the criterion is satisfied, False otherwise.
            )doc"
        );

    class_<VisibilityCriterion::LineOfSight>(
        visibilityCriterionClass,
        "LineOfSight",
        R"doc(
            A line-of-sight visibility criterion.
        )doc"
    )
        .def(
            init<const Environment&>(),
            arg("environment"),
            R"doc(
                Constructs a LineOfSight visibility criterion.

                Args:
                    environment (Environment): The environment to consider for line-of-sight calculations.
            )doc"
        )
        .def(
            "is_satisfied",
            &VisibilityCriterion::LineOfSight::isSatisfied,
            arg("instant"),
            arg("from_position_coordinates"),
            arg("to_position_coordinates"),
            R"doc(
                Checks if the line-of-sight criterion is satisfied between two positions at a given instant.

                Args:
                    instant (Instant): The time at which to perform the check.
                    from_position_coordinates (np.ndarray): The position coordinates (in meters) of the observer.
                    to_position_coordinates (np.ndarray): The position coordinates (in meters) of the target.

                Returns:
                    bool: True if there is a clear line of sight, False otherwise.
            )doc"
        );

    class_<VisibilityCriterion::ElevationInterval>(
        visibilityCriterionClass,
        "ElevationInterval",
        R"doc(
            An elevation interval visibility criterion.
        )doc"
    )
        .def(
            init<const Interval<Real>&>(),
            arg("elevation_interval"),
            R"doc(
                Constructs an ElevationInterval visibility criterion.

                Args:
                    elevation_interval (RealInterval): The elevation interval in degrees.
            )doc"
        )
        .def_readonly(
            "elevation",
            &VisibilityCriterion::ElevationInterval::elevation,
            R"doc(
                Elevation interval in radians.

                :type: RealInterval
            )doc"
        )
        .def(
            "is_satisfied",
            overload_cast<const Real&>(&VisibilityCriterion::ElevationInterval::isSatisfied, const_),
            arg("elevation"),
            R"doc(
                Checks if the given elevation angle satisfies the criterion.

                Args:
                    elevation (float): Elevation angle in radians.

                Returns:
                    bool: True if the criterion is satisfied, False otherwise.
            )doc"
        )
        .def(
            "is_satisfied",
            overload_cast<const Angle&>(&VisibilityCriterion::ElevationInterval::isSatisfied, const_),
            arg("elevation"),
            R"doc(
                Checks if the given elevation angle satisfies the criterion.

                Args:
                    elevation (Angle): Elevation angle.

                Returns:
                    bool: True if the criterion is satisfied, False otherwise.
            )doc"
        );

    visibilityCriterionClass
        .def_static(
            "from_aer_interval",
            &VisibilityCriterion::FromAERInterval,
            arg("azimuth_interval"),
            arg("elevation_interval"),
            arg("range_interval"),
            R"doc(
                Creates a visibility criterion from azimuth, elevation, and range intervals.

                Args:
                    azimuth_interval (RealInterval): Azimuth interval in degrees.
                    elevation_interval (RealInterval): Elevation interval in degrees.
                    range_interval (RealInterval): Range interval in meters.

                Returns:
                    VisibilityCriterion: The visibility criterion instance.
            )doc"
        )
        .def_static(
            "from_aer_mask",
            &VisibilityCriterion::FromAERMask,
            arg("azimuth_elevation_mask"),
            arg("range_interval"),
            R"doc(
                Creates a visibility criterion from an azimuth-elevation mask and range interval.

                Args:
                    azimuth_elevation_mask (dict): A map of azimuth angles (degrees) to elevation angles (degrees).
                    range_interval (RealInterval): Range interval in meters.

                Returns:
                    VisibilityCriterion: The visibility criterion instance.
            )doc"
        )
        .def_static(
            "from_line_of_sight",
            &VisibilityCriterion::FromLineOfSight,
            arg("environment"),
            R"doc(
                Creates a visibility criterion based on line-of-sight considerations.

                Args:
                    environment (Environment): The environment to consider for line-of-sight calculations.

                Returns:
                    VisibilityCriterion: The visibility criterion instance.
            )doc"
        )
        .def_static(
            "from_elevation_interval",
            &VisibilityCriterion::FromElevationInterval,
            arg("elevation_interval"),
            R"doc(
                Creates a visibility criterion from an elevation interval.

                Args:
                    elevation_interval (RealInterval): The elevation interval in degrees.

                Returns:
                    VisibilityCriterion: The visibility criterion instance.
            )doc"
        )
        .def(
            "is_aer_interval",
            &VisibilityCriterion::is<VisibilityCriterion::AERInterval>,
            R"doc(
                Checks if the visibility criterion is an AERInterval.

                Returns:
                    bool: True if it is an AERInterval criterion, False otherwise.
            )doc"
        )
        .def(
            "is_aer_mask",
            &VisibilityCriterion::is<VisibilityCriterion::AERMask>,
            R"doc(
                Checks if the visibility criterion is an AERMask.

                Returns:
                    bool: True if it is an AERMask criterion, False otherwise.
            )doc"
        )
        .def(
            "is_line_of_sight",
            &VisibilityCriterion::is<VisibilityCriterion::LineOfSight>,
            R"doc(
                Checks if the visibility criterion is a LineOfSight.

                Returns:
                    bool: True if it is a LineOfSight criterion, False otherwise.
            )doc"
        )
        .def(
            "is_elevation_interval",
            &VisibilityCriterion::is<VisibilityCriterion::ElevationInterval>,
            R"doc(
                Checks if the visibility criterion is an ElevationInterval.

                Returns:
                    bool: True if it is an ElevationInterval criterion, False otherwise.
            )doc"
        )
        .def(
            "as_aer_interval",
            [](const VisibilityCriterion& self)
            {
                const auto result = self.as<VisibilityCriterion::AERInterval>();
                if (result.has_value())
                    return result;
                else
                    throw value_error("VisibilityCriterion is not AERInterval");
            },
            R"doc(
                Casts the visibility criterion to an AERInterval.

                Returns:
                    AERInterval: The AERInterval criterion.

                Raises:
                    ValueError: If the criterion is not an AERInterval.
            )doc"
        )
        .def(
            "as_aer_mask",
            [](const VisibilityCriterion& self)
            {
                const auto result = self.as<VisibilityCriterion::AERMask>();
                if (result.has_value())
                    return result;
                else
                    throw value_error("VisibilityCriterion is not AERMask");
            },
            R"doc(
                Casts the visibility criterion to an AERMask.

                Returns:
                    AERMask: The AERMask criterion.

                Raises:
                    ValueError: If the criterion is not an AERMask.
            )doc"
        )
        .def(
            "as_line_of_sight",
            [](const VisibilityCriterion& self)
            {
                const auto result = self.as<VisibilityCriterion::LineOfSight>();
                if (result.has_value())
                    return result;
                else
                    throw value_error("VisibilityCriterion is not LineOfSight");
            },
            R"doc(
                Casts the visibility criterion to a LineOfSight.

                Returns:
                    LineOfSight: The LineOfSight criterion.

                Raises:
                    ValueError: If the criterion is not a LineOfSight.
            )doc"
        )
        .def(
            "as_elevation_interval",
            [](const VisibilityCriterion& self)
            {
                const auto result = self.as<VisibilityCriterion::ElevationInterval>();
                if (result.has_value())
                    return result;
                else
                    throw value_error("VisibilityCriterion is not ElevationInterval");
            },
            R"doc(
                Casts the visibility criterion to an ElevationInterval.

                Returns:
                    ElevationInterval: The ElevationInterval criterion.

                Raises:
                    ValueError: If the criterion is not an ElevationInterval.
            )doc"
        );
}
