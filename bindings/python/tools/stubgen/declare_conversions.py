# Apache License 2.0

"""Declare in the generated stubs the conversions that nanobind.stubgen cannot see.

Usage, after `python -m nanobind.stubgen` has written the `.pyi` files:

    python declare_conversions.py PACKAGE_DIRECTORY PACKAGE_NAME
    python declare_conversions.py build/.../ostk/mathematics ostk.mathematics

`implicitly_convertible<std::string, String>()` lets a function that takes a `String` also take
a `str`, but stubgen only writes `String`, so type checkers reject `parse("1, 2, 3, 4", ...)`.
The type casters cannot carry the extra names (`io_name`) for a bound class, because nanobind
then no longer recognises its constructors. So this script edits the stubs, following
CONVERSIONS:

    def parse(string: ostk.core.type.String) -> Quaternion: ...          # before
    def parse(string: ostk.core.type.String | str) -> Quaternion: ...    # after

    def get_instant(self) -> ostk.physics.time.DateTime: ...             # before
    def get_instant(self) -> datetime.datetime: ...                      # after
"""

from __future__ import annotations

import ast
import sys
from pathlib import Path

# Bound class: (types that its parameters also accept, type of its return values or None).
# For example `"ostk.physics.time.DateTime": (("datetime.datetime",), "datetime.datetime")`
# turns `DateTime` parameters into `DateTime | datetime.datetime`, and return values into
# `datetime.datetime`. Builtins are written without a module (`str`), everything else in full.
CONVERSIONS: dict[str, tuple[tuple[str, ...], str | None]] = {
    # implicitly_convertible<std::string, String> (core: Type/String.cpp)
    "ostk.core.type.String": (("str",), None),
    # implicitly_convertible<Integer::ValueType, Integer> (core: Type/Integer.cpp)
    "ostk.core.type.Integer": (("int",), None),
    # implicitly_convertible<Real::ValueType, Real> (core: Type/Real.cpp)
    "ostk.core.type.Real": (("float",), None),
    # implicitly_convertible between unit::Angle and geometry::Angle, both ways
    # (physics: Unit/Derived/Angle.cpp)
    "ostk.physics.unit.Angle": (("ostk.mathematics.geometry.Angle",), None),
    "ostk.mathematics.geometry.Angle": (("ostk.physics.unit.Angle",), None),
}

CALLABLES = {"typing.Callable", "collections.abc.Callable"}


