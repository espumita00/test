# This file is for building as a Godot module.
def can_build(env, platform):
    return True


def configure(env):
    pass


def get_doc_classes():
    return [
        "ExampleNode",
    ]


def get_doc_path():
    return "addons/__BASE_NAME__/doc_classes"


def get_icons_path():
    return "addons/__BASE_NAME__/icons"
