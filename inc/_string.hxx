#pragma once
#include <utility>
#include "_cctype.hxx"

using std::pair;




#pragma region METHODS
#pragma region FIND NEXT
/**
 * Find the next line in a string.
 * @tparam EOL end of line character
 * @param ib begin iterator
 * @param ie end iterator
 * @returns iterator to next line
 */
template <char EOL='\n', class I>
inline I findNextLine(I ib, I ie) {
  for (; ib<ie; ++ib)
    if (*ib==EOL) return ++ib;
  return ie;
}


/**
 * Find the next whitespace in a string.
 * @param ib begin iterator
 * @param ie end iterator
 * @param fw is special whitespace, e.g. comma? (c)
 * @returns iterator to next whitespace
 */
template <class I, class FW>
inline I findNextWhitespace(I ib, I ie, FW fw) {
  for (; ib<ie; ++ib)
    if (isBlank(*ib) || isNewline(*ib) || fw(*ib)) return ib;
  return ie;
}


/**
 * Find the next non-whitespace in a string.
 * @param ib begin iterator
 * @param ie end iterator
 * @param fw is special whitespace, e.g. comma? (c)
 * @returns iterator to next non-whitespace
 */
template <class I, class FW>
inline I findNextNonWhitespace(I ib, I ie, FW fw) {
  for (; ib<ie; ++ib)
    if (!isBlank(*ib) && !isNewline(*ib) && !fw(*ib)) return ib;
  return ie;
}


/**
 * Find the next blank in a string.
 * @param ib begin iterator
 * @param ie end iterator
 * @param fu is special blank, e.g. comma? (c)
 * @returns iterator to next blank
 */
template <class I, class FU>
inline I findNextBlank(I ib, I ie, FU fu) {
  for (; ib<ie; ++ib)
    if (isBlank(*ib) || fu(*ib)) return ib;
  return ie;
}


/**
 * Find the next non-blank in a string.
 * @param ib begin iterator
 * @param ie end iterator
 * @param fu is special blank, e.g. comma? (c)
 * @returns iterator to next non-blank
 */
template <class I, class FU>
inline I findNextNonBlank(I ib, I ie, FU fu) {
  for (; ib<ie; ++ib)
    if (!isBlank(*ib) && !fu(*ib)) return ib;
  return ie;
}


/**
 * Find the next digit in a string.
 * @param ib begin iterator
 * @param ie end iterator
 * @returns iterator to next digit
 */
template <class I>
inline I findNextDigit(I ib, I ie) {
  for (; ib!=ie && !isDigit(*ib); ++ib);
  return ib;
}


/**
 * Find the next non-digit in a string.
 * @param ib begin iterator
 * @param ie end iterator
 * @returns iterator to next non-digit
 */
template <class I>
inline I findNextNonDigit(I ib, I ie) {
  for (; ib!=ie && isDigit(*ib); ++ib);
  return ib;
}


/**
 * Find the next token in a string.
 * @param ib begin iterator
 * @param ie end iterator
 * @param fu is special blank, e.g. comma? (c)
 * @param fw is special whitespace, e.g. comma? (c)
 * @returns [begin, end) iterators to next token
 */
template <class I, class FU, class FW>
inline pair<I, I> findNextToken(I ib, I ie, FU fu, FW fw) {
  auto tb = findNextNonBlank(ib, ie, fu);
  auto te = findNextWhitespace(tb+1, ie, fw);
  return {tb, te};
}
#pragma endregion
#pragma endregion
