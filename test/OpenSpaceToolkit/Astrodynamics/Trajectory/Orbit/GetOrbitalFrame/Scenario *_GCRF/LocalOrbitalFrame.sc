stk.v.12.0
WrittenBy    STK_v12.4.0
BEGIN Scenario
    Name		 LocalOrbitalFrame

    BEGIN Epoch

        Epoch		 1 Jan 2024 00:00:00.000000000
        SmartEpoch		
        BEGIN EVENT
            Epoch		 1 Jan 2024 00:00:00.000000000
            EventEpoch		
            BEGIN EVENT
                Type		 EVENT_LINKTO
                Name		 AnalysisStartTime
            END EVENT
            EpochState		 Implicit
        END EVENT


    END Epoch

    BEGIN Interval

        Start		 1 Jan 2024 00:00:00.000000000
        Stop		 1 Jan 2024 02:00:00.000000000
        SmartInterval		
        BEGIN EVENTINTERVAL
            BEGIN Interval
                Start		 1 Jan 2024 00:00:00.000000000
                Stop		 1 Jan 2024 02:00:00.000000000
            END Interval
            IntervalState		 Explicit
        END EVENTINTERVAL

        EpochUsesAnalStart		 No
        AnimStartUsesAnalStart		 Yes
        AnimStopUsesAnalStop		 Yes

    END Interval

    BEGIN EOPFile

        InheritEOPSource		 No
        EOPFilename		 EOP-v1.1.txt

    END EOPFile

    BEGIN GlobalPrefs
        SatelliteNoOrbWarning		 No
        MissilePerigeeWarning		 No
        MissileStopTimeWarning		 No
        AircraftWGS84Warning		 Always
    END GlobalPrefs

    BEGIN CentralBody

        PrimaryBody		 Earth

    END CentralBody

    BEGIN CentralBodyTerrain

        BEGIN CentralBody
            Name		 Earth
            UseTerrainCache		 Yes
            TotalCacheSize		 402653184

            BEGIN StreamingTerrain
                UseCurrentStreamingTerrainServer		 Yes
                CurrentStreamingTerrainServerName		 http://twsusecovacc01.agi.com/stk-terrain/
                StreamingTerrainTilesetName		 world
                StreamingTerrainServerName		 assets.agi.com/stk-terrain/
                StreamingTerrainAzimuthElevationMaskEnabled		 No
                StreamingTerrainObscurationEnabled		 No
                StreamingTerrainCoverageGridObscurationEnabled		 No
            END StreamingTerrain
        END CentralBody

    END CentralBodyTerrain

    BEGIN StarCollection

        Name		 Hipparcos 2 Mag 8

    END StarCollection

    BEGIN ScenarioLicenses
        Module		 stk_mission_level1v12.4
        Module		 stk_mission_level2v12.4
        Module		 stk_mission_spacev12.4
    END ScenarioLicenses

    BEGIN QuickReports

        BEGIN Report
            Name		 TNW
            Type		 Report
            BaseDir		 User
            Style		 TNW
            AGIViewer		 Yes
            Instance		 Satellite/Satellite1
            BEGIN TimeData
                BEGIN Section
                    SectionNumber		 1
                    SectionType		 2
                    ShowIntervals		 No
                    BEGIN IntervalList

                        DateUnitAbrv		 UTCG

                        BEGIN Intervals

"1 Jan 2024 00:00:00.000000000" "1 Jan 2024 02:00:00.000000000"
                        END Intervals

                    END IntervalList

                    TimeType		 Interval
                    SamplingType		 Default
                    TimeBound		 0
                END Section
            END TimeData
            BEGIN PreDataList
                BEGIN PreData
                    ServiceName		 AxesChooseAxes
                    Data		 Satellite/Satellite1 ICRF
                END PreData
            END PreDataList
            DisplayOnLoad		 No
            FrameType		 0
            DockCircleID		 0
            DockID		 0
            WindowRectLeft		 413
            WindowRectTop		 219
            WindowRectRight		 1392
            WindowRectBottom		 547
        END Report

        BEGIN Report
            Name		 New Report
            Type		 Report
            BaseDir		 User
            Style		 New Report
            AGIViewer		 Yes
            Instance		 *
            BEGIN TimeData
                BEGIN Section
                    SectionNumber		 1
                    SectionType		 2
                    ShowIntervals		 No
                    BEGIN IntervalList

                        DateUnitAbrv		 UTCG

                        BEGIN Intervals

"1 Jan 2024 00:00:00.000000000" "1 Jan 2024 02:00:00.000000000"
                        END Intervals

                    END IntervalList

                    TimeType		 Interval
                    SamplingType		 Default
                    TimeBound		 0
                END Section
            END TimeData
            BEGIN PreDataList
                BEGIN PreData
                    ServiceName		 AxesChooseAxes
                    Data		 CentralBody/Earth Fixed
                END PreData
            END PreDataList
            DisplayOnLoad		 No
            FrameType		 0
            DockCircleID		 0
            DockID		 0
            WindowRectLeft		 415
            WindowRectTop		 221
            WindowRectRight		 1394
            WindowRectBottom		 549
        END Report

        BEGIN Report
            Name		 posvelacc
            Type		 Report
            BaseDir		 User
            Style		 posvelacc
            AGIViewer		 Yes
            Instance		 Satellite/Satellite1
            BEGIN TimeData
                BEGIN Section
                    SectionNumber		 1
                    SectionType		 2
                    ShowIntervals		 No
                    BEGIN IntervalList

                        DateUnitAbrv		 UTCG

                        BEGIN Intervals

"1 Jan 2024 00:00:00.000000000" "1 Jan 2024 02:00:00.000000000"
                        END Intervals

                    END IntervalList

                    TimeType		 Interval
                    SamplingType		 Default
                    TimeBound		 0
                END Section
            END TimeData
            BEGIN PreDataList
                BEGIN PreData
                    ServiceName		 AxesChooseAxes
                    Data		 CentralBody/Earth Fixed
                END PreData
            END PreDataList
            BEGIN ElementUnitList
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 1
                    Instance		 Satellite/Satellite1
                    Name		 Time
                    Type		 ICRF
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DateFormat		 GregorianUTC
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 2
                    Instance		 Satellite/Satellite1
                    Name		 q1
                    Type		 ICRF
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 3
                    Instance		 Satellite/Satellite1
                    Name		 q2
                    Type		 ICRF
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 4
                    Instance		 Satellite/Satellite1
                    Name		 q3
                    Type		 ICRF
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 5
                    Instance		 Satellite/Satellite1
                    Name		 q4
                    Type		 ICRF
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 6
                    Instance		 Satellite/Satellite1
                    Name		 wx
                    Type		 ICRF
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            TimeUnit		 Seconds
                            AngleUnit		 Radians
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 7
                    Instance		 Satellite/Satellite1
                    Name		 wy
                    Type		 ICRF
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            TimeUnit		 Seconds
                            AngleUnit		 Radians
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 8
                    Instance		 Satellite/Satellite1
                    Name		 wz
                    Type		 ICRF
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            TimeUnit		 Seconds
                            AngleUnit		 Radians
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 9
                    Instance		 Satellite/Satellite1
                    Name		 x
                    Type		 Fixed
                    ServiceName		 CartPos
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 10
                    Instance		 Satellite/Satellite1
                    Name		 y
                    Type		 Fixed
                    ServiceName		 CartPos
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 11
                    Instance		 Satellite/Satellite1
                    Name		 z
                    Type		 Fixed
                    ServiceName		 CartPos
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 12
                    Instance		 Satellite/Satellite1
                    Name		 x
                    Type		 Fixed
                    ServiceName		 CartVel
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                            TimeUnit		 Seconds
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 13
                    Instance		 Satellite/Satellite1
                    Name		 y
                    Type		 Fixed
                    ServiceName		 CartVel
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                            TimeUnit		 Seconds
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 14
                    Instance		 Satellite/Satellite1
                    Name		 z
                    Type		 Fixed
                    ServiceName		 CartVel
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                            TimeUnit		 Seconds
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 15
                    Instance		 Satellite/Satellite1
                    Name		 x
                    Type		 Fixed
                    ServiceName		 CartAcc
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                            TimeUnit		 Seconds
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 16
                    Instance		 Satellite/Satellite1
                    Name		 y
                    Type		 Fixed
                    ServiceName		 CartAcc
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                            TimeUnit		 Seconds
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 17
                    Instance		 Satellite/Satellite1
                    Name		 z
                    Type		 Fixed
                    ServiceName		 CartAcc
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                            TimeUnit		 Seconds
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 18
                    Instance		 Satellite/Satellite1
                    Name		 x
                    Type		 ICRF
                    ServiceName		 CartPos
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 19
                    Instance		 Satellite/Satellite1
                    Name		 y
                    Type		 ICRF
                    ServiceName		 CartPos
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 20
                    Instance		 Satellite/Satellite1
                    Name		 z
                    Type		 ICRF
                    ServiceName		 CartPos
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 21
                    Instance		 Satellite/Satellite1
                    Name		 x
                    Type		 ICRF
                    ServiceName		 CartVel
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                            TimeUnit		 Seconds
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 22
                    Instance		 Satellite/Satellite1
                    Name		 y
                    Type		 ICRF
                    ServiceName		 CartVel
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                            TimeUnit		 Seconds
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 23
                    Instance		 Satellite/Satellite1
                    Name		 z
                    Type		 ICRF
                    ServiceName		 CartVel
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                            TimeUnit		 Seconds
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 24
                    Instance		 Satellite/Satellite1
                    Name		 x
                    Type		 ICRF
                    ServiceName		 CartAcc
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                            TimeUnit		 Seconds
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 25
                    Instance		 Satellite/Satellite1
                    Name		 y
                    Type		 ICRF
                    ServiceName		 CartAcc
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                            TimeUnit		 Seconds
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 26
                    Instance		 Satellite/Satellite1
                    Name		 z
                    Type		 ICRF
                    ServiceName		 CartAcc
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DistanceUnit		 Meters
                            TimeUnit		 Seconds
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
            END ElementUnitList
            DisplayOnLoad		 No
            FrameType		 0
            DockCircleID		 0
            DockID		 0
            WindowRectLeft		 439
            WindowRectTop		 245
            WindowRectRight		 1422
            WindowRectBottom		 577
        END Report

        BEGIN Report
            Name		 NED
            Type		 Report
            BaseDir		 User
            Style		 NED
            AGIViewer		 Yes
            Instance		 Satellite/Satellite1
            BEGIN TimeData
                BEGIN Section
                    SectionNumber		 1
                    SectionType		 2
                    ShowIntervals		 No
                    BEGIN IntervalList

                        DateUnitAbrv		 UTCG

                        BEGIN Intervals

