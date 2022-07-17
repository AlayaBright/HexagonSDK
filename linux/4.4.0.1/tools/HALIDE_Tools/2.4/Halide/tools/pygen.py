import halide as hl
import os
import sys

param_dict = {
    "target": (hl.Target, hl.Target()),
    "auto_schedule": (bool, False),
    "machine_params": (str, "16,16777216,40")
}
target = hl.Target()

def add_param(param, param_type, default_val):
    param_dict[param] = (param_type, default_val)


def get_params():
    generator_params = {}
    for param, value in param_dict.items():
        generator_params[param] = get_param(param, value[0], value[1])

    return generator_params


# to parse the target
def get_target(string):
    assert hl.Target.validate_target_string(string), "Specified target " + string + " invalid"
    # Explicitly modify the global 'target' variable used by compile().
    global target
    target = hl.Target(string)
    return target


def parse_halide_type(string):
    halide_types = {
        "int8": hl.Int(8),
        "int16": hl.Int(16),
        "int32": hl.Int(32),
        "int64": hl.Int(64),
        "uint8": hl.UInt(8),
        "uint16": hl.UInt(16),
        "uint32": hl.UInt(32),
        "uint64": hl.UInt(64),
        "float32": hl.Float(32),
        "float64": hl.Float(64)
    }  # should include more Type
    if string in halide_types.keys():
        return halide_types[string]
    else:
        assert False, "Type " + string + " not supported by Halide"


def parse_int_type(string):
    if string.isnumeric():
        return int(string)
    else:
        assert False, "Expected integer type"


def parse_bool_type(string):
    bool_value = string.lower()
    if bool_value == "true":
        return True
    elif bool_value == "false":
        return False
    else:
        assert False, "Expected bool type"


def parse_string_type(string):
    return string


def parse_float_type(string):
    try:
        float(string)
    except AssertionError:
        print("Expected float type")
    return float(string)


# to parse user-defined generator params
def get_param(param, expected_type, default):
    assert expected_type == type(
        default), "The expected type should match the default type"
    switch = {
        float: parse_float_type,
        bool: parse_bool_type,
        str: parse_string_type,
        int: parse_int_type,
        hl.Type: parse_halide_type,
        hl.Target: get_target
    }
    param_list = sys.argv
    flag = 0
    for x in param_list:
        string = x.split('=')
        if string[0] == param:
            flag = 1
            result = switch[expected_type](string[1])
            return result
    if flag == 0 and string[0] != "target":
        return default
    else:
        assert False, "Target must be specified"


# to generate target specific files
def compile(output, args):
    flag_params = sys.argv
    if "-o" in flag_params:
        outputdir = flag_params[flag_params.index("-o") + 1]
    else:
        assert False, " output directory not specified. Use the -o option"
    if "-e" in flag_params:
        emitops = flag_params[flag_params.index("-e") + 1].split(',')
    else:
        assert False, " emit options not specified. Use the -e option"
    if "-f" in flag_params:
        function_name = flag_params[flag_params.index("-f") + 1]
    else:
        assert False, " function name not specified. Use the -f option"

    emitdict = {}
    emit_options = {
        "cpp": hl.Output.c_source,
        "schedule": hl.Output.schedule,
        "h": hl.Output.c_header,
        "html": hl.Output.stmt_html,
        "o": hl.Output.object,
        "py.c": hl.Output.python_extension
    }

    for x in emitops:
        emitdict[emit_options[x]] = os.path.join(outputdir,
                                                 function_name + "." + x)

    # generate target specific files
    output.compile_to(emitdict, args, function_name, target)

