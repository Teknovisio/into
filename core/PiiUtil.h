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

#ifndef _PIIUTIL_H
#define _PIIUTIL_H

#include <cstdlib>
#include <iostream>
#include <cstdarg>

#include <QList>
#include <QVariant>
#include <QDateTime>
#include <QList>
#include <QStringList>
#include <QtGui/QPolygon>
#include <QPoint>
#include <QSettings>
#include <QVariantMap>
#include <QPair>
#include <QMetaObject>
#include <QMetaProperty>

#include "PiiGlobal.h"
#include "PiiTypeTraits.h"
#include "PiiMathDefs.h"
#include "PiiAlgorithm.h"
#include "PiiTemplateExport.h"

namespace Pii
{
  /**
   * Write a QString to an `std` output stream.
   */
  inline std::ostream& operator<< (std::ostream& out, const QString& str)
  {
    out << str.toLocal8Bit().constData();
    return out;
  }

  /**
   * Find the intersection of two lists. The result contains the
   * elements that are present in both lists, or an empty list if the
   * intersection is empty. Any collection defining `size`(),
   * `contains`(), and `append`() functions can be used as a parameter.
   */
  template <class Collection> Collection intersect(const Collection& list1, const Collection& list2)
  {
    Collection result;
    for (int i=0; i<list1.size(); i++)
      if (list2.contains(list1[i]))
        result.append(list1[i]);
    return result;
  }

  /**
   * Find the union of two lists. The result contains the elements
   * that are present in either list, ignoring duplicates. Any
   * collection defining `size`(), `contains`(), and `append`()
   * functions can be used as a parameter.
   */
  template <class Collection> Collection join(const Collection& list1, const Collection& list2)
  {
    Collection result(list1);
    for (int i=0; i<list2.size(); i++)
      if (!result.contains(list2[i]))
        result.append(list2[i]);
    return result;
  }

  /**
   * Create a list that consists of the elements of `list1` that are
   * not present in `list2`. Any collection defining `size`(),
   * `contains`(), and `append`() functions can be used as a parameter.
   */
  template <class Collection> Collection subtract(const Collection& list1, const Collection& list2)
  {
    Collection result;
    for (int i=0; i<list1.size(); i++)
      if (!list2.contains(list1[i]))
        result.append(list1[i]);
    return result;
  }

  /**
   * Property types for properties().
   */
  enum PropertyFlag
    {
      ReadableProperties = 1,
      WritableProperties = 2,
      StoredProperties   = 4,
      ScriptableProperties = 8,
      DesignableProperties = 16,
      DynamicProperties = 32
    };
  Q_DECLARE_FLAGS(PropertyFlags, PropertyFlag);

  /// @internal
  inline void appendPropertyTo(QVariantMap& map, const char* name, const QVariant& value)
  {
    map.insert(name, value);
  }

  /// @internal
  inline void appendPropertyTo(QList<QPair<QString,QVariant> >& lst, const char* name, const QVariant& value)
  {
    lst.append(qMakePair(QString(name), value));
  }

  /**
   * Returns the properties of a QObject as a `Collection`.
   *
   * @param obj the object whose properties are queried
   *
   * @param propertyOffset skip this many properties in the beginning.
   * This value can be used to skip the properties of superclasses.
   * For example, using one as the *propertyOffset* skips the
   * [QObject::objectName] property.
   *
   * @param flags a logical or of property types to be included in the
   * query (see [Properties](Pii::Properties)). The function only
   * returns properties that match all of the flags. For example, to
   * return only stored properties, use `StoredProperties` as the
   * flag. "Readable" is, of course, always a requirement, so one does
   * not need to explicitly specify it. `DynamicProperties` is an
   * exception: if it is set, dynamic properties will be returned,
   * otherwise not. The default value includes all readable
   * properties, including dynamic ones.
   */
  template <class Collection, class T>
  Collection properties(const T* obj,
                        int propertyOffset = 0,
                        PropertyFlags flags = DynamicProperties)
  {
    Collection result;

    const QMetaObject* pMetaObject = obj->metaObject();
    // Loop through the properties
    for (int i=propertyOffset; i<pMetaObject->propertyCount(); ++i)
      {
        // Store only properties that match our flags
        QMetaProperty prop = pMetaObject->property(i);
        if (prop.isReadable() &&
            (!(flags & WritableProperties) || prop.isWritable()) &&
            (!(flags & StoredProperties) || prop.isStored()) &&
            (!(flags & ScriptableProperties) || prop.isScriptable()) &&
            (!(flags & DesignableProperties) || prop.isDesignable()))
          appendPropertyTo(result, prop.name(), obj->property(prop.name()));
      }

    if (flags & DynamicProperties)
      {
        // Dynamic properties
        QList<QByteArray> lstDynamicPropertyNames = obj->dynamicPropertyNames();
        for (int i=0; i<lstDynamicPropertyNames.size(); i++)
          {
            const char* pName = lstDynamicPropertyNames[i];
            appendPropertyTo(result, pName, obj->property(pName));
          }
      }

    return result;
  }

