/**************************************************************************/
/*  test_packed_scene.h                                                   */
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

#ifndef TEST_INSTANCE_PLACEHOLDER_H
#define TEST_INSTANCE_PLACEHOLDER_H

#include "scene/main/instance_placeholder.h"
#include "scene/resources/packed_scene.h"

#include "tests/test_macros.h"

namespace TestInstancePlaceholder {

// class TestInstancePlaceholderNode : public Node {
// 	GDCLASS(TestInstancePlaceholderNode, Node);

// protected:
// 	static void _bind_methods() {
// 		ClassDB::bind_method(D_METHOD("set_int_property", "int_property"), &TestInstancePlaceholderNode::set_int_property);
// 		ClassDB::bind_method(D_METHOD("get_int_property"), &TestInstancePlaceholderNode::get_int_property);

// 		ADD_PROPERTY(PropertyInfo(Variant::INT, "int_property"), "set_int_property", "get_int_property");
// 	}

// public:
// 	int int_property = 0;

// 	void set_int_property(const int &p_int) {
// 		int_property = p_int;
// 	}

// 	int get_int_property() const {
// 		return int_property;
// 	}
// };

// stolen from tests\core\object\test_object.h
class _MockScriptInstance : public ScriptInstance {
	StringName property_name = "NO_NAME";
	Variant property_value;

public:
	bool set(const StringName &p_name, const Variant &p_value) override {
		property_name = p_name;
		property_value = p_value;
		return true;
	}
	bool get(const StringName &p_name, Variant &r_ret) const override {
		if (property_name == p_name) {
			r_ret = property_value;
			return true;
		}
		return false;
	}
	void get_property_list(List<PropertyInfo> *p_properties) const override {
	}
	Variant::Type get_property_type(const StringName &p_name, bool *r_is_valid) const override {
		return Variant::NODE_PATH;
	}
	virtual void validate_property(PropertyInfo &p_property) const override {
	}
	bool property_can_revert(const StringName &p_name) const override {
		return false;
	};
	bool property_get_revert(const StringName &p_name, Variant &r_ret) const override {
		return false;
	};
	void get_method_list(List<MethodInfo> *p_list) const override {
	}
	bool has_method(const StringName &p_method) const override {
		return false;
	}
	int get_method_argument_count(const StringName &p_method, bool *r_is_valid = nullptr) const override {
		if (r_is_valid) {
			*r_is_valid = false;
		}
		return 0;
	}
	Variant callp(const StringName &p_method, const Variant **p_args, int p_argcount, Callable::CallError &r_error) override {
		return Variant();
	}
	void notification(int p_notification, bool p_reversed = false) override {
	}
	Ref<Script> get_script() const override {
		return Ref<Script>();
	}
	const Variant get_rpc_config() const override {
		return Variant();
	}
	ScriptLanguage *get_language() override {
		return nullptr;
	}
};

TEST_CASE("[SceneTree][InstancePlaceholder] Instantiate from placeholder with no overrides") {
	SUBCASE("with non-node values") {
		InstancePlaceholder *ip = memnew(InstancePlaceholder);
		ip->set_name("TestScene");
		Node *root = memnew(Node);
		SceneTree::get_singleton()->get_root()->add_child(root);

		root->add_child(ip);
		// Create a scene to instance.
		Node *scene = memnew(Node);
		scene->set_process_priority(12);

		// Pack the scene.
		PackedScene *packed_scene = memnew(PackedScene);
		const Error err = packed_scene->pack(scene);
		CHECK(err == OK);

		// Instantiate the scene.
		Node *created = ip->create_instance(true, packed_scene);
		CHECK(created != nullptr);
		CHECK(created->get_name() == "TestScene");
		CHECK(created->get_process_priority() == 12);

		root->queue_free();
		memdelete(scene);
	}

	SUBCASE("with node value") {
		InstancePlaceholder *ip = memnew(InstancePlaceholder);
		ip->set_name("TestScene");
		Node *root = memnew(Node);
		SceneTree::get_singleton()->get_root()->add_child(root);

		root->add_child(ip);
		// Create a scene to instance.
		Node *scene = memnew(Node);
		scene->set_process_priority(12);

		// Pack the scene.
		PackedScene *packed_scene = memnew(PackedScene);
		const Error err = packed_scene->pack(scene);
		CHECK(err == OK);

		// Instantiate the scene.
		Node *created = ip->create_instance(true, packed_scene);
		CHECK(created != nullptr);
		CHECK(created->get_name() == "TestScene");
		CHECK(created->get_process_priority() == 12);

		root->queue_free();
		memdelete(scene);
	}

	SUBCASE("with node-array value") {
		InstancePlaceholder *ip = memnew(InstancePlaceholder);
		ip->set_name("TestScene");
		Node *root = memnew(Node);
		SceneTree::get_singleton()->get_root()->add_child(root);

		root->add_child(ip);
		// Create a scene to instance.
		Node *scene = memnew(Node);
		scene->set_process_priority(12);

		// Pack the scene.
		PackedScene *packed_scene = memnew(PackedScene);
		const Error err = packed_scene->pack(scene);
		CHECK(err == OK);

		// Instantiate the scene.
		Node *created = ip->create_instance(true, packed_scene);
		CHECK(created != nullptr);
		CHECK(created->get_name() == "TestScene");
		CHECK(created->get_process_priority() == 12);

		root->queue_free();
		memdelete(scene);
	}
}

TEST_CASE("[SceneTree][InstancePlaceholder] Instantiate from placeholder with overrides") {
	SUBCASE("with non-node values") {
		InstancePlaceholder *ip = memnew(InstancePlaceholder);
		Node *root = memnew(Node);
		SceneTree::get_singleton()->get_root()->add_child(root);

		root->add_child(ip);
		ip->set_name("TestScene");
		ip->set("text", "override");
		// Create a scene to pack.
		Label *scene = memnew(Label);
		scene->set_text("value");

		// Pack the scene.
		PackedScene *packed_scene = memnew(PackedScene);
		packed_scene->pack(scene);

		// Instantiate the scene.
		Label *created = Object::cast_to<Label>(ip->create_instance(true, packed_scene));
		CHECK(created != nullptr);
		CHECK(created->get_text() == "override");

		root->queue_free();
		memdelete(scene);
	}

	SUBCASE("with node values") {
		InstancePlaceholder *ip = memnew(InstancePlaceholder);
		Node *root = memnew(Node);
		SceneTree::get_singleton()->get_root()->add_child(root);

		root->add_child(ip);
		ip->set_name("TestScene");
		ip->set("text", "override");
		// Create a scene to pack.
		Label *scene = memnew(Label);
		scene->set_text("value");

		// Pack the scene.
		PackedScene *packed_scene = memnew(PackedScene);
		packed_scene->pack(scene);

		// Instantiate the scene.
		Label *created = Object::cast_to<Label>(ip->create_instance(true, packed_scene));
		CHECK(created != nullptr);
		CHECK(created->get_text() == "override");

		root->queue_free();
		memdelete(scene);
	}

	SUBCASE("with node-array value") {
		InstancePlaceholder *ip = memnew(InstancePlaceholder);
		Node *root = memnew(Node);
		SceneTree::get_singleton()->get_root()->add_child(root);

		root->add_child(ip);
		ip->set_name("TestScene");
		ip->set("text", "override");
		// Create a scene to pack.
		Label *scene = memnew(Label);
		scene->set_text("value");

		// Pack the scene.
		PackedScene *packed_scene = memnew(PackedScene);
		packed_scene->pack(scene);

		// Instantiate the scene.
		Label *created = Object::cast_to<Label>(ip->create_instance(true, packed_scene));
		CHECK(created != nullptr);
		CHECK(created->get_text() == "override");

		root->queue_free();
		memdelete(scene);
	}
}

} //namespace TestInstancePlaceholder

#endif // TEST_INSTANCE_PLACEHOLDER_H