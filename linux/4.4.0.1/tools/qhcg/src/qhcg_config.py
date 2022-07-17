from enum import Enum, unique

# Enum type for all classes which uses any type of input or output data
bit16 = 16
bit32 = 32
# "b" and "f" chars for adding
bin_str = "b"
float_str = "f"


@unique
class Optimize_by(Enum):
    '''
        qhcg generated functions can be optimized either for accuracy
        or speed variants.  Optimize_by enums defines these options.
    '''
    accuracy = "accuracy"
    speed = "speed"

    def __str__(self):
        return '{0}'.format(self.value)


@unique
class Input_types(Enum):
    '''
        qhcg supports input range and takes in data values in certain types
        This enum defines supported data types.   Currently we support int
        and float data types of sizes 16 and 32 bits.
    '''
    int16 = "int16"
    int32 = "int32"
    float16 = "float16"
    float32 = "float32"

    def __str__(self):
        return '{0}'.format(self.value)

    @staticmethod
    def input_type_to_bit_size(input_types: 'Input_types') -> int:
        if ((input_types == Input_types.int16) or (input_types == Input_types.float16)):
            return bit16
        if ((input_types == Input_types.int32) or (input_types == Input_types.float32)):
            return bit32

    @staticmethod
    def input_type_to_bit_size_str(input_types: 'Input_types') -> str:
        if ((input_types == Input_types.int16) or (input_types == Input_types.int32)):
            return bin_str
        if ((input_types == Input_types.float16) or (input_types == Input_types.float32)):
            return float_str


@unique
class Output_types(Enum):
    '''
       qhcg supports output data types of int and float of size 16 and 32.
       Output_types enum tracks the various supported output data types.
    '''
    int16 = "int16"
    int32 = "int32"
    float16 = "float16"
    float32 = "float32"

    def __str__(self):
        return '{0}'.format(self.value)

    @staticmethod
    def output_type_to_bit_size(output_types: 'Output_types') -> int:
        if ((output_types == Output_types.int16) or (output_types == Output_types.float16)):
            return bit16
        if ((output_types == Output_types.int32) or (output_types == Output_types.float32)):
            return bit32

    @staticmethod
    def output_type_to_bit_size_str(output_types: 'Output_types') -> str:
        if ((output_types == Output_types.int16) or (output_types == Output_types.int32)):
            return bin_str
        if ((output_types == Output_types.float16) or (output_types == Output_types.float32)):
            return float_str


@unique
class Function_parser_methods(Enum):
    '''
       qhcg uses various fit methods to generate polynomial approximations of
       user input non-linear functions.  These fit methods require data samples
       within input range.  We can generate these samples are either linearly spaced
       or randomly distributed.

       Function_parser_method enum tracks the supported input sample generation
       mechanism
    '''
    linspace = "linspace"
    random = "random"

    def __str__(self):
        return '{0}'.format(self.value)


@unique
class Modes_process_output(Enum):
    read_input = "in_"
    read_HVX = "hvx_"
    read_np = "np_"

    def __str__(self):
        return '{0}'.format(self.value)

# Outputer Enums are missing


@unique
class Approximation_methods(Enum):
    '''
       qhcg chooses the best fit method on a segment basis.

       Approximation_methods enum defines currently supported fit methods.
    '''
    polyfit = "polyfit"
    chebfit = "chebfit"
    legfit = "legfit"
    lagfit = "lagfit"
    hermfit = "hermfit"
    fit = "fit"

    def __str__(self):
        return '{0}'.format(self.value)


@unique
class Approximation_norm(Enum):
    '''
        qhcg uses different normalization to compute approximation
        error between actual user input function and polynomial
        approximated function.

        The Approximation_norm defines different normalization that
        can be used in computing error function.
    '''
    min_norm = "min"
    rms_norm = "rms"
    mean_norm = "norm"

    def __str__(self):
        return '{0}'.format(self.value)


# Function which returns list of available methods and options regarding given Enum
def list_of_enum_options(input_enum: Enum) -> list:
    return [item for item in input_enum]


# Function which returns list of available methods and options regarding given Enum
def list_of_enum_options_in_str(input_enum: Enum) -> list:
    return [item.value for item in input_enum]
