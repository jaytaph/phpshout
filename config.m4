
PHP_ARG_ENABLE(shout, whether to enable shout support,
[  --with-shout=[DIR]           Enable shout support])

if test "$PHP_SHOUT" != "no"; then
  SEARCH_PATH="/usr/local /usr"
  SEARCH_FOR="/include/shout/shout.h"
  if test -r $PHP_SHOUT/$SEARCH_FOR; then # path given as parameter
    SHOUT_DIR=$PHP_SHOUT
  else
    AC_MSG_CHECKING([for libshout files in default path])
    for i in $SEARCH_PATH ; do
      if test -r $i/$SEARCH_FOR; then
        SHOUT_DIR=$i
        AC_MSG_RESULT(found in $i)
      fi
    done
  fi

  if test -z "$SHOUT_DIR"; then
    AC_MSG_RESULT([not found])
    AC_MSG_ERROR([The required libshout library was not found.  You can obtain that package from http://www.icecast.org/])
  fi

  PHP_ADD_INCLUDE($SHOUT_DIR/include)

  LIBNAME=shout
  LIBSYMBOL=shout_init

  PHP_CHECK_LIBRARY($LIBNAME,$LIBSYMBOL,
  [
    PHP_ADD_LIBRARY_WITH_PATH($LIBNAME, $SHOUT_DIR/lib, SHOUT_SHARED_LIBADD)
    AC_DEFINE(HAVE_SHOUTLIB,1,[Have libshout])
  ],[
    AC_MSG_ERROR([libshout not found])
  ],[
    -L$SHOUT_DIR/lib
  ])

  PHP_SUBST(SHOUT_SHARED_LIBADD)

  PHP_NEW_EXTENSION(shout, shout.c , $ext_shared)
fi
