/* This file is part of Into.
 * Copyright (C) Intopii 2013.
 * All rights reserved.
 *
 * Licensees holding a commercial Into license may use this file in
 * accordance with the commercial license agreement. Please see
 * LICENSE.commercial for commercial licensing terms.
 *
 * Alternatively, this file may be used under the terms of the GNU
 * Affero General Public License version 3 as published by the Free
 * Software Foundation. In addition, Intopii gives you special rights
 * to use Into as a part of open source software projects. Please
 * refer to LICENSE.AGPL3 for details.
 */

#ifndef _PIIYDINTYPES_H
#define _PIIYDINTYPES_H

#include <PiiMatrixSerialization.h>
#include <PiiSerializationUtil.h>
#include "PiiVariant.h"
#include <QVariant>
#include <QDateTime>
#include <QStringList>
#include <PiiMatrix.h>
#include <PiiUtil.h>
#include "PiiInputSocket.h"
#include "PiiColor.h"
#include "PiiYdin.h"
#include "PiiSocketState.h"
#include <PiiQVariantWrapper.h>
#include <complex>

/**
 * @group piiydintypes_h Common Types
 *
 * Contains a number of preprocessor macros to facilitate handling of
 * multiple object types in operations. Also contains specializations
 * of the typeId function template for the most common types passed
 * between operations. Finally, registers the basic types as
 * serializable objects to PiiVariant.
 */

/// @internal
#define PII_DO_INTEGER_CASES(func, param) \
  case PiiVariant::CharType:              \
    func<char>param;                      \
    break;                                \
  case PiiVariant::ShortType:             \
    func<short>param;                     \
    break;                                \
  case PiiVariant::IntType:               \
    func<int>param;                       \
    break;                                \
  case PiiVariant::Int64Type:             \
    func<qint64>param;                    \
    break

/**
 * Case clauses for signed integer types. This code converts any
 * signed integer value to an `int:`
 *
 * ~~~(c++)
 * void MyOperation::process()
 * {
 *   PiiVariant obj = readInput();
 *   int value = 0;
 *   switch (obj.type())
 *   {
 *     PII_INTEGER_CASES(value = (int)PiiYdin::primitiveAs, obj);
 *   };
 * }
 * ~~~
 *
 * @param func a template function that takes one or no parameters.
 * The function template is instantiated with the primitive type
 * corresponding to the type id given in the switch statement. For
 * example, if obj.type() in the above example equals
 * PiiVariant::CharType, func<char>(obj) will be called.
 *
 * @param param the function parameter. Can be omitted. (e.g.
 * PII_INTEGER_CASES(myFunc,);)
 */
#define PII_INTEGER_CASES(func, param) PII_DO_INTEGER_CASES(func, (param))
/**
 * Same as [PII_INTEGER_CASES] but for multiple parameters. If your
 * template function takes more than one parameter, you may want to
 * use this macro instead.
 *
 * ~~~(c++)
 * void MyOperation::process()
 * {
 *   PiiVariant obj = readInput();
 *   int value = 0;
 *   switch (obj.type())
 *   {
 *     PII_INTEGER_CASES_M(myTemplateFunc, (obj, value));
 *   };
 * }
 *
 * template <class T> void MyOperation::myTemplateFunc(const PiiVariant& a,
 *                                                     int b)
 * {
 *   T value = a.valueAs<T>();
 *   // ...
 * }
 * ~~~
 *
 * @param func a template function that takes one or no parameters.
 * The function template is instantiated with the primitive type
 * corresponding to the type id given in the switch statement. For
 * example, if obj.type() in the above example equals
 * PiiVariant::CharType, func<char>(obj) will be called.
 *
 * @param params the function parameters. Enclose multiple parameters
 * in parens as in the example above.
 */
#define PII_INTEGER_CASES_M(func, params) PII_DO_INTEGER_CASES(func, params)

/// @internal
#define PII_DO_UNSIGNED_INTEGER_CASES(func, param) \
  case PiiVariant::UnsignedCharType:            \
    func<unsigned char>param;                   \
    break;                                      \
  case PiiVariant::UnsignedShortType:           \
    func<unsigned short>param;                  \
    break;                                      \
  case PiiVariant::UnsignedIntType:             \
    func<unsigned int>param;                    \
    break;                                      \
  case PiiVariant::UnsignedInt64Type:           \
    func<quint64>param;                         \
    break

/*case PiiVariant::UnsignedLongLongType:        \
    func<unsigned long long>param;              \
    break
*/

/**
 * Case clauses for unsigned integers. See [PII_INTEGER_CASES] for
 * details.
 */
#define PII_UNSIGNED_INTEGER_CASES(func, param) PII_DO_UNSIGNED_INTEGER_CASES(func, (param))
/**
 * Same as [PII_UNSIGNED_INTEGER_CASES] but for multiple
 * parameters. See [PII_INTEGER_CASES_M] for an example.
 */
