from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext
setup(
    cmdclass = {'build_ext': build_ext},
    ext_modules = [
    Extension("sweeny_c", ["sweeny_c.pyx"],
        extra_objects=[ #"py_sweeny_mc.o",
                        "./foobar.o" #, 
#                        OBJ+"py_sweeny_uf.o",
#                        OBJ+"py_uf.o",
#                        OBJ+"py_queue.o",
#                        OBJ+"py_sweeny_bfs.o",
#                        OBJ+"py_queue_2.o",
#                        OBJ+"py_sweeny_dc.o",
#                        OBJ+"py_dyncon.o",
#                        OBJ+"py_eulertour.o",
#                        OBJ+"py_splay.o",
#                        OBJ+"py_dllist.o"                        
                     ],
        libraries=["m"]
    ),
    ]
)
