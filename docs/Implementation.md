# Implementation

This document is dedicated to the nitty gritty details of how SPSL works under the hood.

## Architecture overview

The string templates `spsl::ArrayString`/`spsl::ArrayStringW` and the classes
`spsl::PasswordString`/`spsl::PasswordStringW` are only typedefs based on the
internal SPSL implementation templates.
These are structured into 3 levels:
1. Storage templates
2. the `spsl::StringCore` template
3. the `spsl::StringBase` template

The "Storage" templates actually implement and encapsulate all the "specialties" of the
different string variants. The `spsl::StorageArray` template implements all the features
for an array-based string and the `spsl::StoragePassword` template handles all specific
memory handling for sensitive data.

The `spsl::StringCore` uses the underlying storage implementation to provide a basic
`std::basic_string`-like class template. It implements assignment, appending, searching,
iterators and so on. There are some methods that it doesn't implement: erasing parts of the
string, more advanced searching, replacing parts of the string and insertion. These are
added by the additional `spsl::StringBase` template.


### Why the distinction between StringCore and StringBase?

The methods that are left out of `spsl::StringCore` are the "more advanced" functionalities
specified for `std::basic_string` by the C++ standard. I chose to separate them in order
to make them "optional" for any string class. The user of the library can decide whether to
include them or not by "typedef'ing" the string class of choice to use either `spsl::StringCore`
or `spsl::StringBase`.

The typedefs `spsl::PasswordString`/`spsl::PasswordStringW` actually use `spsl::StringCore`
instead of `spsl::StringBase` in order to provide a simpler and more restricted API.
When working with password-like strings, there is no need to erase or replace arbitratry parts
of the string, neither will anyone insert something into it. Passwords are usually assigned,
sometimes constructed by appending pieces (e.g. when reading the password character by character
from a terminal), but providing the complex manipulation or search functions is almost always
not useful.

If you disagree, feel free to change the typedef or define your own to use the full-fledged
`spsl::StringBase` for your passwords - it will work. Or even better: Start a discussion -
maybe I'm wrong :-)


### Why storage implementations instead of allocators?

The functionality that is necessary in order to provide useful implementations of an array-based
string or a string that can hold sensitive data goes far beyond allocation.
The storage implementations contain basic implementations for all major classes of functionality:
appending, assigning, replacing, erasing and so on.

The main reason for this design is the `ThrowOnTruncate` option in the
`spsl::StorageArray` template:
A string class that is based on a fixed-size array can behave in two different ways
when it needs to grow past its maximum capacity: It can either throw an exception or it can
silently truncate the data.
The choice depends on the application using the string: If you're replacing C-style arrays with
an `ArrayString`, you might want to preserve the truncation behavior that all the C functions
(`strncpy`, `strncat`, ...), e.g. because the string will be passed to another API that will
itself truncate the string anyway.
But if data loss isn't an option, exceptions can be the way to go. This way, truncation is detected
before it occurs and can be handled by the program.

The default for `spsl::ArrayString`/`spsl::ArrayStringW` is to truncate the string if needed.
Keep in mind that truncation happens on the *byte* level - if you're using UTF-8 strings, this
might lead to broken characters. Use this option wisely.

A last word, just to avoid confusion: `spsl::PasswordString`/`spsl::PasswordStringW` will
*never* truncate - this option only exists for the array variant. Unless, of course, you found
a bug... ;-)


### Are there incompatibilities when replacing std::string/std::wstring with SPSL strings?

In general, all methods and typedefs that are specified for `std::basic_string` are present
in `spsl::StringBase`. `spsl::StringCore` only provides a sub-set, and this is used for
`spsl::PasswordString`/`spsl::PasswordStringW`.

Currently missing are only stream operators (`<<` / `>>`). Feel free to submit a patch ;-)

There are however some minor differences in the actual funtion signatures, that allow "more"
than what `std::basic_string` does:
* When there are differences between different C++ versions, SPSL implements the version
  specified for C++ 17. This usually means more "noexcept" specifiers or additional default
  arguments, so nothing that should break anyone's code.
* Overloads that are supposed to accept an `std::basic_string_view` instance are templatized,
  so that they accept other string-like classes in general.
* There may be more functions marked as "noexcept", even if the standard doesn't require it.

The second item is very useful when mixing different string templates, because you can pass
any other string class to an SPSL string classes' methods. For example, you can assign or append
an `std::string` to an `spsl::ArrayString` or `spsl::PasswordString`, or you can use
`std::string` as argument in replace or find methods.

The only requirement for a "compatible" string class: It must use the same character type
(`char` or `wchar_t`) and it must provide `data()` and `size()` methods to access the
content. This also means: You can also use `std::vector<char_type>`!


## Special memory handling for sensitive data

TODO: Describe all the pain we have to go through in order to safely wipe memory, keep it from
getting swapped to disk and to prevent it from core dump inclusion.