#define PII_UNSIGNED_INTEGER_CASES_M(func, params) PII_DO_UNSIGNED_INTEGER_CASES_M(func, param)


/// @internal
#define PII_DO_FLOAT_CASES(func, param)         \
  case PiiVariant::FloatType:                   \
    func<float>param;                           \
    break;                                      \
  case PiiVariant::DoubleType:                  \
    func<double>param;                          \
    break
  /*  case PiiVariant::LongDoubleType:          \
    func<long double>param;                     \
    break*/

/**
 * Case clauses for floating point values.
 */
#define PII_FLOAT_CASES(func, param) PII_DO_FLOAT_CASES(func, (param))
/**
 * Same as [PII_FLOAT_CASES] but for multiple parameters.
 */
#define PII_FLOAT_CASES_M(func, params) PII_DO_FLOAT_CASES(func, params)

/// @internal
#define PII_DO_NUMERIC_CASES(func, param)       \
  PII_DO_INTEGER_CASES(func, param);            \
  PII_DO_UNSIGNED_INTEGER_CASES(func, param);   \
  PII_DO_FLOAT_CASES(func, param)

/**
 * Case clauses for all numeric primitive types.
 */
#define PII_NUMERIC_CASES(func, param) PII_DO_NUMERIC_CASES(func, (param))

/**
 * Same as [PII_NUMERIC_CASES] but for multiple parameters.
 */
#define PII_NUMERIC_CASES_M(func, params) PII_DO_NUMERIC_CASES(func, params)


/// @internal
#define PII_DO_PRIMITIVE_CASES(func, param)     \
  PII_DO_NUMERIC_CASES(func, param);            \
  case PiiVariant::BoolType:                    \
    func<bool>param;                            \
    break

/**
 * Case clauses for all primitive types.
 */
#define PII_PRIMITIVE_CASES(func, param) PII_DO_PRIMITIVE_CASES(func, (param))
/**
 * Same as [PII_PRIMITIVE_CASES] but for multiple parameters.
 */
#define PII_PRIMITIVE_CASES_M(func, params) PII_DO_PRIMITIVE_CASES(func, params)


/// @internal
#define PII_DO_COMPLEX_CASES(func, param)       \
  case PiiYdin::FloatComplexType:               \
    func<std::complex<float> >param;            \
    break;                                      \
  case PiiYdin::DoubleComplexType:              \
    func<std::complex<double> >param;           \
    break
  /*  case PiiYdin::LongDoubleComplexType: \
    func<std::complex<long double> >param;      \
    break
  */
/**
 * Case clauses for complex types.
 */
#define PII_COMPLEX_CASES(func, param) PII_DO_COMPLEX_CASES(func, (param))
/**
 * Same as [PII_COMPLEX_CASES] but for multiple parameters.
 */
#define PII_COMPLEX_CASES_M(func, params) PII_DO_COMPLEX_CASES(func, params)


/// @internal
#define PII_DO_INTEGER_MATRIX_CASES(func, param) \
  case PiiYdin::CharMatrixType:                 \
    func<char>param;                            \
    break;                                      \
  case PiiYdin::ShortMatrixType:                \
    func<short>param;                           \
    break;                                      \
  case PiiYdin::IntMatrixType:                  \
    func<int>param;                             \
    break;                                      \
  case PiiYdin::Int64MatrixType:                \
    func<qint64>param;                          \
    break

/*case PiiYdin::LongLongMatrixType:             \
    func<long long>param;                       \
    break
*/
/**
 * Case clauses for integer matrices. See [PII_ALL_MATRIX_CASES]
 * for more information.
 */
#define PII_INTEGER_MATRIX_CASES(func, param) PII_DO_INTEGER_MATRIX_CASES(func, (param))
/**
 * Case clauses for integer matrices and multiple function parameters.
 * See [PII_ALL_MATRIX_CASES_M] for more information.
 */
#define PII_INTEGER_MATRIX_CASES_M(func, params) PII_DO_INTEGER_MATRIX_CASES(func, params)


/// @internal
#define PII_DO_UNSIGNED_MATRIX_CASES(func, param) \
  case PiiYdin::UnsignedCharMatrixType:         \
    func<unsigned char>param;                   \
    break;                                      \
  case PiiYdin::UnsignedShortMatrixType:        \
    func<unsigned short>param;                  \
    break;                                      \
  case PiiYdin::UnsignedIntMatrixType:          \
    func<unsigned int>param;                    \
    break;                                      \
  case PiiYdin::UnsignedInt64MatrixType:        \
    func<quint64>param;                         \
    break

/*  case PiiYdin::UnsignedLongLongMatrixType:   \
    func<unsigned long long>param;              \
    break
*/

/**
 * Case clauses for unsigned integer matrices. See
 * [PII_ALL_MATRIX_CASES] for more information.
 */
