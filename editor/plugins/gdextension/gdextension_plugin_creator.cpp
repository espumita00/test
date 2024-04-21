/**************************************************************************/
/*  gdextension_plugin_creator.cpp                                        */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "gdextension_plugin_creator.h"

#include "core/core_bind.h"
#include "core/io/dir_access.h"
#include "core/version.h"
#include "gdextension_template_files.gen.h"

#include "editor/editor_node.h"

void GDExtensionPluginCreator::_git_clone_godot_cpp(const String &p_parent_path, bool p_compile) {
	EditorProgress ep("Preparing GDExtension C++ plugin", "Preparing GDExtension C++ plugin", 3);
	ep.step("Checking for Git...", 0);
	List<String> args;
	args.push_back("--version");
	String output = "";
	int result = OS::get_singleton()->execute("git", args, &output);
	ERR_FAIL_COND_MSG(result != 0 || output.is_empty(), "Could not run git command. Please clone godot-cpp manually in order to have a working GDExtension plugin.");
	args[0] = "clone";
	args.push_back("--single-branch");
	args.push_back("--branch");
	args.push_back(VERSION_BRANCH);
	args.push_back("https://github.com/godotengine/godot-cpp");
	const String godot_cpp_path = p_parent_path.trim_prefix("res://").path_join("godot-cpp");
	args.push_back(godot_cpp_path);
	ep.step("Cloning godot-cpp...", 1);
	output = "";
	result = OS::get_singleton()->execute("git", args, &output);
	Ref<DirAccess> dir = DirAccess::create(DirAccess::ACCESS_RESOURCES);
	if (result != 0 || !dir->dir_exists(godot_cpp_path)) {
		args[3] = "master";
		output = "";
		result = OS::get_singleton()->execute("git", args, &output);
	}
	ERR_FAIL_COND_MSG(result != 0 || !dir->dir_exists(godot_cpp_path), "Failed to clone godot-cpp. Please clone godot-cpp manually in order to have a working GDExtension plugin.");
	ep.step("Performing initial compile... (this may take several minutes)", 2);
	if (p_compile) {
		result = OS::get_singleton()->execute("scons", List<String>());
		ERR_FAIL_COND_MSG(result != 0, "Failed to compile godot-cpp. Please ensure SCons is installed, then run the `scons` command in your project.");
	}
	ep.step("Done!", 3);
}

void GDExtensionPluginCreator::_make_dir_in_res(const String &p_dir_path) {
	Ref<DirAccess> dir = DirAccess::create(DirAccess::ACCESS_RESOURCES);
	dir->make_dir_recursive(p_dir_path);
}

String GDExtensionPluginCreator::_process_template(const String &p_contents) {
	String ret;
	if (_strip_module_defines) {
		bool keep = true;
		PackedStringArray lines = p_contents.split("\n");
		for (int i = 0; i < lines.size(); i++) {
			String line = lines[i];
			if (line == "#if GDEXTENSION" || line == "#else") {
				continue;
			} else if (line == "#elif GODOT_MODULE") {
				keep = false;
				continue;
			} else if (line == "#endif") {
				keep = true;
				continue;
			}
			if (keep) {
				ret += line + "\n";
			}
		}
	} else {
		ret = p_contents;
	}
	ret = ret.replace("__BASE_NAME__", _base_name);
	ret = ret.replace("__LIBRARY_NAME__", _library_name);
	ret = ret.replace("__GODOT_VERSION__", VERSION_BRANCH);
	return ret;
}

void GDExtensionPluginCreator::_write_file(const String &p_file_path, const String &p_contents) {
	Error err;
	Ref<FileAccess> file = FileAccess::open(p_file_path, FileAccess::WRITE, &err);
	ERR_FAIL_COND_MSG(err != OK, "Couldn't write file at path: " + p_file_path + ".");
	file->store_string(_process_template(p_contents));
	file->close();
}

void GDExtensionPluginCreator::_ensure_file_contains(const String &p_file_path, const String &p_new_contents) {
	Error err;
	Ref<FileAccess> file = FileAccess::open(p_file_path, FileAccess::READ_WRITE, &err);
	if (err != OK) {
		_write_file(p_file_path, p_new_contents);
		return;
	}
	String new_contents = _process_template(p_new_contents);
	String existing_contents = file->get_as_text();
	if (existing_contents.is_empty()) {
		file->store_string(new_contents);
	} else {
		file->seek_end();
		PackedStringArray lines = new_contents.split("\n", false);
		for (int i = 0; i < lines.size(); i++) {
			String line = lines[i];
			if (!existing_contents.contains(line)) {
				file->store_string(line + "\n");
			}
		}
	}
	file->close();
}

