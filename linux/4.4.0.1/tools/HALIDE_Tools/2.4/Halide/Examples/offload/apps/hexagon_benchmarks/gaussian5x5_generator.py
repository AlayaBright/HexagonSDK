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
    rows, cols, bounded_input = hl.Func("rows"), hl.Func("cols"),\
				hl.Func("bounded_input")
    x, y, xi, yo, yi = hl.Var("x"), hl.Var("y"), hl.Var("xi"),\
		       hl.Var("yo"), hl.Var("yi")

    pygen.add_param("use_parallel_sched", bool, True)
    pygen.add_param("use_prefetch_sched", bool, False)
    params = pygen.get_params()
   
    target = params["target"]
   
    # the algorithm part
    width = input_img.dim(0).extent()
    x_min = input_img.dim(0).min()
    bounded_input[x, y] = hl.BoundaryConditions.repeat_edge(input_img)[x, y]
    input_16 = hl.Func("input_16")
    input_16[x, y] = hl.cast(hl.Int(16), bounded_input[x, y])
    rows[x, y] = input_16[x - 2, y] + 4 * input_16[x - 1, y]\
                 + 6 * input_16[x, y] + 4 * input_16[x + 1, y] + input_16[x + 2, y]
    cols[x, y] = rows[x, y - 2] + 4 * rows[x, y - 1] + 6 * rows[x, y]\
                 + 4 * rows[x, y + 1] + rows[x, y + 2]
    output[x, y] = hl.cast(hl.UInt(8), cols[x, y] >> 8)

    # the schedule part
    input_img.dim(0).set_min(0)
    input_img.dim(1).set_min(0)
    input_stride = input_img.dim(1).stride()

    if target.has_feature(hl.TargetFeature.HVX):
        vector_size = 128

        input_img.dim(1).set_stride((input_stride / vector_size) * vector_size)
        ht = input_img.dim(1).extent()

        output\
        .hexagon()\
        .split(y, yo, y, ht/2)\
        .tile(x, y, xi, yi, vector_size * 2, 4, hl.TailStrategy.RoundUp)\
        .unroll(yi)\
        .vectorize(xi)

        rows\
        .compute_at(output, y)\
        .store_at(output, yo)\
        .align_storage(x, vector_size)\
        .vectorize(x, vector_size, hl.TailStrategy.RoundUp)

        if params["use_prefetch_sched"]:
            output.prefetch(input_img, y, 2)

        if params["use_parallel_sched"]:
            output.parallel(yo)

    else:
        vector_size = target.natural_vector_size(hl.UInt(8))
        input_img.dim(1).set_stride((input_stride / vector_size) * vector_size)

        output\
        .split(y, yo, y, 16)\
        .tile(x, y, xi, yi, vector_size, 4, hl.TailStrategy.RoundUp)\
        .unroll(yi)\
        .parallel(yo)\
        .vectorize(xi)

        rows\
        .compute_at(output, y)\
        .store_at(output, yo)\
        .vectorize(x, vector_size, hl.TailStrategy.RoundUp)

    args = [input_img]
    pygen.compile(output, args)


if __name__ == "__main__":
    main()