#define PII_UNSIGNED_MATRIX_CASES(func, param) PII_DO_UNSIGNED_MATRIX_CASES(func, (param))
/**
 * Case clauses for unsigned integer matrices and multiple function
 * parameters. See [PII_ALL_MATRIX_CASES_M] for more information.
 */
#define PII_UNSIGNED_MATRIX_CASES_M(func, params) PII_DO_UNSIGNED_MATRIX_CASES(func, params)


/// @internal
#define PII_DO_FLOAT_MATRIX_CASES(func, param) \
  case PiiYdin::FloatMatrixType:               \
    func<float>param;                          \
    break;                                     \
  case PiiYdin::DoubleMatrixType:              \
    func<double>param;                         \
    break

/*  case PiiYdin::LongDoubleMatrixType:             \
    func<long double>param;                         \
    break
*/

/**
 * Case clauses for floating-point matrices. See
 * [PII_ALL_MATRIX_CASES] for more information.
 */
#define PII_FLOAT_MATRIX_CASES(func, param) PII_DO_FLOAT_MATRIX_CASES(func, (param))
/**
 * Case clauses for floating-point matrices and multiple function
 * parameters. See [PII_ALL_MATRIX_CASES_M] for more information.
 */
#define PII_FLOAT_MATRIX_CASES_M(func, params) PII_DO_FLOAT_MATRIX_CASES(func, params)

/// @internal
#define PII_DO_NUMERIC_MATRIX_CASES(func, param)  \
  PII_DO_INTEGER_MATRIX_CASES(func, param);       \
  PII_DO_UNSIGNED_MATRIX_CASES(func, param);      \
  PII_DO_FLOAT_MATRIX_CASES(func, param)

/**
 * Case clauses for all numeric matrices. See
 * [PII_ALL_MATRIX_CASES] for more information.
 */
#define PII_NUMERIC_MATRIX_CASES(func, param) PII_DO_NUMERIC_MATRIX_CASES(func, (param))

/**
 * Case clauses for all numeric matrices and multiple function
 * parameters. See [PII_ALL_MATRIX_CASES_M] for more information.
 */
#define PII_NUMERIC_MATRIX_CASES_M(func, params) PII_DO_NUMERIC_MATRIX_CASES(func, params)

/// @internal
#define PII_DO_PRIMITIVE_MATRIX_CASES(func, param)\
  PII_DO_NUMERIC_MATRIX_CASES(func, param);       \
  case PiiYdin::BoolMatrixType:                   \
    func<bool>param;                              \
    break

/**
 * Case clauses for all primitive matrix types. See
 * [PII_ALL_MATRIX_CASES] for more information.
 */
#define PII_PRIMITIVE_MATRIX_CASES(func, param) PII_DO_PRIMITIVE_MATRIX_CASES(func, (param))
/**
 * Case clauses for all primitive matrix types and multiple function
 * parameters. See [PII_ALL_MATRIX_CASES_M] for more information.
 */
#define PII_PRIMITIVE_MATRIX_CASES_M(func, params) PII_DO_PRIMITIVE_MATRIX_CASES(func, params)


/// @internal
#define PII_DO_COMPLEX_MATRIX_CASES(func, param)       \
  case PiiYdin::FloatComplexMatrixType:                \
    func<std::complex<float> >param;                   \
    break;                                             \
  case PiiYdin::DoubleComplexMatrixType:               \
    func<std::complex<double> >param;                  \
    break
  /*  case PiiYdin::LongDoubleComplexMatrixType: \
    func<std::complex<long double> >param;             \
    break
  */

/**
 * Case clauses for complex matrices. See [PII_ALL_MATRIX_CASES]
 * for more information.
 */
#define PII_COMPLEX_MATRIX_CASES(func, param) PII_DO_COMPLEX_MATRIX_CASES(func, (param))
/**
 * Case clauses for complex matrices and multiple function parameters.
 * See [PII_ALL_MATRIX_CASES_M] for more information.
 */
#define PII_COMPLEX_MATRIX_CASES_M(func, params) PII_DO_COMPLEX_MATRIX_CASES(func, params)

/**
 * All cases for primitive matrices. This is a macro that saves you a
 * lot of typing if you need to support many different matrix types.
 * You only need to create a template function that works for all
 * types. Typically, a PiiVariant is passed as its only argument.
 *
 * ~~~(c++)
 * template <class T> void myFunction(const PiiVariant& obj)
 * {
 *   PiiMatrix<T> value = obj.valueAs<PiiMatrix<T> >();
 *   ...
 * }
 *
 * ...
 *
 * PiiVariant obj = readInput();
 * switch (obj.type())
 *   {
 *     PII_ALL_MATRIX_CASES(myFunction, obj);
 *   default:
 *     sendToDevNull(obj);
 *   };
 * ~~~
 *
 * This code will call `myFunction`<char>(obj) if `obj`.type()
 * returns `PiiYdin::CharMatrixType` and similarly for all
 * primitive types.
 *
 * @param func the name of a function template to call with the given
 * parameter. The function may be a class method, if the macro is used
 * within a class method. If the function returns a value, you may
 * also use the syntax "var = func". For example,
 * `PII_ALL_MATRIX_CASES(myInt = myFunction, obj)`. The
 * [PiiYdin::matrixAs()] function template is a utility function
 * often used as the `func` parameter.
 *
 * @param param a parameter to the function, usually a PiiVariant.
 */
