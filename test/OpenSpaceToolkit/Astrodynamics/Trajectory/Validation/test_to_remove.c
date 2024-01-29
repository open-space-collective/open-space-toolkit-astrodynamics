// OrekitSpherical
// Removed and not replaced because its redudant with the other atmosphere tests
{
    // Current state and instant setup
    const State state = {
        Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC),
        Position::Meters({7000000.0, 0.0, 0.0}, gcrfSPtr_),
        Velocity::MetersPerSecond({0.0, 7546.053290, 0.0}, gcrfSPtr_),
    };

    const State finalState = defaultPropagator_.calculateStateAt(state, state.getInstant() + Duration::Minutes(60.0));

    VectorXd expectedCoordinates(6);
    expectedCoordinates << -5172890.413848375000, -4716058.194055955000, 0000000.000000000000, 05083.946625786208,
        -05576.415230079790, 00000.000000000000;

    VectorXd actualCoordinates(6);
    actualCoordinates.segment(0, 3) = finalState.extractCoordinate(CartesianPosition::Default());
    actualCoordinates.segment(3, 3) = finalState.extractCoordinate(CartesianVelocity::Default());

    const VectorXd residuals = actualCoordinates - expectedCoordinates;
}

// Two body vs GMAT
// Replaced with 001-force-model-spherical-a/b/c
{
    // Current state and instant setup
    const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

    // Reference data setup
    const Table referenceData = Table::Load(
        File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                               "Propagated/GMAT_TwoBody_2hr_run.csv")),
        Table::Format::CSV,
        true
    );

    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

        referencePositionArrayGCRF.add(
            1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
        );
        referenceVelocityArrayGCRF.add(
            1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
        );
    }

    // Setup initial conditions
    const State state = {
        startInstant,
        Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
        Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
    };

    // Propagate all states
    const Array<State> propagatedStateArray = defaultPropagator_.calculateStatesAt(state, instantArray);

    // Validation loop
    for (size_t i = 0; i < instantArray.getSize(); i++)
    {
        // GCRF Compare
        const Position positionGCRF = propagatedStateArray[i].getPosition();
        const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

        const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
        const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

        ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
        ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

        ASSERT_GT(2e-3, positionErrorGCRF);
        ASSERT_GT(2e-6, velocityErrorGCRF);

        // Results console output

        // std::cout << "**************************************" << std::endl;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield);
        // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
        // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
        // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        // std::cout << "**************************************" << std::endl;
    }
}

// EGM96 360x360 perturbation only vs GMAT
// Replaced with 001-force-model-non-spherical-360x360
{
    // Current state and instant setup
    const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

    // Reference data setup
    const Table referenceData = Table::Load(
        File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                               "Propagated/GMAT_EGM96_360x360_2hr_run.csv")),
        Table::Format::CSV,
        true
    );

    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

        referencePositionArrayGCRF.add(
            1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
        );
        referenceVelocityArrayGCRF.add(
            1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
        );
    }

    // Create dynamics
    const Shared<Celestial> earth = std::make_shared<Celestial>(Earth::EGM96(360, 360));
    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earth),
    };

    // Setup Propagator
    const Propagator propagator = {defaultNumericalSolver_, dynamics};

    // Setup initial conditions
    const State state = {
        startInstant,
        Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
        Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
    };

    // Propagate all states
    const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

    // Validation loop
    for (size_t i = 0; i < instantArray.getSize(); i++)
    {
        // GCRF Compare
        const Position positionGCRF = propagatedStateArray[i].getPosition();
        const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

        const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
        const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

        ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
        ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

        ASSERT_GT(9e-2, positionErrorGCRF);
        ASSERT_GT(9e-5, velocityErrorGCRF);

        // Results console output

        // std::cout << "**************************************" << std::endl;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield);
        // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
        // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
        // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        // std::cout << "**************************************" << std::endl;
    }
}


