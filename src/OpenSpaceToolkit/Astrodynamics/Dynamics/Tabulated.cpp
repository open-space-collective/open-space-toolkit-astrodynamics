/// Apache License 2.0

#include <numeric>

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Index.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Dynamics/Tabulated.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace dynamics
{

using ostk::core::type::Index;

using ostk::mathematics::object::VectorXd;

Tabulated::Tabulated(
    const Array<Instant>& anInstantArray,
    const MatrixXd& aContributionProfile,
    const Array<Shared<const CoordinateSubset>>& aWriteCoordinateSubset,
    const Shared<const Frame>& aFrameSPtr,
    const Interpolator::Type& anInterpolationType
)
    : Dynamics("Tabulated"),
      instants_(anInstantArray),
      contributionProfile_(aContributionProfile),
      writeCoordinateSubset_(aWriteCoordinateSubset),
      frameSPtr_(aFrameSPtr)
{
    if (anInstantArray.getSize() != (Index)aContributionProfile.rows())
    {
        throw ostk::core::error::RuntimeError(
            "Contribution profile must have the same number of rows as the number of instants."
        );
    }

    if (aWriteCoordinateSubset
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
            "coordinate subsets."
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

const Shared<const Frame>& Tabulated::accessFrame() const
{
    return frameSPtr_;
}

Shared<const Frame> Tabulated::getFrame() const
{
    return accessFrame();
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
    return writeCoordinateSubset_;
}

VectorXd Tabulated::computeContribution(
    const Instant& anInstant, [[maybe_unused]] const VectorXd& x, const Shared<const Frame>& aFrameSPtr
) const
{
    // TBM: Convert this using the Maneuver class' conversion mehtod.
    if (aFrameSPtr != frameSPtr_)
    {
        throw ostk::core::error::runtime::Wrong("Frame");
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

    for (const auto& subset : writeCoordinateSubset_)
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
