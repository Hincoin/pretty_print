
#include <vector>
#include <type_traits>
#include <utility>
#include <unordered_set>
#include <iterator>
#include <unordered_map>
#include <functional>
#include <list>
#include <forward_list>
#include <string>
#include <deque>
#include <queue>
#include <map>
#include <iostream>
#include <tuple>
#include <set>




using std::begin;
using std::end;



namespace prettyprint
{
	template<class...>
	struct voider{ using type = void; };
	template<class ... args>
	using void_t = typename voider<args...>::type;
	struct FalseType 
	{
	public:
		using type = std::false_type;
		enum
		{
			value = false
		};
	};
	struct TrueType
	{
	public:
		using type = std::true_type;
		enum
		{
			value = true
		};
	};

	template<typename... args>
	struct _Or;

	template<typename... others>
	struct _Or<std::true_type, others...> : public TrueType{};

	template<typename ... others>
	struct _Or<std::false_type,others...>
	{
		using type = typename _Or<others...>::type;
	};

	template<>
	struct _Or<std::false_type> : public FalseType{};




}


template<typename T>
struct is_container
{

private:
	
	template<typename U>

	static std::true_type test(typename U::const_iterator *);
	template<typename U>

	static std::false_type test(...);

	template<typename U>
	static U makeU();

	template<typename U, class = void>
	struct CheckBeginEnd : public prettyprint::FalseType{};

	template<typename U>
	struct CheckBeginEnd<U, prettyprint::void_t<decltype(begin(std::declval<U&>())), decltype(end(std::declval<U&>()))>> 
	{
		using type = typename std::is_same<decltype(begin(std::declval<U&>())),decltype(end(std::declval<U&>()))>::type;

	};

	


public:
	using type =  typename prettyprint::_Or< decltype(test<T>(nullptr)),typename CheckBeginEnd<T>::type>::type;
};

template<typename Char, typename Traits, typename Alloc>
struct is_container< std::basic_string<Char, Traits, Alloc> >
{
	using type = std::false_type;
};

template<typename Char, typename Traits, typename Alloc>
struct is_container<const std::basic_string<Char, Traits, Alloc> >
{
	using type = std::false_type;
};

template<typename T, std::size_t N>
struct is_container<T[N]> {
	using type = std::true_type;
};

template<typename ... Types>
struct is_container<std::tuple<Types...>>
{
	using type = std::true_type;
};

template<class First, class Last>
struct is_container<std::pair<First, Last>>
{
	using type = std::true_type;
};



class default_formatting
{

protected:

	

	template<typename T, class = void>
	struct has_hash : public prettyprint::FalseType{ };

	template<typename T>
	struct has_hash<T, prettyprint::void_t<typename T::hasher>> : public prettyprint::TrueType{ };

	template<typename T, class = void>
	struct has_key_compare : public prettyprint::FalseType{};
	template<typename T>
	struct has_key_compare<T, prettyprint::void_t<typename T::key_compare>> : public prettyprint::TrueType{};

	template<typename T, class = void>
	struct has_const_iter : public prettyprint::FalseType{};

	template<typename T, class = void>
	struct has_underlying_const_iter : public prettyprint::FalseType{};

	template<typename T>
	struct has_underlying_const_iter<T, prettyprint::void_t<typename T::container_type::const_iterator>> : public prettyprint::TrueType{};

	template<typename T, class = void>
	struct has_map_types : public prettyprint::FalseType{};

	template<typename T>
	struct has_map_types<T, prettyprint::void_t<typename T::key_type, typename T::mapped_type>> : public prettyprint::TrueType{};

	template<typename T>
	struct has_const_iter<T, prettyprint::void_t<typename T::const_iterator>> : public prettyprint::TrueType{};

	template<typename T, class = void>
	struct has_pair_types : public prettyprint::FalseType{ using type = std::false_type; };

	template<typename T>
	struct has_pair_types<T, prettyprint::void_t<typename T::first_type, typename T::second_type>> : public prettyprint::TrueType{};


	struct ArbitraryType{ };
	struct SetType{ };
	struct TupleType{ };
	struct PairType{};
	struct MapType { };

	template<typename T>
	struct TypeCategorizer
	{
		
		using category =
			std::conditional_t< has_map_types<T>::type::value, MapType,
			std::conditional_t< has_hash<T>::type::value, SetType,
			std::conditional_t< has_key_compare<T>::type::value,SetType,
			std::conditional_t< has_const_iter<T>::type::value, ArbitraryType,
			std::conditional_t< has_pair_types<T>::type::value, PairType, TupleType>>>>>;

	};

	template<typename T, std::size_t N>
	struct TypeCategorizer<T[N]>
	{
		using category = ArbitraryType;
	};
	void prefix_impl(std::ostream& os, ArbitraryType) const 
	{
		os << "[";
	}

	void separator_impl(std::ostream& os, ArbitraryType) const
	{
		os << ", ";
	}

	void suffix_impl(std::ostream& os, ArbitraryType) const
	{

		os << "]";
	}

	void prefix_impl(std::ostream& os, PairType) const
	{
		os << "(";
	}
 
