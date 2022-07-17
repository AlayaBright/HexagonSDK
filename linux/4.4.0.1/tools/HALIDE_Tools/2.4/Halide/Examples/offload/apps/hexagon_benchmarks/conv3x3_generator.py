# /* ==================================================================================== */
# /*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
# /*                           All Rights Reserved.                                       */
# /*                  QUALCOMM Confidential and Proprietary                               */
# /* ==================================================================================== */

import halide as hl
import pygen


def main():

    input_img = hl.ImageParam(hl.UInt(8), 2)
    bounded_input = hl.Func("bounded_input")
    x, y, yo, xi, yi = hl.Var("x"), hl.Var("y"), hl.Var("yo"),\
		       hl.Var("xi"), hl.Var("yi")
    output = hl.Func("output")
    mask = hl.ImageParam(hl.Int(8), 2)

    pygen.add_param("accumulator_type", hl.Type, hl.Int(16))
    pygen.add_param("use_parallel_sched", bool, True)
    pygen.add_param("use_prefetch_sched", bool, False)
    params = pygen.get_params()

    target = params["target"]

    # the algorithm part
    height = input_img.height()
    width = input_img.dim(0).extent()
    x_min = input_img.dim(0).min()
    bounded_input[x, y] = hl.BoundaryConditions.repeat_edge(input_img)[x, y]

    sum1 = hl.cast(params["accumulator_type"], 0)
    for i in range(-1, 2):
        for j in range(-1, 2):
            sum1 = sum1 + hl.cast(
			    hl.Int(16),hl.Expr(bounded_input[x + j, y + i]))\
			  * hl.cast(hl.Int(16), mask[j + 1, i + 1])

    output[x, y] = hl.cast(hl.UInt(8), hl.clamp(sum1 >> 4, 0, 255))

    input_img.dim(0).set_min(0)
    input_img.dim(1).set_min(0)
    input_stride = input_img.dim(1).stride()

    #the schedule part
    if target.has_feature(hl.TargetFeature.HVX):
        vector_size = 128

        input_img.dim(1).set_stride((input_stride / vector_size) * vector_size)
        ht = input_img.dim(1).extent()

        bounded_input\
        .compute_at(output, y)\
        .align_storage(x, 128)\
        .vectorize(x, vector_size, hl.TailStrategy.RoundUp)

        output.hexagon()\
        .split(y, yo, y, ht/2)\
        .tile(x, y, xi, yi, vector_size, 8, hl.TailStrategy.RoundUp)\
        .vectorize(xi)\
        .unroll(yi)

        if params["use_prefetch_sched"]:
            output.prefetch(input_img, y, 2)

        if params["use_parallel_sched"]:
            output.parallel(yo)

    else:
        vector_size = target.natural_vector_size(hl.UInt(8))
        input_img.dim(1).set_stride((input_stride / vector_size) * vector_size)

        bounded_input\
        .compute_at(output, y)\
        .vectorize(x, vector_size, hl.TailStrategy.RoundUp)

        output\
        .split(y, yo, y, 8)\
        .tile(x, y, xi, yi, vector_size, 8, hl.TailStrategy.RoundUp)\
        .vectorize(xi)\
        .unroll(yi)\
        .parallel(yo)

    args = [input_img, mask]
    pygen.compile(output, args)


if __name__ == "__main__":
    main()

