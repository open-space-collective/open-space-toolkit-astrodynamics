/// Apache License 2.0

#include <algorithm>

#include <OpenSpaceToolkit/Core/Container/Array.hpp>
#include <OpenSpaceToolkit/Core/Error.hpp>
#include <OpenSpaceToolkit/Core/Type/Real.hpp>
#include <OpenSpaceToolkit/Core/Type/Shared.hpp>
#include <OpenSpaceToolkit/Core/Type/String.hpp>

#include <OpenSpaceToolkit/Mathematics/Object/Vector.hpp>

#include <OpenSpaceToolkit/Physics/Coordinate/Frame.hpp>
#include <OpenSpaceToolkit/Physics/Coordinate/Frame/Provider/IAU/Theory.hpp>
#include <OpenSpaceToolkit/Physics/Time/DateTime.hpp>
#include <OpenSpaceToolkit/Physics/Time/Instant.hpp>
#include <OpenSpaceToolkit/Physics/Time/Scale.hpp>
#include <OpenSpaceToolkit/Physics/Unit/Derived.hpp>

#include <OpenSpaceToolkit/Astrodynamics/Conjunction/CloseApproach.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/HardBody/Spherical.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Conjunction/ProbabilityOfCollisionAlgorithm/Alfano2005.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Estimator/CovarianceMatrix.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/LocalOrbitalFrameFactory.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianPosition.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/State/CoordinateSubset/CartesianVelocity.hpp>
#include <OpenSpaceToolkit/Astrodynamics/Trajectory/StateBuilder.hpp>

#include <Global.test.hpp>

using ostk::core::container::Array;
using ostk::core::type::Real;
using ostk::core::type::Shared;
using ostk::core::type::String;

using ostk::mathematics::object::MatrixXd;
using ostk::mathematics::object::Vector3d;
using ostk::mathematics::object::VectorXd;

using ostk::physics::coordinate::Frame;
using ostk::physics::coordinate::frame::provider::iau::Theory;
using ostk::physics::time::DateTime;
using ostk::physics::time::Instant;
using ostk::physics::time::Scale;
using ostk::physics::unit::Derived;

using ostk::astrodynamics::conjunction::CloseApproach;
using ostk::astrodynamics::conjunction::hardbody::Spherical;
using ostk::astrodynamics::conjunction::ProbabilityOfCollisionAlgorithm;
using ostk::astrodynamics::conjunction::probabilityofcollisionalgorithm::Alfano2005;
using ostk::astrodynamics::estimator::CovarianceMatrix;
using ostk::astrodynamics::trajectory::LocalOrbitalFrameFactory;
using ostk::astrodynamics::trajectory::State;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianPosition;
using ostk::astrodynamics::trajectory::state::coordinatesubset::CartesianVelocity;
using ostk::astrodynamics::trajectory::StateBuilder;

