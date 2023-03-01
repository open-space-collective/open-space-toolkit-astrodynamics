################################################################################################################################################################

# @project        Open Space Toolkit ▸ Astrodynamics
# @file           bindings/python/tools/python/ostk/astrodynamics/display.py
# @author         Remy Derollez <remy@loftorbital.com>
# @license        Apache License 2.0

################################################################################################################################################################

from typing import Any

import plotly.graph_objs as go

################################################################################################################################################################

DEFAULT_COLOR: str = 'rgba(255, 0, 0, 0.5)'

DEFAULT_LAYOUT_2D: go.Layout = go.Layout(
    title = None,
    showlegend = False,
    height = 1000,
    geo = go.layout.Geo(
        showland = True,
        landcolor = 'rgb(243, 243, 243)',
        countrycolor = 'rgb(204, 204, 204)',
        lonaxis = dict(
            showgrid = True,
            gridcolor = 'rgb(102, 102, 102)',
            gridwidth = 0.1,
        ),
        lataxis = dict(
            showgrid = True,
            gridcolor = 'rgb(102, 102, 102)',
            gridwidth = 0.1,
        ),
    ),
)

DEFAULT_LAYOUT_3D: go.Layout = go.Layout(
    title = None,
    showlegend = False,
    height = 1000,
    geo = go.layout.Geo(
        showland = True,
        showlakes = True,
        showcountries = False,
        showocean = True,
        countrywidth = 0.0,
        landcolor = 'rgb(100, 100, 100)',
        lakecolor = 'rgb(240, 240, 240)',
        oceancolor = 'rgb(240, 240, 240)',
        projection = dict(
            type = 'orthographic',
            rotation = dict(
                lon = 0,
                lat = 0,
                roll = 0,
            )
        ),
        lonaxis = dict(
            showgrid = True,
            gridcolor = 'rgb(102, 102, 102)',
            gridwidth = 0.5,
        ),
        lataxis = dict(
            showgrid = True,
            gridcolor = 'rgb(102, 102, 102)',
            gridwidth = 0.5,
        ),
    ),
)

################################################################################################################################################################

def convert_to_ground_track_plotting_data (longitude: list,
                                           latitude: list,
                                           color: str = DEFAULT_COLOR) -> go.Scattergeo:

    return go.Scattergeo(
        lon = longitude,
        lat = latitude,
        mode = 'lines',
        line = dict(
            width = 1,
            color = color,
        ),
    )

def create_plotly_figure (data: Any, layout: go.Layout) -> go.Figure:

    return go.Figure(
        data = data,
        layout = layout,
    )

def create_2d_map (data: Any) -> go.Figure:

    return create_plotly_figure(data, DEFAULT_LAYOUT_2D)

def create_3d_globe (data: Any) -> go.Figure:

    return create_plotly_figure(data, DEFAULT_LAYOUT_3D)

################################################################################################################################################################