  PII_CORE_EXPORT QList<QPair<QString,QVariant> > propertyList(const QObject* obj,
                                                               int propertyOffset = 0,
                                                               PropertyFlags flags = DynamicProperties);

  /// @hide
  inline QString propertyNameFrom(QVariantMap::const_iterator i) { return i.key(); }
  inline QVariant propertyValueFrom(QVariantMap::const_iterator i) { return i.value(); }
  template <class Iterator> inline QString propertyNameFrom(Iterator i) { return i->first; }
  template <class Iterator> inline QVariant propertyValueFrom(Iterator i) { return i->second; }
  /// @endhide

  /**
   * Sets many properties in a bunch. Properties will be set in
   * the order *collection* gives them when iterating over it.
   *
   * @param obj the object to be modified
   *
   * @param properties a collection of property name-value pairs
   * (QString, QVariant) or a QVariantMap.
   */
  template <class T, class Collection> bool setProperties(T* obj, const Collection& properties)
  {
    bool bResult = true;
    for (typename Collection::const_iterator i=properties.begin(); i!=properties.end(); ++i)
      bResult &= obj->setProperty(qPrintable(propertyNameFrom(i)),
                                  propertyValueFrom(i));
    return bResult;
  }

  /**
   * Converts a C-style argument list to a QStringList.
   */
  PII_CORE_EXPORT QStringList argsToList(int argc, char* argv[]);

  /**
   * Converts a list of QVariant objects into a QList of type `T`.
   *
   * ~~~(c++)
   * QVariantList lst;
   * lst << 1.0 << 2.3;
   * QList<double> dLst = Pii::variantsToList<double>(lst);
   * ~~~
   */
  template <class T> QList<T> variantsToList(const QVariantList& variants)
  {
    QList<T> result;
    for (int i=0; i<variants.size(); i++)
      result << variants[i].value<T>();
    return result;
  }

  /**
   * Convert any collection to a QVariantList that contains the same
   * values as QVariant objects.
   */
  template <class T, class Collection> QVariantList collectionToVariants(const Collection& lst)
  {
    QVariantList result;
    for (int i=0; i<lst.size(); i++)
      result << lst[i];
    return result;
  }

  /**
   * Convert a QList to a QVariantList that contains the same values
   * as QVariant objects.
   */
  template <class T> inline QVariantList listToVariants(const QList<T>& lst)
  {
    return collectionToVariants<T>(lst);
  }

  /**
   * Convert a QVector to a QVariantList that contains the same values
   * as QVariant objects.
   */
  template <class T> inline QVariantList vectorToVariants(const QVector<T>& lst)
  {
    return collectionToVariants<T>(lst);
  }

  /**
   * Convert a list of QVariant objects into a QVector of type `T`.
   *
   * ~~~(c++)
   * QVariantList lst;
   * lst << 1.0 << 2.3;
   * QVector<double> dLst = Pii::variantsToVector<double>(lst);
   * ~~~
   */
  template <class T> QVector<T> variantsToVector(const QVariantList& variants)
  {
    QVector<T> result(variants.size());
    for (int i=variants.size(); i--; )
      result[i] = variants[i].value<T>();
    return result;
  }

