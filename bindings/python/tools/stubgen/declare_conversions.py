# Apache License 2.0

"""Declare in the generated stubs the conversions that nanobind.stubgen cannot see.

    python declare_conversions.py PACKAGE_DIRECTORY PACKAGE_NAME

`implicitly_convertible<Source, Target>()` makes every bound function that takes a `Target`
also accept a `Source`, and a custom type caster can accept or return other Python types than
the bound class it is named after. nanobind names such parameters and return values after the
bound class, and nanobind.stubgen cannot see conversions, so the generated stubs reject calls
that the bindings accept.

The type casters cannot carry these names (`io_name`) for a bound class: nanobind recognises the
constructors of a class (`__init__`, `__setstate__`) only when their `self` argument is named
with the class placeholder, `const_name<T>()`. So this script rewrites the stubs that
nanobind.stubgen wrote to PACKAGE_DIRECTORY, after it ran, according to CONVERSIONS:

- A parameter of one of these types also accepts the listed Python types: `String` becomes
  `String | str`, also inside containers (`Sequence[String | str]`).
- A return value of one of these types becomes the listed Python type, if one is listed:
  `DateTime` becomes `datetime.datetime`.
- The arguments of a callback (`Callable[[...], ...]`) are values that the bindings pass to
  Python, so they are treated as return values, and the callback's return value as a parameter.

Default values, docstrings and the stubs of pure Python modules (a `.py` source next to the
`.pyi` file) are left unchanged.
"""

from __future__ import annotations

import ast
import sys
from dataclasses import dataclass
from pathlib import Path


@dataclass(frozen=True)
class Conversion:
    # Fully qualified Python types that parameters also accept.
    accepts: tuple[str, ...]
    # Fully qualified Python type of return values, if it is not the bound class.
    returns: str | None = None


# Conversions of the bound classes that the functions of this package take or return.
CONVERSIONS: dict[str, Conversion] = {
    # implicitly_convertible<std::string, String> (core: Type/String.cpp)
    "ostk.core.type.String": Conversion(accepts=("builtins.str",)),
    # implicitly_convertible<Integer::ValueType, Integer> (core: Type/Integer.cpp)
    "ostk.core.type.Integer": Conversion(accepts=("builtins.int",)),
    # implicitly_convertible<Real::ValueType, Real> (core: Type/Real.cpp)
    "ostk.core.type.Real": Conversion(accepts=("builtins.float",)),
    # implicitly_convertible between unit::Angle and geometry::Angle, both ways
    # (physics: Unit/Derived/Angle.cpp)
    "ostk.physics.unit.Angle": Conversion(accepts=("ostk.mathematics.geometry.Angle",)),
    "ostk.mathematics.geometry.Angle": Conversion(accepts=("ostk.physics.unit.Angle",)),
}

PARAMETER = "parameter"
RETURN = "return"
CALLABLES = {"collections.abc.Callable", "typing.Callable"}


class Module:
    """The names bound in a stub module, to resolve annotations and to write new ones."""

    def __init__(self, tree: ast.Module, name: str, is_package: bool) -> None:
        self.name = name
        self.tree = tree
        self.names: dict[str, str] = {}
        self.imports: set[str] = set()

        package = name if is_package else name.rpartition(".")[0]

        for node in tree.body:
            if isinstance(node, ast.Import):
                for alias in node.names:
                    self.imports.add(alias.name)
                    if alias.asname:
                        self.names[alias.asname] = alias.name
                    else:
                        head = alias.name.split(".")[0]
                        self.names[head] = head
            elif isinstance(node, ast.ImportFrom):
                base = node.module or ""
                if node.level:
                    parts = package.split(".")
                    parts = parts[: len(parts) - (node.level - 1)]
                    base = ".".join(parts + ([node.module] if node.module else []))
                for alias in node.names:
                    self.names[alias.asname or alias.name] = f"{base}.{alias.name}"
            else:
                for local in defined_names(node):
                    self.names[local] = f"{name}.{local}"

    def resolve(self, node: ast.expr, scope: dict[str, str]) -> str | None:
        """Return the fully qualified name that `node` refers to, if it is a (dotted) name."""

        if isinstance(node, ast.Name):
            if node.id in scope:
                return scope[node.id]
            return self.names.get(node.id, f"builtins.{node.id}")
        if isinstance(node, ast.Attribute):
            base = self.resolve(node.value, scope)
            return None if base is None else f"{base}.{node.attr}"
        return None

    def render(self, qualified_name: str, scope: dict[str, str]) -> str:
        """Return how to write `qualified_name` in this module, importing its module if needed."""

        module, _, attribute = qualified_name.rpartition(".")

        if module == "builtins":
            if attribute in scope or attribute in self.names:
                raise ValueError(
                    f"{self.name}: `{attribute}` is shadowed, cannot name {qualified_name}"
                )
            return attribute

        for local, target in self.names.items():
            if target == qualified_name and local not in scope:
                return local

        head = module.split(".")[0]
        if head in scope or self.names.get(head, head) != head:
            raise ValueError(
                f"{self.name}: `{head}` is shadowed, cannot name {qualified_name}"
            )
        if module != self.name:
            self.imports.add(module)
        return qualified_name


