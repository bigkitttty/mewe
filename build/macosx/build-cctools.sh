
AC_CHECK_LIB(pthreads, pthread_create,
    MOZ_USE_PTHREADS=1 _PTHREAD_LDFLAGS="-lpthreads",
    AC_CHECK_LIB(pthread, pthread_create,
        MOZ_USE_PTHREADS=1 _PTHREAD_LDFLAGS="-lpthread",
        AC_CHECK_LIB(c_r, pthread_create,
            MOZ_USE_PTHREADS=1 _PTHREAD_LDFLAGS="-lc_r",
            AC_CHECK_LIB(c, pthread_create,
                MOZ_USE_PTHREADS=1
                
            )WINNT)
                *-linux*|*-mingw*|*-dragonfly*|*-freebsd*|*-netbsd*|*-openbsd*)
        )
        WINNT)
         AC_DEFINE(MOZ_CAN_DRAW_IN_TITLEBAR)
                 AC_MSG_ERROR([Drawing in the titlebar is only supported on Windows targets])
        ;;
        
    *)  dnl On !Windows, we only want to link executables against mozglue
    )