class Stub:
    """One `.pyi` file, and the edits that declare its conversions.

    `module` is the module that the file describes (`ostk.mathematics.geometry`), `package` the
    package that its relative imports start from (`ostk.mathematics.geometry` for an
    `__init__.pyi`, `ostk.mathematics` otherwise).
    """

    def __init__(self, path: Path, module: str, package: str) -> None:
        self.module = module
        self.source = path.read_bytes()
        self.tree = ast.parse(self.source)
        self.lines = [0]  # Byte offset of the start of each line, to locate the edits.
        for line in self.source.splitlines(keepends=True):
            self.lines.append(self.lines[-1] + len(line))
        # stubgen writes names in full (`ostk.core.type.String`), except for the names that the
        # module defines or imports with `from ... import`. In `ostk/core/__init__.pyi`:
        #   class Logger: ...       ->  names["Logger"] = "ostk.core.Logger"
        #   from . import type      ->  names["type"] = "ostk.core.type"
        #   import ostk.core.type   ->  imports = {"ostk.core.type"}
        self.names: dict[str, str] = {}
        self.imports: set[str] = set()
        for node in self.tree.body:
            if isinstance(node, ast.Import):
                self.imports.update(alias.name for alias in node.names)
            elif isinstance(node, ast.ImportFrom):
                base = node.module
                if node.level:  # `from . import type`, `from .time import Instant`
                    parts = package.split(".")
                    parts = parts[: len(parts) - node.level + 1]
                    base = ".".join([*parts, node.module] if node.module else parts)
                for alias in node.names:
                    self.names[alias.asname or alias.name] = f"{base}.{alias.name}"
            elif isinstance(node, (ast.ClassDef, ast.FunctionDef)):
                self.names[node.name] = f"{module}.{node.name}"
            elif isinstance(node, (ast.Assign, ast.AnnAssign)):
                targets = (
                    node.targets if isinstance(node, ast.Assign) else [node.target]
                )
                for target in targets:
                    if isinstance(target, ast.Name):
                        self.names[target.id] = f"{module}.{target.id}"
        self.new_imports: set[str] = set()
        self.edits: list[tuple[int, int, str]] = []  # (start byte, end byte, new text)

    def name(self, node: ast.expr) -> str | None:
        """Return the qualified name that an annotation refers to, if it is a (dotted) name.

        In `ostk/core/__init__.pyi`: `type.String` -> `"ostk.core.type.String"`,
        `str` -> `"str"`, `list[str]` -> None (a subscript, not a name).
        """

        if isinstance(node, ast.Attribute):
            base = self.name(node.value)
            return base and f"{base}.{node.attr}"
        if isinstance(node, ast.Name):
            return self.names.get(node.id, node.id)
        return None

    def write(self, name: str) -> str:
        """Return how to write a qualified name in this file, and import its module if needed.

        `"str"` -> `str`; `"datetime.datetime"` -> `datetime.datetime`, adding `import datetime`;
        `"ostk.physics.unit.Angle"` -> `Angle` in the stub of `ostk.physics.unit` itself.
        """

        module, _, attribute = name.rpartition(".")
        if module == self.module:
            return attribute
        if module and module not in self.imports:
            self.new_imports.add(module)
        return name

    def visit(self, node: ast.expr, parameter: bool, union: set | None = None) -> None:
        """Record the edits of one annotation, looking inside containers, unions and callbacks.

        `parameter` tells whether the annotated value goes from Python to C++:

            name: String                         ->  name: String | str
            names: list[String]                  ->  names: list[String | str]
            name: String | str                   ->  unchanged (`str` is already in `union`)
            -> String                            ->  unchanged (no return type listed)
            -> DateTime                          ->  -> datetime.datetime
            callback: Callable[[DateTime], String]
                                                 ->  callback: Callable[[datetime.datetime], String | str]
        """

        name = self.name(node)
        if name in CONVERSIONS:
            accepts, returns = CONVERSIONS[name]
            union = union or set()
            start = self.lines[node.lineno - 1] + node.col_offset
            end = self.lines[node.end_lineno - 1] + node.end_col_offset
            if parameter and (
                added := [self.write(a) for a in accepts if a not in union]
            ):
                self.edits.append(
                    (start, end, " | ".join([self.source[start:end].decode(), *added]))
                )
            elif not parameter and returns and returns not in union:
                self.edits.append((start, end, self.write(returns)))
        elif name is not None:
            return  # Another name, such as `Angle.Unit`: never look inside it.
        elif isinstance(node, ast.Subscript) and self.name(node.value) in CALLABLES:
            # The arguments of a callback go from C++ to Python, its result the other way.
            arguments, result = node.slice.elts
            for argument in arguments.elts:
                self.visit(argument, not parameter)
            self.visit(result, parameter)
        elif isinstance(node, ast.BinOp) and isinstance(node.op, ast.BitOr):
            members = union_members(node)
            for member in members:
                self.visit(member, parameter, {self.name(m) for m in members})
        else:
            for child in ast.iter_child_nodes(node):
                if isinstance(child, ast.expr):
                    self.visit(child, parameter)

    def rewrite(self) -> bytes:
        """Return the file with the parameters and return values of every function edited.

        Sets `count` to the number of edited annotations, and warns about overloads that a
        widened parameter may shadow: a type checker takes the first overload that matches,
        while nanobind tries all of them without conversions first, so an overload taking
        `String | str` may catch a call meant for a later one. That only matters if they return
        different types.
        """

        overloads: dict[tuple[int, str], list[ast.FunctionDef]] = {}
        widened: set[tuple[int, str]] = set()

        for scope in [
            self.tree,
            *(n for n in ast.walk(self.tree) if isinstance(n, ast.ClassDef)),
        ]:
            for node in scope.body:
                if not isinstance(node, ast.FunctionDef):
                    continue
                count = len(self.edits)
                arguments = node.args
                for argument in [
                    *arguments.posonlyargs,
                    *arguments.args,
                    *arguments.kwonlyargs,
                    arguments.vararg,
                    arguments.kwarg,
                ]:
                    if argument and argument.annotation:
                        self.visit(argument.annotation, parameter=True)
                key = (id(scope), node.name)
                if len(self.edits) > count:
                    widened.add(key)
                if node.returns is not None:
                    self.visit(node.returns, parameter=False)
                if any(
                    ast.unparse(d).endswith("overload") for d in node.decorator_list
                ):
                    overloads.setdefault(key, []).append(node)

        for key, group in overloads.items():
            returns = {ast.unparse(n.returns) for n in group if n.returns}
            if key in widened and len(returns) > 1:
                print(f"warning: {self.module}: check the overloads of `{key[1]}`")

        output = self.source
        self.count = len(self.edits)
        if self.new_imports:
            imports = [
                n for n in self.tree.body if isinstance(n, (ast.Import, ast.ImportFrom))
            ]
            position = self.lines[max(n.end_lineno for n in imports)] if imports else 0
            text = "".join(f"import {module}\n" for module in sorted(self.new_imports))
            self.edits.append((position, position, text))
        for start, end, text in sorted(self.edits, reverse=True):
            output = output[:start] + text.encode() + output[end:]
        ast.parse(output)  # The rewritten stub must stay valid Python.
        return output


def union_members(node: ast.expr) -> list[ast.expr]:
    """Flatten a union: `A | B | C` -> `[A, B, C]`."""

    if isinstance(node, ast.BinOp) and isinstance(node.op, ast.BitOr):
        return union_members(node.left) + union_members(node.right)
    return [node]


def main() -> None:
    """Rewrite every binding stub under PACKAGE_DIRECTORY.

    `geometry/__init__.pyi` describes `ostk.mathematics.geometry`, `geometry/d3.pyi`
    `ostk.mathematics.geometry.d3`. Stubs of pure Python modules (a `.py` file next to the
    `.pyi`) are skipped, except for the top-level `__init__.pyi`.
    """

    directory, package = Path(sys.argv[1]), sys.argv[2]
    total = 0
    for path in sorted(directory.rglob("*.pyi")):
        if path.with_suffix(".py").exists() and path != directory / "__init__.pyi":
            continue
        parts = [package, *path.relative_to(directory).with_suffix("").parts]
        if parts[-1] == "__init__":
            stub = Stub(path, ".".join(parts[:-1]), package=".".join(parts[:-1]))
        else:
            stub = Stub(path, ".".join(parts), package=".".join(parts[:-1]))
        output = stub.rewrite()
        if stub.count:
            path.write_bytes(output)
            total += stub.count
    print(f"{package}: declared conversions on {total} annotations")


if __name__ == "__main__":
    main()