def defined_names(node: ast.stmt) -> list[str]:
    if isinstance(node, (ast.ClassDef, ast.FunctionDef, ast.AsyncFunctionDef)):
        return [node.name]
    if isinstance(node, ast.AnnAssign) and isinstance(node.target, ast.Name):
        return [node.target.id]
    if isinstance(node, ast.Assign):
        return [target.id for target in node.targets if isinstance(target, ast.Name)]
    return []


class Rewriter:
    """Collect the edits to the annotations of one stub module."""

    def __init__(self, module: Module, source: bytes) -> None:
        self.module = module
        self.source = source
        self.line_offsets = [0]
        for line in source.splitlines(keepends=True):
            self.line_offsets.append(self.line_offsets[-1] + len(line))
        self.edits: list[tuple[int, int, str]] = []
        self.counts = {PARAMETER: 0, RETURN: 0}
        self.warnings: list[str] = []

    def offsets(self, node: ast.expr) -> tuple[int, int]:
        start = self.line_offsets[node.lineno - 1] + node.col_offset
        end = self.line_offsets[node.end_lineno - 1] + node.end_col_offset
        return start, end

    def run(self) -> None:
        self.visit_body(self.module.tree.body, scope={})

    def visit_body(self, body: list[ast.stmt], scope: dict[str, str]) -> None:
        self.check_overloads(body)
        for node in body:
            if isinstance(node, ast.ClassDef):
                qualified = scope.get(node.name, f"{self.module.name}.{node.name}")
                class_scope = {
                    local: f"{qualified}.{local}"
                    for child in node.body
                    for local in defined_names(child)
                }
                self.visit_body(node.body, class_scope)
            elif isinstance(node, (ast.FunctionDef, ast.AsyncFunctionDef)):
                arguments = node.args
                for argument in (
                    arguments.posonlyargs
                    + arguments.args
                    + arguments.kwonlyargs
                    + [arguments.vararg, arguments.kwarg]
                ):
                    if argument is not None and argument.annotation is not None:
                        self.visit(argument.annotation, PARAMETER, scope)
                if node.returns is not None:
                    self.visit(node.returns, RETURN, scope)

    def check_overloads(self, body: list[ast.stmt]) -> None:
        """Warn about overloads that a widened parameter could shadow.

        nanobind tries every overload without implicit conversions before trying them with
        conversions, while a type checker takes the first overload that matches. Once a
        parameter is widened (`String | str`), an overload can match a call that a later one
        handles at runtime. That only matters if the overloads return different types.
        """

        overloads: dict[str, list[ast.FunctionDef]] = {}
        for node in body:
            if isinstance(node, ast.FunctionDef) and any(
                ast.unparse(decorator) in ("overload", "typing.overload")
                for decorator in node.decorator_list
            ):
                overloads.setdefault(node.name, []).append(node)

        for name, group in overloads.items():
            returns = {unparse(node.returns) for node in group}
            if len(returns) > 1 and any(self.has_conversion(node) for node in group):
                self.warnings.append(
                    f"{self.module.name}: the overloads of `{name}` return different types "
                    f"({', '.join(sorted(returns))}): check that a widened parameter does not "
                    "make an overload match calls that a later overload handles"
                )

    def has_conversion(self, node: ast.FunctionDef) -> bool:
        arguments = node.args
        for argument in arguments.posonlyargs + arguments.args + arguments.kwonlyargs:
            if argument.annotation is not None:
                for child in ast.walk(argument.annotation):
                    if (
                        isinstance(child, ast.expr)
                        and self.module.resolve(child, {}) in CONVERSIONS
                    ):
                        return True
        return False

    def visit(
        self,
        node: ast.expr,
        polarity: str,
        scope: dict[str, str],
        members: frozenset[str | None] = frozenset(),
    ) -> None:
        """Visit an annotation. `members` are the other types of the union that holds it."""

        qualified = self.module.resolve(node, scope)

        if qualified in CONVERSIONS:
            self.convert(node, CONVERSIONS[qualified], polarity, scope, members)
            return

        if qualified is not None:
            return  # Another (dotted) name, such as `Angle.Unit`: never look inside it.

        if isinstance(node, ast.Subscript):
            if self.module.resolve(node.value, scope) in CALLABLES:
                self.visit_callable(node.slice, polarity, scope)
            else:
                self.visit(node.slice, polarity, scope)
            return

        if isinstance(node, ast.BinOp) and isinstance(node.op, ast.BitOr):
            union = union_members(node)
            members = frozenset(self.module.resolve(member, scope) for member in union)
            for member in union:
                self.visit(member, polarity, scope, members)
            return

        for child in ast.iter_child_nodes(node):
            if isinstance(child, ast.expr):
                self.visit(child, polarity, scope)

    def visit_callable(
        self, node: ast.expr, polarity: str, scope: dict[str, str]
    ) -> None:
        if not (isinstance(node, ast.Tuple) and len(node.elts) == 2):
            return
        arguments, result = node.elts
        if isinstance(arguments, ast.List):
            flipped = RETURN if polarity == PARAMETER else PARAMETER
            for argument in arguments.elts:
                self.visit(argument, flipped, scope)
        self.visit(result, polarity, scope)

    def convert(
        self,
        node: ast.expr,
        conversion: Conversion,
        polarity: str,
        scope: dict[str, str],
        members: frozenset[str | None],
    ) -> None:
        start, end = self.offsets(node)
        text = self.source[start:end].decode()

        if polarity == PARAMETER:
            accepted = [
                self.module.render(name, scope)
                for name in conversion.accepts
                if name not in members
            ]
            if accepted:
                self.edits.append((start, end, " | ".join([text, *accepted])))
                self.counts[PARAMETER] += 1
        elif conversion.returns is not None and conversion.returns not in members:
            self.edits.append((start, end, self.module.render(conversion.returns, scope)))
            self.counts[RETURN] += 1

    def result(self, imports_before: set[str]) -> bytes:
        output = self.source
        edits = sorted(self.edits, reverse=True)

        new_imports = sorted(self.module.imports - imports_before)
        if new_imports:
            position = self.import_position()
            text = "".join(f"import {name}\n" for name in new_imports)
            edits.append((position, position, text))
            edits.sort(reverse=True)

        for start, end, text in edits:
            output = output[:start] + text.encode() + output[end:]
        return output

    def import_position(self) -> int:
        body = self.module.tree.body
        imports = [
            node for node in body if isinstance(node, (ast.Import, ast.ImportFrom))
        ]
        if imports:
            return self.line_offsets[max(node.end_lineno for node in imports)]
        if (
            body
            and isinstance(body[0], ast.Expr)
            and isinstance(body[0].value, ast.Constant)
        ):
            return self.line_offsets[body[0].end_lineno]
        return 0


