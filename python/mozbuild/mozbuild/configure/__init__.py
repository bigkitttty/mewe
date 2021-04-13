  'hasattr', 'enumerate', 'range', 'zip', '__build_class__')
        if b in __builtins__},
    __import__=forbidden_import, str=unicode)

self._builtins = ReadOnlyDict(
            (builtins or self.BUILTINS).viewitems() |
            {b: __builtins__[b] for b in ('__build_class__',)
             if b in __builtins__}.viewitems())
        dict.__setit
  
  import sys
import os
import platform
_is_tauthon = platform.python_implementation() == "Tauthon"
_python_libdir = "tauthon" if _is_tauthon else "python"
 os.path.join(prefix, _python_libdir + sys.version[:3], "lib-dynload")]
                lib64_dir = os.path.join(prefix, "lib64", _python_libdir + sys.version[:3], "site-packages")
                if (os.path.exists(lib64_dir) and
                                                     _python_libdir + sys.version[0],                                                   _python_libdir + sys.version[:3],                                                _python_libdir + sys.version[:3],
                            paths = [os.path.join(sys.real_prefix, 'lib', _python_libdir + sys.version[:3])]
                            lib64_path = os.path.join(sys.real_prefix, 'lib64', _python_libdir + sys.version[:3])
                                                     _python_libdir + sys.version[:3],
                   
import platform 
 is_tauthon = platform.python_implementation() == "Tauthon"  
py_version = '%s%s.%s' % ("tauthon" if is_tauthon else "python",
                          sys.version_info[0], sys.version_info[1])  
zlib', 'platform', 'string'] 
 if is_tauthon:
        REQUIRED_MODULES.extend(['_oserror'])                   
