/**************************************************************************/
/*  instance_placeholder.cpp                                              */
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

#include "instance_placeholder.h"

#include "core/io/resource_loader.h"
#include "scene/resources/packed_scene.h"

bool InstancePlaceholder::_set(const StringName &p_name, const Variant &p_value) {
	PropSet ps;
	ps.name = p_name;
	ps.value = p_value;
	stored_values.push_back(ps);
	return true;
}

bool InstancePlaceholder::_get(const StringName &p_name, Variant &r_ret) const {
	for (const PropSet &E : stored_values) {
		if (E.name == p_name) {
			r_ret = E.value;
			return true;
		}
	}
	return false;
}

void InstancePlaceholder::_get_property_list(List<PropertyInfo> *p_list) const {
	for (const PropSet &E : stored_values) {
		PropertyInfo pi;
		pi.name = E.name;
		pi.type = E.value.get_type();
		pi.usage = PROPERTY_USAGE_STORAGE;

		p_list->push_back(pi);
	}
}

void InstancePlaceholder::set_instance_path(const String &p_name) {
	path = p_name;
}

String InstancePlaceholder::get_instance_path() const {
	return path;
}

Node *InstancePlaceholder::create_instance(bool p_replace, const Ref<PackedScene> &p_custom_scene) {
	ERR_FAIL_COND_V(!is_inside_tree(), nullptr);

	Node *base = get_parent();
	if (!base) {
		return nullptr;
	}

	Ref<PackedScene> ps;
	if (p_custom_scene.is_valid()) {
		ps = p_custom_scene;
	} else {
		ps = ResourceLoader::load(path, "PackedScene");
	}

	if (!ps.is_valid()) {
		return nullptr;
	}
	Node *instance = ps->instantiate();
	if (!instance) {
		return nullptr;
	}
	instance->set_name(get_name());
	instance->set_multiplayer_authority(get_multiplayer_authority());
	int pos = get_index();

	for (const PropSet &E : stored_values) {
		set_value_on_instance(this, instance, E);
	}

	if (p_replace) {
		queue_free();
		base->remove_child(this);
	}

	base->add_child(instance);
	base->move_child(instance, pos);

	return instance;
}

// This method will attempt to set the correct values on the placeholder instance
// for regular types this is trivial and unnecessary.
// For nodes however this becomes a bit tricky because they might now have existed until the instantiation,
// so this method will try to find the correct nodes and resolve them.
void InstancePlaceholder::set_value_on_instance(InstancePlaceholder *placeholder, Node *instance, const InstancePlaceholder::PropSet &E) {
	bool is_valid;

	// if we don't have any info, we can't do anything
	// so try setting the value directly
	Variant current = instance->get(E.name, &is_valid);
	if (!is_valid) {
		instance->set(E.name, E.value, &is_valid);
		return;
	}

	Variant::Type current_type = current.get_type();
	Variant::Type placeholder_type = E.value.get_type();

	// check if the variant types match
	if (Variant::evaluate(Variant::OP_EQUAL, current_type, placeholder_type)) {
		instance->set(E.name, E.value, &is_valid);
		if (is_valid)
			return;
		// types match but setting failed? it's probably an array and we have a TypedArray issue
	}

	switch (current.get_type()) {
		case Variant::Type::NIL:
			if (placeholder_type != Variant::Type::NODE_PATH)
				break;
			// if it's nil but we have a nodepath, we guess what works

			instance->set(E.name, E.value, &is_valid);
			if (is_valid)
				break;

			instance->set(E.name, try_get_node(placeholder, instance, E.value), &is_valid);
			break;
		case Variant::Type::OBJECT:
			if (placeholder_type != Variant::Type::NODE_PATH)
				break;
			// easiest case, we want a node, but we have a deferred nodepath
			instance->set(E.name, try_get_node(placeholder, instance, E.value));
			break;
		case Variant::Type::ARRAY: {
			// if we have reached here it means our array types don't match
			// so we will convert the placeholder array into the correct type
			// and resolve nodes if necessary
			Array current_array = current;
			Array converted_array;
			Array placeholder_array = E.value;
			converted_array = current_array.duplicate();
			converted_array.resize(placeholder_array.size());

			if (Variant::evaluate(Variant::OP_EQUAL, current_array.get_typed_builtin(), Variant::Type::NODE_PATH)) {
				// we want a typed nodepath array
				for (int i = 0; i < placeholder_array.size(); i++) {
					converted_array.set(i, placeholder_array[i]);
				}
			} else {
				// we want nodes, convert nodepaths
				for (int i = 0; i < placeholder_array.size(); i++) {
					converted_array.set(i, try_get_node(placeholder, instance, placeholder_array[i]));
				}
			}

			instance->set(E.name, converted_array, &is_valid);
			if (!is_valid)
				WARN_PRINT(vformat("{Property '%s' with type %s could not be set when creating instance of '%s'}", E.name, Variant::get_type_name(current_type), placeholder->get_name()));
			break;
		}
		default:
			WARN_PRINT(vformat("{Property '%s' with type '%s' could not be set when creating instance of '%s'}", E.name, Variant::get_type_name(current_type), placeholder->get_name()));
			break;
	}
}

Node *InstancePlaceholder::try_get_node(InstancePlaceholder *placeholder, Node *instance, const NodePath &np) {
	// first try to resolve internally
	// if that fails try resolving externally
	Node *node = instance->get_node_or_null(np);
	if (node == nullptr)
		node = placeholder->get_node_or_null(np);

	return node;
}

Dictionary InstancePlaceholder::get_stored_values(bool p_with_order) {
	Dictionary ret;
	PackedStringArray order;

	for (const PropSet &E : stored_values) {
		ret[E.name] = E.value;
		if (p_with_order) {
			order.push_back(E.name);
		}
	};

	if (p_with_order) {
		ret[".order"] = order;
	}

	return ret;
};

void InstancePlaceholder::_bind_methods() {
	ClassDB::bind_method(D_METHOD("get_stored_values", "with_order"), &InstancePlaceholder::get_stored_values, DEFVAL(false));
	ClassDB::bind_method(D_METHOD("create_instance", "replace", "custom_scene"), &InstancePlaceholder::create_instance, DEFVAL(false), DEFVAL(Variant()));
	ClassDB::bind_method(D_METHOD("get_instance_path"), &InstancePlaceholder::get_instance_path);
}

InstancePlaceholder::InstancePlaceholder() {
}
