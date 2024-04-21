/**************************************************************************/
/*  gdextension_plugin_creator.h                                          */
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

#ifndef GDEXTENSION_PLUGIN_CREATOR_H
#define GDEXTENSION_PLUGIN_CREATOR_H

#include "core/string/ustring.h"

class GDExtensionPluginCreator {
	// Used by _process_template.
	bool _strip_module_defines = false;
	String _base_name;
	String _library_name;

	void _git_clone_godot_cpp(const String &p_parent_path, bool p_compile);
	void _make_dir_in_res(const String &p_dir_path);
	String _process_template(const String &p_contents);
	void _write_common_files_and_dirs(const String &p_addon_path);
	void _write_file(const String &p_file_path, const String &p_contents);
	void _ensure_file_contains(const String &p_file_path, const String &p_new_contents);

public:
	void create_plugin_only(const String &p_addon_path, bool p_compile);
	void create_plugin_with_module(const String &p_addon_path, bool p_compile);
};

#endif // GDEXTENSION_PLUGIN_CREATOR_H