  /**
   * Find all parent objects of `obj` up to `maxParents` parent
   * objects. By default, all parents are returned. If a template
   * parameter is specified, only parents matching the given type are
   * returned. In the returned list, the closest parent comes first
   * and the furthes one last.
   *
   * ~~~(c++)
   * QList<MyObj*> parents = Pii::findAllParents<MyObj*>(obj);
   * // Returns all parents of obj whose type is MyObj*
   * ~~~
   */
  template <class T> QList<T> findAllParents(const QObject* obj,
                                             unsigned int maxParents = Pii::Numeric<unsigned int>::maxValue())
  {
    QList<T> result;
    QObject* parentObj = obj->parent();
    while (parentObj != 0 && maxParents--)
      {
        T obj = qobject_cast<T>(parentObj);
        if (obj != 0)
          result << obj;
        parentObj = parentObj->parent();
      }
    return result;
  }

  /**
   * Returns `true` if *obj* is an instance of *className*.
   *
   * ~~~(c++)
   * QObject* pWidget = new QWidget;
   * Pii::isA("QWidget", pWidget); // true
   * Pii::isA("QFrame", pWidget); // false
   * ~~~
   */
  PII_CORE_EXPORT bool isA(const char* className, const QObject* obj);

  /**
   * Find all parents independent of their type.
   */
  PII_CORE_EXPORT QList<QObject*> findAllParents(const QObject* obj, unsigned int maxParents = Pii::Numeric<unsigned int>::maxValue());

  /**
   * Find the first parent of `obj` whose type matches `T`.
   * Equivalent to findAllParents(obj,1), but faster.
   */
  template <class T> T findFirstParent(const QObject* obj)
  {
    QObject* parentObj = obj->parent();
    while (parentObj != 0)
      {
        T obj = qobject_cast<T>(parentObj);
        if (obj != 0)
          return obj;
        parentObj = parentObj->parent();
      }
    return 0;
  }

  /**
   * Returns `true` if `parent` is a parent of `child`.
   */
  PII_CORE_EXPORT bool isParent(const QObject* parent, const QObject* child);

  /**
   * Find the object that is the closest common ancestor of `obj1`
   * and `obj2` in the object hierarchy.
   *
   * @param obj1 first object
   *
   * @param obj2 second object
   *
   * @param parentIndex if non-zero, the index of the closest parent
   * object in `obj1`'s parent list is stored here.
   *
   * @return a pointer to the parent found, or zero if the objects
   * don't have a common parent.
   */
  PII_CORE_EXPORT QObject* findCommonParent(const QObject* obj1, const QObject* obj2, int* parentIndex = 0);

  /**
   * Delete all members of a collection. This function works for any
   * stl or Qt collection. It goes through all members of the
   * collection and deletes each.
   *
   * @param c the collection. Any collection (e.g. QList, QMap, or
   * std::list) containing pointers.
   */
  template <class Collection> void deleteAll(Collection& c)
  {
    for (typename Collection::iterator i = c.begin(); i != c.end(); i++)
      delete i.operator*();
  }

  /**
   * Hash function for null-terminated C strings.
   */
  PII_CORE_EXPORT uint qHash(const char* key);

  /**
   * Match a list of crontab-like strings against the given time
   * stamp. Each string in `list` represents a rule with a syntax
   * similar but not equivalent to crontab (man crontab). Each rule
   * contains six fields separated by spaces. The fields are (in this
   * order): minute, hour, day, month, day of week, and week number.
   * Each field may contain a comma-separated list of values or value
   * ranges, or an asterisk (*) that denotes "all". A rule matches if
   * all of its fields match. A set of rules matches if any of its
   * rules matches.
   *
   * ~~~(c++)
   * // Returns true during the first minute after midnight every day and
   * // 4:00-6:00 on the first day of every month
   * matchCrontab(QStringList() <<
   *              "0 0 * * * *" <<
   *              "* 4-5 1 * * *");
   *
   * // Returns true between 8:30 and 8:31 on Monday, Wednesday, and
   * // Friday on weeks 1, 2, 3, 5, 6, 7, 8, and 9
   * matchCrontab(QStringList() <<
   *              "30 8 * * 1,3,5 1-3,5-9);
   * ~~~
   *
   * @param list a list of crontab entries.
   *
   * @param timeStamp the time to match the entries against
   *
   * @return `true` if any of the entries match the time stamp.
   */
  PII_CORE_EXPORT bool matchCrontab(QStringList list, QDateTime timeStamp = QDateTime::currentDateTime());

