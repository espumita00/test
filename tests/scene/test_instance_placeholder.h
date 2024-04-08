/**************************************************************************/
/*  test_instance_placeholder.h                                           */
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

// Declared in global namespace because of GDCLASS macro warning (Windows):
// "Unqualified friend declaration referring to type outside of the nearest enclosing namespace
// is a Microsoft extension; add a nested name specifier".
class _TestInstancePlaceholderNode : public Node {
	GDCLASS(_TestInstancePlaceholderNode, Node);

protected:
	static void _bind_methods() {
		ClassDB::bind_method(D_METHOD("set_int_property", "int_property"), &_TestInstancePlaceholderNode::set_int_property);
		ClassDB::bind_method(D_METHOD("get_int_property"), &_TestInstancePlaceholderNode::get_int_property);

		ADD_PROPERTY(PropertyInfo(Variant::INT, "int_property"), "set_int_property", "get_int_property");

		ClassDB::bind_method(D_METHOD("set_reference_property", "reference_property"), &_TestInstancePlaceholderNode::set_reference_property);
		ClassDB::bind_method(D_METHOD("get_reference_property"), &_TestInstancePlaceholderNode::get_reference_property);

		ADD_PROPERTY(PropertyInfo(Variant::OBJECT, "reference_property", PROPERTY_HINT_NODE_TYPE), "set_reference_property", "get_reference_property");

		ClassDB::bind_method(D_METHOD("set_reference_array_property", "reference_array_property"), &_TestInstancePlaceholderNode::set_reference_array_property);
		ClassDB::bind_method(D_METHOD("get_reference_array_property"), &_TestInstancePlaceholderNode::get_reference_array_property);

		// The hint string value "24/34:Node" is determined from existing PackedScenes with typed Array properties.
		ADD_PROPERTY(PropertyInfo(Variant::ARRAY, "reference_array_property", PROPERTY_HINT_TYPE_STRING, "24/34:Node"), "set_reference_array_property", "get_reference_array_property");
	}

public:
	int int_property = 0;

	void set_int_property(const int &p_int) {
		int_property = p_int;
	}

	int get_int_property() const {
		return int_property;
	}

	Variant reference_property;

	void set_reference_property(const Variant &p_node) {
		reference_property = p_node;
	}

	Variant get_reference_property() const {
		return reference_property;
	}

	Variant reference_array_property;

	void set_reference_array_property(const Variant &p_array) {
		reference_array_property = p_array;
	}

	Variant get_reference_array_property() const {
		return reference_array_property;
	}
};

