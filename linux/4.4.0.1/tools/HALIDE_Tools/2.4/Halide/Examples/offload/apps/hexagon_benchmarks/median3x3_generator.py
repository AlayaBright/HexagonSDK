# /* ==================================================================================== */
# /*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
# /*                           All Rights Reserved.                                       */
# /*                  QUALCOMM Confidential and Proprietary                               */
# /* ==================================================================================== */

import halide as hl
import pygen


def mid(a, b, c):
    return hl.max(hl.min(hl.max(a, b), c), hl.min(a, b))


def main():
    input_img = hl.ImageParam(hl.UInt(8), 2)
    output = hl.Func("output")
    x, y, yo, xi, yi = hl.Var("x"), hl.Var("y"), hl.Var("yo"), hl.Var("xi"),\
		       hl.Var("yi")
    max_y, min_y, mid_y = hl.Func("max_y"), hl.Func("min_y"), hl.Func("mid_y")
    minmax_x, maxmin_x, midmid_x = hl.Func("minmax_x"), hl.Func("maxmin_x"),\
				   hl.Func("midmid_x")
    bounded_input = hl.Func("bounded_input")

    pygen.add_param("use_parallel_sched", bool, True)
    pygen.add_param("use_prefetch_sched", bool, False)
    params = pygen.get_params()

    target = params["target"]

    # the algorithm part
    height = input_img.height()
    width = input_img.dim(0).extent()
    x_min = input_img.dim(0).min()
    bounded_input[x, y] = hl.BoundaryConditions.repeat_edge(input_img)[x, y]

    max_y[x, y] = hl.max(bounded_input[x, y - 1], bounded_input[x, y],\
                         bounded_input[x, y + 1])
    min_y[x, y] = hl.min(bounded_input[x, y - 1], bounded_input[x, y],\
                         bounded_input[x, y + 1])
    mid_y[x, y] = mid(bounded_input[x, y - 1], bounded_input[x, y],\
                         bounded_input[x, y + 1])

    minmax_x[x, y] = hl.min(max_y[x - 1, y], max_y[x, y], max_y[x + 1, y])
    maxmin_x[x, y] = hl.max(min_y[x - 1, y], min_y[x, y], min_y[x + 1, y])
    midmid_x[x, y] = mid(mid_y[x - 1, y], mid_y[x, y], mid_y[x + 1, y])

    output[x, y] = mid(minmax_x[x, y], maxmin_x[x, y], midmid_x[x, y])

    # the schedule part
    input_img.dim(0).set_min(0)
    input_img.dim(1).set_min(0)
    input_stride = input_img.dim(1).stride()

    if target.has_feature(hl.TargetFeature.HVX):
        vector_size = 128

        input_img.dim(1).set_stride((input_stride / vector_size) * vector_size)
        ht = input_img.dim(1).extent()

        bounded_input\
        .compute_at(output, y)\
        .align_storage(x, 128)\
        .vectorize(x, vector_size, hl.TailStrategy.RoundUp)

        output\
        .hexagon()\
        .split(y, yo, y, ht/2)\
        .tile(x, y, xi, yi, vector_size * 2, 8, hl.TailStrategy.RoundUp)\
        .vectorize(xi)\
        .unroll(yi)

        if params["use_prefetch_sched"]:
            output.prefetch(input_img, y, 2)

        if params["use_parallel_sched"]:
            output.parallel(yo)

    else:
        vector_size = hl.Target.natural_vector_size(hl.UInt(8))
        input_img.dim(1).set_stride((input_stride / vector_size) * vector_size)

        bounded_input\
        .compute_at(output, y)\
        .vectorize(x, vector_size, hl.TailStrategy.RoundUp)

        output\
        .split(y, yo, y, 16)\
        .tile(x, y, xi, yi, vector_size, 4)\
        .vectorize(xi)\
        .parallel(yo)\
        .unroll(yi)

    args = [input_img]
    pygen.compile(output, args)


if __name__ == "__main__":
    main()