def unparse(node: ast.expr | None) -> str:
    return "" if node is None else ast.unparse(node)


def union_members(node: ast.expr) -> list[ast.expr]:
    if isinstance(node, ast.BinOp) and isinstance(node.op, ast.BitOr):
        return union_members(node.left) + union_members(node.right)
    return [node]


def is_binding_stub(path: Path, package_directory: Path) -> bool:
    """A stub of a pure Python module has its `.py` source next to it."""

    if path == package_directory / "__init__.pyi":
        return True
    return not path.with_suffix(".py").exists()


def declare_conversions(
    path: Path, package_directory: Path, package_name: str
) -> dict[str, int]:
    relative = path.relative_to(package_directory).with_suffix("")
    is_package = relative.name == "__init__"
    parts = (
        [package_name, *relative.parts[:-1]]
        if is_package
        else [package_name, *relative.parts]
    )
    source = path.read_bytes()
    tree = ast.parse(source)

    module = Module(tree, ".".join(parts), is_package)
    imports_before = set(module.imports)
    rewriter = Rewriter(module, source)
    rewriter.run()

    if rewriter.edits:
        output = rewriter.result(imports_before)
        ast.parse(output)  # The rewritten stub must stay valid Python.
        path.write_bytes(output)

    for warning in rewriter.warnings:
        print(f"warning: {warning}")

    return rewriter.counts


def main() -> None:
    package_directory, package_name = Path(sys.argv[1]), sys.argv[2]
    totals = {PARAMETER: 0, RETURN: 0}

    for path in sorted(package_directory.rglob("*.pyi")):
        if not is_binding_stub(path, package_directory):
            continue
        counts = declare_conversions(path, package_directory, package_name)
        for key, count in counts.items():
            totals[key] += count

    print(
        f"{package_name}: declared conversions on {totals[PARAMETER]} parameters "
        f"and {totals[RETURN]} return values"
    )


if __name__ == "__main__":
    main()
