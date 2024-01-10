/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Types/Index.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>

namespace ostk
{
namespace astro
{
namespace dynamics
{

using ostk::core::types::Index;

using ostk::math::object::VectorXd;

Tabulated::Tabulated(
    const Array<Instant>& anInstantArray,
    const MatrixXd& aContributionProfile,
    const Array<Shared<const CoordinatesSubset>>& aWriteCoordinatesSubsets,
    const Shared<const Frame>& aFrameSPtr
)
    : Dynamics("Tabulated"),
      contributionProfile_(aContributionProfile),
      instants_(anInstantArray),
      writeCoordinatesSubsets_(aWriteCoordinatesSubsets),
      frameSPtr_(aFrameSPtr)
{
    if (anInstantArray.getSize() != (Index)aContributionProfile.rows())
    {
        throw ostk::core::error::RuntimeError(
            "Contribution profile must have the same number of rows as the number of instants."
        );
    }

    if (aWriteCoordinatesSubsets
            .map<Index>(
                [](const auto& coordinatesSubset)
                {
                    return coordinatesSubset->getSize();
                }
            )
            .reduce(std::plus<Index>()) != (Index)aContributionProfile.cols())
    {
        throw ostk::core::error::RuntimeError(
            "Contribution profile must have the same number of columns as the sum of the sizes of the write "
            "coordinates subsets."
        );
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
        // TBI: In the future we can allow the user to specify the interpolator type.
        interpolators_.add(BarycentricRational(timestamps, aContributionProfile.col(i)));
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

Array<Instant> Tabulated::getInstants() const
{
    return accessInstants();
}

const MatrixXd& Tabulated::accessContributionProfile() const
{
    return contributionProfile_;
}

MatrixXd Tabulated::getContributionProfile() const
{
    return accessContributionProfile();
}

bool Tabulated::isDefined() const
{
    return true;
}

Array<Shared<const CoordinatesSubset>> Tabulated::getReadCoordinatesSubsets() const
{
    return {};
}

Array<Shared<const CoordinatesSubset>> Tabulated::getWriteCoordinatesSubsets() const
{
    return writeCoordinatesSubsets_;
}

VectorXd Tabulated::computeContribution(
    const Instant& anInstant, [[maybe_unused]] const VectorXd& x, [[maybe_unused]] const Shared<const Frame>& aFrameSPtr
) const
{
    // TBI: Eventually we can check if the values can be converted using the subset inFrame methods.
    if (aFrameSPtr != frameSPtr_)
    {
        throw ostk::core::error::runtime::Wrong("Frame");
    }

    const double epoch = (anInstant - instants_.accessFirst()).inSeconds();

    VectorXd contribution(interpolators_.getSize());
    for (Index i = 0; i < interpolators_.getSize(); ++i)
    {
        contribution(i) = interpolators_[i].evaluate(epoch);
    }

    return contribution;
}

void Tabulated::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    displayDecorator ? ostk::core::utils::Print::Header(anOutputStream, "Tabulated Dynamics") : void();

    Dynamics::print(anOutputStream, false);

    for (const auto& subset : writeCoordinatesSubsets_)
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
}  // namespace astro
}  // namespace ostk