"1 Jan 2024 00:00:00.000000000" "1 Jan 2024 02:00:00.000000000"
                        END Intervals

                    END IntervalList

                    TimeType		 Interval
                    SamplingType		 Default
                    TimeBound		 0
                END Section
            END TimeData
            BEGIN PreDataList
                BEGIN PreData
                    ServiceName		 AxesChooseAxes
                    Data		 CentralBody/Earth ICRF
                END PreData
            END PreDataList
            BEGIN ElementUnitList
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 1
                    Instance		 Satellite/Satellite1
                    Name		 Time
                    Type		 NorthEastDown
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            DateFormat		 GregorianUTC
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 2
                    Instance		 Satellite/Satellite1
                    Name		 q1
                    Type		 NorthEastDown
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 3
                    Instance		 Satellite/Satellite1
                    Name		 q2
                    Type		 NorthEastDown
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 4
                    Instance		 Satellite/Satellite1
                    Name		 q3
                    Type		 NorthEastDown
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 5
                    Instance		 Satellite/Satellite1
                    Name		 q4
                    Type		 NorthEastDown
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 6
                    Instance		 Satellite/Satellite1
                    Name		 wx
                    Type		 NorthEastDown
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            TimeUnit		 Seconds
                            AngleUnit		 Radians
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 7
                    Instance		 Satellite/Satellite1
                    Name		 wy
                    Type		 NorthEastDown
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            TimeUnit		 Seconds
                            AngleUnit		 Radians
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
                BEGIN ElementUnit
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 8
                    Instance		 Satellite/Satellite1
                    Name		 wz
                    Type		 NorthEastDown
                    ServiceName		 AxesChooseAxes
                    BEGIN ElemUnitData
                        UseDefaultUnits		 No
                        BEGIN ElemDimUnitList
                            TimeUnit		 Seconds
                            AngleUnit		 Radians
                        END ElemDimUnitList
                    END ElemUnitData
                END ElementUnit
            END ElementUnitList
            BEGIN ElementPropList
                BEGIN ElementProp
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 2
                    Instance		 Satellite/Satellite1
                    Name		 q1
                    Type		 NorthEastDown
                    ServiceName		 AxesChooseAxes
                    Title		 q1
                    Format		 %.14f
                    NameInTitle		 Yes
                    SummaryOnly		 No
                    BEGIN ElementEvent
                        Use		 No
                        Value		 0
                        Convergence		 0.002
                        Direction		 Both
                        CreateFile		 No
                    END ElementEvent
                END ElementProp
                BEGIN ElementProp
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 3
                    Instance		 Satellite/Satellite1
                    Name		 q2
                    Type		 NorthEastDown
                    ServiceName		 AxesChooseAxes
                    Title		 q2
                    Format		 %.14f
                    NameInTitle		 Yes
                    SummaryOnly		 No
                    BEGIN ElementEvent
                        Use		 No
                        Value		 0
                        Convergence		 0.002
                        Direction		 Both
                        CreateFile		 No
                    END ElementEvent
                END ElementProp
                BEGIN ElementProp
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 4
                    Instance		 Satellite/Satellite1
                    Name		 q3
                    Type		 NorthEastDown
                    ServiceName		 AxesChooseAxes
                    Title		 q3
                    Format		 %.14f
                    NameInTitle		 Yes
                    SummaryOnly		 No
                    BEGIN ElementEvent
                        Use		 No
                        Value		 0
                        Convergence		 0.002
                        Direction		 Both
                        CreateFile		 No
                    END ElementEvent
                END ElementProp
                BEGIN ElementProp
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 5
                    Instance		 Satellite/Satellite1
                    Name		 q4
                    Type		 NorthEastDown
                    ServiceName		 AxesChooseAxes
                    Title		 q4
                    Format		 %.14f
                    NameInTitle		 Yes
                    SummaryOnly		 No
                    BEGIN ElementEvent
                        Use		 No
                        Value		 0
                        Convergence		 0.002
                        Direction		 Both
                        CreateFile		 No
                    END ElementEvent
                END ElementProp
                BEGIN ElementProp
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 6
                    Instance		 Satellite/Satellite1
                    Name		 wx
                    Type		 NorthEastDown
                    ServiceName		 AxesChooseAxes
                    Title		 wx
                    Format		 %.14f
                    NameInTitle		 Yes
                    SummaryOnly		 No
                    BEGIN ElementEvent
                        Use		 No
                        Value		 0
                        Convergence		 0.002
                        Direction		 Both
                        CreateFile		 No
                    END ElementEvent
                END ElementProp
                BEGIN ElementProp
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 7
                    Instance		 Satellite/Satellite1
                    Name		 wy
                    Type		 NorthEastDown
                    ServiceName		 AxesChooseAxes
                    Title		 wy
                    Format		 %.14f
                    NameInTitle		 Yes
                    SummaryOnly		 No
                    BEGIN ElementEvent
                        Use		 No
                        Value		 0
                        Convergence		 0.002
                        Direction		 Both
                        CreateFile		 No
                    END ElementEvent
                END ElementProp
                BEGIN ElementProp
                    SectionNumber		 1
                    LineNumber		 1
                    ElementNumber		 8
                    Instance		 Satellite/Satellite1
                    Name		 wz
                    Type		 NorthEastDown
                    ServiceName		 AxesChooseAxes
                    Title		 wz
                    Format		 %.14f
                    NameInTitle		 Yes
                    SummaryOnly		 No
                    BEGIN ElementEvent
                        Use		 No
                        Value		 0
                        Convergence		 0.002
                        Direction		 Both
                        CreateFile		 No
                    END ElementEvent
                END ElementProp
            END ElementPropList
            DisplayOnLoad		 No
            FrameType		 0
            DockCircleID		 0
            DockID		 0
            WindowRectLeft		 415
            WindowRectTop		 221
            WindowRectRight		 1394
            WindowRectBottom		 549
        END Report
    END QuickReports

    BEGIN Extensions

        BEGIN ClsApp
            RangeConstraint		 5000
            ApoPeriPad		 30000
            OrbitPathPad		 100000
            TimeDistPad		 30000
            OutOfDate		 2592000
            MaxApoPeriStep		 900
            ApoPeriAngle		 0.7853981633974483
            UseApogeePerigeeFilter		 Yes
            UsePathFilter		 No
            UseTimeFilter		 No
            UseOutOfDate		 Yes
            CreateSats		 No
            MaxSatsToCreate		 500
            UseModelScale		 No
            ModelScale		 0
            UseCrossRefDb		 Yes
            CollisionDB		 stkAllTLE.tce
            CollisionCrossRefDB		 stkAllTLE.sd
            ShowLine		 Yes
            AnimHighlight		 Yes
            StaticHighlight		 Yes
            UseLaunchWindow		 No
            LaunchWindowUseEntireTraj		 Yes
            LaunchWindowTrajMETStart		 0
            LaunchWindowTrajMETStop		 900
            LaunchWindowStart		 7671600
            LaunchWindowStop		 7758000
            LaunchMETOffset		 0
            LaunchWindowUseSecEphem		 No 
            LaunchWindowUseScenFolderForSecEphem		 Yes
            LaunchWindowUsePrimEphem		 No 
            LaunchWindowUseScenFolderForPrimEphem		 Yes
            LaunchWindowIntervalPtr		
            BEGIN EVENTINTERVAL
                BEGIN Interval
                    Start		 29 Mar 2024 19:00:00.000000000
                    Stop		 30 Mar 2024 19:00:00.000000000
                END Interval
                IntervalState		 Explicit
            END EVENTINTERVAL

            LaunchWindowUsePrimMTO		 No 
            GroupLaunches		 No 
            LWTimeConvergence		 0.001
            LWRelValueConvergence		 1e-08
            LWTSRTimeConvergence		 0.0001
            LWTSRRelValueConvergence		 1e-10
            LaunchWindowStep		 300
            MaxTSRStep		  1.8000000000000000e+02
            MaxTSRRelMotion		  2.0000000000000000e+01
            UseLaunchArea		 No 
            LaunchAreaOrientation		 North
            LaunchAreaAzimuth		 0
            LaunchAreaXLimits		 -10000 10000
            LaunchAreaYLimits		 -10000 10000
            LaunchAreaNumXIntrPnts		 1
            LaunchAreaNumYIntrPnts		 1
            LaunchAreaAltReference		 Ellipsoid
            TargetSameStop		 No 
            SkipSurfaceMetric		 No 
            LWAreaTSRRelValueConvergence		 1e-10
            AreaLaunchWindowStep		 300
            AreaMaxTSRStep		  3.0000000000000000e+01
            AreaMaxTSRRelMotion		 1
            ShowLaunchArea		 No 
            ShowBlackoutTracks		 No 
            ShowClearedTracks		 No 
            UseObjectForClearedColor		 No 
            BlackoutColor		 #ff0000
            ClearedColor		 #ffffff
            ShowTracksSegments		 No 
            ShowMinRangeTracks		 No 
            MinRangeTrackTimeStep		 0.5
            UsePrimStepForTracks		 Yes
            GfxTracksTimeStep		 30
            GfxAreaNumXIntrPnts		 1
            GfxAreaNumYIntrPnts		 1
            CreateLaunchMTO		 No 
            CovarianceSigmaScale		 3
            CovarianceMode		 None 
        END ClsApp

        BEGIN Units
            DistanceUnit		 Kilometers
            TimeUnit		 Seconds
            DateFormat		 GregorianUTC
            AngleUnit		 Degrees
            MassUnit		 Kilograms
            PowerUnit		 dBW
            FrequencyUnit		 Gigahertz
            SmallDistanceUnit		 Meters
            LatitudeUnit		 Degrees
            LongitudeUnit		 Degrees
            DurationUnit		 Hr:Min:Sec
            Temperature		 Kelvin
            SmallTimeUnit		 Seconds
            RatioUnit		 Decibel
            RcsUnit		 Decibel
            DopplerVelocityUnit		 MetersperSecond
            SARTimeResProdUnit		 Meter-Second
            ForceUnit		 Newtons
            PressureUnit		 Pascals
            SpecificImpulseUnit		 Seconds
            PRFUnit		 Kilohertz
            BandwidthUnit		 Megahertz
            SmallVelocityUnit		 CentimetersperSecond
            Percent		 Percentage
            SolidAngle		 Steradians
            SpectralBandwidthUnit		 Hertz
            BitsUnit		 MegaBits
            MagneticFieldUnit		 nanoTesla
            VoltageUnit		 Volts
        END Units

        BEGIN ReportUnits
            DistanceUnit		 Kilometers
            TimeUnit		 Seconds
            DateFormat		 GregorianUTC
            AngleUnit		 Degrees
            MassUnit		 Kilograms
            PowerUnit		 dBW
            FrequencyUnit		 Gigahertz
            SmallDistanceUnit		 Meters
            LatitudeUnit		 Degrees
            LongitudeUnit		 Degrees
            DurationUnit		 Hr:Min:Sec
            Temperature		 Kelvin
            SmallTimeUnit		 Seconds
            RatioUnit		 Decibel
            RcsUnit		 Decibel
            DopplerVelocityUnit		 MetersperSecond
            SARTimeResProdUnit		 Meter-Second
            ForceUnit		 Newtons
            PressureUnit		 Pascals
            SpecificImpulseUnit		 Seconds
            PRFUnit		 Kilohertz
            BandwidthUnit		 Megahertz
            SmallVelocityUnit		 CentimetersperSecond
            Percent		 Percentage
            SolidAngle		 Steradians
            SpectralBandwidthUnit		 Hertz
            BitsUnit		 MegaBits
            MagneticFieldUnit		 nanoTesla
            VoltageUnit		 Volts
        END ReportUnits

        BEGIN ConnectReportUnits
            DistanceUnit		 Kilometers
            TimeUnit		 Seconds
            DateFormat		 GregorianUTC
            AngleUnit		 Degrees
            MassUnit		 Kilograms
            PowerUnit		 dBW
            FrequencyUnit		 Gigahertz
            SmallDistanceUnit		 Meters
            LatitudeUnit		 Degrees
            LongitudeUnit		 Degrees
            DurationUnit		 Hr:Min:Sec
            Temperature		 Kelvin
            SmallTimeUnit		 Seconds
            RatioUnit		 Decibel
            RcsUnit		 Decibel
            DopplerVelocityUnit		 MetersperSecond
            SARTimeResProdUnit		 Meter-Second
            ForceUnit		 Newtons
            PressureUnit		 Pascals
            SpecificImpulseUnit		 Seconds
            PRFUnit		 Kilohertz
            BandwidthUnit		 Megahertz
            SmallVelocityUnit		 CentimetersperSecond
            Percent		 Percentage
            SolidAngle		 Steradians
            SpectralBandwidthUnit		 Hertz
            BitsUnit		 MegaBits
            MagneticFieldUnit		 nanoTesla
            VoltageUnit		 Volts
        END ConnectReportUnits

        BEGIN ReportFavorites
            BEGIN Class
                Name		 Satellite
                BEGIN Favorite
                    Type		 Report
                    BaseDir		 User
                    Style		 posvelacc
                END Favorite
                BEGIN Favorite
                    Type		 Report
                    BaseDir		 User
                    Style		 LVLHGDGT
                END Favorite
                BEGIN Favorite
                    Type		 Report
                    BaseDir		 User
                    Style		 LVLH
                END Favorite
                BEGIN Favorite
                    Type		 Report
                    BaseDir		 User
                    Style		 LVLHGD
                END Favorite
                BEGIN Favorite
                    Type		 Report
                    BaseDir		 User
                    Style		 TNW
                END Favorite
                BEGIN Favorite
                    Type		 Report
                    BaseDir		 User
                    Style		 VNC
                END Favorite
                BEGIN Favorite
                    Type		 Report
                    BaseDir		 User
                    Style		 VVLH
                END Favorite
                BEGIN Favorite
                    Type		 Report
                    BaseDir		 Install
                    Style		 Inertial Position Velocity
                END Favorite
                BEGIN Favorite
                    Type		 Report
                    BaseDir		 User
                    Style		 Inertial Position Velocity
                END Favorite
                BEGIN Favorite
                    Type		 Report
                    BaseDir		 User
                    Style		 NED
                END Favorite
            END Class
            BEGIN Class
                Name		 Scenario
                BEGIN Favorite
                    Type		 Report
                    BaseDir		 User
                    Style		 New Report
                END Favorite
            END Class
        END ReportFavorites

        BEGIN ADFFileData
        END ADFFileData

        BEGIN GenDb

            BEGIN Database
                DbType		 Satellite
                DefDb		 stkAllTLE.sd
                UseMyDb		 Off
                MaxMatches		 2000
                Use4SOC		 On

                BEGIN FieldDefaults

                    BEGIN Field
                        Name		 "SSC Number"
                        Default		 "*"
                    END Field

                    BEGIN Field
                        Name		 "Common Name"
                        Default		 "*"
                    END Field

                END FieldDefaults

            END Database

            BEGIN Database
                DbType		 City
                DefDb		 stkCityDb.cd
                UseMyDb		 Off
                MaxMatches		 2000
                Use4SOC		 On

                BEGIN FieldDefaults

                    BEGIN Field
                        Name		 "City Name"
                        Default		 "*"
                    END Field

                END FieldDefaults

            END Database

            BEGIN Database
                DbType		 Facility
                DefDb		 stkFacility.fd
                UseMyDb		 Off
                MaxMatches		 2000
                Use4SOC		 On

                BEGIN FieldDefaults

                END FieldDefaults

            END Database
        END GenDb

        BEGIN SOCDb
            BEGIN Defaults
            END Defaults
        END SOCDb

        BEGIN Msgp4Ext
        END Msgp4Ext

        BEGIN FileLocations
        END FileLocations

        BEGIN Author
            Optimize		 No
            UseBasicGlobe		 No
            SaveEphemeris		 Yes
            SaveScenFolder		 No
            BEGIN ExternalFileTypes
                BEGIN Type
                    FileType		 Calculation Scalar
                    Include		 Yes
                END Type
                BEGIN Type
                    FileType		 Celestial Image
                    Include		 No
                END Type
                BEGIN Type
                    FileType		 Cloud
                    Include		 Yes
                END Type
                BEGIN Type
                    FileType		 EOP
                    Include		 Yes
                END Type
                BEGIN Type
                    FileType		 External Vector Data
                    Include		 Yes
                END Type
                BEGIN Type
                    FileType		 Globe
                    Include		 Yes
                END Type
                BEGIN Type
                    FileType		 Globe Data
                    Include		 No
                END Type
                BEGIN Type
                    FileType		 Map
                    Include		 No
                END Type
                BEGIN Type
                    FileType		 Map Image
                    Include		 No
                END Type
                BEGIN Type
                    FileType		 Marker/Label
                    Include		 Yes
                END Type
                BEGIN Type
                    FileType		 Model
                    Include		 Yes
                END Type
                BEGIN Type
                    FileType		 Object Break-up File
                    Include		 No
                END Type
                BEGIN Type
                    FileType		 Planetary Ephemeris
                    Include		 No
                END Type
                BEGIN Type
                    FileType		 Python Script
                    Include		 Yes
                END Type
                BEGIN Type
                    FileType		 Report Style Script
                    Include		 Yes
                END Type
                BEGIN Type
                    FileType		 Report/Graph Style
                    Include		 Yes
                END Type
                BEGIN Type
                    FileType		 Scalar Calculation File
                    Include		 Yes
                END Type
                BEGIN Type
                    FileType		 Terrain
                    Include		 Yes
                END Type
                BEGIN Type
                    FileType		 Volume Grid Intervals File
                    Include		 Yes
                END Type
                BEGIN Type
                    FileType		 Volumetric File
                    Include		 Yes
                END Type
                BEGIN Type
                    FileType		 WTM
                    Include		 Yes
                END Type
            END ExternalFileTypes
            ReadOnly		 No
            ViewerPassword		 No
            STKPassword		 No
            ExcludeInstallFiles		 No
            BEGIN ExternalFileList
            END ExternalFileList
        END Author

        BEGIN ExportDataFile
            FileType		 Ephemeris
            IntervalType		 Ephemeris
            TimePeriodStart		 0
            TimePeriodStop		 0
            StepType		 Ephemeris
            StepSize		 60
            EphemType		 STK
            UseVehicleCentralBody		 Yes
            CentralBody		 Earth
            SatelliteID		 -200000
            CoordSys		 ICRF
            NonSatCoordSys		 Fixed
            InterpolateBoundaries		 Yes
            EphemFormat		 Current
            InterpType		 9
            InterpOrder		 5
            AttCoordSys		 Fixed
            Quaternions		 0
            ExportCovar		 Position
            AttitudeFormat		 Current
            TimePrecision		 6
            CCSDSDateFormat		 YMD
            CCSDSEphFormat		 SciNotation
            CCSDSTimeSystem		 UTC
            CCSDSRefFrame		 ICRF
            UseSatCenterAndFrame		 No
            IncludeCovariance		 No
            IncludeAcceleration		 No
            CCSDSFileFormat		 KVN
        END ExportDataFile

        BEGIN Desc
        END Desc

        BEGIN RfEnv