// Sun+Moon perturbation only vs GMAT
// Replaced with 004-force-model-sun-moon
{
    // Current state and instant setup
    const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

    // Reference data setup
    const Table referenceData = Table::Load(
        File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                "Propagated/GMAT_TwoBody_Sun_Moon_2hr_run.csv")),
        Table::Format::CSV,
        true
    );

    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

        referencePositionArrayGCRF.add(
            1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
        );
        referenceVelocityArrayGCRF.add(
            1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
        );
    }

    // Create dynamics
    const Shared<Celestial> moonSpherical_ = std::make_shared<Celestial>(Moon::Spherical());
    const Shared<Celestial> sunSpherical_ = std::make_shared<Celestial>(Sun::Spherical());
    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSpherical_),
        std::make_shared<ThirdBodyGravity>(moonSpherical_),
        std::make_shared<ThirdBodyGravity>(sunSpherical_),
    };

    // Setup initial conditions
    const State state = {
        startInstant,
        Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
        Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
    };

    const Propagator propagator = {defaultNumericalSolver_, dynamics};

    // Propagate all states
    const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

    // Validation loop
    for (size_t i = 0; i < instantArray.getSize(); i++)
    {
        // GCRF Compare
        const Position positionGCRF = propagatedStateArray[i].getPosition();
        const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

        const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
        const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

        ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
        ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

        ASSERT_GT(2e-3, positionErrorGCRF);
        ASSERT_GT(2e-6, velocityErrorGCRF);

        // Results console output

        // std::cout << "**************************************" << std::endl;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield);
        // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
        // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
        // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        // std::cout << "**************************************" << std::endl;
    }
}

// Sun perturbation only vs GMAT
// Replaced with 004-force-model-sun
{
    // Current state and instant setup
    const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

    // Reference data setup
    const Table referenceData = Table::Load(
        File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                "Propagated/GMAT_TwoBody_Sun_2hr_run.csv")),
        Table::Format::CSV,
        true
    );

    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

        referencePositionArrayGCRF.add(
            1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
        );
        referenceVelocityArrayGCRF.add(
            1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
        );
    }

    // Setup initial conditions
    const State state = {
        startInstant,
        Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
        Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
    };

    // dynamics setup
    const Shared<Celestial> sunSpherical = std::make_shared<Celestial>(Sun::Spherical());
    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSpherical_),
        std::make_shared<ThirdBodyGravity>(sunSpherical),
    };

    // Setup Propagator
    const Propagator propagator = {defaultNumericalSolver_, dynamics};

    // Propagate all states
    const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

    // Validation loop
    for (size_t i = 0; i < instantArray.getSize(); i++)
    {
        // GCRF Compare
        const Position positionGCRF = propagatedStateArray[i].getPosition();
        const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

        const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
        const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

        ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
        ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

        ASSERT_GT(2e-3, positionErrorGCRF);
        ASSERT_GT(2e-6, velocityErrorGCRF);

        // Results console output

        // std::cout << "**************************************" << std::endl;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield);
        // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
        // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
        // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        // std::cout << "**************************************" << std::endl;
    }
}

// Moon perturbation only vs GMAT
// Replaced with 004-force-model-moon
{
    // Current state and instant setup
    const Instant startInstant = Instant::DateTime(DateTime::Parse("2021-03-20 00:00:00.000"), Scale::UTC);

    // Reference data setup
    const Table referenceData = Table::Load(
        File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/"
                                "Propagated/GMAT_TwoBody_Moon_2hr_run.csv")),
        Table::Format::CSV,
        true
    );

    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(startInstant + Duration::Seconds(referenceRow[1].accessReal()));

        referencePositionArrayGCRF.add(
            1e3 * Vector3d(referenceRow[2].accessReal(), referenceRow[3].accessReal(), referenceRow[4].accessReal())
        );
        referenceVelocityArrayGCRF.add(
            1e3 * Vector3d(referenceRow[5].accessReal(), referenceRow[6].accessReal(), referenceRow[7].accessReal())
        );
    }

    // Setup initial conditions
    const State state = {
        startInstant,
        Position::Meters({referencePositionArrayGCRF[0]}, gcrfSPtr_),
        Velocity::MetersPerSecond({referenceVelocityArrayGCRF[0]}, gcrfSPtr_),
    };

    // dynamics setup
    const Shared<Celestial> moonSpherical = std::make_shared<Celestial>(Moon::Spherical());
    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSpherical_),
        std::make_shared<ThirdBodyGravity>(moonSpherical),
    };

    // Setup Propagator
    const Propagator propagator = {defaultNumericalSolver_, dynamics};

    // Propagate all states
    const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

    // Validation loop
    for (size_t i = 0; i < instantArray.getSize(); i++)
    {
        // GCRF Compare
        const Position positionGCRF = propagatedStateArray[i].getPosition();
        const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

        const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
        const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

        ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
        ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

        ASSERT_GT(2e-3, positionErrorGCRF);
        ASSERT_GT(2e-6, velocityErrorGCRF);

        // Results console output

        // std::cout << "**************************************" << std::endl;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield);
        // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
        // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
        // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        // std::cout << "**************************************" << std::endl;
    }
}


