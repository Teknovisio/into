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

#ifndef _PIICOLOR_H
#define _PIICOLOR_H

#include <functional>
#include "PiiArithmeticBase.h"

// Forward declarations for rebinders
template <class T> class PiiColor;
template <class T> class PiiColor4;

namespace PiiSerialization { struct Accessor; }

/**
 * Type information structure for PiiColorBase. See PiiArithmeticBase for
 * more information.
 *
 */
template <class T>
struct PiiColorBaseTraits
{
  typedef T Type;
  typedef T* Iterator;
  typedef const T* ConstIterator;
};

/**
 * Type information structure for PiiColor. See PiiArithmeticBase for
 * more information.
 *
 */
template <class T>
struct PiiArithmeticTraits<PiiColor<T> > :
  public PiiColorBaseTraits<T>
{
  template <class U> struct Rebind { typedef PiiColor<U> Type; };
};

/**
 * Type information structure for PiiColor4. See PiiArithmeticBase for
 * more information.
 *
 */
template <class T>
struct PiiArithmeticTraits<PiiColor4<T> > :
  public PiiColorBaseTraits<T>
{
  template <class U> struct Rebind { typedef PiiColor4<U> Type; };
};

/**
 * A structure that represents a three-channel color. The data type of
 * the color channels is a template parameter with a default value of
 * `unsigned` `char`. This should work for most cases because color
 * spaces with more than four channels or more than eight bits per
 * channel are seldom used.
 *
 * The generic way of accessing color channels is via the `channel`
 * array. In PiiColor, the array can be accessed with indices from 0
 * to 2. In PiiColor4, 3 can also be used. (The array is defined to
 * have space for three elements only, but the arrangement of classes
 * in memory makes it possible to utilize array overflow in a
 * constructive fashion.)
 *
 * In your code, you can utilized the [channel(int)] function or
 * color space specific names:
 *
 * ~~~(c++)
 * PiiColor<> clr(255, 127, 0);
 * QCOMPARE(clr.channel(1), clr.rgbG);
 * QCOMPARE(clr.rgbG, static_cast<unsigned char>(127));
 * QCOMPARE(clr.channel(2), clr.hsvV);
 * QCOMPARE(clr.c2, clr.rgbaB);
 * ~~~
 *
 * The syntax is <space><Channel>, where <space> is any of the
 * following: rgb, hsv, hsi, yuv, yiq, xyz, lab, luv, cmyk, rgba.
 * <Channel> is the name of the color channel as a capital letter,
 * e.g. R, G, B, or Y. The R channel of the RGB color space is denoted
 * by rgbR. Furthermore, the channels are aliased with generic names
 * c0, c1 and c2. If possible, the color channel names should be
 * preferred over the channel array for code readability.
 *
 * **NOTE**! The order of color channels in the channel array is
 * opposite to what you probably expect. For example, rgbR corresponds
 * to channel[2], and rgbB to channel[0]. This allows one to use
 * PiiColor4<unsigned char> and `int` interchangeably on
 * little-endian hardware (such as the PC). It also makes it possible
 * to use the colors with QRgb. You can, however, use the
 * channel(int) function to access the channels in the "natural"
 * order.
 *
 * @see PiiColor
 * @see PiiColor4
 *
 */
