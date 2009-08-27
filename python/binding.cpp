#include "exception.h"
#include "module.h"
#include "utils.h"

BOOST_PYTHON_MODULE(ppasskeeper)
{
	register_exception_translator<PPassKeeperError>(&translate);
	class_<ModuleList>("ModuleList", init<>())
		.def("__len__", &ModuleList::length)
		.def("__getitem__", &ModuleList::getitem, return_value_policy<manage_new_object>())
		.def("__contains__", &ModuleList::contains)
		;
	class_<Module>("Module", init<std::string>())
		.def(self_ns::str(self))
		.def("read_flags", &Module::read_flags)
		.def("write_flags", &Module::write_flags)
		.def("listing_flags", &Module::listing_flags)
		;
	def("is_locked", &is_locked);
	def("set_password", &set_password);
	def("unlock", &unlock);
	def("settings_directory", ppk_settings_directory);
}
