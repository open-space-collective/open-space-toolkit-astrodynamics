# How to install the toolkit

## Python

Install the package from [PyPI](https://pypi.org/project/open-space-toolkit-astrodynamics/):

```bash
pip install open-space-toolkit-astrodynamics
```

Verify the installation:

```bash
python -c "from ostk.astrodynamics.trajectory import Orbit; print(Orbit)"
```

## C++

Binary packages are published on
[GitHub Releases](https://github.com/open-space-collective/open-space-toolkit-astrodynamics/releases):

- Runtime libraries: `open-space-toolkit-astrodynamics-X.Y.Z-1.x86_64-runtime`
- C++ headers: `open-space-toolkit-astrodynamics-X.Y.Z-1.x86_64-devel`
- Python bindings: `open-space-toolkit-astrodynamics-X.Y.Z-1.x86_64-python`

### Debian / Ubuntu

After downloading the relevant `.deb` packages, install them with:

```bash
apt install ./open-space-toolkit-astrodynamics-*.deb
```

## Run without installing (Docker)

If [Docker](https://www.docker.com/) is available, you can try the toolkit
without installing anything locally.

Start an IPython shell with the toolkit already available:

```bash
docker run -it openspacecollective/open-space-toolkit-astrodynamics-development python3.11 -m IPython
```

Or launch a JupyterLab server with the example notebooks:

```bash
docker run --publish=8888:8888 openspacecollective/open-space-toolkit-astrodynamics-jupyter
```

Then open <http://localhost:8888/lab>.

```{seealso}
To build from source, see the
[contributing guide](https://github.com/open-space-collective/open-space-toolkit-astrodynamics/blob/main/CONTRIBUTING.md).
```
