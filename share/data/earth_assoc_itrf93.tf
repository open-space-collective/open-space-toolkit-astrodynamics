KPL/FK


   SPICE Earth Body-fixed Reference Frame/Body Association Kernel
   =====================================================================

   Original file name:                   earth_assoc_itrf93.tf
   Creation date:                        2007 April 02 16:21
   Created by:                           Nat Bachman  (NAIF/JPL)


   Overview
   =====================================================================

   In the SPICE system, the default body-fixed reference frame
   associated with the Earth is named

       IAU_EARTH

   The IAU_EARTH reference frame is implemented via approximate formulas
   provided by the IAU report [1] and is not suitable for high-accuracy
   work.

   This kernel directs the SPICE system to associate the IERS
   terrestrial reference frame

      ITRF93

   with the Earth. High-accuracy binary Earth PCK files, which 
   provide the orientation of the ITRF93 frame relative to 
   the ICRF are available from the NAIF web site:

      ftp://naif.jpl.nasa.gov/pub/naif/generic_kernels/pck

   When this kernel is loaded via FURNSH, the SPICE frame system
   routines CNMFRM and CIDFRM, which identify the reference frame
   associated with a specified body, will indicate that the ITRF93
   frame is associated with the Earth. In addition, higher-level SPICE
   geometry routines that rely on the CNMFRM or CIDFRM routines will
   use the ITRF93 frame where applicable. As of the release date of
   this kernel, these SPICE routines are:

      ET2LST
      ILLUM
      SRFXPT
      SUBPT
      SUBSOL

   Finally, any code that calls these routines to obtain results
   involving Earth body-fixed frames are affected. Within SPICE, the
   only higher-level system that is affected is the dynamic frame
   system.


   Using this kernel
   =====================================================================

   This kernel must be loaded together with a binary Earth PCK. Below
   an example meta-kernel that loads these files and a small program
   illustrating use of the kernel are shown. The names of the kernels
   used here are current as of the release date of this kernel, but
   should not be assumed to current at later dates.


      Example meta-kernel
      -------------------

      To use the meta-kernel shown below, the '@' characters must be
      replaced with backslash '\' characters.  Backslashes cannot be
      used in this comment block because they would confuse the SPICE
      text kernel parser.


         KPL/FK

         @begintext

             Kernels to load are:

                Earth association FK:      earth_assoc_itrf93.tf

                Earth binary PCK:          earth_000101_070620_070329.bpc

                Text PCK for Earth radii:  pck00008.tpc

                Leapseconds kernel (for
                time conversion):          naif0008.tls

                Planetary ephemeris (for
                sub-Earth computation):    de414.bsp

         @begindata

         KERNELS_TO_LOAD = ( 'earth_assoc_itrf93.tf'
                             'earth_000101_070620_070329.bpc'
                             'pck00008.tpc'
                             'naif0008.tls'         
                             'de414.bsp'                     )
         @begintext

         End of kernel



      Example code
      ------------

      Find the geometric (without light time and stellar aberration
      corrections) sub-Moon point on the Earth at a given UTC time,
      using the ITRF93 reference frame.  Display the name of the
      Earth body-fixed frame used for the computation.


             PROGRAM EX
             IMPLICIT NONE

             DOUBLE PRECISION      DPR

             INTEGER               FILEN 
             PARAMETER           ( FILEN  = 255 )

             INTEGER               FRNMLN
             PARAMETER           ( FRNMLN = 32 )

             INTEGER               TIMLEN
             PARAMETER           ( TIMLEN = 50 )

             CHARACTER*(FRNMLN)    FRNAME
             CHARACTER*(FILEN)     META
             CHARACTER*(TIMLEN)    TIMSTR

             DOUBLE PRECISION      ALT
             DOUBLE PRECISION      ET
             DOUBLE PRECISION      LAT
             DOUBLE PRECISION      LON
             DOUBLE PRECISION      RADIUS
             DOUBLE PRECISION      SPOINT ( 3 )

             INTEGER               FRCODE

             LOGICAL               FOUND

       C
       C     Obtain name of meta-kernel; load kernel.
       C
             CALL PROMPT ( 'Enter meta-kernel name > ', META   )
             CALL FURNSH ( META )

       C
       C     Obtain input time and convert to seconds past J2000 TDB.
       C
             CALL PROMPT ( 'Enter observation time > ', TIMSTR )
             CALL STR2ET ( TIMSTR, ET )

       C
       C     Find the closest point on the Earth to the center
       C     of the Moon at ET.
       C
             CALL SUBPT  ( 'Near point', 'EARTH', ET,  'NONE', 
            .              'MOON',       SPOINT,  ALT          )
            .               
       C
       C     Express the sub-observer point in latitudinal
       C     coordinates.
       C
             CALL RECLAT ( SPOINT, RADIUS, LON, LAT )

       C
       C     Look up the name of the Earth body-fixed frame.
       C     
             CALL CNMFRM ( 'EARTH', FRCODE, FRNAME, FOUND )

       C
       C     Always check the "found" flag.  Signal an error if we
       C     don't find a frame name.
       C
             IF ( .NOT. FOUND ) THEN
                CALL SETMSG ( 'No body-fixed frame found for the Earth.' )
                CALL SIGERR ( 'SPICE(NOFRAME)'                          )
             END IF

             WRITE(*,*) 'Earth body-fixed frame is ', FRNAME
             WRITE(*,*) 'Sub-Moon planetocentric longitude (deg):', 
            .            LON*DPR()
             WRITE(*,*) 'Sub-Moon planetocentric latitude  (deg):',
            .            LAT*DPR()
             END


      Example program output
      ----------------------

      Numeric results and output formatting shown below should be
      expected to differ somewhat across different computing platforms.

      When the above example program is run using the example meta-kernel,
      and the (arbitrary) date 2007 Apr 2 00:00:00 UTC is used
      as the observation time, the output will be:

         Earth body-fixed frame is ITRF93
         Sub-Moon planetocentric longitude (deg): -6.85794256
         Sub-Moon planetocentric latitude  (deg): -3.09099529


   References
   =====================================================================
   [1]   Seidelmann, P.K., Abalakin, V.K., Bursa, M., Davies, M.E.,
         Bergh, C. de, Lieske, J.H., Oberst, J., Simon, J.L.,
         Standish, E.M., Stooke, P., and Thomas, P.C. (2002).
         "Report of the IAU/IAG Working Group on Cartographic
         Coordinates and Rotational Elements of the Planets and
         Satellites: 2000," Celestial Mechanics and Dynamical
         Astronomy, v.82, Issue 1, pp. 83-111.



   Data
   =====================================================================

   The assignment below directs the SPICE system to associate the ITRF93
   reference frame with the Earth.

   For further information, see the Frames Required Reading section
   titled "Connecting an Object to its Body-fixed Frame."

   \begindata

      OBJECT_EARTH_FRAME =  'ITRF93'

   \begintext


   End of kernel
   =====================================================================