  /**
   * Flags for controlling property decoding.
   *
   * - `NoDecodingFlag` - no options apply
   *
   * - `TrimPropertyName` - remove white space in the beginning and
   * end of property name.
   *
   * - `TrimPropertyValue` - remove white space in the beginning and
   * end of property value.
   *
   * - `RemoveQuotes` - remove double quotes around property
   * names/values. Implies `TrimPropertyName` and
   * `TrimPropertyValue`.
   *
   * - `DowncasePropertyName` - convert all property names to lower
   * case.
   */
  enum PropertyDecodingFlag
    {
      NoDecodingFlag = 0,
      TrimPropertyName = 1,
      TrimPropertyValue = 2,
      RemoveQuotes = 4,
      DowncasePropertyName = 8
    };

  Q_DECLARE_FLAGS(PropertyDecodingFlags, PropertyDecodingFlag)
  Q_DECLARE_OPERATORS_FOR_FLAGS(PropertyDecodingFlags)

  /**
   * Returns the index of the first occurrence of *separator* in
   * `str`, starting at *startIndex*. This function ignores occurrences
   * that are preceded by an odd number of *escape* characters. If
   * the separator is not found, returns -1.
   *
   * ~~~(c++)
   * Pii::findSeparator("\"Test \\"string\"", '"', 1); // returns 14
   * ~~~
   */
  PII_CORE_EXPORT int findSeparator(const QString& str, QChar separator, int startIndex = 0, QChar escape = QChar('\\'));

  /**
   * Splits a string in which each part may be quoted.
   *
   * ~~~(c++)
   * Pii::splitQuoted("\"a,b,c\",d,e"); // returns ("a,b,c", "d", "e")
   * ~~~
   */
  PII_CORE_EXPORT QStringList splitQuoted(const QString& str, QChar separator = QChar(','), QChar quote = QChar('"'),
                                          QString::SplitBehavior behavior = QString::KeepEmptyParts);

  /**
   * Decode string-encoded properties into a variant map. This
   * function can be used to parse ini files, css-style properties or
   * anything similar to a map of name-value pairs. All values will be
   * QStrings, but they are stored as QVariants for easy cooperation
   * with [setProperties()].
   *
   * @param encodedProperties the string to decode
   *
   * @param propertySeparator a character that separates name-value
   * pairs
   *
   * @param valueSeparator a character that separates name from value
   *
   * @param escape escape character
   *
   * @param flags flags for controlling the decoding
   *
   * @return a map of name-value pairs
   *
   * ~~~(c++)
   * QVariantMap props = Pii::decodeProperties("color: #fff;font-size: 5pt", ';', ':');
   * // props now has two values:
   * // "color" -> "#fff"
   * // "font-size" -> "5pt"
   * ~~~
   */
  PII_CORE_EXPORT QVariantMap decodeProperties(const QString& encodedProperties,
                                               QChar propertySeparator = QChar('\n'),
                                               QChar valueSeparator = QChar('='),
                                               QChar escape = QChar('\\'),
                                               PropertyDecodingFlags flags = TrimPropertyName | TrimPropertyValue | RemoveQuotes);

  /**
   * Replaces variables in *string* and returns a new string.
   *
   * @param string the input string with variables. Variables are
   * prefixed with a dollar sign and optionally delimited by curly
   * braces. ($variable or ${variable}).
   *
   * @param variables a map of variable values. This can be any type
   * that defines `QString operator[] (const QString&)`.
   *
   * ~~~(c++)
   * QMap<QString,QString> mapVariables;
   * mapVariables["foo"] = "bar";
   * mapVariables["bar"] = "foo";
   * QString strResult = Pii::replaceVariables("$foo ${bar}", mapVariables);
   * // strResult == "bar foo"
   * ~~~
   */
  template <class VariableMap> QString replaceVariables(const QString& string, const VariableMap& variables)
  {
    QString strResult(string);
    QRegExp reVariable("\\$((\\w+)|\\{(\\w+)\\})");
    int index = 0;
    while ((index = reVariable.indexIn(strResult, index)) != -1)
      {
        QString strReplacement;
        if (!reVariable.cap(2).isEmpty()) // no curly braces
          strReplacement = variables[reVariable.cap(2)];
        else if (!reVariable.cap(3).isEmpty()) // curly braces
          strReplacement = variables[reVariable.cap(3)];
        strResult.replace(index, reVariable.matchedLength(), strReplacement);
        index += strReplacement.size();
      }
    return strResult;
  }