template <class T> class PiiColorBase
{
public:
  /**
   * An stl-style const iterator to the first color channel. Note that
   * the first color channel in the RGB space is B.
   */
  typename PiiColorBaseTraits<T>::ConstIterator begin() const { return channels; }
  /**
   * An stl-style iterator to the first color channel. Note that the
   * first color channel in the RGB space is B.
   */
  typename PiiColorBaseTraits<T>::Iterator begin() { return channels; }

  /**
   * Create a new color with the same value on each color channel.
   */
  PiiColorBase(T value = 0) :
    c2(value), c1(value), c0(value)
  { }

  /**
   * Create a new color with the given values for each color channel.
   * Note that the order of channels in the channel array is reversed.
   * When you initialize a PiiColor with RGB values, B will be the
   * first channel.
   *
   * ~~~(c++)
   * // Parameters:       R, G, B
   * PiiColorBase<> pureRed(255, 0, 0);
   * QVERIFY(pureRed.rgbR == 255);
   * QVERIFY(pureRed.c0 == 255);
   * QVERIFY(pureRed.channels[0] == 0); // !
   * ~~~
   */
  PiiColorBase(T channel0, T channel1, T channel2) :
    c2(channel2), c1(channel1), c0(channel0)
  { }

  /**
   * Return the value of the channel denoted by `channelIndex`. This
   * is a convenience function that returns the channels in
   * "traditional" order (`channel`(0) returns the R channel etc.)
   */
  inline T channel(int channelIndex) const { return channels[(2-channelIndex) & 3]; }
  /**
   * Set the value of a color channel. This is a convenience function
   * that allows one to index the channels in "traditional" order.
   */
  inline void setChannel(int channelIndex, T value) { channels[(2-channelIndex) & 3] = value; }

  /**
   * Return the average of all color channels as an `int`.
   */
  explicit operator int() const { return (int(c0) + int(c1) + int(c2))/3; }
  /**
   * Return the average of all color channels as a `short`.
   */
  explicit operator short() const { return short(operator int()); }
  /**
   * Return the average of all color channels as a `char`.
   */
  explicit operator char() const { return char(operator int()); }

  /**
   * Return the average of all color channels as an `unsigned`
   * `int`.
   */
  explicit operator unsigned int() const { return ((unsigned int)c0 + (unsigned int)c1 + (unsigned int)c2)/3; }
  /**
   * Return the average of all color channels as an `unsigned`
   * `short`.
   */
  explicit operator unsigned short() const { return (unsigned short)operator unsigned int(); }
  /**
   * Return the average of all color channels as an `unsigned`
   * `char`.
   */
  explicit operator unsigned char() const { return (unsigned char)operator unsigned int(); }

  /**
   * Return the average of all color channels as a `float`.
   */
  explicit operator float() const { return (float(c0) + float(c1) + float(c2))/3.0f; }
  /**
   * Return the average of all color channels as a `double`.
   */
  explicit operator double() const { return (double(c0) + double(c1) + double(c2))/3.0; }

  union
  {
    /**
     * The color channels.
     */
    T channels[3];
    struct
    {
      union { T c2, rgbB, hsvV, hsiI, yuvV, yiqQ, xyzZ, labB, luvV, cmykY, rgbaB; };
      union { T c1, rgbG, hsvS, hsiS, yuvU, yiqI, xyzY, labA, luvU, cmykM, rgbaG; };
      union { T c0, rgbR, hsvH, hsiH, yuvY, yiqY, xyzX, labL, luvL, cmykC, rgbaR; };
    };
  };
};

/**
 * A three-channel color. This class adds support for arithmetic
 * operations to PiiColorBase.
 *
 */
template <class T = unsigned char> class PiiColor :
  public PiiColorBase<T>,
  public PiiArithmeticBase<PiiColor<T> >
{
  friend struct PiiSerialization::Accessor;
  template <class Archive> inline void serialize(Archive& archive, const unsigned int /*version*/)
  {
    archive & this->channels[0];
    archive & this->channels[1];
    archive & this->channels[2];
  }
public:
  /**
   * Shorthand for the base class type.
   */
  typedef PiiArithmeticBase<PiiColor<T> > BaseType;

  enum { ChannelCount = 3 };

  /**
   * Create a new color with the same value on each color channel.
   */
  PiiColor(T value = 0) : PiiColorBase<T>(value) {}

  /**
   * Create a new color with the given values for color channels.
   *
   * ~~~(c++)
   * // Parameters:  R, G,  B
   * PiiColor<> blue(0, 0, 255);
   * ~~~
   */
  PiiColor(T channel0, T channel1, T channel2) :
    PiiColorBase<T>(channel0, channel1, channel2) { }

  /**
   * Copy another three-channel color.
   */
  PiiColor(const PiiColor& clr) : PiiColorBase<T>(clr.c0, clr.c1, clr.c2) {}

  /**
   * Copy any three or four channel color. If `clr` has a fourth
   * color channel, it'll be ignored.
   */
  PiiColor(const PiiColorBase<T>& clr) : PiiColorBase<T>(clr.c0, clr.c1, clr.c2) {}

  /**
   * Create a typecasted copy of any other color. If `clr` has a
   * fourth color channel, it'll be ignored.
   */
  template <class U> PiiColor(const PiiColorBase<U>& clr) : PiiColorBase<T>(T(clr.c0), T(clr.c1), T(clr.c2)) {}

  /**
   * An stl-style const iterator to the end of color channels.
   */
  typename BaseType::ConstIterator end() const { return this->channels + 3; }
  /**
   * An stl-style iterator to the end of color channels.
   */
  typename BaseType::Iterator end() { return this->channels + 3; }
};