void GDExtensionPluginCreator::_write_common_files_and_dirs(const String &p_addon_path) {
	String identifier = p_addon_path.get_file().validate_identifier();
	if (identifier[0] == '_') {
		_base_name = identifier.substr(1);
		_library_name = "godot" + identifier;
	} else {
		_base_name = identifier;
		_library_name = identifier;
	}
	_make_dir_in_res(p_addon_path.path_join("doc_classes"));
	_make_dir_in_res(p_addon_path.path_join("icons"));
	_make_dir_in_res(p_addon_path.path_join("src"));
	_ensure_file_contains("res://SConstruct", SCONSTRUCT_TOP_LEVEL);
	_write_file(p_addon_path.path_join("doc_classes/ExampleNode.xml"), EXAMPLENODE_XML);
	_write_file(p_addon_path.path_join("icons/ExampleNode.svg"), EXAMPLENODE_SVG);
	_write_file(p_addon_path.path_join("icons/ExampleNode.svg.import"), EXAMPLENODE_SVG_IMPORT);
	_write_file(p_addon_path.path_join("src/.gdignore"), "");
	_write_file(p_addon_path.path_join(".gitignore"), GDEXT_GITIGNORE + "*.obj");
	_write_file(p_addon_path.path_join(_library_name + ".gdextension"), LIBRARY_NAME_GDEXTENSION);
}

void GDExtensionPluginCreator::create_plugin_only(const String &p_addon_path, bool p_compile) {
	_strip_module_defines = true;
	_write_common_files_and_dirs(p_addon_path);
	_ensure_file_contains("res://.gitignore", "*.dblite");
	_write_file(p_addon_path.path_join("src/example_node.cpp"), EXAMPLE_NODE_CPP);
	_write_file(p_addon_path.path_join("src/example_node.h"), EXAMPLE_NODE_H);
	_write_file(p_addon_path.path_join("src/register_types.cpp"), REGISTER_TYPES_CPP);
	_write_file(p_addon_path.path_join("src/register_types.h"), REGISTER_TYPES_H);
	_write_file(p_addon_path.path_join("src/" + _library_name + "_defines.h"), GDEXT_DEFINES_H);
	_write_file(p_addon_path.path_join("src/initialize_gdextension.cpp"), INITIALIZE_GDEXTENSION_CPP.replace("#include \"../../../", "#include \""));
	_write_file(p_addon_path.path_join("SConstruct"), SCONSTRUCT_ADDON.replace(" + Glob(\"../../*.cpp\")", "").replace(",../../", ""));
	_git_clone_godot_cpp(p_addon_path.path_join("src"), p_compile);
}

void GDExtensionPluginCreator::create_plugin_with_module(const String &p_addon_path, bool p_compile) {
	_write_common_files_and_dirs(p_addon_path);
	_make_dir_in_res("res://tests");
	_ensure_file_contains("res://.gitignore", GDEXT_GITIGNORE);
	_write_file("res://SCsub", SCSUB);
	_write_file("res://config.py", CONFIG_PY);
	_write_file("res://example_node.cpp", EXAMPLE_NODE_CPP);
	_write_file("res://example_node.h", EXAMPLE_NODE_H);
	_write_file("res://register_types.cpp", REGISTER_TYPES_CPP);
	_write_file("res://register_types.h", REGISTER_TYPES_H);
	_write_file("res://" + _library_name + "_defines.h", SHARED_DEFINES_H);
	_write_file("res://tests/test_" + _base_name + ".h", TEST_BASE_NAME_H);
	_write_file("res://tests/test_example_node.h", TEST_EXAMPLE_NODE_H);
	_write_file(p_addon_path.path_join("src/initialize_gdextension.cpp"), INITIALIZE_GDEXTENSION_CPP);
	_write_file(p_addon_path.path_join("SConstruct"), SCONSTRUCT_ADDON);
	_git_clone_godot_cpp(p_addon_path.path_join("src"), p_compile);
}