// Exponential Atmosphere vs Orekit
// Replaced with 003-force-model-exponential-600
{
    const Real mass = satelliteDryMass_.inKilograms();
    const Real surfaceArea = satelliteSystem_.getCrossSectionalSurfaceArea();
    const Real dragCoefficient = satelliteSystem_.getDragCoefficient();

    VectorXd initialStateVector(9);
    initialStateVector << 7000000.0, 0.0, 0.0, 0.0, 7546.053290, 0.0, mass, surfaceArea, dragCoefficient;

    // Current state and instant setup
    const State state = {
        Instant::DateTime(DateTime(2023, 1, 1, 0, 0, 0), Scale::UTC),
        initialStateVector,
        gcrfSPtr_,
        dragCoordinatesBrokerSPtr_
    };

    // Setup Propagator
    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
    );

    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);

    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr),
    };
    const Propagator propagator = {defaultNumericalSolver_, dynamics};

    const State finalState =
        propagator.calculateStateAt(state, state.getInstant() + Duration::Seconds(60.0 * 60.0));

    VectorXd expectedCoordinates(9);
    expectedCoordinates << -5172889.585695211000, -4716058.453899897000, 0000000.000008572841, 05083.947538266920,
        -05576.414764013522, -00000.000000010687, satelliteDryMass_.inKilograms(), surfaceArea, dragCoefficient;

    VectorXd actualCoordinates(9);
    actualCoordinates.segment(0, 3) = finalState.extractCoordinate(CartesianPosition::Default());
    actualCoordinates.segment(3, 3) = finalState.extractCoordinate(CartesianVelocity::Default());
    actualCoordinates.segment(6, 1) = finalState.extractCoordinate(CoordinatesSubset::Mass());
    actualCoordinates.segment(7, 1) = finalState.extractCoordinate(CoordinatesSubset::SurfaceArea());
    actualCoordinates.segment(8, 1) = finalState.extractCoordinate(CoordinatesSubset::DragCoefficient());

    const VectorXd residuals = actualCoordinates - expectedCoordinates;

    ASSERT_TRUE((residuals.array() < 1e-6).all()) << String::Format("Residual: {}", residuals.maxCoeff());
}

// Earth with Exponential atmospheric drag compared against Orekit at 500km altitude (Uses RK4 integrator with 30s step)
// Replaced with 003-force-model-exponential-500
{
    // Reference data setup
    const Table referenceData = Table::Load(
        File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
                                "Orekit_Drag_Exponential_500km_2hr_run.csv")),
        Table::Format::CSV,
        true
    );

    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(Instant::DateTime(
            DateTime::Parse(referenceRow[0].accessString(), DateTime::Format::ISO8601), Scale::UTC
        ));

        referencePositionArrayGCRF.add(
            Vector3d(referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal())
        );
        referenceVelocityArrayGCRF.add(
            Vector3d(referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal())
        );
    }

    // Setup dynamics
    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
    );
    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);
    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr),
    };

    const Real mass = satelliteSystem_.getMass().inKilograms();
    const Real surfaceArea = satelliteSystem_.getCrossSectionalSurfaceArea();
    const Real dragCoefficient = satelliteSystem_.getDragCoefficient();

    VectorXd initialStateVector(9);
    initialStateVector << referencePositionArrayGCRF[0], referenceVelocityArrayGCRF[0], mass, surfaceArea,
        dragCoefficient;

    // Current state and instant setup
    const State state = {instantArray[0], initialStateVector, gcrfSPtr_, dragCoordinatesBrokerSPtr_};

    // Setup Propagator model and orbit
    const Propagator propagator = {defaultRK4_, dynamics};

    // Propagate all states
    const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

    // Validation loop
    for (size_t i = 0; i < instantArray.getSize(); i++)
    {
        // GCRF Compare
        const Position positionGCRF = propagatedStateArray[i].getPosition();
        const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

        const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
        const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

        ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
        ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

        ASSERT_GT(2e-3, positionErrorGCRF);
        ASSERT_GT(2e-6, velocityErrorGCRF);

        // Results console output

        // std::cout << "**************************************" << std::endl;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield);
        // std::cout << "Instant is: " << instantArray[i] << std::endl;
        // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
        // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
        // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        // std::cout << "**************************************" << std::endl;
    }
}

