#ifndef Pt_AnyTraits_h
#define Pt_AnyTraits_h

#include <iosfwd>
#include <Pt/Api.h>


namespace Pt {

	template <typename T>
	struct AnyTraits {
		static void output(std::ostream& os, const T& value)
		{ os << value; }

		static void input(std::istream& is, T& value)
		{ is >> value; }
	};


	template <>
	struct PT_EXPORT AnyTraits<bool> {
		static void output(std::ostream& os, const bool& value);
		static void input(std::istream& is, bool& value);
	};


	template <>
	struct PT_EXPORT AnyTraits<int> {
		static void output(std::ostream& os, const int& value);
		static void input(std::istream& is, int& value);
	};


	template <>
	struct PT_EXPORT AnyTraits<float> {
		static void output(std::ostream& os, const float& value);
		static void input(std::istream& is, float& value);
	};


	template <>
	struct PT_EXPORT AnyTraits<double> {
		static void output(std::ostream& os, const double& value);
		static void input(std::istream& is, double& value);
	};
}


#endif
