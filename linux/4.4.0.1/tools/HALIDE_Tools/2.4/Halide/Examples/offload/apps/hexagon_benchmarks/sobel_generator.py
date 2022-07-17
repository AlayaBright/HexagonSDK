# /* ==================================================================================== */
# /*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
# /*                           All Rights Reserved.                                       */
# /*                  QUALCOMM Confidential and Proprietary                               */
# /* ==================================================================================== */

import halide as hl
import pygen


def main():
    input_img = hl.ImageParam(hl.UInt(8), 2)
    output = hl.Func("output")
    x, y, xi, yi, yo = hl.Var("x"), hl.Var("y"), hl.Var("xi"),\
		       hl.Var("yi"), hl.Var("yo")
    sobel_x_avg, sobel_x, sobel_y, sobel_y_avg = hl.Func("sobel_x_avg"),\
		hl.Func("sobel_x"), hl.Func("sobel_y"), hl.Func("sobel_y_avg")

    pygen.add_param("use_parallel_sched", bool, True)
    pygen.add_param("use_prefetch_sched", bool, False)
    params = pygen.get_params()
    target = params["target"]

    # the algorithm part
    height = input_img.height()
    width = input_img.dim(0).extent()
    x_min = input_img.dim(0).min()
    bounded_input = hl.Func("bounded_input")
    bounded_input[x, y] = hl.BoundaryConditions.repeat_edge(input_img)[x, y]

    input_16 = hl.Func("input_16")
    input_16[x, y] = hl.cast(hl.UInt(16), bounded_input[x, y])

    sobel_x_avg[
        x, y] = input_16[x - 1, y] + 2 * input_16[x, y] + input_16[x + 1, y]
    sobel_x[x, y] = hl.absd(sobel_x_avg[x, y - 1], sobel_x_avg[x, y + 1])
    sobel_y_avg[
        x, y] = input_16[x, y - 1] + 2 * input_16[x, y] + input_16[x, y + 1]
    sobel_y[x, y] = hl.absd(sobel_y_avg[x - 1, y], sobel_y_avg[x + 1, y])
    output[x, y] = hl.cast(
        hl.UInt(8), hl.clamp(sobel_x[x, y] + sobel_y[x, y], 0, 255))

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
        vector_size = target.natural_vector_size(hl.UInt(8))
        input_img.dim(1).set_stride((input_stride / vector_size) * vector_size)

        bounded_input\
        .compute_at(output, y)\
        .vectorize(x, vector_size, hl.TailStrategy.RoundUp)

        output\
        .split(y, yo, y, 16)\
        .tile(x, y, xi, yi, vector_size * 2, 4, hl.TailStrategy.RoundUp)\
        .vectorize(xi)\
        .parallel(yo)\
        .unroll(yi)

    args = [input_img]
    pygen.compile(output, args)


if __name__ == "__main__":
    main()