// Earth with Exponential atmospheric drag compared against Orekit at 320km altitude (Uses RK4 integrator with 30s step)
// Replaced with 003-force-model-exponential-320
{
    // Reference data setup
    const Table referenceData = Table::Load(
        File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
                                "Orekit_Drag_Exponential_320km_2hr_run.csv")),
        Table::Format::CSV,
        true
    );

    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(Instant::DateTime(
            DateTime::Parse(referenceRow[0].accessString(), DateTime::Format::ISO8601), Scale::UTC
        ));

        referencePositionArrayGCRF.add(
            Vector3d(referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal())
        );
        referenceVelocityArrayGCRF.add(
            Vector3d(referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal())
        );
    }

    // Setup dynamics
    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(EarthAtmosphericModel::Type::Exponential)
    );
    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);
    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr),
    };

    const Real mass = satelliteSystem_.getMass().inKilograms();
    const Real surfaceArea = satelliteSystem_.getCrossSectionalSurfaceArea();
    const Real dragCoefficient = satelliteSystem_.getDragCoefficient();

    // Setup initial conditions
    VectorXd initialCoordinates(9);
    initialCoordinates << referencePositionArrayGCRF[0], referenceVelocityArrayGCRF[0], mass, surfaceArea,
        dragCoefficient;

    const State state = {instantArray[0], initialCoordinates, gcrfSPtr_, dragCoordinatesBrokerSPtr_};

    // Setup Propagator model and orbit
    const Propagator propagator = {defaultRK4_, dynamics};

    // Propagate all states
    const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);
    // Validation loop
    for (size_t i = 0; i < instantArray.getSize(); i++)
    {
        // GCRF Compare
        const Position positionGCRF = propagatedStateArray[i].getPosition();
        const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();
        const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
        const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();
        ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
        ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

        ASSERT_GT(4e-3, positionErrorGCRF);  // TBI: Investigate wrt 500km case
        ASSERT_GT(5e-6, velocityErrorGCRF);  // TBI: Investigate wrt 500km case

        // Results console output
        // std::cout << "**************************************" << std::endl;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield);
        // std::cout << "Instant is: " << instantArray[i] << std::endl;
        // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
        // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
        // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        // std::cout << "**************************************" << std::endl;
    }
}

