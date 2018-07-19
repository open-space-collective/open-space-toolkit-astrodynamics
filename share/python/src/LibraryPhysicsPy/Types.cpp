////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// @project        Library/Astrodynamics
/// @file           Library/Astrodynamics/LibraryMathematicsPy/Types.hpp
/// @author         Lucas Brémond <lucas@loftorbital.com>
/// @license        TBD

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// #include <Library/Astrodynamics/Types.hpp>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// inline void 					LibraryMathematicsPy_Types_Integer					( )
// {

// 	using namespace boost::python ;

// 	using library::core::types::Integer ;
// 	using library::core::types::Real ;
// 	using library::core::types::String ;

// 	class_<Integer>("Integer", init<int>())

// 		.def(int_(self))

// 		.def(self == self)
// 		.def(self != self)
// 		.def(self < self)
// 		.def(self <= self)
// 		.def(self > self)
// 		.def(self >= self)

// 		.def(self + self)
// 		.def(self += self)
// 		.def(self - self)
// 		.def(self -= self)
// 		.def(self * self)
// 		.def(self *= self)
// 		.def(self / self)
// 		.def(self /= self)

// 		.def(self + int())
// 		.def(self += int())
// 		.def(self - int())
// 		.def(self -= int())
// 		.def(self * int())
// 		.def(self *= int())
// 		.def(self / int())
// 		.def(self /= int())

// 		.def(int() + self)
// 		.def(int() - self)
// 		.def(int() * self)
// 		.def(int() / self)

// 		.def("__str__", +[] (const library::core::types::Integer& anInteger) -> std::string { return anInteger.toString() ; })
// 		.def("__repr__", +[] (const library::core::types::Integer& anInteger) -> std::string { return anInteger.toString() ; })

// 		.def("isDefined", &Integer::isDefined)
// 		.def("isZero", &Integer::isZero)
// 		.def("isPositive", &Integer::isPositive)
// 		.def("isNegative", &Integer::isNegative)
// 		.def("isStrictlyPositive", &Integer::isStrictlyPositive)
// 		.def("isStrictlyNegative", &Integer::isStrictlyNegative)
// 		.def("isInfinity", &Integer::isInfinity)
// 		.def("isPositiveInfinity", &Integer::isPositiveInfinity)
// 		.def("isNegativeInfinity", &Integer::isNegativeInfinity)
// 		.def("getSign", &Integer::getSign)
// 		.def("toString", &Integer::toString)
// 		// .def("getObject", &Integer::getObject)

// 		.def("Undefined", &Integer::Undefined).staticmethod("Undefined")
// 		.def("Zero", &Integer::Zero).staticmethod("Zero")
// 		.def("PositiveInfinity", &Integer::PositiveInfinity).staticmethod("PositiveInfinity")
// 		.def("NegativeInfinity", &Integer::NegativeInfinity).staticmethod("NegativeInfinity")
// 		.def("String", &Integer::String).staticmethod("String")
// 		// .def("Object", &Integer::Object).staticmethod("Object")

// 		;

// 	implicitly_convertible<Integer, int>() ;
// 	implicitly_convertible<int, Integer>() ;

// }

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// inline void 					LibraryMathematicsPy_Types_Real						( )
// {

// 	using namespace boost::python ;

// 	using library::core::types::Integer ;
// 	using library::core::types::Real ;
// 	using library::core::types::String ;

// 	class_<Real>("Real", init<double>())

// 		.def(float_(self))

// 		.def(self == self)
// 		.def(self != self)
// 		.def(self < self)
// 		.def(self <= self)
// 		.def(self > self)
// 		.def(self >= self)

// 		.def(self + self)
// 		.def(self += self)
// 		.def(self - self)
// 		.def(self -= self)
// 		.def(self * self)
// 		.def(self *= self)
// 		.def(self / self)
// 		.def(self /= self)

// 		.def(self + double())
// 		.def(self += double())
// 		.def(self - double())
// 		.def(self -= double())
// 		.def(self * double())
// 		.def(self *= double())
// 		.def(self / double())
// 		.def(self /= double())

// 		.def(double() + self)
// 		.def(double() - self)
// 		.def(double() * self)
// 		.def(double() / self)

// 		.def("__str__", +[] (const library::core::types::Real& aReal) -> std::string { return aReal.toString() ; })
// 		.def("__repr__", +[] (const library::core::types::Real& aReal) -> std::string { return aReal.toString() ; })

// 		.def("isDefined", &Real::isDefined)
// 		.def("isZero", &Real::isZero)
// 		.def("isPositive", &Real::isPositive)
// 		.def("isNegative", &Real::isNegative)
// 		.def("isStrictlyPositive", &Real::isStrictlyPositive)
// 		.def("isStrictlyNegative", &Real::isStrictlyNegative)
// 		.def("isInfinity", &Real::isInfinity)
// 		.def("isPositiveInfinity", &Real::isPositiveInfinity)
// 		.def("isNegativeInfinity", &Real::isNegativeInfinity)
// 		.def("getSign", &Real::getSign)
// 		.def("toString", &Real::toString)
// 		// .def("getObject", &Real::getObject)

// 		.def("Undefined", &Real::Undefined)
// 		.staticmethod("Undefined")
// 		.def("Zero", &Real::Zero)
// 		.staticmethod("Zero")
// 		.def("PositiveInfinity", &Real::PositiveInfinity)
// 		.staticmethod("PositiveInfinity")
// 		.def("NegativeInfinity", &Real::NegativeInfinity)
// 		.staticmethod("NegativeInfinity")
// 		.def("String", &Real::String)
// 		.staticmethod("String")
// 		// .def("Object", &Real::Object).staticmethod("Object")

// 		;

// 	implicitly_convertible<Real, double>() ;
// 	implicitly_convertible<double, Real>() ;

// }

// ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// // http://www.boost.org/doc/libs/1_66_0/libs/python/doc/html/faq/how_can_i_automatically_convert_.html

// inline void 					LibraryMathematicsPy_Types_String					( )
// {

// 	using namespace boost::python ;

// 	using library::core::types::Integer ;
// 	using library::core::types::Real ;
// 	using library::core::types::String ;

// 	class_<String>("String", init<std::string>())

// 		.def(self == self)
// 		.def(self != self)

// 		.def(self + self)
// 		.def(self += self)

// 		// .def(self + str())
// 		// .def(self += string())

// 		// .def(string() + self)

// 		.def("__str__", +[] (const library::core::types::String& aString) -> std::string { return aString ; })
// 		.def("__repr__", +[] (const library::core::types::String& aString) -> std::string { return aString ; })

// 		.def("isEmpty", &String::isEmpty)
//         .def("isUppercase", &String::isUppercase)
//         .def("isLowercase", &String::isLowercase)
//         .def("getLength", &String::getLength)

//         .def("Empty", &String::Empty).staticmethod("Empty")

// 		;

// 	implicitly_convertible<std::string, String>() ;
// 	implicitly_convertible<String, std::string>() ;

// 	// implicitly_convertible<str, String>() ;
// 	// implicitly_convertible<String, str>() ;

// }

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

inline void						LibraryMathematicsPy_Types						( )
{
	
	boost::python::object module(boost::python::handle<>(boost::python::borrowed(PyImport_AddModule("Library.Core.Types")))) ;
	
	boost::python::scope().attr("Types") = module ;
	
	boost::python::scope scope = module ;
	
	// LibraryMathematicsPy_Types_Integer() ;
	// LibraryMathematicsPy_Types_Real() ;
	// LibraryMathematicsPy_Types_String() ;

}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////