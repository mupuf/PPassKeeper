#include "exception.h"
#include "module.h"
#include "entry.h"
#include "data.h"
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
		.def("display_name", &Module::display_name)
		.def("security_level", &Module::security_level)
		.def("is_writable", &Module::is_writable)
		.def("max_data_size", &Module::max_data_size)
		.def("get_entry_count", &Module::get_entry_count)
		.def("has_entry", &Module::has_entry)
		.def("remove_entry", &Module::remove_entry)
		.def("get_entry", &Module::get_entry, return_value_policy<manage_new_object>())
		.def("set_entry", &Module::set_entry)
		.def("get_entry_list", &Module::get_entry_list, return_value_policy<manage_new_object>())
		.def("get_default", &Module::get_default, return_value_policy<manage_new_object>())
		.staticmethod("get_default")
		.def("set_default", &Module::set_default, return_value_policy<manage_new_object>())
		.staticmethod("set_default")
		;
	def("is_locked", &is_locked);
	def("set_password", &set_password);
	def("unlock", &unlock);
	def("settings_directory", ppk_settings_directory);

	class_<EntryList>("EntryList", init<>())
		.def("__len__", &EntryList::length)
		.def("__getitem__", &EntryList::getitem, return_value_policy<manage_new_object>())
		.def("__contains__", &EntryList::contains)
		;
	class_<Entry>("Entry", no_init)
		.def(self_ns::str(self))
		.def("create_network_entry", &Entry::create_network_entry, return_value_policy<manage_new_object>())
		.staticmethod("create_network_entry")
		.def("create_application_entry", &Entry::create_application_entry, return_value_policy<manage_new_object>())
		.staticmethod("create_application_entry")
		.def("create_item_entry", &Entry::create_item_entry, return_value_policy<manage_new_object>())
		.staticmethod("create_item_entry")
		.def("from_key", &Entry::from_key, return_value_policy<manage_new_object>())
		.staticmethod("from_key")
		.def("to_key", &Entry::to_key)
		;

	class_<Data>("Data", no_init)
		.def(self_ns::str(self))
		.def("create_string_data", &Data::create_string_data, return_value_policy<manage_new_object>())
		.staticmethod("create_string_data")
		.def("create_blob_data", &Data::create_blob_data, return_value_policy<manage_new_object>())
		.staticmethod("create_blob_data")
		;

	enum_<ppk_security_level>("SecurityLevel")
		.value("Lowest", ppk_sec_lowest)
		.value("Scrambled", ppk_sec_scrambled)
		.value("Safe", ppk_sec_safe)
		.value("Perfect", ppk_sec_perfect)
		;

	enum_<ppk_readFlag>("ReadFlag")
		.value("None", ppk_rf_none)
		.value("Silent", ppk_rf_silent)
		;

	enum_<ppk_writeFlag>("WriteFlag")
		.value("None", ppk_wf_none)
		.value("Silent", ppk_wf_silent)
		;

	enum_<ppk_listingFlag>("ListingFlag")
		.value("None", ppk_lf_none)
		.value("Silent", ppk_lf_silent)
		;

	enum_<ppk_data_type>("DataType")
		.value("String", ppk_string)
		.value("Blob", ppk_blob)
		;

	enum_<ppk_entry_type>("EntryType")
		.value("Network", ppk_network)
		.value("Application", ppk_application)
		.value("Item", ppk_item)
		;
}