  /**
   * Finds the name of a property in `obj` that matches `name` in a
   * case-insensitive manner.
   *
   * @return the real name of the property or 0 if no such property exists
   *
   * ~~~(c++)
   * QObject obj;
   * const char* objName = Pii::propertyName(obj, "objectname");
   * // returns "objectName"
   * ~~~
   */
  PII_CORE_EXPORT const char* propertyName(const QObject* obj, const QString& name);

  /**
   * Sets properties to an object. For convenient use with
   * configuration files, the function ignores values starting with a
   * comment marker ('#' by default).
   *
   * @param properties a list of property names to set
   *
   * @param valueMap a map that stores the actual property values
   *
   * @param sensitivity if `Qt::CaseSensitive`, the property names
   * must match exactly. If `Qt::CaseInsensitive`, case-insensitive
   * matching will be performed ([propertyName()]).
   *
   * @param commentMark ignore values starting with this string
   *
   * ~~~(c++)
   * // Read properties from an ini file (case-sensitive)
   * QSettings settings(configFile, QSettings::IniFormat);
   * Pii::setProperties(obj, settings.childKeys(), settings);
   * ~~~
   *
   * ~~~(c++)
   * // Read string-encoded properties (case-insensitive)
   * QVariantMap properties = Pii::decodeProperties("objectname=foobar\n"
   *                                                "//comment, ignored");
   * Pii::setProperties(obj, properties.keys(), properties, Qt::CaseInsensitive, "//");
   * ~~~
   */
  template <class Map> bool setProperties(QObject* obj, const QStringList& properties, const Map& valueMap,
                                          Qt::CaseSensitivity sensitivity = Qt::CaseSensitive,
                                          const QString& commentMark = "#");


  // Declare two exported explicit instantiations.
  PII_DECLARE_EXPORTED_FUNCTION_TEMPLATE(bool, setProperties<QSettings>,
                                         (QObject* obj,
                                          const QStringList& properties,
                                          const QSettings& valueMap,
                                          Qt::CaseSensitivity sensitivity,
                                          const QString& commentMark),
                                         PII_BUILDING_CORE);
  PII_DECLARE_EXPORTED_FUNCTION_TEMPLATE(bool, setProperties<QVariantMap>,
                                         (QObject* obj,
                                          const QStringList& properties,
                                          const QVariantMap& valueMap,
                                          Qt::CaseSensitivity sensitivity,
                                          const QString& commentMark),
                                         PII_BUILDING_CORE);

  /**
   * Performs array copy of non-overlapping arrays. If used with
   * primitive type, uses std::memcpy, else uses operator= one by one.
   *
   * @param to Pointer to destination array.
   * @param from Pointer to source array.
   * @param itemCount Number of items of type T to copy.
   *
   * ! Behavior is undefined if arrays `to` and `from` overlap.
   */
  template<class T, class size_type>
  inline void arrayCopy(T* to, const T* from, const size_type itemCount)
  {
    // Using simple if instead of defining further template implementation.
    if (Pii::IsPrimitive<T>::boolValue)
      std::memcpy(to, from, sizeof(T)*itemCount);
    else
      {
        for(size_type i = 0; i<itemCount; ++i)
          to[i] = from[i];
      }
  }