#define PII_ALL_MATRIX_CASES(func, param)         \
  PII_PRIMITIVE_MATRIX_CASES(func, param);        \
  PII_COMPLEX_MATRIX_CASES(func, param)

/**
 * Analogous to [PII_ALL_MATRIX_CASES], but this version accepts
 * many function parameters at once.
 *
 * ~~~(c++)
 * template <class T> void myFunction(const PiiVariant& obj, const char* str)
 * {
 *   PiiMatrix<T> value = obj.valueAs<PiiMatrix<T> >();
 *   ...
 * }
 *
 * ...
 *
 * PiiVariant obj = readInput();
 * switch (obj.type())
 *   {
 *     PII_ALL_MATRIX_CASES_M(myFunction, (obj, "Hello world."));
 *   default:
 *     sendToDevNull(obj);
 *   };
 * ~~~
 *
 * @param func the function template to call with the parameters
 *
 * @param params parameters to the function. The parameter list must
 * be in parens as shown in the example above.
 */
#define PII_ALL_MATRIX_CASES_M(func, params)      \
  PII_PRIMITIVE_MATRIX_CASES_M(func, params);     \
  PII_COMPLEX_MATRIX_CASES_M(func, params)

/// @internal
#define PII_DO_INT_GRAY_IMAGE_CASES(func, param)  \
  case PiiYdin::UnsignedCharMatrixType:           \
    func<unsigned char>param;                     \
    break;                                        \
  case PiiYdin::IntMatrixType:                    \
    func<int>param;                               \
    break

/**
 * Case clauses for integer-valued gray-scale image types. See
 * [PII_ALL_MATRIX_CASES] for more information.
 */
#define PII_INT_GRAY_IMAGE_CASES(func, param) PII_DO_INT_GRAY_IMAGE_CASES(func, (param))
/**
 * Case clauses for integer-valued gray-scale image types with
 * multiple function parameters. See [PII_ALL_MATRIX_CASES_M] for
 * more information.
 */
#define PII_INT_GRAY_IMAGE_CASES_M(func, params) PII_DO_INT_GRAY_IMAGE_CASES(func, params)

/// @internal
#define PII_DO_GRAY_IMAGE_CASES(func, param)      \
  PII_DO_INT_GRAY_IMAGE_CASES(func, param);       \
  case PiiYdin::FloatMatrixType:                  \
    func<float>param;                             \
    break

/**
 * Case clauses for all gray-scale image types. See
 * [PII_ALL_MATRIX_CASES] for more information.
 */
#define PII_GRAY_IMAGE_CASES(func, param) PII_DO_GRAY_IMAGE_CASES(func, (param))
/**
 * Case clauses for all gray-scale image types with multiple function
 * parameters. See [PII_ALL_MATRIX_CASES_M] for more information.
 */
#define PII_GRAY_IMAGE_CASES_M(func, params) PII_DO_GRAY_IMAGE_CASES(func, params)

/// @internal
#define PII_DO_INT_COLOR_IMAGE_CASES(func, param) \
  case PiiYdin::UnsignedCharColorMatrixType:      \
    func<PiiColor<unsigned char> >param;          \
    break;                                        \
  case PiiYdin::UnsignedCharColor4MatrixType:     \
    func<PiiColor4<unsigned char> >param;         \
    break;                                        \
  case PiiYdin::UnsignedShortColorMatrixType:     \
    func<PiiColor<unsigned short> >param;         \
    break

/**
 * Case clauses for integer-valued color image types. See
 * [PII_ALL_MATRIX_CASES] for more information.
 */
#define PII_INT_COLOR_IMAGE_CASES(func, param) PII_DO_INT_COLOR_IMAGE_CASES(func, (param))
/**
 * Case clauses for integer-valued color image types and multiple
 * function parameters. See [PII_ALL_MATRIX_CASES] for more
 * information.
 */
#define PII_INT_COLOR_IMAGE_CASES_M(func, params) PII_DO_INT_COLOR_IMAGE_CASES(func, params)


/// @internal
#define PII_DO_COLOR_IMAGE_CASES(func, param)    \
  PII_DO_INT_COLOR_IMAGE_CASES(func, param);     \
  case PiiYdin::FloatColorMatrixType:            \
    func<PiiColor<float> >param;                 \
    break

