# /* ==================================================================================== */
# /*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
# /*                           All Rights Reserved.                                       */
# /*                  QUALCOMM Confidential and Proprietary                               */
# /* ==================================================================================== */

import halide as hl
import pygen


def main():

    input_img = hl.ImageParam(hl.UInt(8), 2)
    x, y, yo = hl.Var("x"), hl.Var("y"), hl.Var("yo")
    bounded_input = hl.Func("bounded_input")
    max_y = hl.Func("max_y")
    output = hl.Func("output")

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
    output[x, y] = hl.max(max_y[x - 1, y], max_y[x, y], max_y[x + 1, y])

    # the schedule part
    xi, yi = hl.Var("xi"), hl.Var("yi")
    input_img.dim(0).set_min(0)
    input_img.dim(1).set_min(0)
    input_stride = input_img.dim(1).stride()

    if target.has_feature(hl.TargetFeature.HVX):
        vector_size = 128

        input_img.dim(1).set_stride((input_stride / vector_size) * vector_size)
        input_img.set_host_alignment(128)
        ht = input_img.dim(1).extent()

        bounded_input\
        .compute_at(output, y)\
        .align_storage(x, 128)\
        .vectorize(x, vector_size * 2, hl.TailStrategy.RoundUp)

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
        vector_size = target.natural_vector_size(hl.Int(8))
        input_img.dim(1).set_stride((input_stride / vector_size) * vector_size)

        bounded_input\
        .compute_at(output, y)\
        .vectorize(x, vector_size, hl.TailStrategy.RoundUp)

        output\
        .split(y, yo, y, 16)\
        .tile(x, y, xi, yi, vector_size, 4, hl.TailStrategy.RoundUp)\
        .vectorize(xi)\
        .unroll(yi)\
        .parallel(yo)

    args = [input_img]
    pygen.compile(output, args)


if __name__ == "__main__":
    main()

