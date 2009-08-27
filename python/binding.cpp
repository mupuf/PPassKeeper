#include "exception.h"
#include "module.h"
#include "utils.h"

BOOST_PYTHON_MODULE(ppasskeeper)
{
	register_exception_translator<PPassKeeperError>(&translate);
	class_<ModuleList>("ModuleList", init<>())
		.def("__len__", &ModuleList::length)
		.def("__getitem__", &ModuleList::getitem)
		;
	def("is_locked", &is_locked);
	def("set_password", &set_password);
	def("unlock", &unlock);
	def("settings_directory", ppk_settings_directory);
}
