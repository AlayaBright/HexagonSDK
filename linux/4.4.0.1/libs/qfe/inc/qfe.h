/**=============================================================================

@file
    qfe.h

@brief
    Header file of QHMATH library.

Copyright (c) 2020 QUALCOMM Technologies Incorporated.
All Rights Reserved Qualcomm Proprietary
=============================================================================**/

#include <hexagon_protos.h>
#include <stdarg.h>

#ifndef qfe_H
#define qfe_H

// Cross-Platform Macros
#if defined(_MSC_VER)
#define _ALIGNED_ATTRIBUTE_(x) __declspec(align(x))
#else
#define _ALIGNED_ATTRIBUTE_(x) __attribute__((aligned(x)))
#endif


/** @defgroup Types QFE and QFES types
  * @{
  */

typedef uint32_t qf32_t;      /**< 32-bit QF type */
typedef uint16_t qf16_t;      /**< 16-bit QF type */
typedef uint16_t _Float16;    /**< 16-bit IEEE float (interpreted with libnative since not supported by gcc) */

typedef union
{
    HVX_Vector v;
    qf16_t qf16;
    qf32_t qf32;
    _Float16 hf;
    float sf;
} QFE_V;

typedef qf16_t (*fptr_hf) (_Float16*, ...); /**<Function pointer for qhmath_hvx functions returning 16-bit float */
typedef qf32_t (*fptr_sf) (float*, ...);   /**<Function pointer for qhmath_hvx functions returning 32-bit float */

//wrapper function for scalar->scalar qhmath_hvx functions
qf16_t qfes_qf16_generic_function(fptr_hf qhmath_func_ptr, qf16_t a, qf16_t b, int num_args);

//wrapper function for scalar->scalar qhmath_hvx functions
qf32_t qfes_qf32_generic_function(fptr_sf qhmath_func_ptr, qf32_t a, qf32_t b, int num_args);

/**
 *
 * Supported QFE types
 *
**/
typedef enum qfe_type{
    SF,    /**< IEEE float */
    HF,    /**< IEEE half-float */
    QF32,  /**< 32-bit QF */
    QF16   /**< 16-bit QF */
} qfe_type;

/**
 *
 * Generic QF type.
 *
 * Used to track side by side reference values resulting from implementing the entire data flow with one unique type,
 * and actual values, resulting from using the actual specified operations.
 *
**/
typedef struct qf_t {

    /**
      * @name Reference types
      */
    double ref_df;    /**< Reference IEEE 64-bit float */
    float ref_sf;     /**< Reference IEEE 32-bit float */
    qf32_t ref_qf32;  /**< Reference 32-bit QF */
    qf16_t ref_qf16;  /**< Reference 16-bit QF */

    /**
      * @name Actual types
      */
    float actual_sf;       /**< Actual IEEE 32-bit float */
    _Float16 actual_hf;    /**< Actual IEEE 16-bit float */
    qf32_t actual_qf32;    /**< Actual 32-bit QF */
    qf16_t actual_qf16;    /**< Actual 16-bit QF */
    qfe_type actual_type;  /**< Type of the actual value */
} qf_t;

/**
 * @}
 */


/** @defgroup QFES_functions Operations consuming and producing single values
  * @{
  */

/**
 * @brief       Converts an IEEE 32-bit float number to an IEEE 16-bit float number.
 * @param[in]   sf         Input number.
 * @return      Converted number.
 */
_Float16 qfes_hf_equals_sf(float sf);

/**
 * @brief       Converts a QF16 number to an IEEE 16-bit float number.
 * @param[in]   in         Input number.
 * @return      Converted number.
 */
_Float16 qfes_hf_equals_qf16(qf16_t in);

/**
 * @brief       Converts a QF32 number to an IEEE 16-bit float number.
 * @param[in]   in         Input number.
 * @return      Converted number.
 */
_Float16 qfes_hf_equals_qf32(qf32_t in);

/**
 * @brief       Converts an IEEE 16-bit float number to an IEEE 32-bit float number.
 * @param[in]   hf         Input number.
 * @return      Converted number.
 */
float qfes_sf_equals_hf(_Float16 hf);

/**
 * @brief       Converts a QF16 number to an IEEE 32-bit float number.
 * @param[in]   in         Input number.
 * @return      Converted number.
 */
float qfes_sf_equals_qf16(qf16_t in);

