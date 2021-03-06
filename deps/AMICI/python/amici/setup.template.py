"""AMICI model package setup"""

import os
from typing import List

from amici import amici_path, hdf5_enabled
from amici.custom_commands import (set_compiler_specific_extension_options,
                                   compile_parallel)
from amici.setuptools import (get_blas_config,
                              get_hdf5_config,
                              add_coverage_flags_if_required,
                              add_debug_flags_if_required)
from setuptools import find_packages, setup, Extension
from setuptools.command.build_ext import build_ext


class ModelBuildExt(build_ext):
    """Custom build_ext"""

    def build_extension(self, ext):
        # Work-around for compiler-specific build options
        set_compiler_specific_extension_options(
            ext, self.compiler.compiler_type)

        # Monkey-patch compiler instance method for parallel compilation
        import distutils.ccompiler
        self.compiler.compile = compile_parallel.__get__(
            self.compiler, distutils.ccompiler.CCompiler)

        build_ext.build_extension(self, ext)


def get_model_sources() -> List[str]:
    """Get list of source files for the amici base library"""
    import glob
    model_sources = glob.glob('*.cpp')
    try:
        model_sources.remove('main.cpp')
    except ValueError:
        pass
    return model_sources


def get_amici_libs() -> List[str]:
    """
    Get list of libraries for the amici base library
    """
    return ['amici', 'sundials', 'suitesparse']


def get_extension() -> Extension:
    """Get distutils extension object for this AMICI model package"""

    cxx_flags = ['-std=c++14']
    linker_flags = []

    add_coverage_flags_if_required(cxx_flags, linker_flags)
    add_debug_flags_if_required(cxx_flags, linker_flags)

    h5pkgcfg = get_hdf5_config()

    blaspkgcfg = get_blas_config()
    linker_flags.extend(blaspkgcfg.get('extra_link_args', []))

    libraries = [*get_amici_libs(), *blaspkgcfg['libraries']]
    if hdf5_enabled:
        libraries.extend(['hdf5_hl_cpp', 'hdf5_hl', 'hdf5_cpp', 'hdf5'])

    sources = ['swig/TPL_MODELNAME.i', *get_model_sources()]

    # compiler and linker flags for libamici
    if 'AMICI_CXXFLAGS' in os.environ:
        cxx_flags.extend(os.environ['AMICI_CXXFLAGS'].split(' '))
    if 'AMICI_LDFLAGS' in os.environ:
        linker_flags.extend(os.environ['AMICI_LDFLAGS'].split(' '))

    ext_include_dirs = [
        os.getcwd(),
        os.path.join(amici_path, 'include'),
        os.path.join(amici_path, 'ThirdParty/gsl'),
        os.path.join(amici_path, 'ThirdParty/sundials/include'),
        os.path.join(amici_path, 'ThirdParty/SuiteSparse/include'),
        *h5pkgcfg['include_dirs'],
        *blaspkgcfg['include_dirs']
    ]

    ext_library_dirs = [
        *h5pkgcfg['library_dirs'],
        *blaspkgcfg['library_dirs'],
        os.path.join(amici_path, 'libs')
    ]

    # Build shared object
    ext = Extension(
        'TPL_MODELNAME._TPL_MODELNAME',
        sources=sources,
        include_dirs=ext_include_dirs,
        libraries=libraries,
        library_dirs=ext_library_dirs,
        swig_opts=[
            '-c++', '-modern', '-outdir', 'TPL_MODELNAME',
            '-I%s' % os.path.join(amici_path, 'swig'),
            '-I%s' % os.path.join(amici_path, 'include'),
        ],
        extra_compile_args=cxx_flags,
        extra_link_args=linker_flags
    )
    return ext


# Change working directory to setup.py location
os.chdir(os.path.dirname(os.path.abspath(__file__)))

MODEL_EXT = get_extension()

CLASSIFIERS = [
    'Development Status :: 3 - Alpha',
    'Intended Audience :: Science/Research',
    'Operating System :: POSIX :: Linux',
    'Operating System :: MacOS :: MacOS X',
    'Programming Language :: Python',
    'Programming Language :: C++',
    'Topic :: Scientific/Engineering :: Bio-Informatics',
]

CMDCLASS = {
    # For parallel compilation
    'build_ext': ModelBuildExt,
}

# Install
setup(
    name='TPL_MODELNAME',
    cmdclass=CMDCLASS,
    version='TPL_PACKAGE_VERSION',
    description='AMICI-generated module for model TPL_MODELNAME',
    url='https://github.com/ICB-DCM/AMICI',
    author='model-author-todo',
    author_email='model-author-todo',
    # license = 'BSD',
    ext_modules=[MODEL_EXT],
    packages=find_packages(),
    install_requires=['amici==TPL_AMICI_VERSION'],
    extras_require={'wurlitzer': ['wurlitzer']},
    python_requires='>=3.7',
    package_data={},
    zip_safe=False,
    include_package_data=True,
    classifiers=CLASSIFIERS,
)