<?xml version = "1.0" standalone = "yes"?>
<SCOPE>
    <VAR name = "PropagationChannel">
        <SCOPE>
            <VAR name = "UseITU618Section2p5">
                <BOOL>false</BOOL>
            </VAR>
            <VAR name = "UseCloudFogModel">
                <BOOL>false</BOOL>
            </VAR>
            <VAR name = "CloudFogModel">
                <SCOPE Class = "LinkEmbedControl">
                    <VAR name = "ReferenceType">
                        <STRING>&quot;Unlinked&quot;</STRING>
                    </VAR>
                    <VAR name = "Component">
                        <VAR name = "ITU-R_P840-7">
                            <SCOPE Class = "CloudFogLossModel">
                                <VAR name = "Version">
                                    <STRING>&quot;1.0.0 a&quot;</STRING>
                                </VAR>
                                <VAR name = "IdentifierInformation">
                                    <SCOPE>
                                        <VAR name = "Identifier">
                                            <STRING>&quot;{5F38432F-1038-4C19-BC4C-109D4C0FCBBB}&quot;</STRING>
                                        </VAR>
                                        <VAR name = "Version">
                                            <STRING>&quot;1&quot;</STRING>
                                        </VAR>
                                        <VAR name = "SdfInformation">
                                            <SCOPE>
                                                <VAR name = "Version">
                                                    <STRING>&quot;0.0&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Url">
                                                    <STRING>&quot;&quot;</STRING>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                        <VAR name = "SourceIdentifierInformation">
                                            <SCOPE>
                                                <VAR name = "Identifier">
                                                    <STRING>&quot;{E7BA4392-37BE-4446-A5C7-6068165B166A}&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Version">
                                                    <STRING>&quot;1&quot;</STRING>
                                                </VAR>
                                                <VAR name = "SdfInformation">
                                                    <SCOPE>
                                                        <VAR name = "Version">
                                                            <STRING>&quot;0.0&quot;</STRING>
                                                        </VAR>
                                                        <VAR name = "Url">
                                                            <STRING>&quot;&quot;</STRING>
                                                        </VAR>
                                                    </SCOPE>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                    </SCOPE>
                                </VAR>
                                <VAR name = "ComponentName">
                                    <STRING>&quot;ITU-R_P840-7&quot;</STRING>
                                </VAR>
                                <VAR name = "Description">
                                    <STRING>&quot;ITU-R P840-7&quot;</STRING>
                                </VAR>
                                <VAR name = "Type">
                                    <STRING>&quot;ITU-R P840-7&quot;</STRING>
                                </VAR>
                                <VAR name = "UserComment">
                                    <STRING>&quot;ITU-R P840-7&quot;</STRING>
                                </VAR>
                                <VAR name = "ReadOnly">
                                    <BOOL>false</BOOL>
                                </VAR>
                                <VAR name = "Clonable">
                                    <BOOL>true</BOOL>
                                </VAR>
                                <VAR name = "Category">
                                    <STRING>&quot;@Top&quot;</STRING>
                                </VAR>
                                <VAR name = "LiquidWaterDensityValueChoice">
                                    <STRING>&quot;Liquid Water Content Density Value&quot;</STRING>
                                </VAR>
                                <VAR name = "CloudCeiling">
                                    <QUANTITY Dimension = "DistanceUnit" Unit = "m">
                                        <REAL>3000</REAL>
                                    </QUANTITY>
                                </VAR>
                                <VAR name = "CloudLayerThickness">
                                    <QUANTITY Dimension = "DistanceUnit" Unit = "m">
                                        <REAL>500</REAL>
                                    </QUANTITY>
                                </VAR>
                                <VAR name = "CloudTemp">
                                    <QUANTITY Dimension = "Temperature" Unit = "K">
                                        <REAL>273.15</REAL>
                                    </QUANTITY>
                                </VAR>
                                <VAR name = "CloudLiqWaterDensity">
                                    <QUANTITY Dimension = "SmallDensity" Unit = "kg*m^-3">
                                        <REAL>0.0001</REAL>
                                    </QUANTITY>
                                </VAR>
                                <VAR name = "AnnualAveragePercentValue">
                                    <QUANTITY Dimension = "Percent" Unit = "unitValue">
                                        <REAL>0.01</REAL>
                                    </QUANTITY>
                                </VAR>
                                <VAR name = "MonthlyAveragePercentValue">
                                    <QUANTITY Dimension = "Percent" Unit = "unitValue">
                                        <REAL>0.01</REAL>
                                    </QUANTITY>
                                </VAR>
                                <VAR name = "LiqWaterAverageDataMonth">
                                    <INT>1</INT>
                                </VAR>
                                <VAR name = "UseRainHeightAsCloudThickness">
                                    <BOOL>false</BOOL>
                                </VAR>
                            </SCOPE>
                        </VAR>
                    </VAR>
                </SCOPE>
            </VAR>
            <VAR name = "UseTropoScintModel">
                <BOOL>false</BOOL>
            </VAR>
            <VAR name = "TropoScintModel">
                <SCOPE Class = "LinkEmbedControl">
                    <VAR name = "ReferenceType">
                        <STRING>&quot;Unlinked&quot;</STRING>
                    </VAR>
                    <VAR name = "Component">
                        <VAR name = "ITU-R_P618-12">
                            <SCOPE Class = "TropoScintLossModel">
                                <VAR name = "Version">
                                    <STRING>&quot;1.0.0 a&quot;</STRING>
                                </VAR>
                                <VAR name = "IdentifierInformation">
                                    <SCOPE>
                                        <VAR name = "Identifier">
                                            <STRING>&quot;{D91ED4CA-8C3C-42C5-9074-95306ADB5F1C}&quot;</STRING>
                                        </VAR>
                                        <VAR name = "Version">
                                            <STRING>&quot;1&quot;</STRING>
                                        </VAR>
                                        <VAR name = "SdfInformation">
                                            <SCOPE>
                                                <VAR name = "Version">
                                                    <STRING>&quot;0.0&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Url">
                                                    <STRING>&quot;&quot;</STRING>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                        <VAR name = "SourceIdentifierInformation">
                                            <SCOPE>
                                                <VAR name = "Identifier">
                                                    <STRING>&quot;{BC27045B-5A54-458E-BF17-702BCFE40CA8}&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Version">
                                                    <STRING>&quot;1&quot;</STRING>
                                                </VAR>
                                                <VAR name = "SdfInformation">
                                                    <SCOPE>
                                                        <VAR name = "Version">
                                                            <STRING>&quot;0.0&quot;</STRING>
                                                        </VAR>
                                                        <VAR name = "Url">
                                                            <STRING>&quot;&quot;</STRING>
                                                        </VAR>
                                                    </SCOPE>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                    </SCOPE>
                                </VAR>
                                <VAR name = "ComponentName">
                                    <STRING>&quot;ITU-R_P618-12&quot;</STRING>
                                </VAR>
                                <VAR name = "Description">
                                    <STRING>&quot;ITU-R P618-12&quot;</STRING>
                                </VAR>
                                <VAR name = "Type">
                                    <STRING>&quot;ITU-R P618-12&quot;</STRING>
                                </VAR>
                                <VAR name = "UserComment">
                                    <STRING>&quot;ITU-R P618-12&quot;</STRING>
                                </VAR>
                                <VAR name = "ReadOnly">
                                    <BOOL>false</BOOL>
                                </VAR>
                                <VAR name = "Clonable">
                                    <BOOL>true</BOOL>
                                </VAR>
                                <VAR name = "Category">
                                    <STRING>&quot;@Top&quot;</STRING>
                                </VAR>
                                <VAR name = "FadeDepthAverageTimeChoice">
                                    <STRING>&quot;Fade depth for the average year&quot;</STRING>
                                </VAR>
                                <VAR name = "ComputeDeepFade">
                                    <BOOL>false</BOOL>
                                </VAR>
                                <VAR name = "FadeOutage">
                                    <QUANTITY Dimension = "Percent" Unit = "unitValue">
                                        <REAL>0.001</REAL>
                                    </QUANTITY>
                                </VAR>
                                <VAR name = "PercentTimeRefracGrad">
                                    <QUANTITY Dimension = "Percent" Unit = "unitValue">
                                        <REAL>0.1</REAL>
                                    </QUANTITY>
                                </VAR>
                                <VAR name = "SurfaceTemperature">
                                    <QUANTITY Dimension = "Temperature" Unit = "K">
                                        <REAL>273.15</REAL>
                                    </QUANTITY>
                                </VAR>
                            </SCOPE>
                        </VAR>
                    </VAR>
                </SCOPE>
            </VAR>
            <VAR name = "UseIonoFadingModel">
                <BOOL>false</BOOL>
            </VAR>
            <VAR name = "IonoFadingModel">
                <SCOPE Class = "LinkEmbedControl">
                    <VAR name = "ReferenceType">
                        <STRING>&quot;Unlinked&quot;</STRING>
                    </VAR>
                    <VAR name = "Component">
                        <VAR name = "ITU-R_P531-13">
                            <SCOPE Class = "IonoFadingLossModel">
                                <VAR name = "Version">
                                    <STRING>&quot;1.0.0 a&quot;</STRING>
                                </VAR>
                                <VAR name = "IdentifierInformation">
                                    <SCOPE>
                                        <VAR name = "Identifier">
                                            <STRING>&quot;{0DF58ABC-5CBD-452C-8399-8CEF9964A20F}&quot;</STRING>
                                        </VAR>
                                        <VAR name = "Version">
                                            <STRING>&quot;1&quot;</STRING>
                                        </VAR>
                                        <VAR name = "SdfInformation">
                                            <SCOPE>
                                                <VAR name = "Version">
                                                    <STRING>&quot;0.0&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Url">
                                                    <STRING>&quot;&quot;</STRING>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                        <VAR name = "SourceIdentifierInformation">
                                            <SCOPE>
                                                <VAR name = "Identifier">
                                                    <STRING>&quot;{1699891E-9828-41C7-ADD4-4BE20EFC34A8}&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Version">
                                                    <STRING>&quot;1&quot;</STRING>
                                                </VAR>
                                                <VAR name = "SdfInformation">
                                                    <SCOPE>
                                                        <VAR name = "Version">
                                                            <STRING>&quot;0.0&quot;</STRING>
                                                        </VAR>
                                                        <VAR name = "Url">
                                                            <STRING>&quot;&quot;</STRING>
                                                        </VAR>
                                                    </SCOPE>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                    </SCOPE>
                                </VAR>
                                <VAR name = "ComponentName">
                                    <STRING>&quot;ITU-R_P531-13&quot;</STRING>
                                </VAR>
                                <VAR name = "Description">
                                    <STRING>&quot;ITU-R P531-13&quot;</STRING>
                                </VAR>
                                <VAR name = "Type">
                                    <STRING>&quot;ITU-R P531-13&quot;</STRING>
                                </VAR>
                                <VAR name = "UserComment">
                                    <STRING>&quot;ITU-R P531-13&quot;</STRING>
                                </VAR>
                                <VAR name = "ReadOnly">
                                    <BOOL>false</BOOL>
                                </VAR>
                                <VAR name = "Clonable">
                                    <BOOL>true</BOOL>
                                </VAR>
                                <VAR name = "Category">
                                    <STRING>&quot;@Top&quot;</STRING>
                                </VAR>
                                <VAR name = "UseAlternateAPFile">
                                    <BOOL>false</BOOL>
                                </VAR>
                                <VAR name = "AlternateAPDataFile">
                                    <STRING>
                                        <PROP name = "FullName">
                                            <STRING>&quot;&quot;</STRING>
                                        </PROP>&quot;&quot;</STRING>
                                </VAR>
                            </SCOPE>
                        </VAR>
                    </VAR>
                </SCOPE>
            </VAR>
            <VAR name = "UseRainModel">
                <BOOL>false</BOOL>
            </VAR>
            <VAR name = "RainModel">
                <SCOPE Class = "LinkEmbedControl">
                    <VAR name = "ReferenceType">
                        <STRING>&quot;Unlinked&quot;</STRING>
                    </VAR>
                    <VAR name = "Component">
                        <VAR name = "ITU-R_P618-12">
                            <SCOPE Class = "RainLossModel">
                                <VAR name = "Version">
                                    <STRING>&quot;1.0.0 a&quot;</STRING>
                                </VAR>
                                <VAR name = "IdentifierInformation">
                                    <SCOPE>
                                        <VAR name = "Identifier">
                                            <STRING>&quot;{5DDAEAD8-825D-4D98-ADCD-F76D79E3586E}&quot;</STRING>
                                        </VAR>
                                        <VAR name = "Version">
                                            <STRING>&quot;1&quot;</STRING>
                                        </VAR>
                                        <VAR name = "SdfInformation">
                                            <SCOPE>
                                                <VAR name = "Version">
                                                    <STRING>&quot;0.0&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Url">
                                                    <STRING>&quot;&quot;</STRING>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                        <VAR name = "SourceIdentifierInformation">
                                            <SCOPE>
                                                <VAR name = "Identifier">
                                                    <STRING>&quot;{1113D770-D1E5-4DEF-99A3-6B3F4D5CE16A}&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Version">
                                                    <STRING>&quot;1&quot;</STRING>
                                                </VAR>
                                                <VAR name = "SdfInformation">
                                                    <SCOPE>
                                                        <VAR name = "Version">
                                                            <STRING>&quot;0.0&quot;</STRING>
                                                        </VAR>
                                                        <VAR name = "Url">
                                                            <STRING>&quot;&quot;</STRING>
                                                        </VAR>
                                                    </SCOPE>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                    </SCOPE>
                                </VAR>
                                <VAR name = "ComponentName">
                                    <STRING>&quot;ITU-R_P618-12&quot;</STRING>
                                </VAR>
                                <VAR name = "Description">
                                    <STRING>&quot;ITU-R P618-12 rain model&quot;</STRING>
                                </VAR>
                                <VAR name = "Type">
                                    <STRING>&quot;ITU-R P618-12&quot;</STRING>
                                </VAR>
                                <VAR name = "UserComment">
                                    <STRING>&quot;ITU-R P618-12 rain model&quot;</STRING>
                                </VAR>
                                <VAR name = "ReadOnly">
                                    <BOOL>false</BOOL>
                                </VAR>
                                <VAR name = "Clonable">
                                    <BOOL>true</BOOL>
                                </VAR>
                                <VAR name = "Category">
                                    <STRING>&quot;@Top&quot;</STRING>
                                </VAR>
                                <VAR name = "SurfaceTemperature">
                                    <QUANTITY Dimension = "Temperature" Unit = "K">
                                        <REAL>273.15</REAL>
                                    </QUANTITY>
                                </VAR>
                                <VAR name = "EnableDepolarizationLoss">
                                    <BOOL>false</BOOL>
                                </VAR>
                            </SCOPE>
                        </VAR>
                    </VAR>
                </SCOPE>
            </VAR>
            <VAR name = "UseAtmosAbsorptionModel">
                <BOOL>false</BOOL>
            </VAR>
            <VAR name = "AtmosAbsorptionModel">
                <SCOPE Class = "LinkEmbedControl">
                    <VAR name = "ReferenceType">
                        <STRING>&quot;Unlinked&quot;</STRING>
                    </VAR>
                    <VAR name = "Component">
                        <VAR name = "ITU-R_P676-9">
                            <SCOPE Class = "AtmosphericAbsorptionModel">
                                <VAR name = "Version">
                                    <STRING>&quot;1.0.1 a&quot;</STRING>
                                </VAR>
                                <VAR name = "IdentifierInformation">
                                    <SCOPE>
                                        <VAR name = "Identifier">
                                            <STRING>&quot;{F7CE7601-CD89-490E-8311-70E2F8336862}&quot;</STRING>
                                        </VAR>
                                        <VAR name = "Version">
                                            <STRING>&quot;1&quot;</STRING>
                                        </VAR>
                                        <VAR name = "SdfInformation">
                                            <SCOPE>
                                                <VAR name = "Version">
                                                    <STRING>&quot;0.0&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Url">
                                                    <STRING>&quot;&quot;</STRING>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                        <VAR name = "SourceIdentifierInformation">
                                            <SCOPE>
                                                <VAR name = "Identifier">
                                                    <STRING>&quot;{5DBDF434-D4CA-44F6-8097-A6EBF681200D}&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Version">
                                                    <STRING>&quot;1&quot;</STRING>
                                                </VAR>
                                                <VAR name = "SdfInformation">
                                                    <SCOPE>
                                                        <VAR name = "Version">
                                                            <STRING>&quot;0.0&quot;</STRING>
                                                        </VAR>
                                                        <VAR name = "Url">
                                                            <STRING>&quot;&quot;</STRING>
                                                        </VAR>
                                                    </SCOPE>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                    </SCOPE>
                                </VAR>
                                <VAR name = "ComponentName">
                                    <STRING>&quot;ITU-R_P676-9&quot;</STRING>
                                </VAR>
                                <VAR name = "Description">
                                    <STRING>&quot;ITU-R P676-9 gaseous absorption model&quot;</STRING>
                                </VAR>
                                <VAR name = "Type">
                                    <STRING>&quot;ITU-R P676-9&quot;</STRING>
                                </VAR>
                                <VAR name = "UserComment">
                                    <STRING>&quot;ITU-R P676-9 gaseous absorption model&quot;</STRING>
                                </VAR>
                                <VAR name = "ReadOnly">
                                    <BOOL>false</BOOL>
                                </VAR>
                                <VAR name = "Clonable">
                                    <BOOL>true</BOOL>
                                </VAR>
                                <VAR name = "Category">
                                    <STRING>&quot;@Top&quot;</STRING>
                                </VAR>
                                <VAR name = "UseApproxMethod">
                                    <BOOL>true</BOOL>
                                </VAR>
                                <VAR name = "UseSeasonalRegional">
                                    <BOOL>true</BOOL>
                                </VAR>
                            </SCOPE>
                        </VAR>
                    </VAR>
                </SCOPE>
            </VAR>
            <VAR name = "UseUrbanTerresPropLossModel">
                <BOOL>false</BOOL>
            </VAR>
            <VAR name = "UrbanTerresPropLossModel">
                <SCOPE Class = "LinkEmbedControl">
                    <VAR name = "ReferenceType">
                        <STRING>&quot;Unlinked&quot;</STRING>
                    </VAR>
                    <VAR name = "Component">
                        <VAR name = "Two_Ray">
                            <SCOPE Class = "UrbanTerrestrialPropagationLossModel">
                                <VAR name = "Version">
                                    <STRING>&quot;1.0.0 a&quot;</STRING>
                                </VAR>
                                <VAR name = "IdentifierInformation">
                                    <SCOPE>
                                        <VAR name = "Identifier">
                                            <STRING>&quot;{3613279B-3520-4F90-9691-D1F317D95676}&quot;</STRING>
                                        </VAR>
                                        <VAR name = "Version">
                                            <STRING>&quot;1&quot;</STRING>
                                        </VAR>
                                        <VAR name = "SdfInformation">
                                            <SCOPE>
                                                <VAR name = "Version">
                                                    <STRING>&quot;0.0&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Url">
                                                    <STRING>&quot;&quot;</STRING>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                        <VAR name = "SourceIdentifierInformation">
                                            <SCOPE>
                                                <VAR name = "Identifier">
                                                    <STRING>&quot;{60FA4C9B-5D74-4743-A449-66CEB6DFC97B}&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Version">
                                                    <STRING>&quot;1&quot;</STRING>
                                                </VAR>
                                                <VAR name = "SdfInformation">
                                                    <SCOPE>
                                                        <VAR name = "Version">
                                                            <STRING>&quot;0.0&quot;</STRING>
                                                        </VAR>
                                                        <VAR name = "Url">
                                                            <STRING>&quot;&quot;</STRING>
                                                        </VAR>
                                                    </SCOPE>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                    </SCOPE>
                                </VAR>
                                <VAR name = "ComponentName">
                                    <STRING>&quot;Two_Ray&quot;</STRING>
                                </VAR>
                                <VAR name = "Description">
                                    <STRING>&quot;Two Ray (Fourth Power Law) atmospheric absorption model&quot;</STRING>
                                </VAR>
                                <VAR name = "Type">
                                    <STRING>&quot;Two Ray&quot;</STRING>
                                </VAR>
                                <VAR name = "UserComment">
                                    <STRING>&quot;Two Ray (Fourth Power Law) atmospheric absorption model&quot;</STRING>
                                </VAR>
                                <VAR name = "ReadOnly">
                                    <BOOL>false</BOOL>
                                </VAR>
                                <VAR name = "Clonable">
                                    <BOOL>true</BOOL>
                                </VAR>
                                <VAR name = "Category">
                                    <STRING>&quot;@Top&quot;</STRING>
                                </VAR>
                                <VAR name = "SurfaceTemperature">
                                    <QUANTITY Dimension = "Temperature" Unit = "K">
                                        <REAL>273.15</REAL>
                                    </QUANTITY>
                                </VAR>
                                <VAR name = "LossFactor">
                                    <REAL>1</REAL>
                                </VAR>
                            </SCOPE>
                        </VAR>
                    </VAR>
                </SCOPE>
            </VAR>
            <VAR name = "UseCustomA">
                <BOOL>false</BOOL>
            </VAR>
            <VAR name = "UseCustomB">
                <BOOL>false</BOOL>
            </VAR>
            <VAR name = "UseCustomC">
                <BOOL>false</BOOL>
            </VAR>
        </SCOPE>
    </VAR>
    <VAR name = "EarthTemperature">
        <QUANTITY Dimension = "Temperature" Unit = "K">
            <REAL>290</REAL>
        </QUANTITY>
    </VAR>
    <VAR name = "RainOutagePercent">
        <REAL>0.1</REAL>
    </VAR>
    <VAR name = "ActiveCommSystem">
        <LINKTOOBJ>
            <STRING>&quot;None&quot;</STRING>
        </LINKTOOBJ>
    </VAR>
    <VAR name = "MagneticNorthPoleLatitude">
        <QUANTITY Dimension = "AngleUnit" Unit = "rad">
            <REAL>1.387536755335492</REAL>
        </QUANTITY>
    </VAR>
    <VAR name = "MagneticNorthPoleLongitude">
        <QUANTITY Dimension = "AngleUnit" Unit = "rad">
            <REAL>-1.204277183876087</REAL>
        </QUANTITY>
    </VAR>