namespace
{

State buildState(
    const Instant& anInstant,
    const Shared<const Frame>& aFrameSPtr,
    const Vector3d& aPosition_m,
    const Vector3d& aVelocity_m_per_s
)
{
    const StateBuilder stateBuilder = {
        aFrameSPtr,
        {CartesianPosition::Default(), CartesianVelocity::Default()},
    };

    VectorXd coordinates(6);
    coordinates << aPosition_m, aVelocity_m_per_s;

    return stateBuilder.build(anInstant, coordinates);
}

MatrixXd buildCovarianceCoordinates(const std::initializer_list<std::initializer_list<double>>& aRowList)
{
    MatrixXd coordinates(6, 6);

    Eigen::Index rowIndex = 0;
    for (const std::initializer_list<double>& row : aRowList)
    {
        Eigen::Index columnIndex = 0;
        for (const double value : row)
        {
            coordinates(rowIndex, columnIndex++) = value;
        }
        ++rowIndex;
    }

    return coordinates;
}

CloseApproach buildCloseApproach(
    const Vector3d& anObject1Position_m,
    const Vector3d& anObject1Velocity_m_per_s,
    const MatrixXd& anObject1Covariance,
    const Vector3d& anObject2Position_m,
    const Vector3d& anObject2Velocity_m_per_s,
    const MatrixXd& anObject2Covariance
)
{
    const Instant instant = Instant::J2000();
    const Shared<const Frame> gcrfFrameSPtr = Frame::GCRF();

    State object1State = buildState(instant, gcrfFrameSPtr, anObject1Position_m, anObject1Velocity_m_per_s);
    State object2State = buildState(instant, gcrfFrameSPtr, anObject2Position_m, anObject2Velocity_m_per_s);

    object1State.setCovarianceMatrix(CovarianceMatrix(
        instant, anObject1Covariance, gcrfFrameSPtr, {CartesianPosition::Default(), CartesianVelocity::Default()}
    ));
    object2State.setCovarianceMatrix(CovarianceMatrix(
        instant, anObject2Covariance, gcrfFrameSPtr, {CartesianPosition::Default(), CartesianVelocity::Default()}
    ));

    return {object1State, object2State};
}

// PoC test cases from:
//
// Alfano, Salvatore. (2009). Satellite Conjunction Monte Carlo Analysis. AAS 09-233.
//
// Although most of these cases are low-relative velocity encounters (outside the short-term encounter assumption of
// Alfano 2005), they were designed by Alfano as a benchmark for probability of collision methods, mixing linear and
// nonlinear relative motion. The expected values are those of the 2D probability of collision integral (not the Monte
// Carlo truth), hence these tests check the numerical integration rather than the physical validity of the method.
//
// The data (ECI position [m], velocity [m/s] and position-velocity covariance [m, s] at TCA), the combined hard-body
// radii and the expected probabilities of collision are taken from NASA CARA Analysis Tools (PcCircle_UnitTest.m):
// https://github.com/nasa/CARA_Analysis_Tools

// PoC Case 01 (GEO, nonlinear relative motion) from Alfano (2009), at TCA
CloseApproach buildPoCCase01()
{
    return buildCloseApproach(
        Vector3d(153446.76456028, 41874155.869566, 0.0),
        Vector3d(3066.8747609105, -11.373614956472, 0.0),
        buildCovarianceCoordinates(
            {{6494.0796232671, -376.13858334524, 0.0, 0.015989484167213, -0.49426167732977, 0.0},
             {-376.13858334524, 22.559465194894, 0.0, -0.00098831404423753, 0.028569460577143, 0.0},
             {0.0, 0.0, 1.2050395223076, 0.0, 0.0, -6.0708763444925e-05},
             {0.015989484167213, -0.00098831404423753, 0.0, 4.4372750192502e-08, -1.2122341939459e-06, 0.0},
             {-0.49426167732977, 0.028569460577143, 0.0, -1.2122341939459e-06, 3.7622976036729e-05, 0.0},
             {0.0, 0.0, -6.0708763444925e-05, 0.0, 0.0, 3.3903900107678e-09}}
        ),
        Vector3d(153447.2642029, 41874156.369903, 4.9999660257629),
        Vector3d(3066.8647607073, -11.363614817883, -1.3580619953668e-06),
        buildCovarianceCoordinates(
            {{6494.2249318545,
              -376.15611665373,
              -4.4917291988265e-05,
              0.015992172760904,
              -0.49427210175926,
              -5.9018359337438e-08},
             {-376.15611665373,
              22.560645821883,
              2.5501472714422e-06,
              -0.00098846458097965,
              0.028570758726652,
              3.4187295427319e-09},
             {-4.4917291988265e-05,
              2.5501472714422e-06,
              1.2046746471441,
              -1.1803345036853e-10,
              3.4189216426795e-09,
              -6.0715345613191e-05},
             {0.015992172760904,
              -0.00098846458097965,
              -1.1803345036853e-10,
              4.4383012786732e-08,
              -1.2124371638594e-06,
              -1.4477056719694e-13},
             {-0.49427210175926,
              0.028570758726652,
              3.4189216426795e-09,
              -1.2124371638594e-06,
              3.7623723352821e-05,
              4.4920403553475e-12},
             {-5.9018359337438e-08,
              3.4187295427319e-09,
              -6.0715345613191e-05,
              -1.4477056719694e-13,
              4.4920403553475e-12,
              3.3920803457708e-09}}
        )
    );
}

// PoC Case 02 (GEO, nonlinear relative motion) from Alfano (2009), at TCA
CloseApproach buildPoCCase02()
{
    return buildCloseApproach(
        Vector3d(153446.17961085, 41874155.871735, 0.0),
        Vector3d(3066.8747610694, -11.373571599494, 0.0),
        buildCovarianceCoordinates(
            {{6494.0796358449, -376.13849196421, 0.0, 0.015989491092283, -0.49426167765486, 0.0},
             {-376.13849196421, 22.559454565828, 0.0, -0.00098831421764957, 0.028569453583889, 0.0},
             {0.0, 0.0, 1.2050395210891, 0.0, 0.0, -6.0708763434883e-05},
             {0.015989491092283, -0.00098831421764957, 0.0, 4.4372784079894e-08, -1.2122347193874e-06, 0.0},
             {-0.49426167765486, 0.028569453583889, 0.0, -1.2122347193874e-06, 3.7622976013613e-05, 0.0},
             {0.0, 0.0, -6.0708763434883e-05, 0.0, 0.0, 3.3903900131841e-09}}
        ),
        Vector3d(153446.67923983, 41874156.372071, 4.999966026022),
        Vector3d(3066.8647608662, -11.36357145973, -1.3580568142621e-06),
        buildCovarianceCoordinates(
            {{6494.224944434,
              -376.15602526787,
              -4.4917281068031e-05,
              0.015992179686283,
              -0.49427210208441,
              -5.9018359376242e-08},
             {-376.15602526787,
              22.560635191994,
              2.5501460018111e-06,
              -0.0009884647543741,
              0.028570751733036,
              3.4187287076596e-09},
             {-4.4917281068031e-05,
              2.5501460018111e-06,
              1.2046746459254,
              -1.1803347105468e-10,
              3.418920806973e-09,
              -6.0715345603113e-05},
             {0.015992179686283,
              -0.0009884647543741,
              -1.1803347105468e-10,
              4.4383046680581e-08,
              -1.2124376893235e-06,
              -1.4477062993971e-13},
             {-0.49427210208441,
              0.028570751733036,
              3.418920806973e-09,
              -1.2124376893235e-06,
              3.7623723329706e-05,
              4.4920403525857e-12},
             {-5.9018359376242e-08,
              3.4187287076596e-09,
              -6.0715345603113e-05,
              -1.4477062993971e-13,
              4.4920403525857e-12,
              3.3920803481865e-09}}
        )
    );
}

// PoC Case 03 (GEO, linear relative motion) from Alfano (2009), at TCA
CloseApproach buildPoCCase03()
{
    return buildCloseApproach(
        Vector3d(153951.4752631, 41874153.994752, 0.0),
        Vector3d(3066.8746235984, -11.411024582792, 0.0),
        buildCovarianceCoordinates(
            {{6494.0687699936, -376.21742938622, 0.0, 0.015983509029739, -0.4942613967814, 0.0},
             {-376.21742938622, 22.568637218841, 0.0, -0.00098816434691557, 0.028575494554259, 0.0},
             {0.0, 0.0, 1.2050405732107, 0.0, 0.0, -6.0708772120198e-05},
             {0.015983509029739, -0.00098816434691557, 0.0, 4.4343516692378e-08, -1.2117808283202e-06, 0.0},
             {-0.4942613967814, 0.028575494554259, 0.0, -1.2117808283202e-06, 3.7622995977956e-05, 0.0},
             {0.0, 0.0, -6.0708772120198e-05, 0.0, 0.0, 3.390387928148e-09}}
        ),
        Vector3d(153951.97327397, 41874156.744625, 2.7520743270427),
        Vector3d(3066.8646233948, -0.044998591717235, -11.356027211548),
        buildCovarianceCoordinates(
            {{6539.7159476886,
              -354.4827622708,
              -24.21583392685,
              0.016136440793024,
              -0.49756753134344,
              -6.67805505728e-05},
             {-354.4827622708,
              19.987684161349,
              1.3128407317001,
              -0.00093662999878721,
              0.026911710021045,
              3.8371255372188e-06},
             {-24.21583392685,
              1.3128407317001,
              1.2674955658292,
              -5.9987007527256e-05,
              0.0018427685478909,
              -6.0239191348913e-05},
             {0.016136440793024,
              -0.00093662999878721,
              -5.9987007527256e-05,
              4.4783634184199e-08,
              -1.2229871227354e-06,
              -1.697954617528e-10},
             {-0.49756753134344,
              0.026911710021045,
              0.0018427685478909,
              -1.2229871227354e-06,
              3.7861909909241e-05,
              5.0463685349191e-09},
             {-6.67805505728e-05,
              3.8371255372188e-06,
              -6.0239191348913e-05,
              -1.697954617528e-10,
              5.0463685349191e-09,
              3.4465981691675e-09}}
        )
    );
}

// PoC Case 04 (GEO, nonlinear relative motion) from Alfano (2009), at TCA
CloseApproach buildPoCCase04()
{
    return buildCloseApproach(
        Vector3d(-28570333.448385, -29444167.191947, 0.0),
        Vector3d(-2264.3786988144, 2161.3955655441, 0.0),
        buildCovarianceCoordinates(
            {{3149.7395861594, -3009.1218002121, 0.0, -0.23302644062163, -0.23220620539474, 0.0},
             {-3009.1218002121, 2874.8685578726, 0.0, 0.22261527096903, 0.22183798952038, 0.0},
             {0.0, 0.0, 0.045936290392913, 0.0, 0.0, -7.6019546328751e-06},
             {-0.23302644062163, 0.22261527096903, 0.0, 1.7244984336726e-05, 1.7183923885606e-05, 0.0},
             {-0.23220620539474, 0.22183798952038, 0.0, 1.7183923885606e-05, 1.7123548916969e-05, 0.0},
             {0.0, 0.0, -7.6019546328751e-06, 0.0, 0.0, 9.965752792065e-09}}
        ),
        Vector3d(-28570452.261986, -29444104.463515, -3.7579884937972),
        Vector3d(-2264.3698138968, 2161.4123960717, 0.00025033152399579),
        buildCovarianceCoordinates(
            {{3149.8133559411,
              -3009.2292261314,
              -0.00034854080727125,
              -0.2330319013629,
              -0.23221038587232,
              -2.9682404241872e-08},
             {-3009.2292261314,
              2875.0065497795,
              0.00033298927638278,
              0.22262320209625,
              0.22184469174607,
              2.8358339352467e-08},
             {-0.00034854080727125,
              0.00033298927638278,
              0.045966311979646,
              2.5785074346198e-08,
              2.569582790162e-08,
              -7.6210856423979e-06},
             {-0.2330319013629,
              0.22262320209625,
              2.5785074346198e-08,
              1.7245388624804e-05,
              1.7184231903355e-05,
              2.1965297919875e-12},
             {-0.23221038587232,
              0.22184469174607,
              2.569582790162e-08,
              1.7184231903355e-05,
              1.7123761548397e-05,
              2.1876403853848e-12},
             {-2.9682404241872e-08,
              2.8358339352467e-08,
              -7.6210856423979e-06,
              2.1965297919875e-12,
              2.1876403853848e-12,
              9.9655797275807e-09}}
        )
    );
}

// PoC Case 05 (LEO, linear relative motion) from Alfano (2009), at TCA
CloseApproach buildPoCCase05()
{
    return buildCloseApproach(
        Vector3d(6878090.1622937, -17948.6785967, -17948.6785967),
        Vector3d(28.09377718079, 5382.8902061517, 5382.8902061517),
        buildCovarianceCoordinates(
            {{0.06420452636324,
              -18.990691938367,
              -18.990691938367,
              0.029712221972673,
              -0.00016879879779863,
              -0.00016879879779863},
             {-18.990691938367, 7904.0447002725, 7903.9662413044, -12.380432521341, 0.064200212052864, 0.064168687514337
             },
             {-18.990691938367, 7903.9662413044, 7904.0447002725, -12.380432521341, 0.064168687514337, 0.064200212052864
             },
             {0.029712221972673,
              -12.380432521341,
              -12.380432521341,
              0.019392185895765,
              -0.00010050864871277,
              -0.00010050864871277},
             {-0.00016879879779863,
              0.064200212052864,
              0.064168687514337,
              -0.00010050864871277,
              5.4472961901962e-07,
              5.2059220585993e-07},
             {-0.00016879879779863,
              0.064168687514337,
              0.064200212052864,
              -0.00010050864871277,
              5.2059220585993e-07,
              5.4472961901962e-07}}
        ),
        Vector3d(6878089.1619754, -17946.678929416, -17947.678286012),
        Vector3d(28.39378131578, 5383.1902163515, 5382.5902081838),
        buildCovarianceCoordinates(
            {{0.062112348981822,
              -18.552069769383,
              -18.550001998859,
              0.029023568403742,
              -0.00016522330639711,
              -0.0001652175349509},
             {-18.552069769383, 7905.3548684174, 7904.3953083026, -12.381795445702, 0.064205601367087, 0.064172326730475
             },
             {-18.550001998859, 7904.3953083026, 7903.5927814582, -12.380415416999, 0.064166926903371, 0.064196696195881
             },
             {0.029023568403742,
              -12.381795445702,
              -12.380415416999,
              0.019393241165813,
              -0.00010051149946813,
              -0.00010050875442127},
             {-0.00016522330639711,
              0.064205601367087,
              0.064166926903371,
              -0.00010051149946813,
              5.4472590966975e-07,
              5.2057908867893e-07},
             {-0.0001652175349509,
              0.064172326730475,
              0.064196696195881,
              -0.00010050875442127,
              5.2057908867893e-07,
              5.4469753157094e-07}}
        )
    );
}

// PoC Case 06 (LEO, near-linear relative motion) from Alfano (2009), at TCA
CloseApproach buildPoCCase06()
{
    return buildCloseApproach(
        Vector3d(6877715.634247, 53834.214039436, 53834.214039436),
        Vector3d(-84.26282780536, 5382.5970952289, 5382.5970952289),
        buildCovarianceCoordinates(
            {{430.02827939777, -18393.891381385, -18393.891381385, 28.811775614851, 0.14985494859082, 0.14985494859082},
             {-18393.891381385, 790190.51867865, 790186.9676547, -1237.8765173077, -6.5012185382348, -6.5024449244912},
             {-18393.891381385, 790186.9676547, 790190.51867865, -1237.8765173077, -6.5024449244912, -6.5024449244912},
             {28.811775614851, -1237.8765173077, -1237.8765173077, 1.9392157223809, 0.010188218207003, 0.010188218207003
             },
             {0.14985494859082,
              -6.5012185382348,
              -6.5024449244912,
              0.010188218207003,
              5.5474681548641e-05,
              5.3924699534112e-05},
             {0.14985494859082,
              -6.5024449244912,
              -6.5012185382348,
              0.010188218207003,
              5.3924699534112e-05,
              5.5474681548641e-05}}
        ),
        Vector3d(6877716.6344015, 53835.214140512, 53836.213772982),
        Vector3d(-84.16281513631, 5382.6970892007, 5382.4970647006),
        buildCovarianceCoordinates(
            {{429.35284572627, -18379.895353487, -18379.212284332, 28.78930421662, 0.14973961436546, 0.14974660730687},
             {-18379.895353487, 790234.93895821, 790202.01968289, -1237.9226383171, -6.5015746157359, -6.5030993172088},
             {-18379.212284332, 790202.01968289, 790176.20377729, -1237.8766321984, -6.5025593138425, -6.5025593138425},
             {28.78930421662, -1237.9226383171, -1237.8766321984, 1.9392512139259, 0.010188582861453, 0.010189050148708
             },
             {0.14973961436546,
              -6.5015746157359,
              -6.5025593138425,
              0.010188582861453,
              5.5477431358815e-05,
              5.392995296381e-05},
             {0.14974660730687,
              -6.5030993172088,
              -6.5016312695197,
              0.010189050148708,
              5.392995296381e-05,
              5.5482312043825e-05}}
        )
    );
}

// PoC Case 07 (LEO, nonlinear relative motion) from Alfano (2009), at TCA
CloseApproach buildPoCCase07()
{
    return buildCloseApproach(
        Vector3d(-6877469.169824, -67773.18379991, -67773.18379991),
        Vector3d(106.08062887158, -5382.4047050208, -5382.4047050208),
        buildCovarianceCoordinates(
            {{47752.931002152, -1772101.1180686, -1772101.1180686, 2775.8528221228, 20.739334423161, 20.739334423161},
             {-1772101.1180686, 65779308.882914, 65778953.792515, -103038.88708843, -770.14105253792, -770.26370445419},
             {-1772101.1180686, 65778953.792515, 65779308.882914, -103038.88708843, -770.26370445419, -770.14105253792},
             {2775.8528221228, -103038.88708843, -103038.88708843, 161.40405348647, 1.2065006151755, 1.2065006151755},
             {20.739334423161,
              -770.14105253792,
              -770.26370445419,
              1.2065006151755,
              0.0091040078465583,
              0.0089489952282481},
             {20.739334423161,
              -770.26370445419,
              -770.14105253792,
              1.2065006151755,
              0.0089489952282481,
              0.0091040078465583}}
        ),
        Vector3d(-6877468.6856734, -67775.155096794, -67775.63603513),
        Vector3d(106.21663006962, -5382.5006947728, -5382.3006891309),
        buildCovarianceCoordinates(
            {{47842.556250334, -1773779.529826, -1773713.6133993, 2778.4317194583, 20.758959911492, 20.759400142645},
             {-1773779.529826, 65780452.931669, 65777653.286419, -103038.81551237, -770.15294459463, -770.29183080786},
             {-1773713.6133993, 65777653.286419, 65775564.028398, -103034.98641991, -770.24689380951, -770.14063157659},
             {2778.4317194583, -103038.81551237, -103034.98641991, 161.40102219608, 1.2064973414976, 1.2065228957635},
             {20.758959911492,
              -770.15294459463,
              -770.24689380951,
              1.2064973414976,
              0.0091040748211739,
              0.0089493348927616},
             {20.759400142645,
              -770.29183080786,
              -770.14063157659,
              1.2065228957635,
              0.0089493348927616,
              0.0091044620423118}}
        )
    );
}

// PoC Case 08 (MEO, nonlinear relative motion) from Alfano (2009), at TCA
CloseApproach buildPoCCase08()
{
    return buildCloseApproach(
        Vector3d(14971649.316903, -20270398.211949, 4076385.1140421),
        Vector3d(1012.256512097, 1460.8114696427, 3527.3938005475),
        buildCovarianceCoordinates(
            {{153.41029880099,
              250.60609601194,
              567.38203178725,
              -0.059078881462783,
              0.081043167438988,
              -0.014865617039894},
             {250.60609601194,
              411.01058797583,
              929.49019659682,
              -0.096738898128766,
              0.13280023847998,
              -0.024239891541315},
             {567.38203178725, 929.49019659682, 2104.6873950138, -0.21904874053446, 0.30061285407618, -0.055007273173277
             },
             {-0.059078881462783,
              -0.096738898128766,
              -0.21904874053446,
              2.2800456871845e-05,
              -3.1287003959382e-05,
              5.7271493246217e-06},
             {0.081043167438988,
              0.13280023847998,
              0.30061285407618,
              -3.1287003959382e-05,
              4.2940647603998e-05,
              -7.8529855374707e-06},
             {-0.014865617039894,
              -0.024239891541315,
              -0.055007273173277,
              5.7271493246217e-06,
              -7.8529855374707e-06,
              1.4467870077839e-06}}
        ),
        Vector3d(14971649.362587, -20270399.371468, 4076387.8292675),
        Vector3d(1012.2574006063, 1460.8115969929, 3527.3938402543),
        buildCovarianceCoordinates(
            {{153.41084903437,
              250.60648831196,
              567.38320417048,
              -0.059078997697842,
              0.081043314135805,
              -0.014865678390079},
             {250.60648831196, 411.01040332214, 929.49023783034, -0.096738893907636, 0.1328002113806, -0.024239942513815
             },
             {567.38320417048, 929.49023783034, 2104.6885373437, -0.2190488400577, 0.30061294064373, -0.055007415866958
             },
             {-0.059078997697842,
              -0.096738893907636,
              -0.2190488400577,
              2.2800465138493e-05,
              -3.1287010197161e-05,
              5.7271636219629e-06},
             {0.081043314135805,
              0.1328002113806,
              0.30061294064373,
              -3.1287010197161e-05,
              4.29406491462e-05,
              -7.8530038400398e-06},
             {-0.014865678390079,
              -0.024239942513815,
              -0.055007415866958,
              5.7271636219629e-06,
              -7.8530038400398e-06,
              1.446793684243e-06}}
        )
    );
}

// PoC Case 09 (HEO, nonlinear relative motion) from Alfano (2009), at TCA
CloseApproach buildPoCCase09()
{
    return buildCloseApproach(
        Vector3d(-5532700.6575059, 20132673.95812, 40010548.546273),
        Vector3d(-1450.9451284357, -311.60857222859, -671.30191250233),
        buildCovarianceCoordinates(
            {{67.013620376984,
              14.572096438565,
              31.362985178843,
              -0.0018373240924696,
              0.0039149399934552,
              0.0077543657627383},
             {14.572096438565,
              3.2132921306927,
              6.8233552264667,
              -0.00039892190369618,
              0.00085775667714172,
              0.0016882826408446},
             {31.362985178843,
              6.8233552264667,
              14.72880348181,
              -0.00085890275305298,
              0.0018343498030328,
              0.0036389040914032},
             {-0.0018373240924696,
              -0.00039892190369618,
              -0.00085890275305298,
              5.2625869314701e-08,
              -1.0901907556364e-07,
              -2.1622909141821e-07},
             {0.0039149399934552,
              0.00085775667714172,
              0.0018343498030328,
              -1.0901907556364e-07,
              2.3880799931959e-07,
              4.531837818429e-07},
             {0.0077543657627383,
              0.0016882826408446,
              0.0036389040914032,
              -2.1622909141821e-07,
              4.531837818429e-07,
              9.0751452858224e-07}}
        ),
        Vector3d(-5532694.0174556, 20132676.507378, 40010553.862016),
        Vector3d(-1450.9465079251, -311.60785722481, -671.30053151494),
        buildCovarianceCoordinates(
            {{67.014285755464,
              14.572190820724,
              31.363196175928,
              -0.0018373334329171,
              0.0039149699432129,
              0.0077544252668539},
             {14.572190820724,
              3.2133009783397,
              6.8233774594117,
              -0.00039892244761992,
              0.00085775960971626,
              0.0016882897157403},
             {31.363196175928,
              6.8233774594117,
              14.728854328372,
              -0.00085890413424338,
              0.0018343579101633,
              0.0036389196118305},
             {-0.0018373334329171,
              -0.00039892244761992,
              -0.00085890413424338,
              5.2625918217667e-08,
              -1.0901938617321e-07,
              -2.1622971004667e-07},
             {0.0039149699432129,
              0.00085775960971626,
              0.0018343579101633,
              -1.0901938617321e-07,
              2.388092081231e-07,
              4.5318617899403e-07},
             {0.0077544252668539,
              0.0016882897157403,
              0.0036389196118305,
              -2.1622971004667e-07,
              4.5318617899403e-07,
              9.0751929688805e-07}}
        )
    );
}

// PoC Case 10 (HEO, nonlinear relative motion) from Alfano (2009), at TCA
CloseApproach buildPoCCase10()
{
    return buildCloseApproach(
        Vector3d(-5532700.6575059, 20132673.95812, 40010548.546273),
        Vector3d(-1450.9451284357, -311.60857222859, -671.30191250233),
        buildCovarianceCoordinates(
            {{67.013620376984,
              14.572096438565,
              31.362985178843,
              -0.0018373240924696,
              0.0039149399934552,
              0.0077543657627383},
             {14.572096438565,
              3.2132921306927,
              6.8233552264667,
              -0.00039892190369618,
              0.00085775667714172,
              0.0016882826408446},
             {31.362985178843,
              6.8233552264667,
              14.72880348181,
              -0.00085890275305298,
              0.0018343498030328,
              0.0036389040914032},
             {-0.0018373240924696,
              -0.00039892190369618,
              -0.00085890275305298,
              5.2625869314701e-08,
              -1.0901907556364e-07,
              -2.1622909141821e-07},
             {0.0039149399934552,
              0.00085775667714172,
              0.0018343498030328,
              -1.0901907556364e-07,
              2.3880799931959e-07,
              4.531837818429e-07},
             {0.0077543657627383,
              0.0016882826408446,
              0.0036389040914032,
              -2.1622909141821e-07,
              4.531837818429e-07,
              9.0751452858224e-07}}
        ),
        Vector3d(-5532694.0174556, 20132676.507378, 40010553.862016),
        Vector3d(-1450.9465079251, -311.60785722481, -671.30053151494),
        buildCovarianceCoordinates(
            {{67.014285755464,
              14.572190820724,
              31.363196175928,
              -0.0018373334329171,
              0.0039149699432129,
              0.0077544252668539},
             {14.572190820724,
              3.2133009783397,
              6.8233774594117,
              -0.00039892244761992,
              0.00085775960971626,
              0.0016882897157403},
             {31.363196175928,
              6.8233774594117,
              14.728854328372,
              -0.00085890413424338,
              0.0018343579101633,
              0.0036389196118305},
             {-0.0018373334329171,
              -0.00039892244761992,
              -0.00085890413424338,
              5.2625918217667e-08,
              -1.0901938617321e-07,
              -2.1622971004667e-07},
             {0.0039149699432129,
              0.00085775960971626,
              0.0018343579101633,
              -1.0901938617321e-07,
              2.388092081231e-07,
              4.5318617899403e-07},
             {0.0077544252668539,
              0.0016882897157403,
              0.0036389196118305,
              -2.1622971004667e-07,
              4.5318617899403e-07,
              9.0751929688805e-07}}
        )
    );
}

// PoC Case 11 (LEO, leader-follower) from Alfano (2009), at TCA
CloseApproach buildPoCCase11()
{
    return buildCloseApproach(
        Vector3d(1315785.8155696, 6751109.2628038, 0.0),
        Vector3d(-7472.0159764697, 1456.2899595947, 0.0),
        buildCovarianceCoordinates(
            {{672549.6029462, -127037.13594612, 0.0, 147.09662117227, 745.6801208292, 0.0},
             {-127037.13594612, 24000.066037901, 0.0, -27.78046281733, -140.85155334472, 0.0},
             {0.0, 0.0, 0.15424708214425, 0.0, 0.0, -0.00082957202192127},
             {147.09662117227, -27.78046281733, 0.0, 0.032176982800041, 0.16309042037779, 0.0},
             {745.6801208292, -140.85155334472, 0.0, 0.16309042037779, 0.82676277106866, 0.0},
             {0.0, 0.0, -0.00082957202192127, 0.0, 0.0, 4.720930402259e-06}}
        ),
        Vector3d(1315711.0953284, 6751123.82529, 0.0),
        Vector3d(-7472.0320939884, 1456.2072604687, 0.0),
        buildCovarianceCoordinates(
            {{672552.41491736, -127029.95787575, 0.0, 147.08867554203, 745.68330770247, 0.0},
             {-127029.95787575, 23997.254065287, 0.0, -27.777275942449, -140.8436077145, 0.0},
             {0.0, 0.0, 0.15424708214409, 0.0, 0.0, -0.00082957202192083},
             {147.08867554203, -27.777275942449, 0.0, 0.032173372781716, 0.16308162599342, 0.0},
             {745.68330770247, -140.8436077145, 0.0, 0.16308162599342, 0.8267663810852, 0.0},
             {0.0, 0.0, -0.00082957202192083, 0.0, 0.0, 4.7209304022592e-06}}
        )
    );
}

// PoC Case 12 (LEO, identical orbits) from Alfano (2009), at TCA
CloseApproach buildPoCCase12()
{
    return buildCloseApproach(
        Vector3d(1315785.8155696, 6751109.2628038, 0.0),
        Vector3d(-7472.0159764697, 1456.2899595947, 0.0),
        buildCovarianceCoordinates(
            {{672549.6029462, -127037.13594612, 0.0, 147.09662117227, 745.6801208292, 0.0},
             {-127037.13594612, 24000.066037901, 0.0, -27.78046281733, -140.85155334472, 0.0},
             {0.0, 0.0, 0.15424708214425, 0.0, 0.0, -0.00082957202192127},
             {147.09662117227, -27.78046281733, 0.0, 0.032176982800041, 0.16309042037779, 0.0},
             {745.6801208292, -140.85155334472, 0.0, 0.16309042037779, 0.82676277106866, 0.0},
             {0.0, 0.0, -0.00082957202192127, 0.0, 0.0, 4.720930402259e-06}}
        ),
        Vector3d(1315785.8155696, 6751109.2628038, 0.0),
        Vector3d(-7472.0159764697, 1456.2899595947, 0.0),
        buildCovarianceCoordinates(
            {{672549.6029462, -127037.13594612, 0.0, 147.09662117227, 745.6801208292, 0.0},
             {-127037.13594612, 24000.066037901, 0.0, -27.78046281733, -140.85155334472, 0.0},
             {0.0, 0.0, 0.15424708214425, 0.0, 0.0, -0.00082957202192127},
             {147.09662117227, -27.78046281733, 0.0, 0.032176982800041, 0.16309042037779, 0.0},
             {745.6801208292, -140.85155334472, 0.0, 0.16309042037779, 0.82676277106866, 0.0},
             {0.0, 0.0, -0.00082957202192127, 0.0, 0.0, 4.720930402259e-06}}
        )
    );
}

// Analysis test cases from NASA CARA Analysis Tools (DilutionMaxPc_UnitTest.m, Omitron CDM test cases):
// https://github.com/nasa/CARA_Analysis_Tools
//
// The data (EME2000 position [km] and velocity [km/s], RTN position covariance [m^2] at TCA), the combined hard-body
// radii and the expected maximum probabilities of collision are taken from the CDMs and the unit test. As in NASA CARA,
// only the position covariances are used.
//
// NASA CARA scales both covariances by the square of its scale factor (i.e. its scale factor applies to the standard
// deviations), whereas the scaling factor of computeProbabilityAnalysis applies to the covariances: the expected
// maximum probabilities of collision are independent of this convention, the scale factors at which they occur are
// not.

MatrixXd buildRTNPositionCovarianceCoordinates(const std::initializer_list<std::initializer_list<double>>& aRowList)
{
    MatrixXd coordinates(3, 3);

    Eigen::Index rowIndex = 0;
    for (const std::initializer_list<double>& row : aRowList)
    {
        Eigen::Index columnIndex = 0;
        for (const double value : row)
        {
            coordinates(rowIndex, columnIndex++) = value;
        }
        ++rowIndex;
    }

    return coordinates;
}

CloseApproach buildAnalysisCloseApproach(
    const Instant& anInstant,
    const Vector3d& anObject1Position_km,
    const Vector3d& anObject1Velocity_km_per_s,
    const MatrixXd& anObject1RTNPositionCovariance,
    const Vector3d& anObject2Position_km,
    const Vector3d& anObject2Velocity_km_per_s,
    const MatrixXd& anObject2RTNPositionCovariance
)
{
    const Shared<const Frame> j2000FrameSPtr = Frame::J2000(Theory::IAU_2006);
    const Shared<const LocalOrbitalFrameFactory> rtnFrameFactorySPtr = LocalOrbitalFrameFactory::QSW(j2000FrameSPtr);

    State object1State =
        buildState(anInstant, j2000FrameSPtr, anObject1Position_km * 1e3, anObject1Velocity_km_per_s * 1e3);
    State object2State =
        buildState(anInstant, j2000FrameSPtr, anObject2Position_km * 1e3, anObject2Velocity_km_per_s * 1e3);

    // RTN frames (equivalent to QSW frames)
    object1State.setCovarianceMatrix(CovarianceMatrix(
        anInstant,
        anObject1RTNPositionCovariance,
        rtnFrameFactorySPtr->generateFrame(object1State),
        {CartesianPosition::Default()}
    ));
    object2State.setCovarianceMatrix(CovarianceMatrix(
        anInstant,
        anObject2RTNPositionCovariance,
        rtnFrameFactorySPtr->generateFrame(object2State),
        {CartesianPosition::Default()}
    ));

    return {object1State, object2State};
}

// Analysis Case 01: High probability of collision (Omitron test 01, HighPc), at TCA
CloseApproach buildAnalysisCase01()
{
    return buildAnalysisCloseApproach(
        Instant::DateTime(DateTime::Parse("2008-06-27 15:34:55.320"), Scale::UTC),
        Vector3d(-1818.269382, 1040.563930, -6772.707308),
        Vector3d(-3.609802798, 6.269245755, 1.933211527),
        buildRTNPositionCovarianceCoordinates(
            {{1.858000000000000e+01, -1.744999999999664e-01, 2.086000000000027e+00},
             {-1.744999999999664e-01, 1.190000000000000e+03, 1.275000000000094e+00},
             {2.086000000000027e+00, 1.275000000000094e+00, 3.391999999999984e+00}}
        ),
        Vector3d(-1818.277060, 1040.554778, -6772.707872),
        Vector3d(6.316590032, -3.383797284, -2.177018239),
        buildRTNPositionCovarianceCoordinates(
            {{1.406000000000001e+02, -5.145999999999999e+02, -2.912999999999997e+01},
             {-5.145999999999999e+02, 9.417000000000000e+03, 3.658000000000002e+02},
             {-2.912999999999997e+01, 3.658000000000002e+02, 5.071000000000074e+01}}
        )
    );
}

// Analysis Case 02: High radial uncertainty (Omitron test 02, MaxRadialSigma), at TCA
CloseApproach buildAnalysisCase02()
{
    return buildAnalysisCloseApproach(
        Instant::DateTime(DateTime::Parse("2014-07-16 22:32:26.716"), Scale::UTC),
        Vector3d(6703.392053, -1969.223265, 3276.274276),
        Vector3d(3.527732779, 2.322656571, -5.815904333),
        buildRTNPositionCovarianceCoordinates(
            {{5.841000000000003e+01, -9.881000000000006e+01, 1.875000000000004e+01},
             {-9.881000000000006e+01, 1.253000000000000e+03, 2.552999999999933e+00},
             {1.875000000000004e+01, 2.552999999999933e+00, 6.209000000000003e+01}}
        ),
        Vector3d(6703.301055, -1969.231944, 3276.271432),
        Vector3d(2.538967996, 6.496491661, 5.104787983),
        buildRTNPositionCovarianceCoordinates(
            {{4.355000000000002e+07, 1.242000000000000e+08, 2.325999999999982e+04},
             {1.242000000000000e+08, 3.542999999999999e+08, 6.465999999997421e+04},
             {2.325999999999982e+04, 6.465999999997421e+04, 8.801999999867694e+02}}
        )
    );
}

// Analysis Case 03: High in-track uncertainty (Omitron test 03, MaxIntrackSigma), at TCA
CloseApproach buildAnalysisCase03()
{
    return buildAnalysisCloseApproach(
        Instant::DateTime(DateTime::Parse("2012-01-29 18:53:07.663"), Scale::UTC),
        Vector3d(5483.232690, 4603.687306, -595.447099),
        Vector3d(0.255132042, -1.241060505, -7.341124839),
        buildRTNPositionCovarianceCoordinates(
            {{7.569000000000006e+02, -1.724000000000000e+03, -4.311999999999975e+01},
             {-1.724000000000000e+03, 6.053000000000002e+04, 7.017000000000076e+01},
             {-4.311999999999975e+01, 7.017000000000076e+01, 5.832000000000030e+01}}
        ),
        Vector3d(5482.925919, 4604.102180, -595.505972),
        Vector3d(0.164160898, 0.730858653, 7.399012108),
        buildRTNPositionCovarianceCoordinates(
            {{1.508999999999955e+06, -4.805000000000038e+08, -7.826000000001164e+04},
             {-4.805000000000038e+08, 1.538000000000001e+11, 2.288999999999906e+07},
             {-7.826000000001164e+04, 2.288999999999906e+07, 2.352000000000363e+05}}
        )
    );
}

// Analysis Case 04: High cross-track uncertainty (Omitron test 04, MaxCrossTrackSigma), at TCA
CloseApproach buildAnalysisCase04()
{
    return buildAnalysisCloseApproach(
        Instant::DateTime(DateTime::Parse("2013-09-06 08:39:23.593"), Scale::UTC),
        Vector3d(-3802.232572, 4720.000417, -3676.847773),
        Vector3d(3.363036829, -2.209266805, -6.322865860),
        buildRTNPositionCovarianceCoordinates(
            {{3.387000000000121e+02, -1.213000000000000e+03, 1.543000000001494e+01},
             {-1.213000000000000e+03, 3.894999999999999e+05, 1.023999999999975e+02},
             {1.543000000001494e+01, 1.023999999999975e+02, 6.560000000000376e+01}}
        ),
        Vector3d(-3803.021006, 4719.171563, -3677.090477),
        Vector3d(-4.174950964, 1.324905095, 6.124732791),
        buildRTNPositionCovarianceCoordinates(
            {{6.505999999999749e+05, 1.540000000000006e+07, -3.021999999999974e+06},
             {1.540000000000006e+07, 5.965999999999999e+08, -7.986000000000000e+07},
             {-3.021999999999974e+06, -7.986000000000000e+07, 1.559999999999997e+07}}
        )
    );
}

// Analysis Case 05: Minimal miss distance (Omitron test 05, MinMiss), at TCA
CloseApproach buildAnalysisCase05()
{
    return buildAnalysisCloseApproach(
        Instant::DateTime(DateTime::Parse("2016-04-13 00:27:40.810"), Scale::UTC),
        Vector3d(1935.852328, 562.737829, 6779.432519),
        Vector3d(-4.907663525, -5.371074492, 1.843433804),
        buildRTNPositionCovarianceCoordinates(
            {{5.234999999999809e+02, -2.154999999999997e+04, 3.285999999999808e+02},
             {-2.154999999999997e+04, 1.131999999999999e+06, -1.602000000000015e+04},
             {3.285999999999808e+02, -1.602000000000015e+04, 3.836000000000072e+02}}
        ),
        Vector3d(1935.849453, 562.740211, 6779.433593),
        Vector3d(-3.808612275, 6.469359352, 0.530245571),
        buildRTNPositionCovarianceCoordinates(
            {{2.597000000000006e+03, -6.251999999999988e+04, -2.500000000000111e+03},
             {-6.251999999999988e+04, 1.259000000000000e+07, 6.707000000000856e+03},
             {-2.500000000000111e+03, 6.707000000000856e+03, 2.974000000000337e+03}}
        )
    );
}

State removeCovarianceMatrix(const State& aState)
{
    return buildState(
        aState.accessInstant(),
        aState.accessFrame(),
        aState.getPosition().getCoordinates(),
        aState.getVelocity().getCoordinates()
    );
}

bool containsMessage(const Array<String>& aMessageArray, const String& aMessageCore)
{
    return std::any_of(
        aMessageArray.begin(),
        aMessageArray.end(),
        [&aMessageCore](const String& aMessage) -> bool
        {
            return aMessage.find(aMessageCore) != std::string::npos;
        }
    );
}

struct ComputeProbabilityOfCollisionParams
{
    String name;
    CloseApproach closeApproach;
    Real combinedHardBodyRadius;
    Real expectedProbabilityOfCollision;
};

struct ComputeProbabilityAnalysisParams
{
    String name;
    CloseApproach closeApproach;
    Real combinedHardBodyRadius;
    Real sigmaScalingFactorLowerBound;
    Real sigmaScalingFactorUpperBound;
    ProbabilityOfCollisionAlgorithm::ProbabilityRegion expectedRegion;
    Real expectedMaximumProbabilityOfCollision;
};

}  // namespace

class OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005 : public ::testing::Test
{
   protected:
    const Alfano2005 algorithm_ = {};
};

TEST_F(OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005, Constructor)
{
    {
        EXPECT_NO_THROW(Alfano2005());
    }

    {
        EXPECT_NO_THROW(Alfano2005(Derived(10.0, Derived::Unit::MeterPerSecond()), Frame::J2000(Theory::IAU_2006)));
    }

    {
        EXPECT_THROW(
            try {
                Alfano2005(Derived(100.0, Derived::Unit::MeterPerSecond()), nullptr);
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Frame} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            try {
                Alfano2005(Derived(100.0, Derived::Unit::MeterPerSecond()), Frame::Undefined());
            } catch (const ostk::core::error::runtime::Undefined& e) {
                EXPECT_EQ("{Frame} is undefined.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Undefined
        );
    }

    {
        EXPECT_THROW(
            try {
                Alfano2005(Derived(100.0, Derived::Unit::MeterPerSecond()), Frame::ITRF());
            } catch (const ostk::core::error::runtime::Wrong& e) {
                EXPECT_EQ("Frame = ITRF is wrong.", e.getMessage());
                throw;
            },
            ostk::core::error::runtime::Wrong
        );
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005,
    IdentifyUnsatisfiedAssumptions
)
{
    // PoC Case 03 has a relative velocity of ~16 m/s: below the default threshold (100 m/s), above 10 m/s
    const Alfano2005 algorithm = {Derived(10.0, Derived::Unit::MeterPerSecond())};

    const CloseApproach closeApproach = buildPoCCase03();
    const State object1State = closeApproach.getObject1State();
    const State object2State = closeApproach.getObject2State();

    // All assumptions satisfied
    {
        EXPECT_TRUE(algorithm.identifyUnsatisfiedAssumptions(closeApproach).isEmpty());
    }

    // No covariance matrix for Object 1
    {
        const Array<String> unsatisfiedAssumptions =
            algorithm.identifyUnsatisfiedAssumptions({removeCovarianceMatrix(object1State), object2State});

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 1"));
    }

    // No covariance matrix for Object 2
    {
        const Array<String> unsatisfiedAssumptions =
            algorithm.identifyUnsatisfiedAssumptions({object1State, removeCovarianceMatrix(object2State)});

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 2"));
    }

    // Covariance matrix without position uncertainty for Object 1
    {
        State object1StateWithVelocityCovarianceOnly = removeCovarianceMatrix(object1State);
        object1StateWithVelocityCovarianceOnly.setCovarianceMatrix(CovarianceMatrix(
            object1State.accessInstant(),
            MatrixXd::Identity(3, 3),
            object1State.accessFrame(),
            {CartesianVelocity::Default()}
        ));

        const Array<String> unsatisfiedAssumptions =
            algorithm.identifyUnsatisfiedAssumptions({object1StateWithVelocityCovarianceOnly, object2State});

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 1"));
    }

    // Relative velocity below the (default) threshold
    {
        const Array<String> unsatisfiedAssumptions = algorithm_.identifyUnsatisfiedAssumptions(closeApproach);

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 1);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "Relative velocity"));
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "is below the threshold"));
    }

    // All assumptions unsatisfied
    {
        const Array<String> unsatisfiedAssumptions = algorithm_.identifyUnsatisfiedAssumptions(
            {removeCovarianceMatrix(object1State), removeCovarianceMatrix(object2State)}
        );

        EXPECT_EQ(unsatisfiedAssumptions.getSize(), 3);
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 1"));
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "No position uncertainty for Object 2"));
        EXPECT_TRUE(containsMessage(unsatisfiedAssumptions, "is below the threshold"));
    }
}