/**
 * @brief       Converts a QF32 number to an IEEE 32-bit float number.
 * @param[in]   in         Input number.
 * @return      Converted number.
 */
float qfes_sf_equals_qf32(qf32_t in);

/**
 * @brief       Converts an IEEE 16-bit float number to a QF16 number.
 * @param[in]   hf         Input number.
 * @return      Converted number.
 */
qf16_t qfes_qf16_equals_hf(_Float16 hf);

/**
 * @brief       Converts an IEEE 32-bit float number to a QF16 number.
 * @param[in]   sf         Input number.
 * @return      Converted number.
 */
qf16_t qfes_qf16_equals_sf(float sf);

/**
 * @brief       Converts a QF32 number to a QF16 number.
 * @param[in]   in         Input number.
 * @return      Converted number.
 */
qf16_t qfes_qf16_equals_qf32(qf32_t in);

/**
 * @brief       Converts an IEEE 16-bit float number to a QF32 number.
 * @param[in]   hf         Input number.
 * @return      Converted number.
 */
qf32_t qfes_qf32_equals_hf(_Float16 hf);

/**
 * @brief       Converts an IEEE 32-bit float number to a QF32 number.
 * @param[in]   sf         Input number.
 * @return      Converted number.
 */
qf32_t qfes_qf32_equals_sf(float sf);

/**
 * @brief       Converts an QF16 number to a QF32 number.
 * @param[in]   in         Input number.
 * @return      Converted number.
 */
qf32_t qfes_qf32_equals_qf16(qf16_t sf);


/**
 * @brief       Adds QF16, QF16 -> QF16.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Sum of both operands.
 */
qf16_t qfes_qf16_add_qf16qf16(qf16_t a, qf16_t b);

/**
 * @brief       Adds QF16, HF -> QF16.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Sum of both operands.
 */
qf16_t qfes_qf16_add_qf16hf(qf16_t a, _Float16 b);

/**
 * @brief       Adds HF, HF -> QF16.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Sum of both operands.
 */
qf16_t qfes_qf16_add_hfhf(_Float16 a, _Float16 b);


/**
 * @brief       Adds QF32, QF32 -> QF32.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Sum of both operands.
 */
qf32_t qfes_qf32_add_qf32qf32(qf32_t a, qf32_t b);

/**
 * @brief       Adds QF32, SF -> QF32.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Sum of both operands.
 */
qf32_t qfes_qf32_add_qf32sf(qf32_t a, float b);

/**
 * @brief       Adds SF, SF -> QF32.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Sum of both operands.
 */
qf32_t qfes_qf32_add_sfsf(float a, float b);


/**
 * @brief       Subtracts QF16, QF16 -> QF16.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Sum of both operands.
 */
qf16_t qfes_qf16_sub_qf16qf16(qf16_t a, qf16_t b);

/**
 * @brief       Subtracts QF16, HF -> QF16.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Difference of both operands.
 */
qf16_t qfes_qf16_sub_qf16hf(qf16_t a, _Float16 b);

/**
 * @brief       Subtracts HF, HF -> QF16.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Difference of both operands.
 */
qf16_t qfes_qf16_sub_hfhf(_Float16 a, _Float16 b);


/**
 * @brief       Subtracts QF32, QF32 -> QF32.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Difference of both operands.
 */
qf32_t qfes_qf32_sub_qf32qf32(qf32_t a, qf32_t b);

/**
 * @brief       Subtracts QF32, SF -> QF32.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Difference of both operands.
 */
qf32_t qfes_qf32_sub_qf32sf(qf32_t a, float b);

/**
 * @brief       Subtracts SF, SF -> QF32.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Difference of both operands.
 */
qf32_t qfes_qf32_sub_sfsf(float a, float b);


/**
 * @brief       Multiplies QF16, QF16 -> QF16.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Product of both operands.
 */
qf16_t qfes_qf16_mpy_qf16qf16(qf16_t a, qf16_t b);

/**
 * @brief       Multiplies QF16, HF -> QF16.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Product of both operands.
 */
qf16_t qfes_qf16_mpy_qf16hf(qf16_t a, _Float16 b);

/**
 * @brief       Multiplies HF, HF -> QF16.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Product of both operands.
 */
qf16_t qfes_qf16_mpy_hfhf(_Float16 a, _Float16 b);

/**
 * @brief       Multiplies QF16, QF16 -> QF32.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Product of both operands.
 */
