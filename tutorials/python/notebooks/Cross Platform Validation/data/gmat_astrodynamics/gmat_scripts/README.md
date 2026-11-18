# GMAT reference scripts

These [GMAT](https://software.nasa.gov/software/GSC-17177-1) scripts generate the reference data that the cross platform validation notebooks compare OSTk against:

- `scenario001-mission-sequence.script` → `gmat_astrodynamics/scenario001-mission-sequence.csv`
- `scenario002-mission-sequence.script` → `gmat_astrodynamics/scenario002-mission-sequence.csv`
- `scenario003-mission-sequence.script` → `gmat_astrodynamics/scenario003-mission-sequence.csv`
- `scenario004-mission-sequence.script` → `gmat_astrodynamics/scenario004-mission-sequence.csv`
- `scenario005-mission-sequence.script` → `gmat_astrodynamics/scenario005-mission-sequence.csv`

Scenario 5 has no notebook yet. The notebooks read these CSV files from `data/gmat_astrodynamics/`, the parent of this directory.

## Where the report is written

Each script sets only a bare file name:

```
GMAT ReportFile1.Filename = 'scenario001-mission-sequence.csv';
```

GMAT writes a report file that has no directory in its name to `OUTPUT_PATH`, which is set in the GMAT startup file. The default is `<GMAT>/output/`. To regenerate the data in place, set `OUTPUT_PATH` to the `data/gmat_astrodynamics/` directory of your checkout:

1. Copy GMAT's default startup file and set `OUTPUT_PATH` to an absolute path, ending with a `/`. Keep the copy in GMAT's `bin` directory, because GMAT resolves relative paths in a startup file against the directory of the executable.
2. Run each script with `GmatConsole --startup_file <copy> --run <script>`.

From the root of this repository:

```shell
GMAT_BIN=/path/to/GMAT/bin  # contains GmatConsole and gmat_startup_file.txt
SCRIPTS_DIR="$PWD/tutorials/python/notebooks/Cross Platform Validation/data/gmat_astrodynamics/gmat_scripts"
OUTPUT_DIR="$(dirname "$SCRIPTS_DIR")/"  # data/gmat_astrodynamics/
STARTUP_FILE="$GMAT_BIN/gmat_startup_file.ostk.txt"

# LOG_FILE defaults to OUTPUT_PATH/GmatLog.txt; keep the log in GMAT's own output directory.
sed -e "s|^OUTPUT_PATH .*|OUTPUT_PATH = $OUTPUT_DIR|" \
    -e "s|^LOG_FILE .*|LOG_FILE = ROOT_PATH/output/GmatLog.txt|" \
    "$GMAT_BIN/gmat_startup_file.txt" > "$STARTUP_FILE"

for script in "$SCRIPTS_DIR"/*.script; do
    "$GMAT_BIN/GmatConsole" --startup_file "$STARTUP_FILE" --run "$script"
done
```

To write the reports somewhere else, set `OUTPUT_DIR` to any absolute directory ending with `/`. The notebook (`OSTk Cross Platform Validation (Mission Sequence Scenario N).ipynb`) reads only from `data/gmat_astrodynamics/`, so copy the CSV files there afterwards.
