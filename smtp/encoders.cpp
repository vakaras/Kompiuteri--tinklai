#include "encoders.h"


using namespace std;


/// Copied from: http://stackoverflow.com/questions/5288076/doing-base64-encoding-and-decoding-in-openssl-c
string smtp::encoders::base64_encode(const string &bindata) {
  using std::string;
  using std::numeric_limits;

  if (bindata.size() >
      (numeric_limits<string::size_type>::max() / 4u) * 3u) {
    throw length_error("Converting too large a string to base64.");
    }

  const size_t binlen = bindata.size();
  // Use = signs so the end is properly padded.
  string retval((((binlen + 2) / 3) * 4), '=');
  size_t outpos = 0;
  int bits_collected = 0;
  unsigned int accumulator = 0;
  const string::const_iterator binend = bindata.end();

  for (string::const_iterator i = bindata.begin(); i != binend; ++i) {
    accumulator = (accumulator << 8) | (*i & 0xffu);
    bits_collected += 8;
    while (bits_collected >= 6) {
      bits_collected -= 6;
      retval[outpos++] = B64_TABLE[(accumulator >> bits_collected) & 0x3fu];
    }
  }
  if (bits_collected > 0) { // Any trailing bits that are missing.
    assert(bits_collected < 6);
    accumulator <<= 6 - bits_collected;
    retval[outpos++] = B64_TABLE[accumulator & 0x3fu];
  }
  assert(outpos >= (retval.size() - 2));
  assert(outpos <= retval.size());
  return retval;
}

/// Copied from: http://stackoverflow.com/questions/5288076/doing-base64-encoding-and-decoding-in-openssl-c
string smtp::encoders::base64_decode(const string &ascdata) {
  using std::string;
  string retval;
  const string::const_iterator last = ascdata.end();
  int bits_collected = 0;
  unsigned int accumulator = 0;

  for (string::const_iterator i = ascdata.begin(); i != last; ++i) {
    const int c = *i;
    if (isspace(c) || c == '=') {
      // Skip whitespace and padding. Be liberal in what you accept.
      continue;
    }
    if ((c > 127) || (c < 0) || (REVERSE_TABLE[c] > 63)) {
      throw invalid_argument(
          "This contains characters not legal in a base64 encoded string.");
    }
    accumulator = (accumulator << 6) | REVERSE_TABLE[c];
    bits_collected += 6;
    if (bits_collected >= 8) {
      bits_collected -= 8;
      retval += (char)((accumulator >> bits_collected) & 0xffu);
      }
    }
  return retval;
  }


/// Encodes data into Encoded-Word format.
string smtp::encoders::word_encode(const string &data) {

  string encoded_data = base64_encode(data);
  return "=?utf-8?B?" + encoded_data + "?=";
  }