qf32_t qfes_qf32_mpy_qf16qf16(qf16_t a, qf16_t b);

/**
 * @brief       Multiplies QF16, HF -> QF32.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Product of both operands.
 */
qf32_t qfes_qf32_mpy_qf16hf(qf16_t a, _Float16 b);

/**
 * @brief       Multiplies HF, HF -> QF32.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Product of both operands.
 */
qf32_t qfes_qf32_mpy_hfhf(_Float16 a, _Float16 b);

/**
 * @brief       Multiplies QF32, QF32 -> QF32.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Product of both operands.
 */
qf32_t qfes_qf32_mpy_qf32qf32(qf32_t a, qf32_t b);

/**
 * @brief       Multiplies QF32, SF -> QF32.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Product of both operands.
 */
qf32_t qfes_qf32_mpy_qf32sf(qf32_t a, float b);

/**
 * @brief       Multiplies SF, SF -> QF32.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Product of both operands.
 */
qf32_t qfes_qf32_mpy_sfsf(float a, float b);


/**
 * @brief       Divides QF16, QF16 -> QF16.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Quotient of first operand divided by second operand.
 */
qf16_t qfes_qf16_div_qf16qf16(qf16_t a, qf16_t b);

/**
 * @brief       Divides QF32, QF32 -> QF32.
 * @param[in]   a         First operand.
 * @param[in]   b         Second operand.
 * @return      Quotient of first operand divided by second operand.
 */
qf32_t qfes_qf32_div_qf32qf32(qf32_t a, qf32_t b);


/**
 * @brief       Computes multiplicative inverse of QF16 -> QF16.
 * @param[in]   a         First operand.
 * @return      Multiplicative inverse of the operand.
 */
qf16_t qfes_qf16_inv_qf16(qf16_t a);

/**
 * @brief       Computes multiplicative inverse of QF32 -> QF32.
 * @param[in]   a         First operand.
 * @return      Multiplicative inverse of the operand.
 */
qf32_t qfes_qf32_inv_qf32(qf32_t a);


/**
 * @brief       Displays a QF16 number as both the float value it represents and its binary form.
 *
 * The QF16 number is converted to a float number using qfes_sf_equals_qf16(). Then printf is invoked
 * to return a pair of numbers as follows: (float value,hexadecimal representation of QF16)
 *
 * @param[in]   in      QF16 number.
 */
void qfes_printf_qf16(qf16_t in);

/**
 * @brief       Appends to a string a QF16 number as both the float value it represents and its binary form.
 *
 * The QF16 number is converted to a float number using qfes_sf_equals_qf16(). Then printf is invoked
 * to return a pair of numbers as follows: (float value,hexadecimal representation of QF16)
 *
 * @param[in]   string      String to which to append the QF16 string representation.
 * @param[in]   in          QF16 input value.
 */
void qfes_strcat_qf16(char* string, qf16_t in);

/**
 * @brief       Displays a QF32 number as both the float value it represents and its binary form.
 *
 * The QF32 number is converted to a float number using qfes_sf_equals_qf32(). Then printf is invoked
 * to return a pair of numbers as follows: (float value,hexadecimal representation of QF32)
 *
 * @param[in]   in      QF32 number.
 */
void qfes_printf_qf32(qf32_t in);

/**
 * @brief       Appends to a string a QF32 number as both the float value it represents and its binary form.
 *
 * The QF32 number is converted to a float number using qfes_sf_equals_qf32(). Then printf is invoked
 * to return a pair of numbers as follows: (float value,hexadecimal representation of QF32)
 *
 * @param[in]   string      String to which to append the QF32 string representation.
 * @param[in]   in          QF32 input value.
 */
void qfes_strcat_qf32(char* string, qf32_t in);

/**
 * @}
 */

/** @defgroup QFE_functions  Operations consuming and producing QF values
  * @{
  */

/**
 * @brief       Converts an IEEE 32-bit float number to an IEEE 16-bit float number.
 * @param[in]   sf         Input number.
 * @return      Converted number.
 */
void qfe_init_sf(float init_value, qf_t* out);


/**
 * @brief       Converts an IEEE 32-bit float number to an IEEE 16-bit float number.
 *
 * The QF actual @qfe_type is updated to @HF and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    in          Input.
 * @param[out]   out         Output.
 */
void qfe_hf_equals_sf(qf_t in, qf_t* out);

