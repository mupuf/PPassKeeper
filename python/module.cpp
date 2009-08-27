#include <boost/python.hpp>
using namespace boost::python;

#include "../include/ppasskeeper.h"

class ModuleList
{
public:
	ModuleList()
		: m_length(-1)
	{
		m_list = ppk_module_list_new();
	}
	~ModuleList() { ppk_module_list_free(m_list); }
	int length()
	{
		if (m_length != -1)
			return m_length;
		if (m_list == NULL)
			m_length = 0;
		else
		{
			m_length = 0;
			while (m_list[m_length] != NULL)
				m_length++;
		}
		return m_length;
	};
private:
	char** m_list;
	int m_length;
};

BOOST_PYTHON_MODULE(ppasskeeper)
{
	class_<ModuleList>("ModuleList", init<>())
		.def("__len__", &ModuleList::length)
		;
}
