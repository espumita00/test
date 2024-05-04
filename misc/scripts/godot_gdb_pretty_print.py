# Load this file to your GDB session to enable pretty-printing
# of some Godot C++ types.
# GDB command: source misc/scripts/godot_gdb_pretty_print.py
#
# To load these automatically in Visual Studio Code,
# add the source command to the setupCommands of your configuration
# in launch.json.
# "setupCommands": [
# ...
# {
#     "description": "Load custom pretty-printers for Godot types.",
#     "text": "source ${workspaceRoot}/misc/scripts/godot_gdb_pretty_print.py"
# }
# ]
# Other UI:s that use GDB under the hood are likely to have their own ways to achieve this.
import gdb

# Printer for Godot StringName variables.
class GodotStringNamePrinter:
    def __init__(self, value):
        self.value = value

    def to_string(self):
        return self.value["_data"]["name"]["_cowdata"]["_ptr"]

    def display_hint(self):
        return 'StringName'

# Printer for Godot String variables.
class GodotStringPrinter:
    def __init__(self, value):
        self.value = value

    def to_string(self):
        return self.value["_cowdata"]["_ptr"]

    def display_hint(self):
        return 'String'

# Tries to find a pretty printer for a debugger value.
def lookup_pretty_printer(value):
    if value.type.name == "StringName":
            return GodotStringNamePrinter(value)
    if value.type.name == "String":
            return GodotStringPrinter(value)
    return None

# Register our printer lookup function.
# The first parameter could be used to limit the scope of the printer
# to a specific object file, but that is unnecessary for us.
gdb.printing.register_pretty_printer(None, lookup_pretty_printer)
