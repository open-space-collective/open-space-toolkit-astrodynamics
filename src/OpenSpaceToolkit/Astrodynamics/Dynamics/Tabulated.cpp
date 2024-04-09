/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateBroker.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace dynamics
{

using ostk::core::type::Index;

using ostk::mathematics::object::VectorXd;

using ostk::astrodynamics::trajectory::state::CoordinateBroker;

const Shared<const Frame> Tabulated::DefaultContributionFrameSPtr = Frame::GCRF();

Tabulated::Tabulated(
    const Array<Instant>& anInstantArray,
    const MatrixXd& aContributionProfile,
    const Array<Shared<const CoordinateSubset>>& aWriteCoordinateSubsets,
    const Shared<const Frame>& aFrameSPtr,
    const Interpolator::Type& anInterpolationType
)
    : Dynamics("Tabulated"),
      instants_(anInstantArray),
      contributionProfile_(aContributionProfile),
      writeCoordinateSubsets_(aWriteCoordinateSubsets),
      frameSPtr_(aFrameSPtr)
{
    if (anInstantArray.getSize() != (Index)aContributionProfile.rows())
    {
        throw ostk::core::error::RuntimeError(
            "Contribution profile must have the same number of rows as the number of instants."
        );
    }

    if (aWriteCoordinateSubsets
            .map<Index>(
                [](const auto& coordinateSubset)
                {
                    return coordinateSubset->getSize();
                }
            )
            .reduce(std::plus<Index>()) != static_cast<Index>(aContributionProfile.cols()))
    {
        throw ostk::core::error::RuntimeError(
            "Contribution profile must have the same number of columns as the sum of the sizes of the write "
            "coordinate subsets."
        );
    }

    if (aFrameSPtr != DefaultContributionFrameSPtr)
    {
        throw ostk::core::error::RuntimeError("Contributions must be expressed in an inertial frame.");
    }

    VectorXd timestamps(anInstantArray.getSize());
    Index i = 0;
    for (const auto& instant : anInstantArray)
    {
        timestamps(i) = (instant - anInstantArray.accessFirst()).inSeconds();
        ++i;
    }

    interpolators_.reserve(aContributionProfile.cols());
    for (i = 0; i < (Index)aContributionProfile.cols(); ++i)
    {
        interpolators_.add(
            Interpolator::GenerateInterpolator(anInterpolationType, timestamps, aContributionProfile.col(i))
        );
    }
}

std::ostream& operator<<(std::ostream& anOutputStream, const Tabulated& aTabulated)
{
    aTabulated.print(anOutputStream);

    return anOutputStream;
}

const Array<Instant>& Tabulated::accessInstants() const
{
    return instants_;
}

const MatrixXd& Tabulated::accessContributionProfile() const
{
    return contributionProfile_;
}

MatrixXd Tabulated::getContributionProfileFromCoordinateSubsets(
    const Array<Shared<const CoordinateSubset>>& aCoordinateSubsetArray
) const
{
    if (aCoordinateSubsetArray.isEmpty())
    {
        throw ostk::core::error::RuntimeError("Specified Coordinate Subset array is empty.");
    }

    MatrixXd customContributionProfile(
        instants_.getSize(),
        aCoordinateSubsetArray
            .map<Index>(
                [](const auto& coordinateSubset)
                {
                    return coordinateSubset->getSize();
                }
            )
            .reduce(std::plus<Index>())
    );

    Index customMatrixColIndex = 0;
    for (const auto& desiredCoordinateSubsetSPtr : aCoordinateSubsetArray)
    {
        bool coordinateSubsetFound = false;
        Index existingMatrixColIndex = 0;

        for (const auto& existingCoordinateSubsetSPtr : writeCoordinateSubsets_)
        {
            if (*desiredCoordinateSubsetSPtr == *existingCoordinateSubsetSPtr)
            {
                customContributionProfile.block(
                    0, customMatrixColIndex, instants_.getSize(), existingCoordinateSubsetSPtr->getSize()
                ) =
                    contributionProfile_.block(
                        0, existingMatrixColIndex, instants_.getSize(), existingCoordinateSubsetSPtr->getSize()
                    );
                coordinateSubsetFound = true;
                break;
            }
            existingMatrixColIndex += existingCoordinateSubsetSPtr->getSize();
        }

        if (!coordinateSubsetFound)
        {
            throw ostk::core::error::RuntimeError("Coordinate subset not found in write coordinate subsets.");
        }
        customMatrixColIndex += desiredCoordinateSubsetSPtr->getSize();
    }

    return customContributionProfile;
}

const Shared<const Frame>& Tabulated::accessFrame() const
{
    return frameSPtr_;
}

Interpolator::Type Tabulated::getInterpolationType() const
{
    // Since all interpolators are of the same type, we can just return the type of the first one.
    return interpolators_[0]->getInterpolationType();
}

bool Tabulated::isDefined() const
{
    return true;
}

Array<Shared<const CoordinateSubset>> Tabulated::getReadCoordinateSubsets() const
{
    return {};
}

Array<Shared<const CoordinateSubset>> Tabulated::getWriteCoordinateSubsets() const
{
    return writeCoordinateSubsets_;
}

VectorXd Tabulated::computeContribution(
    const Instant& anInstant, [[maybe_unused]] const VectorXd& x, const Shared<const Frame>& aFrameSPtr
) const
{
    // TBM: Allow frame conversion through `CoordinateSubset.inFrame` method, once we have a `CartesianAcceleration`
    // class
    if (aFrameSPtr != frameSPtr_)
    {
        throw ostk::core::error::RuntimeError("Contribution Frame conversion to non-inertial not yet supported.");
    }

    if (anInstant < instants_.accessFirst() || anInstant > instants_.accessLast())
    {
        return VectorXd::Zero(interpolators_.getSize());
    }

    const double epoch = (anInstant - instants_.accessFirst()).inSeconds();

    VectorXd contribution(interpolators_.getSize());
    for (Index i = 0; i < interpolators_.getSize(); ++i)
    {
        contribution(i) = interpolators_[i]->evaluate(epoch);
    }

    return contribution;
}

void Tabulated::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Tabulated Dynamics") : void();

    Dynamics::print(anOutputStream, false);

    for (const auto& subset : writeCoordinateSubsets_)
    {
        ostk::core::utils::Print::Line(anOutputStream) << subset->getName() << subset->getSize();
    }

    const String profileDimensions = String::Format("{}x{}", contributionProfile_.rows(), contributionProfile_.cols());

    ostk::core::utils::Print::Line(anOutputStream) << "Contribution Profile:" << profileDimensions;

    const String interval =
        String::Format("[{}, {}]", instants_.accessFirst().toString(), instants_.accessLast().toString());

    ostk::core::utils::Print::Line(anOutputStream) << "Interval:" << interval;

    ostk::core::utils::Print::Line(anOutputStream) << "Frame:" << frameSPtr_->getName();

    displayDecorator ? ostk::core::utils::Print::Footer(anOutputStream) : void();
}

}  // namespace dynamics
}  // namespace astrodynamics
}  // namespace ostk