	void separator_impl(std::ostream& os, PairType) const
	{
		os << ", ";
	}

	void suffix_impl(std::ostream& os, PairType) const
	{
		os << ")";
	}


 
	void prefix_impl(std::ostream& os, SetType) const
	{
		os << "{";
	}

	void separator_impl(std::ostream& os, SetType) const
	{
		os << ", ";
	}

	void suffix_impl(std::ostream& os, SetType) const
	{
		os << "}";
	}


	void prefix_impl(std::ostream& os, TupleType) const
	{
		os << "(";
	}

	void separator_impl(std::ostream& os, TupleType) const
	{
		os << ", ";
	}

	void suffix_impl(std::ostream& os, TupleType) const
	{
		os << ")";
	}

	void prefix_impl(std::ostream& os, MapType) const
	{
		os << "{\n\t";
	}

	void separator_impl(std::ostream& os, MapType) const
	{
		os << ",\n\t";
	}
	void suffix_impl(std::ostream& os, MapType) const
	{
		os << "\n}";
	}

	template<class, class = void>
	struct has_hasher : std::false_type{};

	template<class T>
	struct has_hasher<T, prettyprint::void_t<typename T::hasher>> : std::true_type{};
public:


	template<typename T>
	void prefix(std::ostream& os, const T& t) const
	{
		prefix_impl(os, typename TypeCategorizer<T>::category());
	}
	template<typename T>
	void separator(std::ostream& os, const T&) const
	{
		separator_impl(os, typename TypeCategorizer<T>::category());
	}

	template<typename T>
	void suffix(std::ostream& os, const T&) const
	{
		suffix_impl(os, typename TypeCategorizer<T>::category());
	}

	template<typename T>
	void element(std::ostream& os, const T& t) const
	{
		os << t;
	}

	template<typename Char, typename Traits, typename Alloc>
	void element(std::ostream& os, const std::basic_string<Char, Traits, Alloc>& str) const
	{
		os << "\"" << str << "\"";
	}


};


template<typename T, typename Fmt>
void print_entity(const T& t, std::ostream& os, std::false_type, const Fmt& formatter)
{
	formatter.element(os, t);
}
template<typename First, typename Last, typename Fmt>
void print_entity(const std::pair<First, Last>& pair, std::ostream& os, std::true_type,const Fmt& formatter)
{
	formatter.prefix(os, pair);
	print_entity(pair.first, os, typename is_container<First>::type(), formatter);
	formatter.separator(os, pair);
	print_entity(pair.second, os, typename is_container<Last>::type(), formatter);
	formatter.suffix(os, pair);
}


template< typename Fmt, typename ... Args>
void print_tuple(const std::tuple<Args...>& tuple, std::ostream& os, std::integral_constant<std::size_t, 1>, const Fmt& formatter)
{
	const std::size_t N = sizeof...(Args);
	print_entity(std::get<N - 1>(tuple), os, typename is_container<std::decay_t<decltype(std::get<N - 1>(tuple))>>::type(), formatter);
}

template< typename Fmt, typename ... Args>
void print_tuple(const std::tuple<Args...>& tuple, std::ostream& os, std::integral_constant<std::size_t, 0>, const Fmt& formatter)
{
	// do nothing for tuples of size 0
}

template<size_t index, typename Fmt, typename ... Args>
void print_tuple(const std::tuple<Args...>& tuple, std::ostream& os, std::integral_constant<std::size_t, index>, const Fmt& formatter)
{
	const std::size_t N = sizeof...(Args);
	print_entity(std::get<N - index>(tuple), os, typename is_container<std::decay_t<decltype(std::get<N - index>(tuple))>>::type(), formatter);
	formatter.separator(os, tuple);
	print_tuple(tuple, os, std::integral_constant<std::size_t, index - 1>(), formatter);
}


template<typename Fmt, typename ... Args>
void print_entity(const std::tuple<Args...>& tuple, std::ostream& os, std::true_type, const Fmt& formatter)
{
	// call the tuple-print helper which performs the countdown
	formatter.prefix(os, tuple);
	print_tuple(tuple, os, std::integral_constant<std::size_t, sizeof...(Args)>(), formatter);
	formatter.suffix(os, tuple);
}
template<typename T, typename Fmt>
void print_entity(const T& t, std::ostream& os, std::true_type, const Fmt& formatter)
{
	using std::begin;
	using std::end;
	formatter.prefix(os, t);
	auto begin_iter = begin(t);
	auto end_iter = end(t);
	while (begin_iter != end_iter)
	{
		print_entity(*begin_iter, os, typename is_container<std::decay_t<decltype(*begin_iter)>>::type(), formatter);
		if (++begin_iter != end_iter)
			formatter.separator(os, t);
	}
	formatter.suffix(os, t);
}

template<typename T, typename Fmt>
void print_line(const T& t, std::ostream& os, const Fmt& formatter)
{
//	TD<T> td1;

	print_entity(t, os, typename is_container<T>::type(), formatter);
	os << std::endl;
}


template<typename T>
void print_line(const T& t, std::ostream& os)
{
	default_formatting df;
	print_line(t, os, df);
}