  /**
   * Converts a string into a number. This function differs from
   * QString::toDouble() and friends in that it recognizes magnitude
   * suffices.
   *
   * ~~~(c++)
   * Pii::toDouble("10k"); // returns 10000
   * Pii::toDouble("-2M"); // returns -2000000
   * Pii::toDouble("1.2m"); // returns 0.0012
   * ~~~
   *
   * The following suffices are recognized:
   *
   * <table style="border: 0">
   * <tr><th>Name</th><th>Symbol</th><th>Magnitude</th></tr>
   * <tr><td>yotta</td><td>Y</td><td>\(10^{24}\)</td></tr>
   * <tr><td>zetta</td><td>Z</td><td>\(10^{21}\)</td></tr>
   * <tr><td>exa</td><td>E</td><td>\(10^{18}\)</td></tr>
   * <tr><td>peta</td><td>P</td><td>\(10^{15}\)</td></tr>
   * <tr><td>tera</td><td>T</td><td>\(10^{12}\)</td></tr>
   * <tr><td>giga</td><td>G</td><td>\(10^{9}\)</td></tr>
   * <tr><td>mega</td><td>M</td><td>\(10^{6}\)</td></tr>
   * <tr><td>kilo</td><td>k</td><td>\(10^{3}\)</td></tr>
   * <tr><td>hecto</td><td>h</td><td> \(10^{2}\)</td></tr>
   * <tr><td>deka</td><td>e</td><td> \(10^{1}\)</td></tr>
   * <tr><td>deci</td><td>d</td><td>\(10^{-1}\)</td></tr>
   * <tr><td>centi</td><td>c</td><td> \(10^{-2}\)</td></tr>
   * <tr><td>milli</td><td>m</td><td> \(10^{-3}\)</td></tr>
   * <tr><td>micro</td><td>u</td><td> \(10^{-6}\)</td></tr>
   * <tr><td>nano</td><td>n</td><td>\(10^{-9}\)</td></tr>
   * <tr><td>pico</td><td>p</td><td>\(10^{-12}\)</td></tr>
   * <tr><td>femto</td><td>f</td><td> \(10^{-15}\)</td></tr>
   * <tr><td>atto</td><td>a</td><td>\(10^{-18}\)</td></tr>
   * <tr><td>zepto</td><td>z</td><td> \(10^{-21}\)</td></tr>
   * <tr><td>yocto</td><td>y</td><td> \(10^{-24}\)</td></tr>
   * </table>
   *
   * @param number the string to convert
   *
   * @param ok an optional pointer to a `bool` that indicates if the
   * conversion was successful.
   */
  PII_CORE_EXPORT double toDouble(const QString& number, bool* ok = 0);

  /**
   * Converts a string to any type.
   */
  template <class T> inline T stringTo(const QString& number, bool* ok = 0) { return T(number.toInt(ok)); }
  template <> inline short stringTo<short>(const QString& number, bool* ok) { return number.toShort(ok); }
  template <> inline unsigned short stringTo<unsigned short>(const QString& number, bool* ok) { return number.toUShort(ok); }
  template <> inline int stringTo<int>(const QString& number, bool* ok) { return number.toInt(ok); }
  template <> inline unsigned int stringTo<unsigned int>(const QString& number, bool* ok) { return number.toUInt(ok); }
  template <> inline long stringTo<long>(const QString& number, bool* ok) { return number.toLong(ok); }
  template <> inline unsigned long stringTo<unsigned long>(const QString& number, bool* ok) { return number.toULong(ok); }
  template <> inline long long stringTo<long long>(const QString& number, bool* ok) { return number.toLongLong(ok); }
  template <> inline unsigned long long stringTo<unsigned long long>(const QString& number, bool* ok) { return number.toULongLong(ok); }
  template <> inline float stringTo<float>(const QString& number, bool* ok) { return number.toFloat(ok); }
  template <> inline double stringTo<double>(const QString& number, bool* ok) { return number.toDouble(ok); }

  /**
   * Replaces non-ASCII characters with backslash codes and adds
   * backslashes in front of backslashes and double quotation marks.
   */
  PII_CORE_EXPORT QString escape(const QString& source);

  /**
   * Converts a variant to a string suitable for use as a value in
   * many programming languages. If *value* is a string, it will be
   * quoted and converted to ASCII-only characters. Numeric and
   * boolean values will be converted with QVariant::toString(). If
   * *value* is not any of the supported types, a null string will be
   * returned.
   */
  PII_CORE_EXPORT QString escape(const QVariant& value);

  /**
   * Strips escape sequences from *value*.
   */
  PII_CORE_EXPORT QString unescapeString(const QString& value);

  /**
   * Decodes *value* and returns the result as a variant. This
   * function recognizes numbers, boolean values and strings.
   */
  PII_CORE_EXPORT QVariant unescapeVariant(const QString& value);

