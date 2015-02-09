#include "pretty_print.hpp"


class network_switch
{

public:
	network_switch(const std::string& str) : switch_name(str){}
	void associate(const std::string& mac_addr, int port)
	{
		mac_address_to_ports[mac_addr] = port;
	}
	const std::string& name() const
	{
		return switch_name;
	}
	const std::unordered_map<std::string,int>& table() const
	{
		return mac_address_to_ports;
	}
private:
	std::unordered_map<std::string,int> mac_address_to_ports;
	std::string switch_name;

};

auto begin( const network_switch& ns)
{
	return ns.table().begin();
}
auto end(const network_switch& ns)
{
	return ns.table().end();
}

class network_switch_formatting : public default_formatting
{
private:
	std::string switch_name;
protected:
	using default_formatting::prefix_impl;
	using default_formatting::separator_impl;
	using default_formatting::suffix_impl;

	template<typename T>
	struct is_network_type
	{
		using type = typename std::is_same<std::decay_t<T>,network_switch>::type;
	};

	struct NetworkSwitchType{};

	template<typename T>
	struct TypeCategorizer2
	{
		using is_switch = typename is_network_type<T>::type;
		using category = std::conditional_t<is_switch::value,NetworkSwitchType, typename TypeCategorizer<T>::category>;
	};

	void prefix_impl(std::ostream& os, NetworkSwitchType) const
	{
		os << "Table for switch " << switch_name << "\n--------------\n\t";
	}
	void separator_impl(std::ostream& os, NetworkSwitchType) const
	{
		os << ",\n\t";
	}
	void suffix_impl(std::ostream& os, NetworkSwitchType) const
	{
		os << "\n--------------\n";
	}
public:
	network_switch_formatting(std::string str) : switch_name(str) {}

	template<typename T>
	void prefix(std::ostream& os, const T& t) const
	{
		prefix_impl(os,typename TypeCategorizer2<T>::category());
	}

	template<typename T>
	void separator(std::ostream& os, const T& t) const
	{
		separator_impl(os,typename TypeCategorizer2<T>::category());
	}

	template<typename T>
	void suffix(std::ostream& os, const T& t) const
	{
		suffix_impl(os,typename TypeCategorizer2<T>::category());
	}
};
class special2 : public default_formatting
{
private:
	std::string dev_name;
protected:

	// include the virtual functions from the default_formatter to participate in overload resolution
	using default_formatting::prefix_impl;
	using default_formatting::separator_impl;
	using default_formatting::suffix_impl;

	struct ListType {  };



	struct NetworkType{ };

	template<typename T, class = void>
	struct has_network_type : public prettyprint::FalseType{};

	template<typename T>
	struct has_network_type<T, prettyprint::void_t<typename T::const_iterator>> : public prettyprint::TrueType{};

	template<typename T, class=void>
	struct has_list_iterator : public prettyprint::FalseType{};
	
	template<typename T>
	struct has_list_iterator<T, prettyprint::void_t<typename T::const_iterator>>{
		using const_iterator = typename T::const_iterator;
		using forward_check = typename std::is_same<typename std::iterator_traits<const_iterator>::iterator_category,std::forward_iterator_tag>::type;
		using bidi_check = typename std::is_same<typename std::iterator_traits<const_iterator>::iterator_category,std::bidirectional_iterator_tag>::type;


		using type = typename prettyprint::_Or<forward_check,bidi_check>::type;
	};

	template<typename T>
	struct is_network_type
	{
		using type = typename std::is_same<std::decay_t<T>,network_switch>::type;
	};



	void prefix_impl(std::ostream& os, ListType) const
	{
		os << "[[\t--> ";
	}
	void separator_impl(std::ostream& os, ListType) const
	{
		os << " \n\t--> ";
	}
	void suffix_impl(std::ostream& os, ListType) const
	{
		os << "\n]]";
	}

	void prefix_impl(std::ostream& os, MapType) const
	{
		os << "{{\n\t";
	}
	void suffix_impl(std::ostream& os, MapType) const
	{
		os << "\n}}";
	}

	void prefix_impl(std::ostream& os, ArbitraryType) const
	{
		os << "<";
	}
	void suffix_impl(std::ostream& os, ArbitraryType) const
	{
		os << ">";
	}