/**
 * Case clauses for all color image types. See
 * [PII_ALL_MATRIX_CASES] for more information.
 */
#define PII_COLOR_IMAGE_CASES(func, param) PII_DO_COLOR_IMAGE_CASES(func, (param))
/**
 * Case clauses for all color image types and multiple function
 * parameters. See [PII_ALL_MATRIX_CASES_M] for more information.
 */
#define PII_COLOR_IMAGE_CASES_M(func, params) PII_DO_COLOR_IMAGE_CASES(func, params)


/**
 * Case clauses for all gray scale and color image types. See
 * [PII_ALL_MATRIX_CASES] for more information.
 */
#define PII_ALL_IMAGE_CASES(func, param)     \
  PII_GRAY_IMAGE_CASES(func, param);         \
  PII_COLOR_IMAGE_CASES(func, param)

/**
 * Case clauses for all gray scale and color image types, and multiple
 * function parameters. See [PII_ALL_MATRIX_CASES_M] for more
 * information.
 */
#define PII_ALL_IMAGE_CASES_M(func, params)  \
  PII_GRAY_IMAGE_CASES_M(func, params);      \
  PII_COLOR_IMAGE_CASES_M(func, params)


/**
 * A namespace for some base types used by most operations. Such types
 * include, for example, control objects (for synchronization etc.),
 * matrices and colors, and strings. Primitive types are already
 * handled by PiiVariant.
 *
 */
