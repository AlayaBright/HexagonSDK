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
    max_y, min_y, mx, mn, fin, bounded_input = hl.Func("max_y"), hl.Func("min_y"),\
				hl.Func("mx"), hl.Func("mn"), hl.Func("fin"), hl.Func("bounded_input")
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

    max_y[x, y] = hl.max(bounded_input[x, y - 1], bounded_input[x, y],\
                        bounded_input[x, y + 1])
    min_y[x, y] = hl.min(bounded_input[x, y - 1], bounded_input[x, y],\
                        bounded_input[x, y + 1])

    mx[x, y] = hl.max(max_y[x - 1, y], max_y[x, y], max_y[x + 1, y])
    mn[x, y] = hl.min(min_y[x - 1, y], min_y[x, y], min_y[x + 1, y])
    fin[x, y] = hl.select((mx[x, y] - mn[x, y]) <= 25, 0, (mx[x, y] - mn[x, y]))
    output[x,y] = hl.cast(hl.UInt(8),(hl.clamp(2 * fin[x, y], 0, 255)))

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
        .tile(x, y, xi, yi, vector_size , 8, hl.TailStrategy.RoundUp)\
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
        .tile(x, y, xi, yi, vector_size, 4, hl.TailStrategy.RoundUp)\
        .unroll(yi)\
        .parallel(yo)\
        .vectorize(xi)

    args = [input_img]
    pygen.compile(output, args)


if __name__ == "__main__":
    main()
