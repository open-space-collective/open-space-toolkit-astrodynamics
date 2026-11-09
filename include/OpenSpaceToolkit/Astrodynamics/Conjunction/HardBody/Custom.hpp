/// Apache License 2.0

#ifndef __OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Custom__
#define __OpenSpaceToolkit_Astrodynamics_Conjunction_HardBody_Custom__

#include <functional>
#include <variant>

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
/// Vertices are in meters, relative to the object position. The vertex frame, either a fixed frame or a local
/// orbital frame generated from the state, only defines their direction: they are rotated, never translated.
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

    /// @brief Constructor
    ///
    /// @details The vertices are expressed either in a fixed frame, or in a local orbital frame generated from the
    /// state by a factory. Raises an Undefined error if the vertex generator, the frame or the factory is undefined.
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
    ///              Custom custom = { vertexGenerator, Frame::GCRF() } ;
    ///
    ///              // Same cube, with edges aligned with the VNC axes
    ///              Custom lofCustom = { vertexGenerator, LocalOrbitalFrameFactory::VNC(Frame::GCRF()) } ;
    /// @endcode
    ///
    /// @param aVertexGenerator A vertex generator
    /// @param aFrameOrLOFFactory A vertex frame, or a local orbital frame factory generating the vertex frame
    Custom(
        const VertexGenerator& aVertexGenerator,
        const std::variant<Shared<const Frame>, LocalOrbitalFrameFactory>& aFrameOrLOFFactory
    );

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

   private:
    VertexGenerator vertexGenerator_;
    std::variant<Shared<const Frame>, LocalOrbitalFrameFactory> frameOrLOF_;
};

}  // namespace hardbody
}  // namespace conjunction
}  // namespace astrodynamics
}  // namespace ostk

#endif