/**
 * @brief       Converts a QF16 number to an IEEE 16-bit float number.
 *
 * The QF actual @qfe_type is updated to @HF and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    in          Input.
 * @param[out]   out         Output.
 */
void qfe_hf_equals_qf16(qf_t in, qf_t* out);

/**
 * @brief       Converts a QF32 number to an IEEE 16-bit float number.
 *
 * The QF actual @qfe_type is updated to @HF and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    in          Input.
 * @param[out]   out         Output.
 */
void qfe_hf_equals_qf32(qf_t in, qf_t* out);

/**
 * @brief       Converts an IEEE 32-bit float number to an IEEE 32-bit float number.
 *
 * The QF actual @qfe_type is updated to @SF and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    in          Input.
 * @param[out]   out         Output.
 */
void qfe_sf_equals_hf(qf_t in, qf_t* out);

/**
 * @brief       Converts a QF16 number to an IEEE 32-bit float number.
 *
 * The QF actual @qfe_type is updated to @SF and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    in          Input.
 * @param[out]   out         Output.
 */
void qfe_sf_equals_qf16(qf_t in, qf_t* out);

/**
 * @brief       Converts a QF32 number to an IEEE 32-bit float number.
 *
 * The QF actual @qfe_type is updated to @SF and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    in          Input.
 * @param[out]   out         Output.
 */
void qfe_sf_equals_qf32(qf_t in, qf_t* out);

/**
 * @brief       Converts an IEEE 16-bit float number to a QF16 number.
 *
 * The QF actual @qfe_type is updated to @QF16 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    in          Input.
 * @param[out]   out         Output.
 */
void qfe_qf16_equals_hf(qf_t in, qf_t* out);

/**
 * @brief       Converts an IEEE 32-bit float number to a QF16 number.
 *
 * The QF actual @qfe_type is updated to @QF16 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    in          Input.
 * @param[out]   out         Output.
 */
void qfe_qf16_equals_sf(qf_t in, qf_t* out);

/**
 * @brief       Converts a QF32 number to a QF16 number.
 *
 * The QF actual @qfe_type is updated to @QF16 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    in          Input.
 * @param[out]   out         Output.
 */
void qfe_qf16_equals_qf32(qf_t in, qf_t* out);

/**
 * @brief       Converts an IEEE 16-bit float number to a QF32 number.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    in          Input.
 * @param[out]   out         Output.
 */
void qfe_qf32_equals_hf(qf_t in, qf_t* out);

/**
 * @brief       Converts an IEEE 32-bit float number to a QF32 number.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    in          Input.
 * @param[out]   out         Output.
 */
void qfe_qf32_equals_sf(qf_t in, qf_t* out);

/**
 * @brief       Converts a QF16 number to a QF32 number.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    in          Input.
 * @param[out]   out         Output.
 */
void qfe_qf32_equals_qf16(qf_t in, qf_t* out);