#ifdef Q_MOC_RUN
class PiiYdin
#else
namespace PiiYdin
#endif
{
#ifdef Q_MOC_RUN
  Q_GADGET

  Q_ENUMS(MatrixTypeId ColorTypeId ComplexTypeId QtTypeId);
public:
#endif
  /// @internal
  extern PII_YDIN_EXPORT const QMetaObject staticMetaObject;

  /**
   * A traits structure that converts primitive types to those
   * supported by QVariant.
   */
  template <class T> struct QVariantTraits { typedef T Type; };
  //template <> struct QVariantTraits<long> { typedef int Type; };
  //template <> struct QVariantTraits<unsigned long> { typedef unsigned int Type; };
  //template <> struct QVariantTraits<long double> { typedef double Type; };

  /**
   * Type IDs for control objects. Control objects are
   * programmer-invisible objects passed between operations e.g. for
   * synchronization purposes. The ID numbers 0x20-0x3f
   * (network/netmask: 0x20/~0x1f) are reserved for control objects.
   *
   * - `SynchronizationTagType` - synchronization tag.
   * Synchronization tags either raise or lower the current flow level
   * in a synchronous socket group. The value of a synchronization tag
   * is a signed `int` that will be added to the current flow level.
   *
   * - `StopTagType` - a tag that causes the end of execution of
   * an operation when received.
   *
   * - `PauseTagType` - a tag that suspends the execution of an
   * operation when received.
   *
   * - `ResumeTagType` - used to restore flow levels after pausing.
   * Works almost equally to a synchronization tag, but informs the
   * receiver that partial content may follow. The value of a resume
   * tag is a PiiSocketState.
   *
   * - `ReconfigurationTagType` - a tag used for synchronized
   * reconfigurations. Reconfiguration tags work almost equivalently
   * to pause tags, but they don't change the state of the receiving
   * operation. The value of the tag is a QString that specifies the
   * property set to apply.
   */
  enum ControlTypeId
    {
      SynchronizationTagType = 0x20,
      StopTagType,
      PauseTagType,
      ResumeTagType,
      ReconfigurationTagType
    };

  /**
   * Check if a type id represents a control object.
   */
  inline bool isControlType(int type) {  return (type & ~0x1f) == 0x20; }
  /**
   * Check if a type id represents a non-control object.
   */
  inline bool isNonControlType(int type) {  return (type & ~0x1f) != 0x20; }

  /**
   * Type IDs for matrices. The ID numbers 0x40-0x7f are reserved for
   * different types of matrices.
   */
  enum MatrixTypeId
    {
      CharMatrixType = 0x40,
      ShortMatrixType,
      IntMatrixType,
      Int64MatrixType,
      //LongLongMatrixType,
      UnsignedCharMatrixType = 0x40 + PiiVariant::UnsignedCharType,
      UnsignedShortMatrixType,
      UnsignedIntMatrixType,
      UnsignedInt64MatrixType,
      //UnsignedLongLongMatrixType,
      FloatMatrixType = 0x40 + PiiVariant::FloatType,
      DoubleMatrixType,
      //LongDoubleMatrixType,
      BoolMatrixType = 0x40 + PiiVariant::BoolType,

      UnsignedCharColorMatrixType,
      UnsignedCharColor4MatrixType,
      UnsignedShortColorMatrixType,
      FloatColorMatrixType,

      IntComplexMatrixType,
      FloatComplexMatrixType,
      DoubleComplexMatrixType
      //LongDoubleComplexMatrixType
    };

  /**
   * Returns `true` if *type* is in the matrix type id range,
   * `false` otherwise.
   */
  inline bool isMatrixType(int type)
  {
    return (type & ~0x1f) == 0x40;
  }

  /**
   * Type IDs for colors, points and areas. Colors reserve ID numbers
   * 0x80-0x9f (0x80/~0x1f).
   */
  enum ColorTypeId
    {
      UnsignedCharColorType = 0x80,
      UnsignedCharColor4Type,
      UnsignedShortColorType,
      FloatColorType
    };

  /**
   * Type IDs for complex numbers. The std::complex template is used
   * as a complex number class. Complex numbers occupy type ID numbers
   * 0xa0-0xbf (0xa0/~0x1f).
   */
  enum ComplexTypeId
    {
      IntComplexType = 0xa0 + PiiVariant::IntType,
      FloatComplexType = 0xa0 + PiiVariant::FloatType,
      DoubleComplexType = 0xa0 + PiiVariant::DoubleType
      //LongDoubleComplexType = 0xa0 + PiiVariant::LongDoubleType
    };

  /**
   * Type IDs for Qt classes. Qt classes occupy type ID numbers
   * 0xc0-0xdf (0xc0/~0x1f).
   */
  enum QtTypeId
    {
      QStringType = 0xc0,
      QDateType,
      QTimeType,
      QDateTimeType,
      QImageType,
      QStringListType
    };

  /**
   * A utility function that returns a copy of a primitive object held
   * by `obj` as a type compatible with QVariant. The function
   * returns a primitive value that can be directly set to a QVariant.
   *
   * ~~~(c++)
   * QVariant value;
   * switch (obj.type())
   *   {
   *      PII_PRIMITIVE_CASES(value = qVariantAs, obj);
   *   }
   * ~~~
   */
  template <class T> typename QVariantTraits<T>::Type qVariantAs(const PiiVariant& obj)
  {
    return static_cast<typename QVariantTraits<T>::Type>(obj.valueAs<T>());
  }

  /**
   * A utility function template that returns a reference to
   * PiiMatrix<T> held by the variant argument. This can be used with
   * the `PII_XXX_MATRIX_CASES` macros as the function argument. This
   * code converts any integer matrix to PiiMatrix<int>:
   *
   * ~~~(c++)
   * PiiVariant obj = readInput();
   * PiiMatrix<int> mat;
   * switch (obj.type())
   *   {
   *     PII_INTEGER_MATRIX_CASES(mat = (PiiMatrix<int>)PiiYdin::matrixAs, obj);
   *   }
   * ~~~
   */
  template <class T> inline PiiMatrix<T> matrixAs(const PiiVariant& obj)
  {
    return obj.valueAs<PiiMatrix<T> >();
  }

  /**
   * Returns the primitive type held by the variant argument. This can
   * be used with the `PII_XXX_CASES` macros as the function
   * argument.
   */
  template <class T> inline T primitiveAs(const PiiVariant& obj)
  {
    return obj.valueAs<T>();
  }

  /**
   * Converts the primitive value held by *obj* to `T`, if possible.
   * If the value cannot be converted, returns a default-constructed
   * value.
   */
  template <class T> inline T convertPrimitiveTo(const PiiVariant& obj)
  {
    switch (obj.type())
      {
        PII_PRIMITIVE_CASES(return (T)primitiveAs, obj);
      case QStringType:
        {
          bool ok = false;
          T value = Pii::stringTo<T>(obj.valueAs<QString>(), &ok);
          if (!ok)
            return T();
          return value;
        }
      }
    return T();
  }

  /**
   * A utility function that returns the value in `input` as the
   * given type. Make sure there is an object in `input`.
   *
   * @param input an input socket with a non-null object received
   *
   * @return the value of the incoming object as the given type `T`
   *
   * @exception PiiExecutionException& if the received object cannot
   * be converted to `T`.
   */
  template <class T> T primitiveAs(PiiInputSocket* input)
  {
    PiiVariant obj = input->firstObject();
    if (obj.type() == Pii::typeId<T>())
      return obj.valueAs<T>();

    switch (obj.type())
      {
        PII_PRIMITIVE_CASES(return (T)primitiveAs, obj);
      case QStringType:
        {
          bool ok = false;
          T value = Pii::stringTo<T>(obj.valueAs<QString>(), &ok);
          if (!ok)
            PII_THROW_UNKNOWN_TYPE(input);
          return value;
        }
      default:
        PII_THROW_UNKNOWN_TYPE(input);
      }
    return 0;
  }

  /**
   * Returns the number of columns in the matrix stored in `obj`.
   *
   * ! The caller must ensure that *obj* holds an instance of
   * PiiTypelessMatrix.
   *
   * @see isMatrixType()
   */
  inline int matrixColumns(const PiiVariant& obj)
  {
    return obj.valueAs<PiiTypelessMatrix>().columns();
  }

  /**
   * Returns the number of rows in the matrix stored in `obj`.
   *
   * ! The caller must ensure that *obj* holds an instance of
   * PiiTypelessMatrix.
   *
   * @see isMatrixType()
   */
  inline int matrixRows(const PiiVariant& obj)
  {
    return obj.valueAs<PiiTypelessMatrix>().rows();
  }

  /**
   * Returns the stride of the matrix stored in `obj`.
   *
   * ! The caller must ensure that *obj* holds an instance of
   * PiiTypelessMatrix.
   *
   * @see isMatrixType()
   */
  inline std::size_t matrixStride(const PiiVariant& obj)
  {
    return obj.valueAs<PiiTypelessMatrix>().stride();
  }

  /**
   * Converts a PiiVariant containing a numeric type into a
   * QString.
   *
   * ! The caller must ensure that *obj* holds an instance of
   * QString.
   */
  template <class T> inline QString numberToQString(const PiiVariant& obj)
  {
    return QString::number(obj.valueAs<T>());
  }

  /**
   * Returns either "true" or "false", depending on the boolean value
   * in *obj*.
   *
   * ! The caller must ensure that *obj* holds a boolean value.
   */
  template <> inline QString numberToQString<bool>(const PiiVariant& obj)
  {
    return obj.valueAs<bool>() ? "true" : "false";
  }

  /*template <> inline QString numberToQString<long double>(const PiiVariant& obj)
  {
    return QString::number(double(obj.valueAs<long double>()));
  }*/

  inline PiiVariant createStartTag() { return PiiVariant(1, SynchronizationTagType); }
  inline PiiVariant createEndTag() { return PiiVariant(-1, SynchronizationTagType); }
  inline PiiVariant createStopTag() { return PiiVariant(0, StopTagType); }
  inline PiiVariant createPauseTag() { return PiiVariant(0, PauseTagType); }
  inline PiiVariant createReconfigurationTag(const QString& name) { return PiiVariant(name, ReconfigurationTagType); }
  //inline PiiVariant createResumeTag(PiiSocketState state) { return PiiVariant(state); }

  /**
   * Converts the object in `input` to a QString, if possible.
   *
   * @exception PiiExecutionException& if the received object cannot
   * be converted to QString.
   */
  PII_YDIN_EXPORT QString convertToQString(PiiInputSocket* input);

  /**
   * Converts *variant* to a QString, if possible. Otherwise returns
   * a `null` QString.
   */
  PII_YDIN_EXPORT QString convertToQString(const PiiVariant& variant);

  /**
   * Convert the object in `input` into the type specified by `T`.
   *
   * ~~~(c++)
   * double dValue = PiiYdin::convertPrimitiveTo<double>(input(0));
   * ~~~
   *
   * @exception PiiExecutionException& if the received object cannot
   * be converted to `T`.
   */
  template <class T> T convertPrimitiveTo(PiiInputSocket* input)
  {
    PiiVariant obj = input->firstObject();
    switch (obj.type())
      {
        PII_PRIMITIVE_CASES(return (T)primitiveAs, obj);
      }
    PII_THROW_UNKNOWN_TYPE(input);
  }

  /**
   * Converts *obj* into a matrix whose element type is specified by
   * `T`. If the object cannot be converted, returns an empty
   * PiiMatrix<T>.
   */
  template <class T> PiiMatrix<T> convertMatrixTo(const PiiVariant& obj)
  {
    if (obj.type() == Pii::typeId<PiiMatrix<T> >())
      return obj.valueAs<PiiMatrix<T> >();
    switch (obj.type())
      {
        PII_PRIMITIVE_MATRIX_CASES(return matrixAs, obj);
      }
    return PiiMatrix<T>();
  }

  /**
   * Converts the object in `input` into a matrix whose element type
   * is specified by `T`.
   *
   * ~~~(c++)
   * PiiMatrix<double> m = PiiYdin::convertMatrixTo<double>(input(0));
   * ~~~
   *
   * @exception PiiExecutionException& if the received object cannot
   * be converted to PiiMatrix<T>.
   */
  template <class T> PiiMatrix<T> convertMatrixTo(PiiInputSocket* input)
  {
    PiiVariant obj = input->firstObject();
    if (obj.type() == Pii::typeId<PiiMatrix<T> >())
      return obj.valueAs<PiiMatrix<T> >();
    switch (obj.type())
      {
        PII_PRIMITIVE_MATRIX_CASES(return matrixAs, obj);
      }
    PII_THROW_UNKNOWN_TYPE(input);
  }

  /**
   * Reads an object from *input* as the type specified by the
   * template parameter `T`. If the object is not an instance of
   * `T`, a PiiExecutionException& will be thrown.
   */
  template <class T> T& readInputAs(PiiInputSocket* input)
  {
    PiiVariant& obj = input->firstObject();
    if (obj.type() != Pii::typeId<T>())
      PII_THROW_UNKNOWN_TYPE(input);
    return obj.valueAs<T>();
  }

}; // namespace PiiYdin