namespace TestInstancePlaceholder {

TEST_CASE("[SceneTree][InstancePlaceholder] Instantiate from placeholder with no overrides") {
	GDREGISTER_CLASS(_TestInstancePlaceholderNode);

	SUBCASE("with non-node values") {
		InstancePlaceholder *ip = memnew(InstancePlaceholder);
		ip->set_name("TestScene");
		Node *root = memnew(Node);
		SceneTree::get_singleton()->get_root()->add_child(root);

		root->add_child(ip);
		// Create a scene to instance.
		_TestInstancePlaceholderNode *scene = memnew(_TestInstancePlaceholderNode);
		scene->set_int_property(12);

		// Pack the scene.
		PackedScene *packed_scene = memnew(PackedScene);
		const Error err = packed_scene->pack(scene);
		REQUIRE(err == OK);

		// Instantiate the scene.
		_TestInstancePlaceholderNode *created = Object::cast_to<_TestInstancePlaceholderNode>(ip->create_instance(true, packed_scene));
		REQUIRE(created != nullptr);
		CHECK(created->get_name() == "TestScene");
		CHECK(created->get_int_property() == 12);

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
		_TestInstancePlaceholderNode *scene = memnew(_TestInstancePlaceholderNode);
		Node *referenced = memnew(Node);
		scene->add_child(referenced);
		referenced->set_owner(scene);
		scene->set_reference_property(referenced);
		// Pack the scene.
		PackedScene *packed_scene = memnew(PackedScene);
		const Error err = packed_scene->pack(scene);
		REQUIRE(err == OK);

		// Instantiate the scene.
		_TestInstancePlaceholderNode *created = Object::cast_to<_TestInstancePlaceholderNode>(ip->create_instance(true, packed_scene));
		REQUIRE(created != nullptr);
		CHECK(created->get_name() == "TestScene");
		CHECK(created->get_child_count() == 1);
		CHECK(created->get_reference_property().identity_compare(created->get_child(0, false)));
		CHECK_FALSE(created->get_reference_property().identity_compare(referenced));

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
		_TestInstancePlaceholderNode *scene = memnew(_TestInstancePlaceholderNode);
		Node *referenced1 = memnew(Node);
		Node *referenced2 = memnew(Node);
		scene->add_child(referenced1);
		scene->add_child(referenced2);
		referenced1->set_owner(scene);
		referenced2->set_owner(scene);
		Array node_array;
		node_array.set_typed(Variant::OBJECT, "Node", Variant());
		node_array.push_back(referenced1);
		node_array.push_back(referenced2);
		scene->set_reference_array_property(node_array);
		// Pack the scene.
		PackedScene *packed_scene = memnew(PackedScene);
		const Error err = packed_scene->pack(scene);
		REQUIRE(err == OK);

		// Instantiate the scene.
		_TestInstancePlaceholderNode *created = Object::cast_to<_TestInstancePlaceholderNode>(ip->create_instance(true, packed_scene));
		REQUIRE(created != nullptr);
		CHECK(created->get_name() == "TestScene");
		CHECK(created->get_child_count() == 2);
		Array created_array = created->get_reference_array_property();
		REQUIRE(created_array.size() == node_array.size());
		REQUIRE(created_array.size() == created->get_child_count());

		// Iterate over all nodes, since the ordering is not guaranteed.
		for (int i = 0; i < node_array.size(); i++) {
			bool node_found = false;
			for (int j = 0; j < created->get_child_count(); j++) {
				if (created_array[i].identity_compare(created->get_child(j, true))) {
					node_found = true;
				}
			}
			CHECK(node_found);
		}
		root->queue_free();
		memdelete(scene);
	}
}

TEST_CASE("[SceneTree][InstancePlaceholder] Instantiate from placeholder with overrides") {
	GDREGISTER_CLASS(_TestInstancePlaceholderNode);

	SUBCASE("with non-node values") {
		InstancePlaceholder *ip = memnew(InstancePlaceholder);
		Node *root = memnew(Node);
		SceneTree::get_singleton()->get_root()->add_child(root);

		root->add_child(ip);
		ip->set_name("TestScene");
		ip->set("int_property", 45);
		// Create a scene to pack.
		_TestInstancePlaceholderNode *scene = memnew(_TestInstancePlaceholderNode);
		scene->set_int_property(12);

		// Pack the scene.
		PackedScene *packed_scene = memnew(PackedScene);
		packed_scene->pack(scene);

		// Instantiate the scene.
		_TestInstancePlaceholderNode *created = Object::cast_to<_TestInstancePlaceholderNode>(ip->create_instance(true, packed_scene));
		REQUIRE(created != nullptr);
		CHECK(created->get_int_property() == 45);

		root->queue_free();
		memdelete(scene);
	}

	SUBCASE("with node values") {
		InstancePlaceholder *ip = memnew(InstancePlaceholder);
		ip->set_name("TestScene");
		Node *root = memnew(Node);
		Node *overriding = memnew(Node);
		SceneTree::get_singleton()->get_root()->add_child(root);

		root->add_child(ip);
		root->add_child(overriding);
		ip->set("reference_property", overriding);
		// Create a scene to instance.
		_TestInstancePlaceholderNode *scene = memnew(_TestInstancePlaceholderNode);
		Node *referenced = memnew(Node);
		scene->add_child(referenced);
		referenced->set_owner(scene);
		scene->set_reference_property(referenced);
		// Pack the scene.
		PackedScene *packed_scene = memnew(PackedScene);
		const Error err = packed_scene->pack(scene);
		REQUIRE(err == OK);

		// Instantiate the scene.
		_TestInstancePlaceholderNode *created = Object::cast_to<_TestInstancePlaceholderNode>(ip->create_instance(true, packed_scene));
		REQUIRE(created != nullptr);
		CHECK(created->get_name() == "TestScene");
		CHECK(created->get_child_count() == 1);
		CHECK(created->get_reference_property().identity_compare(overriding));
		CHECK_FALSE(created->get_reference_property().identity_compare(referenced));

		root->queue_free();
		memdelete(scene);
	}

	SUBCASE("with node-array value") {
		InstancePlaceholder *ip = memnew(InstancePlaceholder);
		ip->set_name("TestScene");
		Node *root = memnew(Node);
		SceneTree::get_singleton()->get_root()->add_child(root);

		Node *override1 = memnew(Node);
		Node *override2 = memnew(Node);
		Node *override3 = memnew(Node);
		root->add_child(ip);
		root->add_child(override1);
		root->add_child(override2);
		root->add_child(override3);

		Array override_node_array;
		override_node_array.set_typed(Variant::OBJECT, "Node", Variant());
		override_node_array.push_back(override1);
		override_node_array.push_back(override2);
		override_node_array.push_back(override3);

		ip->set("reference_array_property", override_node_array);

		// Create a scene to instance.
		_TestInstancePlaceholderNode *scene = memnew(_TestInstancePlaceholderNode);
		Node *referenced1 = memnew(Node);
		Node *referenced2 = memnew(Node);

		scene->add_child(referenced1);
		scene->add_child(referenced2);

		referenced1->set_owner(scene);
		referenced2->set_owner(scene);
		Array referenced_array;
		referenced_array.set_typed(Variant::OBJECT, "Node", Variant());
		referenced_array.push_back(referenced1);
		referenced_array.push_back(referenced2);

		scene->set_reference_array_property(referenced_array);
		// Pack the scene.
		PackedScene *packed_scene = memnew(PackedScene);
		const Error err = packed_scene->pack(scene);
		REQUIRE(err == OK);

		// Instantiate the scene.
		_TestInstancePlaceholderNode *created = Object::cast_to<_TestInstancePlaceholderNode>(ip->create_instance(true, packed_scene));
		REQUIRE(created != nullptr);
		CHECK(created->get_name() == "TestScene");
		CHECK(created->get_child_count() == 2);
		Array created_array = created->get_reference_array_property();
		REQUIRE_FALSE(created_array.size() == referenced_array.size());
		REQUIRE(created_array.size() == override_node_array.size());
		REQUIRE_FALSE(created_array.size() == created->get_child_count());

		// Iterate over all nodes, since the ordering is not guaranteed.
		for (int i = 0; i < override_node_array.size(); i++) {
			bool node_found = false;
			for (int j = 0; j < created_array.size(); j++) {
				if (override_node_array[i].identity_compare(created_array[j])) {
					node_found = true;
				}
			}
			CHECK(node_found);
		}
		root->queue_free();
		memdelete(scene);
	}
}

} //namespace TestInstancePlaceholder

#endif // TEST_INSTANCE_PLACEHOLDER_H