</SCOPE>        END RfEnv

        BEGIN LaserEnv
<?xml version = "1.0" standalone = "yes"?>
<SCOPE>
    <VAR name = "PropagationChannel">
        <SCOPE>
            <VAR name = "EnableAtmosphericLossModel">
                <BOOL>false</BOOL>
            </VAR>
            <VAR name = "AtmosphericLossModel">
                <SCOPE Class = "LinkEmbedControl">
                    <VAR name = "ReferenceType">
                        <STRING>&quot;Unlinked&quot;</STRING>
                    </VAR>
                    <VAR name = "Component">
                        <VAR name = "Beer-Bouguer-Lambert_Law">
                            <SCOPE Class = "LaserAtmosphericAbsorptionLossModel">
                                <VAR name = "Version">
                                    <STRING>&quot;1.0.0 a&quot;</STRING>
                                </VAR>
                                <VAR name = "IdentifierInformation">
                                    <SCOPE>
                                        <VAR name = "Identifier">
                                            <STRING>&quot;{2DDA70AB-B280-46E0-8D35-7FE242AA081C}&quot;</STRING>
                                        </VAR>
                                        <VAR name = "Version">
                                            <STRING>&quot;1&quot;</STRING>
                                        </VAR>
                                        <VAR name = "SdfInformation">
                                            <SCOPE>
                                                <VAR name = "Version">
                                                    <STRING>&quot;0.0&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Url">
                                                    <STRING>&quot;&quot;</STRING>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                        <VAR name = "SourceIdentifierInformation">
                                            <SCOPE>
                                                <VAR name = "Identifier">
                                                    <STRING>&quot;{6896684B-630D-472D-8027-385684842E74}&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Version">
                                                    <STRING>&quot;1&quot;</STRING>
                                                </VAR>
                                                <VAR name = "SdfInformation">
                                                    <SCOPE>
                                                        <VAR name = "Version">
                                                            <STRING>&quot;0.0&quot;</STRING>
                                                        </VAR>
                                                        <VAR name = "Url">
                                                            <STRING>&quot;&quot;</STRING>
                                                        </VAR>
                                                    </SCOPE>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                    </SCOPE>
                                </VAR>
                                <VAR name = "ComponentName">
                                    <STRING>&quot;Beer-Bouguer-Lambert_Law&quot;</STRING>
                                </VAR>
                                <VAR name = "Description">
                                    <STRING>&quot;Model atmospheric loss for laser receivers using the Beer-Bouguer-Lambert Law&quot;</STRING>
                                </VAR>
                                <VAR name = "Type">
                                    <STRING>&quot;Beer-Bouguer-Lambert Law&quot;</STRING>
                                </VAR>
                                <VAR name = "UserComment">
                                    <STRING>&quot;Model atmospheric loss for laser receivers using the Beer-Bouguer-Lambert Law&quot;</STRING>
                                </VAR>
                                <VAR name = "ReadOnly">
                                    <BOOL>false</BOOL>
                                </VAR>
                                <VAR name = "Clonable">
                                    <BOOL>true</BOOL>
                                </VAR>
                                <VAR name = "Category">
                                    <STRING>&quot;@Top&quot;</STRING>
                                </VAR>
                                <VAR name = "LayerList">
                                    <LIST>
                                        <SCOPE>
                                            <VAR name = "LayerNum">
                                                <INT>1</INT>
                                            </VAR>
                                            <VAR name = "LayerTop">
                                                <QUANTITY Dimension = "DistanceUnit" Unit = "m">
                                                    <REAL>100000</REAL>
                                                </QUANTITY>
                                            </VAR>
                                            <VAR name = "ExtinctionCoefficient">
                                                <QUANTITY Dimension = "UnitlessPerSmallDistance" Unit = "m^-1">
                                                    <REAL>0</REAL>
                                                </QUANTITY>
                                            </VAR>
                                        </SCOPE>
                                    </LIST>
                                </VAR>
                                <VAR name = "EnableEvenlySpacedHeights">
                                    <BOOL>true</BOOL>
                                </VAR>
                                <VAR name = "MaxLayerHeight">
                                    <QUANTITY Dimension = "DistanceUnit" Unit = "m">
                                        <REAL>100000</REAL>
                                    </QUANTITY>
                                </VAR>
                            </SCOPE>
                        </VAR>
                    </VAR>
                </SCOPE>
            </VAR>
            <VAR name = "EnableTropoScintLossModel">
                <BOOL>false</BOOL>
            </VAR>
            <VAR name = "TropoScintLossModel">
                <SCOPE Class = "LinkEmbedControl">
                    <VAR name = "ReferenceType">
                        <STRING>&quot;Unlinked&quot;</STRING>
                    </VAR>
                    <VAR name = "Component">
                        <VAR name = "ITU-R_P1814">
                            <SCOPE Class = "LaserTropoScintLossModel">
                                <VAR name = "Version">
                                    <STRING>&quot;1.0.0 a&quot;</STRING>
                                </VAR>
                                <VAR name = "IdentifierInformation">
                                    <SCOPE>
                                        <VAR name = "Identifier">
                                            <STRING>&quot;{6350D731-D0D5-4E49-84A9-4C21B0F1DFC8}&quot;</STRING>
                                        </VAR>
                                        <VAR name = "Version">
                                            <STRING>&quot;1&quot;</STRING>
                                        </VAR>
                                        <VAR name = "SdfInformation">
                                            <SCOPE>
                                                <VAR name = "Version">
                                                    <STRING>&quot;0.0&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Url">
                                                    <STRING>&quot;&quot;</STRING>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                        <VAR name = "SourceIdentifierInformation">
                                            <SCOPE>
                                                <VAR name = "Identifier">
                                                    <STRING>&quot;{651AF2C8-7D6D-457E-8F99-1FB796A460BF}&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Version">
                                                    <STRING>&quot;1&quot;</STRING>
                                                </VAR>
                                                <VAR name = "SdfInformation">
                                                    <SCOPE>
                                                        <VAR name = "Version">
                                                            <STRING>&quot;0.0&quot;</STRING>
                                                        </VAR>
                                                        <VAR name = "Url">
                                                            <STRING>&quot;&quot;</STRING>
                                                        </VAR>
                                                    </SCOPE>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                    </SCOPE>
                                </VAR>
                                <VAR name = "ComponentName">
                                    <STRING>&quot;ITU-R_P1814&quot;</STRING>
                                </VAR>
                                <VAR name = "Description">
                                    <STRING>&quot;ITU-R P1814&quot;</STRING>
                                </VAR>
                                <VAR name = "Type">
                                    <STRING>&quot;ITU-R P1814&quot;</STRING>
                                </VAR>
                                <VAR name = "UserComment">
                                    <STRING>&quot;ITU-R P1814&quot;</STRING>
                                </VAR>
                                <VAR name = "ReadOnly">
                                    <BOOL>false</BOOL>
                                </VAR>
                                <VAR name = "Clonable">
                                    <BOOL>true</BOOL>
                                </VAR>
                                <VAR name = "Category">
                                    <STRING>&quot;@Top&quot;</STRING>
                                </VAR>
                                <VAR name = "AtmosphericTurbulenceModel">
                                    <VAR name = "Constant">
                                        <SCOPE Class = "AtmosphericTurbulenceModel">
                                            <VAR name = "ConstantRefractiveIndexStructureParameter">
                                                <REAL>1.7e-14</REAL>
                                            </VAR>
                                            <VAR name = "Type">
                                                <STRING>&quot;Constant&quot;</STRING>
                                            </VAR>
                                        </SCOPE>
                                    </VAR>
                                </VAR>
                            </SCOPE>
                        </VAR>
                    </VAR>
                </SCOPE>
            </VAR>
        </SCOPE>
    </VAR>