// Earth with NRLMSISE00 atmospheric drag compared against Orekit at 470km altitude with small cross section (Uses RK4 integrator with 30s step)
// Replaced with 003-force-model-nrlmsis-470-small-area
{
    // Reference data setup
    const Table referenceData = Table::Load(
        File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
                                "Orekit_Drag_NRLMSISE00_470km_2hr_run.csv")),
        Table::Format::CSV,
        true
    );

    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceAccelerationArrayGCRF = Array<Vector3d>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(Instant::DateTime(
            DateTime::Parse(referenceRow[0].accessString(), DateTime::Format::ISO8601), Scale::UTC
        ));

        referencePositionArrayGCRF.add(
            Vector3d(referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal())
        );
        referenceVelocityArrayGCRF.add(
            Vector3d(referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal())
        );
        referenceAccelerationArrayGCRF.add(
            Vector3d(referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal())
        );
    }

    // Use the same space weather input file that Orekit uses
    SWManager::Get().reset();
    CSSISpaceWeather swData = CSSISpaceWeather::LoadLegacy(File::Path(Path::Parse(
        "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/SpaceWeather-All-v1.2.txt"
    )));
    SWManager::Get().loadCSSISpaceWeather(swData);

    const Sun sun = Sun::Default();
    const Shared<Celestial> sunSPtr = std::make_shared<Celestial>(sun);

    // Setup dynamics
    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(
            EarthAtmosphericModel::Type::NRLMSISE00,
            EarthAtmosphericModel::InputDataType::CSSISpaceWeatherFile,
            Real::Undefined(),
            Real::Undefined(),
            Real::Undefined(),
            Frame::ITRF(),
            EarthGravitationalModel::WGS84.equatorialRadius_,
            EarthGravitationalModel::WGS84.flattening_,
            sunSPtr
        )
    );
    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);

    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0},
        {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
        {1.0, 2.0, 3.0}
    ));

    SatelliteSystem satelliteSystem = {
        Mass(100.0, Mass::Unit::Kilogram),
        satelliteGeometry,
        Matrix3d::Identity(),
        1.0,
        2.2,
    };

    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr),
    };

    // Setup initial conditions
    const Real mass = satelliteDryMass_.inKilograms();
    const Real surfaceArea = satelliteSystem.getCrossSectionalSurfaceArea();
    const Real dragCoefficient = satelliteSystem.getDragCoefficient();

    // Setup initial conditions
    VectorXd initialCoordinates(9);
    initialCoordinates << referencePositionArrayGCRF[0], referenceVelocityArrayGCRF[0], mass, surfaceArea,
        dragCoefficient;

    const State state = {instantArray[0], initialCoordinates, gcrfSPtr_, dragCoordinatesBrokerSPtr_};

    // Setup Propagator model and orbit
    const Propagator propagator = {defaultRK4_, dynamics};

    // Propagate all states
    const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

    // Validation loop
    for (size_t i = 0; i < instantArray.getSize(); i++)
    {
        // GCRF Compare
        const Position positionGCRF = propagatedStateArray[i].getPosition();
        const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

        const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
        const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

        ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
        ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

        ASSERT_GT(8e-3, positionErrorGCRF);
        ASSERT_GT(1e-5, velocityErrorGCRF);

        // Results console output

        // std::cout << "**************************************" << std::endl;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield);
        // std::cout << "Instant is: " << instantArray[i] << std::endl;
        // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
        // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
        // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        // std::cout << "**************************************" << std::endl;
    }
    SWManager::Get().reset();
}