TEST_F(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005,
    ComputeProbabilityOfCollision_PoCCase12
)
{
    // Identical orbits: zero miss distance and zero relative velocity, the encounter plane is undefined
    const CloseApproach closeApproach = buildPoCCase12();

    EXPECT_THROW(
        try {
            algorithm_.computeProbabilityOfCollision(
                closeApproach, std::make_shared<Spherical>(2.0), std::make_shared<Spherical>(2.0)
            );
        } catch (const ostk::core::error::RuntimeError& e) {
            EXPECT_EQ("Relative velocity is zero.", e.getMessage());
            throw;
        },
        ostk::core::error::RuntimeError
    );
}

class OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005_Parameterized
    : public ::testing::TestWithParam<ComputeProbabilityOfCollisionParams>
{
   protected:
    const Alfano2005 algorithm_ = {};
    const Real relativeTolerance_ = 0.001;
};

INSTANTIATE_TEST_SUITE_P(
    Cases,
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005_Parameterized,
    ::testing::Values(
        ComputeProbabilityOfCollisionParams {
            "PoCCase01",
            buildPoCCase01(),
            15.0,
            1.46749549e-1,
        },
        ComputeProbabilityOfCollisionParams {
            "PoCCase02",
            buildPoCCase02(),
            4.0,
            6.22226700e-3,
        },
        ComputeProbabilityOfCollisionParams {
            "PoCCase03",
            buildPoCCase03(),
            15.0,
            1.00351176e-1,
        },
        ComputeProbabilityOfCollisionParams {
            "PoCCase04",
            buildPoCCase04(),
            15.0,
            4.93234060e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "PoCCase05",
            buildPoCCase05(),
            10.0,
            4.44873860e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "PoCCase06",
            buildPoCCase06(),
            10.0,
            4.33545500e-3,
        },
        ComputeProbabilityOfCollisionParams {
            "PoCCase07",
            buildPoCCase07(),
            10.0,
            1.58147000e-4,
        },
        ComputeProbabilityOfCollisionParams {
            "PoCCase08",
            buildPoCCase08(),
            4.0,
            3.69480080e-2,
        },
        ComputeProbabilityOfCollisionParams {
            "PoCCase09",
            buildPoCCase09(),
            6.0,
            2.90146291e-1,
        },
        ComputeProbabilityOfCollisionParams {
            "PoCCase10",
            buildPoCCase10(),
            6.0,
            2.90146291e-1,
        },
        ComputeProbabilityOfCollisionParams {
            "PoCCase11",
            buildPoCCase11(),
            4.0,
            2.67202600e-3,
        }
    ),
    [](const ::testing::TestParamInfo<ComputeProbabilityOfCollisionParams>& aParamInfo) -> std::string
    {
        return aParamInfo.param.name;
    }
);