</SCOPE>        END LaserEnv

        BEGIN CommRad
        END CommRad

        BEGIN RadarCrossSection
<?xml version = "1.0" standalone = "yes"?>
<SCOPE>
    <VAR name = "Model">
        <SCOPE Class = "LinkEmbedControl">
            <VAR name = "ReferenceType">
                <STRING>&quot;Unlinked&quot;</STRING>
            </VAR>
            <VAR name = "Component">
                <VAR name = "Radar_Cross_Section">
                    <SCOPE Class = "RCS">
                        <VAR name = "Version">
                            <STRING>&quot;1.0.0 a&quot;</STRING>
                        </VAR>
                        <VAR name = "IdentifierInformation">
                            <SCOPE>
                                <VAR name = "Identifier">
                                    <STRING>&quot;{B5CAD9E5-B9A3-4DF9-AAF2-73D90DA8065D}&quot;</STRING>
                                </VAR>
                                <VAR name = "Version">
                                    <STRING>&quot;1&quot;</STRING>
                                </VAR>
                                <VAR name = "SdfInformation">
                                    <SCOPE>
                                        <VAR name = "Version">
                                            <STRING>&quot;0.0&quot;</STRING>
                                        </VAR>
                                        <VAR name = "Url">
                                            <STRING>&quot;&quot;</STRING>
                                        </VAR>
                                    </SCOPE>
                                </VAR>
                                <VAR name = "SourceIdentifierInformation">
                                    <SCOPE>
                                        <VAR name = "Identifier">
                                            <STRING>&quot;{EF03E656-5AB7-4F70-A363-4753683F2BD4}&quot;</STRING>
                                        </VAR>
                                        <VAR name = "Version">
                                            <STRING>&quot;1&quot;</STRING>
                                        </VAR>
                                        <VAR name = "SdfInformation">
                                            <SCOPE>
                                                <VAR name = "Version">
                                                    <STRING>&quot;0.0&quot;</STRING>
                                                </VAR>
                                                <VAR name = "Url">
                                                    <STRING>&quot;&quot;</STRING>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                    </SCOPE>
                                </VAR>
                            </SCOPE>
                        </VAR>
                        <VAR name = "ComponentName">
                            <STRING>&quot;Radar_Cross_Section&quot;</STRING>
                        </VAR>
                        <VAR name = "Description">
                            <STRING>&quot;Radar Cross Section&quot;</STRING>
                        </VAR>
                        <VAR name = "Type">
                            <STRING>&quot;Radar Cross Section&quot;</STRING>
                        </VAR>
                        <VAR name = "UserComment">
                            <STRING>&quot;Radar Cross Section&quot;</STRING>
                        </VAR>
                        <VAR name = "ReadOnly">
                            <BOOL>false</BOOL>
                        </VAR>
                        <VAR name = "Clonable">
                            <BOOL>true</BOOL>
                        </VAR>
                        <VAR name = "Category">
                            <STRING>&quot;@Top&quot;</STRING>
                        </VAR>
                        <VAR name = "FrequencyBandList">
                            <LIST>
                                <SCOPE>
                                    <VAR name = "MinFrequency">
                                        <QUANTITY Dimension = "BandwidthUnit" Unit = "Hz">
                                            <REAL>2997920</REAL>
                                        </QUANTITY>
                                    </VAR>
                                    <VAR name = "ComputeTypeStrategy">
                                        <VAR name = "Constant Value">
                                            <SCOPE Class = "RCS Compute Strategy">
                                                <VAR name = "ConstantValue">
                                                    <QUANTITY Dimension = "RcsUnit" Unit = "sqm">
                                                        <REAL>1</REAL>
                                                    </QUANTITY>
                                                </VAR>
                                                <VAR name = "Type">
                                                    <STRING>&quot;Constant Value&quot;</STRING>
                                                </VAR>
                                                <VAR name = "ComponentName">
                                                    <STRING>&quot;Constant Value&quot;</STRING>
                                                </VAR>
                                            </SCOPE>
                                        </VAR>
                                    </VAR>
                                    <VAR name = "SwerlingCase">
                                        <STRING>&quot;0&quot;</STRING>
                                    </VAR>
                                </SCOPE>
                            </LIST>
                        </VAR>
                    </SCOPE>
                </VAR>
            </VAR>
        </SCOPE>
    </VAR>
