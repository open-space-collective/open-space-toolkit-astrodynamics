# coding=utf-8

################################################################################################################################################################

# @project        Library/Astrodynamics
# @file           Library/Astrodynamics/LibraryMathematicsPy/Types.test.py
# @author         Lucas Brémond <lucas@loftorbital.com>
# @license        TBD

################################################################################################################################################################

# Types

from LibraryMathematicsPy import Types

## Integer

assert Types.Integer(0) == 0
assert Types.Integer(123) == 123
assert Types.Integer(+123) == +123
assert Types.Integer(-123) == -123

assert Types.Integer(0).toString() == "0"
assert Types.Integer(123).toString() == "123"
assert Types.Integer(+123).toString() == "123"
assert Types.Integer(-123).toString() == "-123"

## Real

################################################################################################################################################################