#ifndef Q_MOC_RUN // moc fails

// Declares both PiiVariant and QVariant
#define PII_DECLARE_SHARED_VARIANT_BOTH(TYPE, ID, BUILDING_LIB) \
  PII_DECLARE_SHARED_VARIANT_TYPE(TYPE, ID, BUILDING_LIB); \
  PII_SERIALIZATION_NAME_CUSTOM(PiiQVariantWrapper::Template<TYPE >, "PiiQVariantWrapper<" PII_STRINGIZE(TYPE) ">"); \
  PII_DECLARE_EXPORTED_CLASS_TEMPLATE(class, PiiQVariantWrapper::Template<TYPE >, BUILDING_LIB); \
  PII_DECLARE_FACTORY(PiiQVariantWrapper::Template<TYPE >, BUILDING_LIB); \
  Q_DECLARE_METATYPE(TYPE)
// complex numbers
PII_DECLARE_SHARED_VARIANT_BOTH(std::complex<int>, PiiYdin::IntComplexType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(std::complex<float>, PiiYdin::FloatComplexType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(std::complex<double>, PiiYdin::DoubleComplexType, PII_BUILDING_YDIN);
//PII_DECLARE_SHARED_VARIANT_BOTH(std::complex<long double>, PiiYdin::LongDoubleComplexType, PII_BUILDING_YDIN);

// matrices
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<char>, PiiYdin::CharMatrixType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<short>, PiiYdin::ShortMatrixType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<int>, PiiYdin::IntMatrixType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<qint64>, PiiYdin::Int64MatrixType, PII_BUILDING_YDIN);
//PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<long long>, PiiYdin::LongLongMatrixType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<unsigned char>, PiiYdin::UnsignedCharMatrixType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<unsigned short>, PiiYdin::UnsignedShortMatrixType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<unsigned int>, PiiYdin::UnsignedIntMatrixType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<quint64>, PiiYdin::UnsignedInt64MatrixType, PII_BUILDING_YDIN);
//PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<unsigned long long>, PiiYdin::UnsignedLongLongMatrixType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<float>, PiiYdin::FloatMatrixType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<double>, PiiYdin::DoubleMatrixType, PII_BUILDING_YDIN);
//PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<long double>, PiiYdin::LongDoubleMatrixType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<bool>, PiiYdin::BoolMatrixType, PII_BUILDING_YDIN);

// colors
PII_DECLARE_SHARED_VARIANT_BOTH(PiiColor<unsigned char>, PiiYdin::UnsignedCharColorType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiColor4<unsigned char>, PiiYdin::UnsignedCharColor4Type, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiColor<unsigned short>, PiiYdin::UnsignedShortColorType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiColor<float>, PiiYdin::FloatColorType, PII_BUILDING_YDIN);

// color images
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<PiiColor<unsigned char> >, PiiYdin::UnsignedCharColorMatrixType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<PiiColor4<unsigned char> >, PiiYdin::UnsignedCharColor4MatrixType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<PiiColor<unsigned short> >, PiiYdin::UnsignedShortColorMatrixType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<PiiColor<float> >, PiiYdin::FloatColorMatrixType, PII_BUILDING_YDIN);

// complex matrices
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<std::complex<int> >, PiiYdin::IntComplexMatrixType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<std::complex<float> >, PiiYdin::FloatComplexMatrixType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<std::complex<double> >, PiiYdin::DoubleComplexMatrixType, PII_BUILDING_YDIN);
//PII_DECLARE_SHARED_VARIANT_BOTH(PiiMatrix<std::complex<long double> >, PiiYdin::LongDoubleComplexMatrixType, PII_BUILDING_YDIN);

// Qt classes
PII_DECLARE_SHARED_VARIANT_TYPE(QString, PiiYdin::QStringType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_TYPE(QStringList, PiiYdin::QStringListType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_TYPE(QDate, PiiYdin::QDateType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_TYPE(QTime, PiiYdin::QTimeType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_TYPE(QDateTime, PiiYdin::QDateTimeType, PII_BUILDING_YDIN);
PII_DECLARE_SHARED_VARIANT_TYPE(QImage, PiiYdin::QImageType, PII_BUILDING_YDIN);

PII_DECLARE_SHARED_VARIANT_TYPE(PiiSocketState, PiiYdin::ResumeTagType, PII_BUILDING_YDIN);

/// @endgroup

#endif // Q_MOC_RUN

#endif //_PIIYDINTYPES_H