</SCOPE>        END RadarCrossSection

        BEGIN RadarClutter
<?xml version = "1.0" standalone = "yes"?>
<SCOPE>
    <VAR name = "ClutterMap">
        <VAR name = "Constant Coefficient">
            <SCOPE Class = "Clutter Map">
                <VAR name = "ClutterCoefficient">
                    <QUANTITY Dimension = "RatioUnit" Unit = "units">
                        <REAL>1</REAL>
                    </QUANTITY>
                </VAR>
                <VAR name = "Type">
                    <STRING>&quot;Constant Coefficient&quot;</STRING>
                </VAR>
                <VAR name = "ComponentName">
                    <STRING>&quot;Constant Coefficient&quot;</STRING>
                </VAR>
            </SCOPE>
        </VAR>
    </VAR>
</SCOPE>        END RadarClutter

        BEGIN Gator
            RPOComponentsLoaded		 False
        END Gator

        BEGIN Crdn
        END Crdn

        BEGIN SpiceExt
            OutputErrorMsgsOnLoad		 No
            SpiceFile		 "planets.bsp"

        END SpiceExt

        BEGIN Graphics

            BEGIN Animation

                StartTime		 1 Jan 2024 00:00:00.000000000
                EndTime		 1 Jan 2024 02:00:00.000000000
                CurrentTime		 1 Jan 2024 00:00:00.000000000
                Direction		 Forward
                UpdateDelta		 60
                RefreshDelta		 0.010000
                XRealTimeMult		 1
                RealTimeOffset		 0
                XRtStartFromPause		                Yes		
                TimeArrayIncrement		 1

            END Animation


            BEGIN DisplayFlags
                ShowLabels		 On
                ShowPassLabel		 Off
                ShowElsetNum		 Off
                ShowGndTracks		 On
                ShowGndMarkers		 On
                ShowOrbitMarkers		 On
                ShowPlanetOrbits		 Off
                ShowPlanetCBIPos		 On
                ShowPlanetCBILabel		 On
                ShowPlanetGndPos		 On
                ShowPlanetGndLabel		 On
                ShowSensors		 On
                ShowWayptMarkers		 Off
                ShowWayptTurnMarkers		 Off
                ShowOrbits		 On
                ShowDtedRegions		 Off
                ShowAreaTgtCentroids		 On
                ShowToolBar		 On
                ShowStatusBar		 On
                ShowScrollBars		 On
                AllowAnimUpdate		 On
                AccShowLine		 On
                AccAnimHigh		 On
                AccStatHigh		 On
                AccAnimLineLineWidth		  1.0000000000000000e+00
                ShowPrintButton		 On
                ShowAnimButtons		 On
                ShowAnimModeButtons		 On
                ShowZoomMsrButtons		 On
                ShowMapCbButton		 Off
            END DisplayFlags

            BEGIN WinFonts

                Consolas,12,700,0
                Consolas,14,700,0
                Consolas,16,700,0

            END WinFonts

            BEGIN MapData

                BEGIN TerrainConverterData
                    NorthLat		  0.0000000000000000e+00
                    EastLon		  0.0000000000000000e+00
                    SouthLat		  0.0000000000000000e+00
                    WestLon		  0.0000000000000000e+00
                    ColorByRGB		 No
                    AltsFromMSL		 No
                    UseColorRamp		 Yes
                    UseRegionMinMax		 Yes
                    SizeSameAsSrc		 Yes
                    MinAltHSV		  0.0000000000000000e+00  6.9999999999999996e-01  8.0000000000000004e-01  4.0000000000000002e-01
                    MaxAltHSV		  1.0000000000000000e+06  0.0000000000000000e+00  2.0000000000000001e-01  1.0000000000000000e+00
                    SmoothColors		 Yes
                    CreateChunkTrn		 No
                    OutputFormat		 PDTTX
                END TerrainConverterData

                DisableDefKbdActions		 Off
                TextShadowStyle		 Dark
                TextShadowColor		 #000000
                BingLevelOfDetailScale		 2
                BEGIN Map
                    MapNum		 1
                    TrackingMode		 LatLon
                    PickEnabled		 On
                    PanEnabled		 On

                    BEGIN MapAttributes
                        PrimaryBody		 Earth
                        SecondaryBody		 Sun
                        CenterLatitude		 0
                        CenterLongitude		 0
                        ProjectionAltitude		 63621860
                        FieldOfView		 35
                        OrthoDisplayDistance		 20000000
                        TransformTrajectory		 On
                        EquatorialRadius		 6378137
                        BackgroundColor		 #000000
                        LatLonLines		 On
                        LatSpacing		 30
                        LonSpacing		 30
                        LatLonLineColor		 #999999
                        LatLonLineStyle		 2
                        ShowOrthoDistGrid		 Off
                        OrthoGridXSpacing		 5
                        OrthoGridYSpacing		 5
                        OrthoGridColor		 #ffffff
                        ShowImageExtents		 Off
                        ImageExtentLineColor		 #ffffff
                        ImageExtentLineStyle		 0
                        ImageExtentLineWidth		 1
                        ShowImageNames		 Off
                        ImageNameFont		 0
                        Projection		 EquidistantCylindrical
                        Resolution		 VeryLow
                        CoordinateSys		 ECF
                        UseBackgroundImage		 On
                        UseBingForBackground		 On
                        BingType		 Aerial
                        BingLogoHorizAlign		 Right
                        BingLogoVertAlign		 Bottom
                        BackgroundImageFile		 Basic.bmp
                        UseNightLights		 Off
                        NightLightsFactor		 3.5
                        UseCloudsFile		 Off
                        BEGIN ZoomLocations
                            BEGIN ZoomLocation
                                CenterLat		 0
                                CenterLon		 0
                                ZoomWidth		 360
                                ZoomHeight		 180
                            END ZoomLocation
                        END ZoomLocations
                        UseVarAspectRatio		 No
                        SwapMapResolution		 Yes
                        NoneToVLowSwapDist		 2000000
                        VLowToLowSwapDist		 20000
                        LowToMediumSwapDist		 10000
                        MediumToHighSwapDist		 5000
                        HighToVHighSwapDist		 1000
                        VHighToSHighSwapDist		 100
                        BEGIN Axes
                            DisplayAxes		 no
                            CoordSys		 CBI
                            2aryCB		 Sun
                            Display+x		 yes
                            Label+x		 yes
                            Color+x		 #ffffff
                            Scale+x		 3
                            Display-x		 yes
                            Label-x		 yes
                            Color-x		 #ffffff
                            Scale-x		 3
                            Display+y		 yes
                            Label+y		 yes
                            Color+y		 #ffffff
                            Scale+y		 3
                            Display-y		 yes
                            Label-y		 yes
                            Color-y		 #ffffff
                            Scale-y		 3
                            Display+z		 yes
                            Label+z		 yes
                            Color+z		 #ffffff
                            Scale+z		 3
                            Display-z		 yes
                            Label-z		 yes
                            Color-z		 #ffffff
                            Scale-z		 3
                        END Axes

                    END MapAttributes

                    BEGIN MapList
                        BEGIN Detail
                            Alias		 RWDB2_Coastlines
                            Show		 Yes
                            Color		 #8fbc8f
                        END Detail
                        BEGIN Detail
                            Alias		 RWDB2_International_Borders
                            Show		 No
                            Color		 #8fbc8f
                        END Detail
                        BEGIN Detail
                            Alias		 RWDB2_Islands
                            Show		 No
                            Color		 #8fbc8f
                        END Detail
                        BEGIN Detail
                            Alias		 RWDB2_Lakes
                            Show		 No
                            Color		 #87cefa
                        END Detail
                        BEGIN Detail
                            Alias		 RWDB2_Provincial_Borders
                            Show		 No
                            Color		 #8fbc8f
                        END Detail
                        BEGIN Detail
                            Alias		 RWDB2_Rivers
                            Show		 No
                            Color		 #87cefa
                        END Detail
                    END MapList


                    BEGIN MapAnnotations
                    END MapAnnotations

                    BEGIN DisplayFlags
                        ShowLabels		 On
                        ShowPassLabel		 Off
                        ShowElsetNum		 Off
                        ShowGndTracks		 On
                        ShowGndMarkers		 On
                        ShowOrbitMarkers		 On
                        ShowPlanetOrbits		 Off
                        ShowPlanetCBIPos		 On
                        ShowPlanetCBILabel		 On
                        ShowPlanetGndPos		 On
                        ShowPlanetGndLabel		 On
                        ShowSensors		 On
                        ShowWayptMarkers		 Off
                        ShowWayptTurnMarkers		 Off
                        ShowOrbits		 On
                        ShowDtedRegions		 Off
                        ShowAreaTgtCentroids		 On
                        ShowToolBar		 On
                        ShowStatusBar		 On
                        ShowScrollBars		 On
                        AllowAnimUpdate		 Off
                        AccShowLine		 On
                        AccAnimHigh		 On
                        AccStatHigh		 On
                        AccAnimLineLineWidth		  1.0000000000000000e+00
                        ShowPrintButton		 On
                        ShowAnimButtons		 On
                        ShowAnimModeButtons		 On
                        ShowZoomMsrButtons		 On
                        ShowMapCbButton		 Off
                    END DisplayFlags

                    BEGIN RecordMovie
                        OutputFormat		 VIDEO
                        SdfSelected		 No
                        BaseName		 Frame
                        Digits		 4
                        Frame		 0
                        LastAnimTime		 0
                        OutputMode		 Normal
                        HiResAssembly		 Assemble
                        HRWidth		 6000
                        HRHeight		 4500
                        HRDPI		 600
                        UseSnapInterval		 No
                        SnapInterval		 0
                        VideoCodec		 "H264"
                        Framerate		 30
                        Bitrate		 10000000
                    END RecordMovie


                    BEGIN TimeDisplay
                        Show		 0
                        TextColor		 #ffffff
                        TextTranslucency		 0
                        ShowBackground		 0
                        BackColor		 #4d4d4d
                        BackTranslucency		 0.4
                        XPosition		 20
                        YPosition		 -20
                    END TimeDisplay

                    BEGIN LightingData
                        DisplayAltitude		 0
                        SubsolarPoint		 Off
                        SubsolarPointColor		 #ffff00
                        SubsolarPointMarkerStyle		 2

                        ShowUmbraLine		 Off
                        UmbraLineColor		 #000000
                        UmbraLineStyle		 0
                        UmbraLineWidth		 2
                        FillUmbra		 On
                        UmbraFillColor		 #000000
                        ShowSunlightLine		 Off
                        SunlightLineColor		 #ffff00
                        SunlightLineStyle		 0
                        SunlightLineWidth		 2
                        FillSunlight		 On
                        SunlightFillColor		 #ffffff
                        SunlightMinOpacity		 0
                        SunlightMaxOpacity		 0.2
                        UmbraMaxOpacity		 0.7
                        UmbraMinOpacity		 0.4
                    END LightingData
                END Map

                BEGIN MapStyles

                    UseStyleTime		 No

                    BEGIN Style
                        Name		 DefaultWithBing
                        Time		 7671600
                        UpdateDelta		 10

                        BEGIN MapAttributes
                            PrimaryBody		 Earth
                            SecondaryBody		 Sun
                            CenterLatitude		 0
                            CenterLongitude		 0
                            ProjectionAltitude		 63621860
                            FieldOfView		 35
                            OrthoDisplayDistance		 20000000
                            TransformTrajectory		 On
                            EquatorialRadius		 6378137
                            BackgroundColor		 #000000
                            LatLonLines		 On
                            LatSpacing		 30
                            LonSpacing		 30
                            LatLonLineColor		 #999999
                            LatLonLineStyle		 2
                            ShowOrthoDistGrid		 Off
                            OrthoGridXSpacing		 5
                            OrthoGridYSpacing		 5
                            OrthoGridColor		 #ffffff
                            ShowImageExtents		 Off
                            ImageExtentLineColor		 #ffffff
                            ImageExtentLineStyle		 0
                            ImageExtentLineWidth		 1
                            ShowImageNames		 Off
                            ImageNameFont		 0
                            Projection		 EquidistantCylindrical
                            Resolution		 VeryLow
                            CoordinateSys		 ECF
                            UseBackgroundImage		 On
                            UseBingForBackground		 On
                            BingType		 Aerial
                            BingLogoHorizAlign		 Right
                            BingLogoVertAlign		 Bottom
                            BackgroundImageFile		 Basic.bmp
                            UseNightLights		 Off
                            NightLightsFactor		 3.5
                            UseCloudsFile		 Off
                            BEGIN ZoomLocations
                                BEGIN ZoomLocation
                                    CenterLat		 0
                                    CenterLon		 0
                                    ZoomWidth		 359.999998
                                    ZoomHeight		 180
                                END ZoomLocation
                            END ZoomLocations
                            UseVarAspectRatio		 No
                            SwapMapResolution		 Yes
                            NoneToVLowSwapDist		 2000000
                            VLowToLowSwapDist		 20000
                            LowToMediumSwapDist		 10000
                            MediumToHighSwapDist		 5000
                            HighToVHighSwapDist		 1000
                            VHighToSHighSwapDist		 100
                            BEGIN Axes
                                DisplayAxes		 no
                                CoordSys		 CBI
                                2aryCB		 Sun
                                Display+x		 yes
                                Label+x		 yes
                                Color+x		 #ffffff
                                Scale+x		 3
                                Display-x		 yes
                                Label-x		 yes
                                Color-x		 #ffffff
                                Scale-x		 3
                                Display+y		 yes
                                Label+y		 yes
                                Color+y		 #ffffff
                                Scale+y		 3
                                Display-y		 yes
                                Label-y		 yes
                                Color-y		 #ffffff
                                Scale-y		 3
                                Display+z		 yes
                                Label+z		 yes
                                Color+z		 #ffffff
                                Scale+z		 3
                                Display-z		 yes
                                Label-z		 yes
                                Color-z		 #ffffff
                                Scale-z		 3
                            END Axes

                        END MapAttributes

                        BEGIN MapList
                            BEGIN Detail
                                Alias		 RWDB2_Coastlines
                                Show		 Yes
                                Color		 #8fbc8f
                            END Detail
                            BEGIN Detail
                                Alias		 RWDB2_International_Borders
                                Show		 No
                                Color		 #8fbc8f
                            END Detail
                            BEGIN Detail
                                Alias		 RWDB2_Islands
                                Show		 No
                                Color		 #8fbc8f
                            END Detail
                            BEGIN Detail
                                Alias		 RWDB2_Lakes
                                Show		 No
                                Color		 #87cefa
                            END Detail
                            BEGIN Detail
                                Alias		 RWDB2_Provincial_Borders
                                Show		 No
                                Color		 #8fbc8f
                            END Detail
                            BEGIN Detail
                                Alias		 RWDB2_Rivers
                                Show		 No
                                Color		 #87cefa
                            END Detail
                        END MapList


                        BEGIN MapAnnotations
                        END MapAnnotations

                        BEGIN RecordMovie
                            OutputFormat		 VIDEO
                            SdfSelected		 No
                            BaseName		 Frame
                            Digits		 4
                            Frame		 0
                            LastAnimTime		 0
                            OutputMode		 Normal
                            HiResAssembly		 Assemble
                            HRWidth		 6000
                            HRHeight		 4500
                            HRDPI		 600
                            UseSnapInterval		 No
                            SnapInterval		 0
                            VideoCodec		 "H264"
                            Framerate		 30
                            Bitrate		 10000000
                        END RecordMovie


                        BEGIN TimeDisplay
                            Show		 0
                            TextColor		 #ffffff
                            TextTranslucency		 0
                            ShowBackground		 0
                            BackColor		 #4d4d4d
                            BackTranslucency		 0.4
                            XPosition		 20
                            YPosition		 -20
                        END TimeDisplay

                        BEGIN LightingData
                            DisplayAltitude		 0
                            SubsolarPoint		 Off
                            SubsolarPointColor		 #ffff00
                            SubsolarPointMarkerStyle		 2

                            ShowUmbraLine		 Off
                            UmbraLineColor		 #000000
                            UmbraLineStyle		 0
                            UmbraLineWidth		 2
                            FillUmbra		 On
                            UmbraFillColor		 #000000
                            ShowSunlightLine		 Off
                            SunlightLineColor		 #ffff00
                            SunlightLineStyle		 0
                            SunlightLineWidth		 2
                            FillSunlight		 On
                            SunlightFillColor		 #ffffff
                            SunlightMinOpacity		 0
                            SunlightMaxOpacity		 0.2
                            UmbraMaxOpacity		 0.7
                            UmbraMinOpacity		 0.4
                        END LightingData

                        ShowDtedRegions		 Off

                    END Style

                    BEGIN Style
                        Name		 DefaultWithoutBing
                        Time		 7671600
                        UpdateDelta		 10

                        BEGIN MapAttributes
                            PrimaryBody		 Earth
                            SecondaryBody		 Sun
                            CenterLatitude		 0
                            CenterLongitude		 0
                            ProjectionAltitude		 63621860
                            FieldOfView		 35
                            OrthoDisplayDistance		 20000000
                            TransformTrajectory		 On
                            EquatorialRadius		 6378137
                            BackgroundColor		 #000000
                            LatLonLines		 On
                            LatSpacing		 30
                            LonSpacing		 30
                            LatLonLineColor		 #999999
                            LatLonLineStyle		 2
                            ShowOrthoDistGrid		 Off
                            OrthoGridXSpacing		 5
                            OrthoGridYSpacing		 5
                            OrthoGridColor		 #ffffff
                            ShowImageExtents		 Off
                            ImageExtentLineColor		 #ffffff
                            ImageExtentLineStyle		 0
                            ImageExtentLineWidth		 1
                            ShowImageNames		 Off
                            ImageNameFont		 0
                            Projection		 EquidistantCylindrical
                            Resolution		 VeryLow
                            CoordinateSys		 ECF
                            UseBackgroundImage		 On
                            UseBingForBackground		 Off
                            BingType		 Aerial
                            BingLogoHorizAlign		 Right
                            BingLogoVertAlign		 Bottom
                            BackgroundImageFile		 Basic.bmp
                            UseNightLights		 Off
                            NightLightsFactor		 3.5
                            UseCloudsFile		 Off
                            BEGIN ZoomLocations
                                BEGIN ZoomLocation
                                    CenterLat		 0
                                    CenterLon		 0
                                    ZoomWidth		 359.999998
                                    ZoomHeight		 180
                                END ZoomLocation
                            END ZoomLocations
                            UseVarAspectRatio		 No
                            SwapMapResolution		 Yes
                            NoneToVLowSwapDist		 2000000
                            VLowToLowSwapDist		 20000
                            LowToMediumSwapDist		 10000
                            MediumToHighSwapDist		 5000
                            HighToVHighSwapDist		 1000
                            VHighToSHighSwapDist		 100
                            BEGIN Axes
                                DisplayAxes		 no
                                CoordSys		 CBI
                                2aryCB		 Sun
                                Display+x		 yes
                                Label+x		 yes
                                Color+x		 #ffffff
                                Scale+x		 3
                                Display-x		 yes
                                Label-x		 yes
                                Color-x		 #ffffff
                                Scale-x		 3
                                Display+y		 yes
                                Label+y		 yes
                                Color+y		 #ffffff
                                Scale+y		 3
                                Display-y		 yes
                                Label-y		 yes
                                Color-y		 #ffffff
                                Scale-y		 3
                                Display+z		 yes
                                Label+z		 yes
                                Color+z		 #ffffff
                                Scale+z		 3
                                Display-z		 yes
                                Label-z		 yes
                                Color-z		 #ffffff
                                Scale-z		 3
                            END Axes

                        END MapAttributes

                        BEGIN MapList
                            BEGIN Detail
                                Alias		 RWDB2_Coastlines
                                Show		 Yes
                                Color		 #8fbc8f
                            END Detail
                            BEGIN Detail
                                Alias		 RWDB2_International_Borders
                                Show		 No
                                Color		 #8fbc8f
                            END Detail
                            BEGIN Detail
                                Alias		 RWDB2_Islands
                                Show		 No
                                Color		 #8fbc8f
                            END Detail
                            BEGIN Detail
                                Alias		 RWDB2_Lakes
                                Show		 No
                                Color		 #87cefa
                            END Detail
                            BEGIN Detail
                                Alias		 RWDB2_Provincial_Borders
                                Show		 No
                                Color		 #8fbc8f
                            END Detail
                            BEGIN Detail
                                Alias		 RWDB2_Rivers
                                Show		 No
                                Color		 #87cefa
                            END Detail
                        END MapList


                        BEGIN MapAnnotations
                        END MapAnnotations

                        BEGIN RecordMovie
                            OutputFormat		 VIDEO
                            SdfSelected		 No
                            BaseName		 Frame
                            Digits		 4
                            Frame		 0
                            LastAnimTime		 0
                            OutputMode		 Normal
                            HiResAssembly		 Assemble
                            HRWidth		 6000
                            HRHeight		 4500
                            HRDPI		 600
                            UseSnapInterval		 No
                            SnapInterval		 0
                            VideoCodec		 "H264"
                            Framerate		 30
                            Bitrate		 3000000
                        END RecordMovie


                        BEGIN TimeDisplay
                            Show		 0
                            TextColor		 #ffffff
                            TextTranslucency		 0
                            ShowBackground		 0
                            BackColor		 #4d4d4d
                            BackTranslucency		 0.4
                            XPosition		 20
                            YPosition		 -20
                        END TimeDisplay

                        BEGIN LightingData
                            DisplayAltitude		 0
                            SubsolarPoint		 Off
                            SubsolarPointColor		 #ffff00
                            SubsolarPointMarkerStyle		 2

                            ShowUmbraLine		 Off
                            UmbraLineColor		 #000000
                            UmbraLineStyle		 0
                            UmbraLineWidth		 2
                            FillUmbra		 On
                            UmbraFillColor		 #000000
                            ShowSunlightLine		 Off
                            SunlightLineColor		 #ffff00
                            SunlightLineStyle		 0
                            SunlightLineWidth		 2
                            FillSunlight		 On
                            SunlightFillColor		 #ffffff
                            SunlightMinOpacity		 0
                            SunlightMaxOpacity		 0.2
                            UmbraMaxOpacity		 0.7
                            UmbraMinOpacity		 0.4
                        END LightingData

                        ShowDtedRegions		 Off

                    END Style

                END MapStyles

            END MapData

            BEGIN GfxClassPref

            END GfxClassPref


            BEGIN ConnectGraphicsOptions

                AsyncPickReturnUnique		 OFF

            END ConnectGraphicsOptions

        END Graphics

        BEGIN Overlays
        END Overlays

        BEGIN VO
        END VO

    END Extensions

    BEGIN SubObjects

        Class Satellite

            Satellite1		

        END Class

    END SubObjects

    BEGIN References
        Instance *
            *		
        END Instance
        Instance Satellite/Satellite1
            Satellite/Satellite1		
        END Instance
    END References

END Scenario