TEST_P(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005_Parameterized,
    ComputeProbabilityOfCollision
)
{
    const ComputeProbabilityOfCollisionParams& params = GetParam();

    // The combined hard-body radius is split evenly between both objects
    const Real probabilityOfCollision = algorithm_.computeProbabilityOfCollision(
        params.closeApproach,
        std::make_shared<Spherical>(params.combinedHardBodyRadius / 2.0),
        std::make_shared<Spherical>(params.combinedHardBodyRadius / 2.0)
    );

    EXPECT_NEAR(
        probabilityOfCollision,
        params.expectedProbabilityOfCollision,
        std::abs(params.expectedProbabilityOfCollision) * relativeTolerance_
    );
}

class OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005_AnalysisParameterized
    : public ::testing::TestWithParam<ComputeProbabilityAnalysisParams>
{
   protected:
    const Alfano2005 algorithm_ = {};
    const Real relativeTolerance_ = 0.005;
};

INSTANTIATE_TEST_SUITE_P(
    Cases,
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005_AnalysisParameterized,
    ::testing::Values(
        // Miss (12 m) inside HBR (20 m), small covariances: maximum reached within the default bounds
        ComputeProbabilityAnalysisParams {
            "AnalysisCase01",
            buildAnalysisCase01(),
            20.0,
            0.01,
            100.0,
            ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted,
            9.99e-1,
        },
        // Miss (91 m) inside HBR (100 m), very large radial covariance (6.6 km): much lower bound required
        ComputeProbabilityAnalysisParams {
            "AnalysisCase02",
            buildAnalysisCase02(),
            100.0,
            0.0001,
            100.0,
            ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted,
            9.99e-1,
        },
        // Miss (519 m) outside HBR (100 m), extreme in-track covariance (392 km): maximum at a small scaling
        ComputeProbabilityAnalysisParams {
            "AnalysisCase03",
            buildAnalysisCase03(),
            100.0,
            0.01,
            100.0,
            ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted,
            9.10e-2,
        },
        // Miss (1169 m) outside HBR (100 m), large cross-track covariance (3.9 km): maximum near 0.4x sigmas
        ComputeProbabilityAnalysisParams {
            "AnalysisCase04",
            buildAnalysisCase04(),
            100.0,
            0.01,
            100.0,
            ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted,
            5.73e-3,
        },
        // Miss (3.9 m) inside HBR (20 m), large in-track covariance (3.5 km): much lower bound required
        ComputeProbabilityAnalysisParams {
            "AnalysisCase05",
            buildAnalysisCase05(),
            20.0,
            0.0001,
            100.0,
            ProbabilityOfCollisionAlgorithm::ProbabilityRegion::Diluted,
            9.99e-1,
        }
    ),
    [](const ::testing::TestParamInfo<ComputeProbabilityAnalysisParams>& aParamInfo) -> std::string
    {
        return aParamInfo.param.name;
    }
);

TEST_P(
    OpenSpaceToolkit_Astrodynamics_Conjunction_ProbabilityOfCollisionAlgorithm_Alfano2005_AnalysisParameterized,
    ComputeProbabilityAnalysis
)
{
    const ComputeProbabilityAnalysisParams& params = GetParam();

    // The combined hard-body radius is split evenly between both objects
    const ProbabilityOfCollisionAlgorithm::Analysis analysis = algorithm_.computeProbabilityAnalysis(
        params.closeApproach,
        std::make_shared<Spherical>(params.combinedHardBodyRadius / 2.0),
        std::make_shared<Spherical>(params.combinedHardBodyRadius / 2.0),
        true,
        true,
        params.sigmaScalingFactorLowerBound,
        params.sigmaScalingFactorUpperBound
    );

    EXPECT_EQ(analysis.region, params.expectedRegion);
    EXPECT_NEAR(
        analysis.maximumProbabilityOfCollision,
        params.expectedMaximumProbabilityOfCollision,
        std::abs(params.expectedMaximumProbabilityOfCollision) * relativeTolerance_
    );
}