  /**
   * Recursively connects neighbors given as a list of pairs. The
   * input contains a list of index pairs, each indicating a
   * "neighbor" relationship. The function scans through the list and
   * forms chains of neighbors. Finally, it returns a list in which
   * each entry contains a list of indices that were chained. Each
   * list is sorted in ascending order.
   *
   * ~~~(c++)
   * QLinkedList<QPair<int,int> > lstPairs;
   * lstPairs << qMakePair(0,1) << qMakePair(0,3) << qMakePair(0,4) << qMakePair(3,4) << qMakePair(3,5);
   * lstPairs << qMakePair(2,6) << qMakePair(6,8) << qMakePair(7,8);
   *
   * QList<QList<int> > lstNeighbors(Pii::findNeighbors(lstPairs));
   * // ((0, 1, 3, 4, 5), (2, 6, 7, 8))
   * ~~~
   */
  PII_CORE_EXPORT QList<QList<int> > findNeighbors(QLinkedList<QPair<int,int> >& pairs);

  /// @relates findDependencies
  enum DependencyOrder
    {
      AnyValidOrder,
      AnyLayeredOrder,
      SortedLayeredOrder
    };

  /**
   * Given a directed acyclic graph represented as a list of edges
   * between vertices, produces a valid topological sort. In English:
   * given a list of dependencies, produces a valid ordering. This
   * function can be used to resolve execution order for dependent
   * operations etc.
   *
   * Graph edges are represented as pairs. Each pair (a,b) in *edges*
   * denotes a directed edge from vertex a to verted b. In other
   * words, b depends on a. Each vertex is an integer, and it is
   * typically used as an array index when resolving dependencies
   * between objects.
   *
   * The returned value contains 0-N lists, each storing a number of
   * indices from *edges*.
   *
   * - If *order* is `AnyValidOrder`, there is at most one output
   *   list, and it contains the input elements in an order that is a
   *   valid topological sort.
   *
   * - If *order* is `AnyLayeredOrder`, the elements in each list only
   *   depend on those on the previous lists, and have no
   *   inter-dependencies. The elements in the first list don't depend
   *   on anything. The elements in each list are in an unspecified
   *   order.
   *
   * - If *order* is `SortedLayeredOrder`, the behaviour is otherwise
   *   equal to `AnyLayeredOrder`, but the elements in each list are
   *   sorted in ascending order.
   *
   * The result is empty if the input contains no loopless graphs. If
   * the graph contains loops, they will be left in *edges*.
   *
   * ! The input list is modified by this function. If there are no
   *   loops in the graph, *edges* will be empty upon return.
   *
   * ~~~(c++)
   * QLinkedList<QPair<int,int> > lstEdges;
   * lstEdges << qMakePair(0,1) << qMakePair(0,3) << qMakePair(0,4) << qMakePair(3,4) << qMakePair(3,5)
   *          << qMakePair(2,6) << qMakePair(6,8) << qMakePair(7,8);
   *
   * QList<QList<int> > lstValid(Pii::findDependencies(lstEdges), Pii::AnyValidOrder);
   * // ((0, 1, 3, 4, 5, 2, 6, 7, 8))
   *
   * // lstEdges is empty now. Assume it is filled with the same data again.
   * QList<QList<int> > lstLayered(Pii::findDependencies(lstEdges), Pii::SortedLayeredOrder);
   * // ((0, 2, 7), (1, 3, 6), (4, 5, 8))
   * ~~~
   */
  PII_CORE_EXPORT QList<QList<int> > findDependencies(QLinkedList<QPair<int,int> >& edges,
                                                      DependencyOrder order = AnyValidOrder);
}

Q_DECLARE_OPERATORS_FOR_FLAGS(Pii::PropertyFlags);

template <class T> QList<T> operator&& (const QList<T>& list1, const QList<T>& list2)
{
  return Pii::intersect(list1, list2);
}

inline QStringList operator&& (const QStringList& list1, const QStringList& list2)
{
  return Pii::intersect(list1, list2);
}

template <class T> QList<T> operator|| (const QList<T>& list1, const QList<T>& list2)
{
  return Pii::join(list1, list2);
}

inline QStringList operator|| (const QStringList& list1, const QStringList& list2)
{
  return Pii::join(list1, list2);
}

template <class T> QList<T> operator- (const QList<T>& list1, const QList<T>& list2)
{
  return Pii::subtract(list1, list2);
}

inline QStringList operator- (const QStringList& list1, const QStringList& list2)
{
  return Pii::subtract(list1, list2);
}

#endif //_PIIUTIL_H
