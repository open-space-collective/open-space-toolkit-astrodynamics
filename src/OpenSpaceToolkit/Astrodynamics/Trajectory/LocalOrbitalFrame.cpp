/// Apache License 2.0

#include <iostream>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Types/Shared.hpp>
#include <OpenSpaceToolkit/Core/Utilities.hpp>

#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationMatrix.hpp>
#include <OpenSpaceToolkit/Mathematics/Geometry/3D/Transformations/Rotations/RotationVector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Manager.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Providers/Dynamic.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Utilities.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Transform.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrame.hpp>

namespace ostk
{
namespace astro
{
namespace trajectory
{

using ostk::core::types::Shared;
using ostk::math::geom::d3::trf::rot::Quaternion;
using ostk::math::geom::d3::trf::rot::RotationMatrix;
using ostk::math::geom::d3::trf::rot::RotationVector;
using ostk::math::obj::Vector3d;

using ostk::physics::coord::Frame;
using FrameManager = ostk::physics::coord::frame::Manager;
using DynamicProvider = ostk::physics::coord::frame::provider::Dynamic;
using ostk::physics::coord::Transform;
using ostk::physics::coord::Position;
using ostk::physics::coord::Velocity;

using ostk::astro::trajectory::State;
using ostk::astro::trajectory::LOFProvider;

struct SharedFrameEnabler : public LocalOrbitalFrame
{
    SharedFrameEnabler(
        const String& aName,
        const Shared<const Frame>& aParentFrame,
        const Shared<const LOFProvider>& aProvider
    )
        : LocalOrbitalFrame(aName, aParentFrame, aProvider)
    {
    }
};

Shared<const LocalOrbitalFrame> LocalOrbitalFrame::VNC()
{

    // X axis along velocity vector
    // Y axis along orbital momentum

    // const String frameName = LocalOrbitalFrame::generateFrameName(LocalOrbitalFrame::Type::VNC);

    // if (FrameManager::Get().hasFrameWithName(frameName))
    // {
    //     return FrameManager::Get().accessFrameWithName(frameName);
    // }

    const auto generateLOFProvider =
        [](const auto& anAttitudeGenerator, const Shared<const Frame>& aReferenceFrame) -> auto
    {
        const Shared<const LOFProvider> LOFProviderSPtr = std::make_shared<const LOFProvider>(
            [anAttitudeGenerator, aReferenceFrame](const Instant& anInstant, const Position& aPosition, const Velocity& aVelocity) -> Transform
            {

                const State state = {anInstant, aPosition, aVelocity};

                const Vector3d x_GCRF = state.getPosition().accessCoordinates();
                const Vector3d v_GCRF_in_GCRF = state.getVelocity().accessCoordinates();

                const Vector3d x_VVLH_GCRF_in_GCRF = -x_GCRF;          // [m]
                const Vector3d v_VVLH_GCRF_in_GCRF = -v_GCRF_in_GCRF;  // [m/s]
                const Quaternion q_VVLH_GCRF = anAttitudeGenerator(state);

                // TBI
                // const Duration delta = Duration::Seconds(1.0);  // TBM This should be a parameter
                // const Quaternion q_VVLH_next_GCRF =
                //     anAttitudeGenerator(this->getStateAt(anInstant + delta).inFrame(Frame::GCRF()));
                // const Quaternion q_VVLH_next_VVLH = (q_VVLH_next_GCRF * q_VVLH_GCRF.toConjugate()).toNormalized();
                // const RotationVector rv_VVLH_next_VVLH = RotationVector::Quaternion(q_VVLH_next_VVLH);

                const Vector3d w_VVLH_GCRF_in_VVLH = {0.0, 0.0, 0.0};

                return {
                    anInstant,
                    x_VVLH_GCRF_in_GCRF,
                    v_VVLH_GCRF_in_GCRF,
                    q_VVLH_GCRF,
                    w_VVLH_GCRF_in_VVLH,
                    Transform::Type::Passive};
            }
        );

        return LOFProviderSPtr;
    };

    const auto calculateAttitude = [](const State& aState) -> Quaternion
    {
        const Vector3d x_GCRF = aState.getPosition().accessCoordinates();
        const Vector3d v_GCRF = aState.getVelocity().accessCoordinates();

        const Vector3d xAxis = v_GCRF.normalized();
        const Vector3d yAxis = x_GCRF.cross(v_GCRF).normalized();
        const Vector3d zAxis = xAxis.cross(yAxis);

        const Quaternion q_VNC_GCRF =
            Quaternion::RotationMatrix(RotationMatrix::Rows(xAxis, yAxis, zAxis)).toNormalized().rectify();

        return q_VNC_GCRF;
    };

    Shared<const LocalOrbitalFrame> localOrbitalFrameSPtr = nullptr;

    localOrbitalFrameSPtr = LocalOrbitalFrame::Construct(
        "VNCC", Frame::GCRF(), generateLOFProvider(calculateAttitude, Frame::GCRF())
    );

    FrameManager::Get().addFrame(localOrbitalFrameSPtr);

    return localOrbitalFrameSPtr;

}

Shared<const LocalOrbitalFrame> LocalOrbitalFrame::Construct(
    const String& aName,
    const Shared<const Frame>& aParentFrame,
    const Shared<const LOFProvider>& aProvider
)
{
    if (FrameManager::Get().hasFrameWithName(aName))
    {
        throw ostk::core::error::RuntimeError("Frame with name [{}] already exist.", aName);
    }

    return LocalOrbitalFrame::Emplace(aName, aParentFrame, aProvider);
}

// String LocalOrbitalFrame::generateFrameName(const LocalOrbitalFrame::Type& aType)
// {
//     return String::Format("{} @ Local Orbital Frame [{}]", LocalOrbitalFrame::StringFromType(aType), fmt::ptr(this));
// }

String LocalOrbitalFrame::StringFromType(const LocalOrbitalFrame::Type& aType)
{
    switch (aType)
    {
        case LocalOrbitalFrame::Type::Undefined:
            return "Undefined";

        case LocalOrbitalFrame::Type::NED:
            return "NED";

        case LocalOrbitalFrame::Type::LVLH:
            return "LVLH";

        case LocalOrbitalFrame::Type::VVLH:
            return "VVLH";

        case LocalOrbitalFrame::Type::LVLHGD:
            return "LVLHGD";

        case LocalOrbitalFrame::Type::QSW:
            return "QSW";

        case LocalOrbitalFrame::Type::TNW:
            return "TNW";

        case LocalOrbitalFrame::Type::VNC:
            return "VNC";

        default:
            throw ostk::core::error::runtime::Wrong("Frame type");
            break;
    }

    return String::Empty();
}

Shared<const LocalOrbitalFrame> LocalOrbitalFrame::Emplace(
    const String& aName,
    const Shared<const Frame>& aParentFrame,
    const Shared<const LOFProvider>& aProvider
)
{
    // if (const auto frameSPtr = FrameManager::Get().accessFrameWithName(aName))
    // {
    //     return std::static_pointer_cast<LocalOrbitalFrame>(frameSPtr);
    // }

    const Shared<const LocalOrbitalFrame> frameSPtr =
        std::make_shared<const SharedFrameEnabler>(aName, aParentFrame, aProvider);

    FrameManager::Get().addFrame(frameSPtr);

    return frameSPtr;
}

LocalOrbitalFrame::LocalOrbitalFrame(
    const String& aName,
    const Shared<const Frame>& aParentFrame,
    const Shared<const LOFProvider>& aProvider
)
    : Frame(aName, false, aParentFrame, aProvider)
{
}

}  // namespace trajectory
}  // namespace astro
}  // namespace ostk
