# Apache License 2.0

from typing import Any

import pandas as pd

import plotly.graph_objs as go

from ostk.mathematics.objects import RealInterval

from ostk.physics.coordinate import Position
from ostk.physics.coordinate import Frame
from ostk.physics.coordinate.spherical import LLA
from ostk.physics.environment.objects import Celestial
from ostk.physics.time import Duration
from ostk.physics.time import Instant
from ostk.physics.units import Length

from ostk.astrodynamics import Access
from ostk.astrodynamics import Trajectory


DEFAULT_COLOR: str = "rgba(255, 0, 0, 0.5)"

DEFAULT_LAYOUT_2D: go.Layout = go.Layout(
    title=None,
    showlegend=False,
    height=1000,
    geo=go.layout.Geo(
        showland=True,
        landcolor="rgb(243, 243, 243)",
        countrycolor="rgb(204, 204, 204)",
        lonaxis=dict(
            showgrid=True,
            gridcolor="rgb(102, 102, 102)",
            gridwidth=0.1,
        ),
        lataxis=dict(
            showgrid=True,
            gridcolor="rgb(102, 102, 102)",
            gridwidth=0.1,
        ),
    ),
)

DEFAULT_LAYOUT_3D: go.Layout = go.Layout(
    title=None,
    showlegend=False,
    height=1000,
    geo=go.layout.Geo(
        showland=True,
        showlakes=True,
        showcountries=False,
        showocean=True,
        countrywidth=0.0,
        landcolor="rgb(100, 100, 100)",
        lakecolor="rgb(240, 240, 240)",
        oceancolor="rgb(240, 240, 240)",
        projection=dict(
            type="orthographic",
            rotation=dict(
                lon=0,
                lat=0,
                roll=0,
            ),
        ),
        lonaxis=dict(
            showgrid=True,
            gridcolor="rgb(102, 102, 102)",
            gridwidth=0.5,
        ),
        lataxis=dict(
            showgrid=True,
            gridcolor="rgb(102, 102, 102)",
            gridwidth=0.5,
        ),
    ),
)


def convert_to_ground_track_plotting_data(
    longitude: list, latitude: list, color: str = DEFAULT_COLOR
) -> go.Scattergeo:
    return go.Scattergeo(
        lon=longitude,
        lat=latitude,
        mode="lines",
        line=dict(
            width=1,
            color=color,
        ),
    )


def create_plotly_figure(data: Any, layout: go.Layout) -> go.Figure:
    return go.Figure(
        data=data,
        layout=layout,
    )


def create_2d_map(data: Any) -> go.Figure:
    return create_plotly_figure(data, DEFAULT_LAYOUT_2D)


def create_3d_globe(data: Any) -> go.Figure:
    return create_plotly_figure(data, DEFAULT_LAYOUT_3D)


class AccessesPlot:
    """
    Accesses plot. Display a 2D world map, with 1 ground station and
    multiple satellites, highlighting the accesses.
    """

    def __init__(
        self,
        earth: Celestial,
        interval: RealInterval,
        trajectory_step: Duration,
        access_step: Duration,
        ground_station_lla: LLA,
        color: str,
    ):
        self._earth = earth
        self._trajectory_grid: list[Instant] = interval.generate_grid(trajectory_step)
        self._access_step: Instant = access_step
        self._ned_frame: Frame = self._earth.get_frame_at(
            ground_station_lla, self._earth.FrameType.NED
        )
        self._ground_station_position_ned: Position = Position(
            [0.0, 0.0, 0.0],
            Length.Unit.Meter,
            self._ned_frame,
        )
        self._data = []
        self._data.append(
            dict(
                type="scattergeo",
                lon=[float(ground_station_lla.get_longitude().in_degrees())],
                lat=[float(ground_station_lla.get_latitude().in_degrees())],
                mode="markers",
                marker=dict(
                    size=10,
                    color=color,
                ),
            )
        )

    def _generate_and_append_data(
        self, data: list[list[float]], trajectory: Trajectory, grid: list[Instant]
    ) -> None:
        for state in trajectory.get_states_at(grid):
            lla: LLA = LLA.cartesian(
                state.get_position()
                .in_frame(Frame.ITRF(), state.get_instant())
                .get_coordinates(),
                self._earth.get_equatorial_radius(),
                self._earth.get_flattening(),
            )
            data.append(
                [
                    float(lla.get_longitude().in_degrees()),
                    float(lla.get_latitude().in_degrees()),
                ]
            )

    def _append_line(
        self, df: pd.DataFrame, width: int, rgb: list[int], alpha: float
    ) -> None:
        self._data.append(
            dict(
                type="scattergeo",
                lon=df["Longitude"],
                lat=df["Latitude"],
                mode="lines",
                line=dict(
                    width=width,
                    color=f"rgba({str(rgb[0])},{str(rgb[1])},{str(rgb[2])},{str(alpha)})",
                ),
            )
        )

    def add_satellite(
        self, trajectory: Trajectory, accesses: list[Access], rgb: list[int]
    ) -> None:
        """
        Add a satellite trajectory to the plot, including a highligh ot the interference
        accesses.
        """

        # Satellite trajectory
        satellite_trajectory_data: list[list[float]] = []
        self._generate_and_append_data(
            satellite_trajectory_data, trajectory, self._trajectory_grid
        )
        satellite_trajectory_df: pd.Dataframe = pd.DataFrame(
            satellite_trajectory_data, columns=["Longitude", "Latitude"]
        )
        self._append_line(satellite_trajectory_df, 1, rgb, 0.1)

        # Satellite accesses
        for access in accesses:
            satellite_access_data: list[list[float]] = []
            self._generate_and_append_data(
                satellite_access_data,
                trajectory,
                access.get_interval().generate_grid(self._access_step),
            )
            satellite_access_df: pd.Dataframe = pd.DataFrame(
                satellite_access_data, columns=["Longitude", "Latitude"]
            )
            self._append_line(satellite_access_df, 2, rgb, 1.0)

    def show(self):
        """
        Display the figure.
        """
        figure = create_2d_map(data=self._data)
        figure.show()