/**
 * A four-channel color. Fourth color channel is sometimes useful in
 * aligning the color values to word boundaries in memory.
 * Furthermore, some colors are composed of four distinct channels.
 *
 * The class functions equivalently to PiiColor, but provides space
 * for a fourth channel. One can access the fourth color channel of
 * the CMYK and RGBA color spaces via `cmykK` and `rgbaA`. All
 * channels can be accessed via the `channel` array and the color
 * channel names inherited from PiiColor.
 *
 */
template <class T = unsigned char> class PiiColor4 :
  public PiiColorBase<T>,
  public PiiArithmeticBase<PiiColor4<T> >
{
  friend struct PiiSerialization::Accessor;
  template <class Archive> inline void serialize(Archive& archive, const unsigned int /*version*/)
  {
    archive & this->channels[0];
    archive & this->channels[1];
    archive & this->channels[2];
    archive & this->channels[3];
  }
public:
  /**
   * Shorthand for the base class type.
   */
  typedef PiiArithmeticBase<PiiColor4<T> > BaseType;

  enum { ChannelCount = 4 };

  /**
   * Create a new color with the same value on each color channel.
   */
  PiiColor4(T value = 0) : PiiColorBase<T>(value), c3(value) { }
  /**
   * Create a new color with the given values for each color channel.
   * To create an RGBA color, do this:
   *
   * ~~~(c++)
   * // Parameters:              R, G,  B,   A
   * PiiColor4<> transparentBlue(0, 0, 255, 127);
   * ~~~
   */
  PiiColor4(T channel0, T channel1, T channel2, T channel3 = 0) :
    PiiColorBase<T>(channel0, channel1, channel2), c3(channel3) { }
  /**
   * Copy another color.
   */
  PiiColor4(const PiiColor4& clr) : PiiColorBase<T>(clr.c0, clr.c1, clr.c2), c3(clr.c3) { }
  /**
   * Create a new four-channel color with the three channel values
   * taken from the given three-channel color and the fourth channel
   * set to zero.
   */
  PiiColor4(const PiiColorBase<T>& clr) : PiiColorBase<T>(clr.c0, clr.c1, clr.c2), c3(0) { }

  /**
   * Create a typecasted copy.
   */
  template <class U> PiiColor4(const PiiColor4<U>& clr) : PiiColorBase<T>(T(clr.c0), T(clr.c1), T(clr.c2)), c3(T(clr.c3)) { }

  /**
   * Create a typecasted copy of a three-channel color. The fourt
   * channel will be set to zero.
   */
  template <class U> PiiColor4(const PiiColorBase<U>& clr) : PiiColorBase<T>(T(clr.c0), T(clr.c1), T(clr.c2)), c3(0) {}

  /**
   * An stl-style const iterator to the end of color channels.
   */
  typename BaseType::ConstIterator end() const { return this->channels + 4; }
  /**
   * An stl-style iterator to the end of color channels.
   */
  typename BaseType::Iterator end() { return this->channels + 4; }

  /**
   * Alternative names for the fourth color channel.
   */
  union { T c3, cmykK, rgbaA; };
};

#include "PiiTypeTraits.h"

namespace Pii
{
  /**
   * Constructs a real-valued counterpart of an integer-channel color.
   */
  template <class T> struct ToFloatingPoint<PiiColor<T> >
  {
    typedef PiiColor<typename ToFloatingPoint<T>::Type> Type;
    typedef typename ToFloatingPoint<T>::Type PrimitiveType;
  };

  /**
   * Constructs a real-valued counterpart of an integer-channel color.
   */
  template <class T> struct ToFloatingPoint<PiiColor4<T> >
  {
    typedef PiiColor4<typename ToFloatingPoint<T>::Type> Type;
    typedef typename ToFloatingPoint<T>::Type PrimitiveType;
  };

  /**
   * A type trait for checking the colorness of a type. Evaluates to
   * `false` with all types but PiiColor and PiiColor4.
   */
  template <class T> struct IsColor : False {};
  template <class T> struct IsColor<PiiColor<T> > : True {};
  template <class T> struct IsColor<PiiColor4<T> > : True {};

  template <class T> struct IsColor3 : False {};
  template <class T> struct IsColor3<PiiColor<T> > : True {};

  template <class T> struct IsColor4 : False {};
  template <class T> struct IsColor4<PiiColor4<T> > : True {};
};

#endif //_PIICOLOR_H
