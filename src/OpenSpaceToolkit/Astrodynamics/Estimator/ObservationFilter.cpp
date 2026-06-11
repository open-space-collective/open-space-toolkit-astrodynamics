/// Apache License 2.0

#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Utility.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Estimator/ObservationFilter.hpp>

namespace ostk
{
namespace astrodynamics
{
namespace estimator
{

ObservationFilter::Analysis::Analysis(const VectorXi& anInlierMask)
    : inlierMask(anInlierMask)
{
}

std::ostream& operator<<(std::ostream& anOutputStream, const ObservationFilter::Analysis& anAnalysis)
{
    anAnalysis.print(anOutputStream);

    return anOutputStream;
}

void ObservationFilter::Analysis::print(std::ostream& anOutputStream, bool displayDecorator) const
{
    if (displayDecorator)
    {
        ostk::core::utils::Print::Header(anOutputStream, "Observation Filter Analysis");
    }

    ostk::core::utils::Print::Line(anOutputStream) << "Inlier count:" << getInlierCount();
    ostk::core::utils::Print::Line(anOutputStream) << "Outlier count:" << getOutlierCount();

    if (displayDecorator)
    {
        ostk::core::utils::Print::Footer(anOutputStream);
    }
}

Size ObservationFilter::Analysis::getInlierCount() const
{
    return static_cast<Size>(inlierMask.sum());
}

Size ObservationFilter::Analysis::getOutlierCount() const
{
    return static_cast<Size>(inlierMask.size()) - getInlierCount();
}

Array<State> ObservationFilter::Analysis::getFilteredObservations(const Array<State>& anObservationStateArray) const
{
    if (static_cast<Size>(inlierMask.size()) != anObservationStateArray.getSize())
    {
        throw ostk::core::error::RuntimeError("Observation array size does not match the inlier mask size.");
    }

    Array<State> filteredObservations = Array<State>::Empty();
    filteredObservations.reserve(getInlierCount());

    for (Size i = 0; i < anObservationStateArray.getSize(); ++i)
    {
        if (inlierMask(i) != 0)
        {
            filteredObservations.add(anObservationStateArray[i]);
        }
    }

    return filteredObservations;
}

}  // namespace estimator
}  // namespace astrodynamics
}  // namespace ostk