/**
 * @brief       Adds QF16, QF16 -> QF16.
 *
 * The QF actual @qfe_type is updated to @QF16 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf16_add_qf16qf16(qf_t a, qf_t b, qf_t* out);

/**
 * @brief       Adds QF16, HF -> QF16.
 *
 * The QF actual @qfe_type is updated to @QF16 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf16_add_qf16hf(qf_t a, qf_t b, qf_t* out);

/**
 * @brief       Adds HF, HF -> QF16.
 *
 * The QF actual @qfe_type is updated to @QF16 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf16_add_hfhf(qf_t a, qf_t b, qf_t* out);


/**
 * @brief       Adds QF32, QF32 -> QF32.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf32_add_qf32qf32(qf_t a, qf_t b, qf_t* out);

/**
 * @brief       Adds QF32, SF -> QF32.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf32_add_qf32sf(qf_t a, qf_t b, qf_t* out);

/**
 * @brief       Adds SF, SF -> QF32.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf32_add_sfsf(qf_t a, qf_t b, qf_t* out);


/**
 * @brief       Subtracts QF16, QF16 -> QF16.
 *
 * The QF actual @qfe_type is updated to @QF16 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf16_sub_qf16qf16(qf_t a, qf_t b, qf_t* out);

/**
 * @brief       Subtracts QF16, HF -> QF16.
 *
 * The QF actual @qfe_type is updated to @QF16 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf16_sub_qf16hf(qf_t a, qf_t b, qf_t* out);

/**
 * @brief       Subtracts HF, HF -> QF16.
 *
 * The QF actual @qfe_type is updated to @QF16 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf16_sub_hfhf(qf_t a, qf_t b, qf_t* out);


/**
 * @brief       Subtracts QF32, QF32 -> QF32.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf32_sub_qf32qf32(qf_t a, qf_t b, qf_t* out);

/**
 * @brief       Subtracts QF32, SF -> QF32.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf32_sub_qf32sf(qf_t a, qf_t b, qf_t* out);

/**
 * @brief       Subtracts SF, SF -> QF32.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf32_sub_sfsf(qf_t a, qf_t b, qf_t* out);


/**
 * @brief       Multiplies QF16, QF16 -> QF16.
 *
 * The QF actual @qfe_type is updated to @QF16 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf16_mpy_qf16qf16(qf_t a, qf_t b, qf_t* out);

/**
 * @brief       Multiplies QF16, HF -> QF16.
 *
 * The QF actual @qfe_type is updated to @QF16 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf16_mpy_qf16hf(qf_t a, qf_t b, qf_t* out);

/**
 * @brief       Multiplies HF, HF -> QF16.
 *
 * The QF actual @qfe_type is updated to @QF16 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf16_mpy_hfhf(qf_t a, qf_t b, qf_t* out);


/**
 * @brief       Multiplies QF16, QF16 -> QF32.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf32_mpy_qf16qf16(qf_t a, qf_t b, qf_t* out);

/**
 * @brief       Multiplies QF16, HF -> QF32.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf32_mpy_qf16hf(qf_t a, qf_t b, qf_t* out);

/**
 * @brief       Multiplies HF, HF -> QF32.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf32_mpy_hfhf(qf_t a, qf_t b, qf_t* out);


/**
 * @brief       Multiplies QF32, QF32 -> QF32.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf32_mpy_qf32qf32(qf_t a, qf_t b, qf_t* out);

/**
 * @brief       Multiplies SF, SF -> QF32.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf32_mpy_sfsf(qf_t a, qf_t b, qf_t* out);

/**
 * @brief       Divides QF16, QF16 -> QF16.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf16_div_qf16qf16(qf_t a, qf_t b, qf_t* out);

/**
 * @brief       Divides QF32, QF32 -> QF32.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf32_div_qf32qf32(qf_t a, qf_t b, qf_t* out);


/**
 * @brief       Computes multiplicative inverse of QF16 -> QF16.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf16_inv_qf16(qf_t a, qf_t* out);

/**
 * @brief       Computes multiplicative inverse of QF32 -> QF32.
 *
 * The QF actual @qfe_type is updated to @QF32 and the actual value is updated according to the operation.
 * All the @qf_t reference values are also updated by applying the corresponding operation.
 *
 * @param[in]    a           First operand.
 * @param[in]    b           Second operand.
 * @param[out]   out         Output.
 */
void qfe_qf32_inv_qf32(qf_t a, qf_t* out);


/**
 * @brief       Displays a QF element and accuracy statistics.
 *
 * All values are converted to float numbers for display and comparisons against the reference double value.
 *
 * The report returns the following:
 *
 *              REFS -- sf:(<ref float value>,<ref float hexadecimal representation>) err=<error between ref float and ref double values>
 *                  | qf32:(<ref qf32 value>,<ref qf32 hexadecimal representation>) err=<error between ref qf32 and ref double values>
 *                  | qf16:(<ref qf16 value>,<ref qf16 hexadecimal representation>) err=<error between ref qf16 and ref double values>
 * ACTUAL -- <actual type>:(<actual value>,<actual hexadecimal representation>) err=<error between actual and ref double values>
 *
 * @param[in]   in      Input.
 */
void qfe_printf(qf_t x);

/**
 * @brief       Appends to a string a QF number as its type, the float value it represents and its binary form.
 *
 * The string format that is appended is as follows: <actual type>:(<actual value>,<actual hexadecimal representation>)
 *
 * @param[in]   string      String to which to append the QF string representation.
 * @param[in]   in          QF input value.
 */
void qfe_strcat(char* string, qf_t x);


/**
 * @}
 */

// Cross-platform aligned memory allocation
 void* allocate_aligned_memory(int size, int alignment);

// Cross-platform aligned memory free
 void free_aligned_memory(void *ptr);


#endif
