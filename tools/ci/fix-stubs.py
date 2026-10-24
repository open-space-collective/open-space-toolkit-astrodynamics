# Apache License 2.0

import pathlib
import sys

root = pathlib.Path(sys.argv[1])
for path in root.rglob("*.pyi"):
    path.write_text(path.read_text().replace(": ...", ": typing.Any"))