	void prefix_impl(std::ostream& os, NetworkType) const
	{
		os << "Switch at " << dev_name << ":\n\t";
	}
	void suffix_impl(std::ostream& os, NetworkType) const
	{
		os << "\n-----------------";
	}
	void separator_impl(std::ostream& os, NetworkType) const
	{
		os << ",\n\t";
	}

	template<typename T>
	struct TypeCategorizer2
	{
		using is_switch = typename is_network_type<T>::type;
		using category = std::conditional_t<is_switch::value,NetworkType,typename TypeCategorizer<T>::category>;

	}; 

	
	
	// partially specialize TypeCategorizer for std::forward_list<>
	// can partially specialize the categorizer for any other type
	// should a single type be necessary, as opposed to a generic tag

	template<typename T,typename Alloc>
	struct TypeCategorizer2<std::forward_list<T, Alloc>>
	{
		using category = ListType;
	};

	

public:

	template<typename T>
	class TD;

	template<typename T>
	void prefix(std::ostream& os, const T& t) const
	{
		//TD<typename TypeCategorizer2<T>::category> td1;
		prefix_impl(os, typename TypeCategorizer2<T>::category());
	}

	template<typename T>
	void separator(std::ostream& os, const T& t) const
	{
		separator_impl(os, typename TypeCategorizer2<T>::category());
	}

	template<typename T>
	void suffix(std::ostream& os, const T& t) const
	{
		suffix_impl(os, typename TypeCategorizer2<T>::category());
	}



};


int main()
{
	default_formatting df;
	special2 sp2;
	std::vector<std::vector<int>> vvi;
	for (int i = 0; i < 10; ++i)
	{
		std::vector<int> vtor_sub{ i, i + 1, i + 2 };
		vvi.push_back(vtor_sub);
	}
	std::cout << "vector<vector<int>>: " << std::endl;
	print_line(vvi, std::cout, df);
	std::cout << std::endl;

	std::tuple< int, char, std::vector<std::vector<double>>, double > tuple_1{ 1, 'a', { { 4.0, 5.0 }, { 6.0, 7.0 } }, 5.6 };

	std::pair<std::vector<std::vector<int>>, std::vector<double>> pair_1{ { { 5, 6, 7 }, { 8, 9, 10 } }, { 5.6, 7.8, 9.45 } };

	std::pair<int, int> p_int_int{ 5, 7 };
	std::unordered_map<int, std::string> my_map;
	my_map[1] = "Hello!";
	my_map[2] = "goodbye!";
	std::map<int, std::vector<int>> map_int_vec;
	map_int_vec[0] = { 4, 5, 6 };
	map_int_vec[1] = { 7, 8, 9 };

	std::cout << "map<int,vector<int>>: " << std::endl;
	print_line(map_int_vec, std::cout, sp2);
	std::cout << std::endl;


	std::cout << "vector<vector<int>>:" << std::endl;
	print_line(vvi, std::cout, sp2);
	std::cout << std::endl;

	std::cout << "std::string: " << std::endl;
	print_line(std::string("hi!"), std::cout, sp2);
	std::cout << std::endl;

	std::cout << "std::tuple< int, char, std::vector<std::vector<double>>, double >: " << std::endl;
	print_line(tuple_1, std::cout, sp2);
	std::cout << std::endl;

	std::cout << "std::pair<int, int>:\n";
	print_line(p_int_int, std::cout, sp2);
	std::cout << "\n";

	std::cout << "std::pair<std::vector<std::vector<int>>, std::vector<double>>:\n";
	print_line(pair_1, std::cout, sp2);
	std::cout << std::endl; 

	std::cout << "C-style array: " << std::endl;
	int a[5] = { 1, 2, 3, 4, 5 };
	print_line(a, std::cout, sp2);
	std::cout << std::endl;

	std::cout << "forward_list<int>: " << std::endl;
	std::forward_list<int> flist = { 1, 2, 3, 4, 5 };
	print_line(flist, std::cout, sp2);
	std::cout << std::endl;

	std::cout << "list<int>: " << std::endl;
	std::list<int> mylist{2,4,6,8,10};
	print_line(mylist,std::cout,sp2);
	std::cout << std::endl;

	std::cout << "[*] Testing network_switch\n";
	network_switch ns1("SW1-MIAMI");
	ns1.associate("ab-de-3f-45-6b-7f",2);
	ns1.associate("ab-de-3f-39-47-21",10);
	network_switch_formatting ns_formatter(ns1.name());
	print_line(ns1,std::cout,ns_formatter);

}