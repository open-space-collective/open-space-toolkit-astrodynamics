/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Custom__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Custom__

#include <functional>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Object/Point.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace conjunction
{
namespace hardbody
{

using ostk::core::container::Array;
using ostk::core::type::Shared;

using ostk::physics::coordinate::Frame;

using ostk::astrodynamics::conjunction::HardBody;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;

/// @brief Custom hard body.
///
/// @details A set of 3D vertices returned by a user-defined generator, which may depend on the
/// state (e.g. to account for the attitude).
///
/// Vertices are in meters, relative to the object position. The vertex frame, either an inertial frame (see
/// FromInertial) or a local orbital frame (see FromLocalOrbitalFrame), only defines their direction: they are
/// rotated, never translated.
///
/// The cross section is the convex hull of the projected vertices, with a zero radius. For a non-convex object,
/// it conservatively encloses the actual shadow.
class Custom : public HardBody
{
   public:
    /// @brief 3D vertex of the hard body
    using Vertex = ostk::mathematics::geometry::d3::object::Point;

    /// @brief Vertex generator
    ///
    /// @details Returns the vertices [m] for a given state, relative to the object position and expressed along
    /// the vertex frame axes.
    using VertexGenerator = std::function<Array<Vertex>(const State&)>;

    /// @brief Destructor
    virtual ~Custom() override = default;

    /// @brief Get the vertex generator
    ///
    /// @return The vertex generator
    VertexGenerator getVertexGenerator() const;

    /// @brief Calculate the cross section of the hard body at a given state
    ///
    /// @details The vertices are rotated from the vertex frame into the given frame, then projected. Raises an
    /// Undefined error if the vertex generator returns no vertex.
    ///
    /// @param aState A state of the object
    /// @param aFrameSPtr A frame whose axes define the projection
    /// @return The cross section
    virtual HardBody::CrossSection calculateCrossSectionAt(const State& aState, const Shared<const Frame>& aFrameSPtr)
        const override;

    /// @brief Construct a custom hard body with vertices expressed in an inertial frame
    ///
    /// @details Raises an Undefined error if the frame or the vertex generator is undefined, and a Wrong error if
    /// the frame is not quasi-inertial.
    ///
    /// @code{.cpp}
    ///              // Cube of side 2.0 m, with edges aligned with the GCRF axes
    ///              Custom::VertexGenerator vertexGenerator = [](const State& aState) -> Array<Custom::Vertex>
    ///              {
    ///                  Array<Custom::Vertex> vertices = Array<Custom::Vertex>::Empty() ;
    ///                  for (double x : { -1.0, 1.0 })
    ///                      for (double y : { -1.0, 1.0 })
    ///                          for (double z : { -1.0, 1.0 })
    ///                              vertices.add({ x, y, z }) ;
    ///                  return vertices ;
    ///              } ;
    ///
    ///              Custom custom = Custom::FromInertial(Frame::GCRF(), vertexGenerator) ;
    /// @endcode
    ///
    /// @param aFrameSPtr An inertial (or quasi-inertial) vertex frame
    /// @param aVertexGenerator A vertex generator
    /// @return A custom hard body
    static Custom FromInertial(const Shared<const Frame>& aFrameSPtr, const VertexGenerator& aVertexGenerator);

    /// @brief Construct a custom hard body with vertices expressed in a local orbital frame
    ///
    /// @details The vertex frame is generated from the state by the factory. Raises an Undefined error if the
    /// factory or the vertex generator is undefined.
    ///
    /// @code{.cpp}
    ///              // Same cube, with edges aligned with the VNC axes
    ///              Custom custom =
    ///                  Custom::FromLocalOrbitalFrame(LocalOrbitalFrameFactory::VNC(Frame::GCRF()), vertexGenerator) ;
    /// @endcode
    ///
    /// @param aLocalOrbitalFrameFactorySPtr A local orbital frame factory generating the vertex frame
    /// @param aVertexGenerator A vertex generator
    /// @return A custom hard body
    static Custom FromLocalOrbitalFrame(
        const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactorySPtr,
        const VertexGenerator& aVertexGenerator
    );

   private:
    VertexGenerator vertexGenerator_;
    Shared<const Frame> frameSPtr_;
    Shared<const LocalOrbitalFrameFactory> localOrbitalFrameFactorySPtr_;

    Custom(
        const VertexGenerator& aVertexGenerator,
        const Shared<const Frame>& aFrameSPtr,
        const Shared<const LocalOrbitalFrameFactory>& aLocalOrbitalFrameFactorySPtr
    );
};

}  // namespace hardbody
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk

#endif
