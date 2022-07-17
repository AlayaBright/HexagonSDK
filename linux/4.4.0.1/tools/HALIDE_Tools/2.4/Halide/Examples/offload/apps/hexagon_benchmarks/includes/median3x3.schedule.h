/* ==================================================================================== */
/*     Copyright (c) 2016-2021 QUALCOMM Technologies, Inc. and/or its subsidiaries.     */
/*                           All Rights Reserved.                                       */
/*                  QUALCOMM Confidential and Proprietary                               */
/* ==================================================================================== */

#ifndef median3x3_SCHEDULE_H
#define median3x3_SCHEDULE_H

// MACHINE GENERATED -- DO NOT EDIT
// This schedule was automatically generated by Adams2019
// for target=arm-64-android-hvx-hvx_128  // NOLINT
// with machine_params=16,24000000,40

#include "Halide.h"


inline void apply_schedule_median3x3(
    ::Halide::Pipeline pipeline,
    ::Halide::Target target
) {
    using ::Halide::Func;
    using ::Halide::MemoryType;
    using ::Halide::RVar;
    using ::Halide::TailStrategy;
    using ::Halide::Var;
    Func output = pipeline.get_func(10);
    Func minmax_x = pipeline.get_func(9);
    Func max_y = pipeline.get_func(8);
    Func midmid_x = pipeline.get_func(7);
    Func mid_y = pipeline.get_func(6);
    Func maxmin_x = pipeline.get_func(5);
    Func min_y = pipeline.get_func(4);
    Func bounded_input = pipeline.get_func(3);
    Func repeat_edge = pipeline.get_func(2);
    Func lambda_0 = pipeline.get_func(1);
    Func input_im = pipeline.get_func(0);
    Var _0(repeat_edge.get_schedule().dims()[0].var);
    Var _0i("_0i");
    Var _1(repeat_edge.get_schedule().dims()[1].var);
    Var x(output.get_schedule().dims()[0].var);
    Var xi("xi");
    Var xii("xii");
    Var xiii("xiii");
    Var y(output.get_schedule().dims()[1].var);
    Var yi("yi");
    Var yii("yii");
    output
        .split(y, y, yi, 9, TailStrategy::ShiftInwards)
        .split(yi, yi, yii, 3, TailStrategy::ShiftInwards)
        .split(x, x, xi, 1920, TailStrategy::ShiftInwards)
        .split(xi, xi, xii, 1024, TailStrategy::ShiftInwards)
        .split(xii, xii, xiii, 128, TailStrategy::ShiftInwards)
        .unroll(xii)
        .vectorize(xiii)
        .align_storage(x, 128)
        .compute_root()
        .hexagon()
        .reorder({xiii, xii, xi, yii, x, yi, y})
        .parallel(y)
        .prefetch(input_im, yi, 2);
    minmax_x
        .store_in(MemoryType::Stack)
        .split(x, x, xi, 384, TailStrategy::RoundUp)
        .split(xi, xi, xii, 128, TailStrategy::RoundUp)
        .unroll(xi)
        .unroll(y)
        .vectorize(xii)
        .align_storage(x, 128)
        .compute_at(output, yi)
        .reorder({xii, xi, y, x});
    max_y
        .store_in(MemoryType::Stack)
        .split(x, x, xi, 128, TailStrategy::RoundUp)
        .unroll(x)
        .unroll(y)
        .vectorize(xi)
        .align_storage(x, 128)
        .compute_at(minmax_x, x)
        .reorder({xi, x, y});
    mid_y
        .store_in(MemoryType::Stack)
        .split(x, x, xi, 128, TailStrategy::RoundUp)
        .vectorize(xi)
        .align_storage(x, 128)
        .compute_at(output, yii)
        .store_at(output, x)
        .reorder({xi, x, y});
    maxmin_x
        .store_in(MemoryType::Stack)
        .split(x, x, xi, 128, TailStrategy::RoundUp)
        .unroll(x)
        .vectorize(xi)
        .align_storage(x, 128)
        .compute_at(output, xi)
        .reorder({xi, x, y});
    min_y
        .store_in(MemoryType::Stack)
        .split(x, x, xi, 128, TailStrategy::RoundUp)
        .vectorize(xi)
        .align_storage(x, 128)
        .compute_at(output, yii)
        .store_at(output, x)
        .reorder({xi, x, y});
    repeat_edge
        .store_in(MemoryType::Stack)
        .split(_0, _0, _0i, 128, TailStrategy::ShiftInwards)
        .vectorize(_0i)
        .align_storage(_0, 128)
        .compute_at(output, yi)
        .store_at(output, y)
        .reorder({_0i, _0, _1});

}

#endif  // median3x3_SCHEDULE_H