
PHP_ARG_ENABLE(asn1, whether to enable asn.1 support,
[  --with-asn1=[DIR]           Enable asn.1 support])

if test "$PHP_ASN1" != "no"; then
  SEARCH_PATH="/usr/local /usr"
  SEARCH_FOR="/include/libtasn1.h"
  if test -r $PHP_ASN1/$SEARCH_FOR; then # path given as parameter
    ASN1_DIR=$PHP_ASN1
  else
    AC_MSG_CHECKING([for libtasn1 files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        ASN1_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$ASN1_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([The required libtasn1 library was not found.  You can obtain that package from http://www.gnu.org/software/libtasn1/])
  fi

  PHP_ADD_INCLUDE($ASN1_DIR/include)

  LIBNAME=tasn1
  LIBSYMBOL=asn1_parser2tree

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $ASN1_DIR/lib, ASN1_SHARED_LIBADD)
    AC_DEFINE(HAVE_ASN1LIB,1,[Have libasn1])
  ],[
    AC_MSG_ERROR([libasn1 not found])
  ],[
    -L$ASN1_DIR/lib
  ])

  PHP_SUBST(ASN1_SHARED_LIBADD)

  PHP_NEW_EXTENSION(asn1, asn1.c , $ext_shared)
fi