// Earth with NRLMSISE00 atmospheric drag compared against Orekit at 470km altitude with large cross section (Uses RK4 integrator with 30s step)
// Replaced with 003-force-model-nrlmsis-470-large-area-short-duration
{
    // Reference data setup
    const Table referenceData = Table::Load(
        File::Path(Path::Parse("/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/"
                                "Orekit_Drag_NRLMSISE00_Large_Cross_Section_470km_2hr_run.csv")),
        Table::Format::CSV,
        true
    );

    Array<Instant> instantArray = Array<Instant>::Empty();
    Array<Vector3d> referencePositionArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceVelocityArrayGCRF = Array<Vector3d>::Empty();
    Array<Vector3d> referenceAccelerationArrayGCRF = Array<Vector3d>::Empty();

    for (const auto& referenceRow : referenceData)
    {
        instantArray.add(Instant::DateTime(
            DateTime::Parse(referenceRow[0].accessString(), DateTime::Format::ISO8601), Scale::UTC
        ));

        referencePositionArrayGCRF.add(
            Vector3d(referenceRow[1].accessReal(), referenceRow[2].accessReal(), referenceRow[3].accessReal())
        );
        referenceVelocityArrayGCRF.add(
            Vector3d(referenceRow[4].accessReal(), referenceRow[5].accessReal(), referenceRow[6].accessReal())
        );
        referenceAccelerationArrayGCRF.add(
            Vector3d(referenceRow[7].accessReal(), referenceRow[8].accessReal(), referenceRow[9].accessReal())
        );
    }

    // Use the same space weather input file that Orekit uses
    SWManager::Get().reset();
    CSSISpaceWeather swData = CSSISpaceWeather::LoadLegacy(File::Path(Path::Parse(
        "/app/test/OpenSpaceToolkit/Astrodynamics/Trajectory/Orbit/Models/Propagated/SpaceWeather-All-v1.2.txt"
    )));
    SWManager::Get().loadCSSISpaceWeather(swData);

    const Sun sun = Sun::Default();
    const Shared<Celestial> sunSPtr = std::make_shared<Celestial>(sun);

    // Setup dynamics
    const Earth earth = Earth::FromModels(
        std::make_shared<EarthGravitationalModel>(EarthGravitationalModel::Type::Spherical),
        std::make_shared<EarthMagneticModel>(EarthMagneticModel::Type::Undefined),
        std::make_shared<EarthAtmosphericModel>(
            EarthAtmosphericModel::Type::NRLMSISE00,
            EarthAtmosphericModel::InputDataType::CSSISpaceWeatherFile,
            Real::Undefined(),
            Real::Undefined(),
            Real::Undefined(),
            Frame::ITRF(),
            EarthGravitationalModel::WGS84.equatorialRadius_,
            EarthGravitationalModel::WGS84.flattening_,
            sunSPtr
        )
    );
    const Shared<Celestial> earthSPtr = std::make_shared<Celestial>(earth);

    const Composite satelliteGeometry(Cuboid(
        {0.0, 0.0, 0.0},
        {Vector3d {1.0, 0.0, 0.0}, Vector3d {0.0, 1.0, 0.0}, Vector3d {0.0, 0.0, 1.0}},
        {1.0, 2.0, 3.0}
    ));

    SatelliteSystem satelliteSystem = {
        Mass(100.0, Mass::Unit::Kilogram),
        satelliteGeometry,
        Matrix3d::Identity(),
        500.0,
        2.2,
    };

    const Array<Shared<Dynamics>> dynamics = {
        std::make_shared<PositionDerivative>(),
        std::make_shared<CentralBodyGravity>(earthSPtr),
        std::make_shared<AtmosphericDrag>(earthSPtr),
    };

    // Setup initial conditions
    const Real mass = satelliteDryMass_.inKilograms();
    const Real surfaceArea = satelliteSystem.getCrossSectionalSurfaceArea();
    const Real dragCoefficient = satelliteSystem.getDragCoefficient();

    // Setup initial conditions
    VectorXd initialCoordinates(9);
    initialCoordinates << referencePositionArrayGCRF[0], referenceVelocityArrayGCRF[0], mass, surfaceArea,
        dragCoefficient;

    const State state = {instantArray[0], initialCoordinates, gcrfSPtr_, dragCoordinatesBrokerSPtr_};

    // Setup Propagator model and orbit
    const Propagator propagator = {defaultRK4_, dynamics};

    // Propagate all states
    const Array<State> propagatedStateArray = propagator.calculateStatesAt(state, instantArray);

    // Validation loop
    for (size_t i = 0; i < instantArray.getSize(); i++)
    {
        // GCRF Compare
        const Position positionGCRF = propagatedStateArray[i].getPosition();
        const Velocity velocityGCRF = propagatedStateArray[i].getVelocity();

        const double positionErrorGCRF = (positionGCRF.accessCoordinates() - referencePositionArrayGCRF[i]).norm();
        const double velocityErrorGCRF = (velocityGCRF.accessCoordinates() - referenceVelocityArrayGCRF[i]).norm();

        ASSERT_EQ(*Frame::GCRF(), *positionGCRF.accessFrame());
        ASSERT_EQ(*Frame::GCRF(), *velocityGCRF.accessFrame());

        ASSERT_GT(5.0, positionErrorGCRF);
        ASSERT_GT(5e-3, velocityErrorGCRF);

        // Results console output

        // std::cout << "**************************************" << std::endl;
        // std::cout.setf(std::ios::scientific,std::ios::floatfield);
        // std::cout << "Instant is: " << instantArray[i] << std::endl;
        // std::cout << "Position error is: " << positionErrorGCRF << "m" << std::endl;
        // std::cout << "Velocity error is: " << velocityErrorGCRF <<  "m/s" << std::endl;
        // std::cout.setf(std::ios::fixed,std::ios::floatfield);
        // std::cout << "**************************************" << std::endl;
    }
    SWManager::Get().reset();
}
