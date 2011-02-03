/*
*******************************************************************************
*   Copyright (C) 2011, International Business Machines
*   Corporation and others.  All Rights Reserved.
*******************************************************************************
*   file name:  appendable.h
*   encoding:   US-ASCII
*   tab size:   8 (not used)
*   indentation:4
*
*   created on: 2010dec07
*   created by: Markus W. Scherer
*/

#ifndef __APPENDABLE_H__
#define __APPENDABLE_H__

/**
 * \file
 * \brief C++ API: Appendable interface: Sink for Unicode code points and 16-bit code units (UChars).
 */

#include "unicode/utypes.h"
#include "unicode/uobject.h"

U_NAMESPACE_BEGIN

/**
 * Interface/mixin class for objects to which Unicode characters and strings can be appended.
 * Combines elements of Java Appendable and ICU4C ByteSink.
 *
 * This class is not intended for public subclassing.
 * You should subclass AppendableAdapter instead, in case methods are added to this interface.
 *
 * This interface can be used in APIs where it does not matter whether the actual destination is
 * a UnicodeString, a UChar[] array, a UnicodeSet, or any other object
 * that receives and processes characters and/or strings.
 *
 * Note: UnicodeString is-an Appendable.
 *
 * The methods do not take UErrorCode parameters.
 * If an error occurs (e.g., out-of-memory),
 * in addition to returning FALSE from failing operations,
 * the implementation must prevent unexpected behavior (e.g., crashes)
 * from further calls and should make the error condition available separately
 * (e.g., store a UErrorCode, make/keep a UnicodeString bogus).
 * @draft ICU 4.8
 */
class U_COMMON_API Appendable /* not : public UObject because this is an interface/mixin class */ {
public:
    /**
     * Destructor.
     * @draft ICU 4.8
     */
    virtual ~Appendable();

    /**
     * Appends a 16-bit code unit.
     * @param c code unit
     * @return TRUE if the operation succeeded
     * @draft ICU 4.8
     */
    virtual UBool appendCodeUnit(UChar c) = 0;

    /**
     * Appends a code point.
     * @param c code point 0..0x10ffff
     * @return TRUE if the operation succeeded
     * @draft ICU 4.8
     */
    virtual UBool appendCodePoint(UChar32 c) = 0;

    /**
     * Appends a string.
     * @param s string, must not be NULL if length!=0
     * @param length string length, or -1 if NUL-terminated
     * @return TRUE if the operation succeeded
     * @draft ICU 4.8
     */
    virtual UBool appendString(const UChar *s, int32_t length) = 0;

    /**
     * Tells the object that the caller is going to append roughly
     * appendCapacity UChars. A subclass might use this to pre-allocate
     * a larger buffer if necessary.
     * @param appendCapacity estimated number of UChars that will be appended
     * @return TRUE if the operation succeeded
     * @draft ICU 4.8
     */
    virtual UBool reserveAppendCapacity(int32_t appendCapacity) = 0;

    /**
     * Returns a writable buffer for appending and writes the buffer's capacity to
     * *resultCapacity. Guarantees *resultCapacity>=minCapacity.
     * May return a pointer to the caller-owned scratch buffer which must have
     * scratchCapacity>=minCapacity.
     * The returned buffer is only valid until the next operation
     * on this Appendable.
     *
     * After writing at most *resultCapacity UChars, call appendString() with the
     * pointer returned from this function and the number of UChars written.
     * Many appendString() implementations will avoid copying UChars if this function
     * returned an internal buffer.
     *
     * Partial usage example:
     * \code
     *  int32_t capacity;
     *  UChar* buffer = app.getAppendBuffer(..., &capacity);
     *  ... Write n UChars into buffer, with n <= capacity.
     *  app.appendString(buffer, n);
     * \endcode
     * In many implementations, that call to append will avoid copying UChars.
     *
     * If the Appendable allocates or reallocates an internal buffer, it should use
     * the desiredCapacityHint if appropriate.
     * If a caller cannot provide a reasonable guess at the desired capacity,
     * it should pass desiredCapacityHint=0.
     *
     * If a non-scratch buffer is returned, the caller may only pass
     * a prefix to it to appendString().
     * That is, it is not correct to pass an interior pointer to appendString().
     *
     * @param minCapacity required minimum capacity of the returned buffer;
     *                    must be non-negative
     * @param desiredCapacityHint desired capacity of the returned buffer;
     *                            must be non-negative
     * @param scratch default caller-owned buffer
     * @param scratchCapacity capacity of the scratch buffer
     * @param resultCapacity pointer to an integer which will be set to the
     *                       capacity of the returned buffer
     * @return a buffer with *resultCapacity>=minCapacity
     * @draft ICU 4.8
     */
    virtual UChar *getAppendBuffer(int32_t minCapacity,
                                   int32_t desiredCapacityHint,
                                   UChar *scratch, int32_t scratchCapacity,
                                   int32_t *resultCapacity) = 0;
};

/**
 * Default Appendable implementation.
 * This class provides default implementations for all Appendable methods
 * except appendCodeUnit(), to make subclassing easy.
 * @draft ICU 4.8
 */
class U_COMMON_API AppendableAdapter : public UObject, public Appendable {
public:
    /**
     * Appends a code point.
     * The default implementation calls appendCodeUnit(UChar) once or twice.
     * @param c code point 0..0x10ffff
     * @return TRUE if the operation succeeded
     * @draft ICU 4.8
     */
    virtual UBool appendCodePoint(UChar32 c);

    /**
     * Appends a string.
     * The default implementation calls appendCodeUnit(UChar) for each code unit.
     * @param s string, must not be NULL if length!=0
     * @param length string length, or -1 if NUL-terminated
     * @return TRUE if the operation succeeded
     * @draft ICU 4.8
     */
    virtual UBool appendString(const UChar *s, int32_t length);

    /**
     * Tells the object that the caller is going to append roughly
     * appendCapacity UChars. A subclass might use this to pre-allocate
     * a larger buffer if necessary.
     * The default implementation does nothing. (It always returns TRUE.)
     * @param appendCapacity estimated number of UChars that will be appended
     * @return TRUE if the operation succeeded
     * @draft ICU 4.8
     */
    virtual UBool reserveAppendCapacity(int32_t appendCapacity);

    /**
     * Returns a writable buffer for appending and writes the buffer's capacity to
     * *resultCapacity. Guarantees *resultCapacity>=minCapacity.
     * May return a pointer to the caller-owned scratch buffer which must have
     * scratchCapacity>=minCapacity.
     * The returned buffer is only valid until the next operation
     * on this Appendable.
     *
     * For details see Appendable::getAppendBuffer().
     *
     * The default implementation always returns the scratch buffer.
     *
     * @param minCapacity required minimum capacity of the returned buffer;
     *                    must be non-negative
     * @param desiredCapacityHint desired capacity of the returned buffer;
     *                            must be non-negative
     * @param scratch default caller-owned buffer
     * @param scratchCapacity capacity of the scratch buffer
     * @param resultCapacity pointer to an integer which will be set to the
     *                       capacity of the returned buffer
     * @return a buffer with *resultCapacity>=minCapacity
     * @draft ICU 4.8
     */
    virtual UChar *getAppendBuffer(int32_t minCapacity,
                                   int32_t desiredCapacityHint,
                                   UChar *scratch, int32_t scratchCapacity,
                                   int32_t *resultCapacity);

private:
    // No ICU "poor man's RTTI" for this class nor its subclasses.
    virtual UClassID getDynamicClassID() const;
};

U_NAMESPACE_END

#endif  // __APPENDABLE_H__
