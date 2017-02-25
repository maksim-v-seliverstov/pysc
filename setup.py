# -*- coding: utf-8 -*-

import platform

from setuptools import setup, find_packages


if platform.system() == 'Windows':
    from distutils.errors import LinkError
    from distutils.msvccompiler import MSVCCompiler

    c = MSVCCompiler()

    obj = c.compile(
        sources=['win_service/svc/svc.cc'],
        include_dirs=['include'],
        macros=[
            ('SECURITY_WIN32', None),
            ('BOOST_BIND_ENABLE_STDCALL', None),
            ('unicode', None)
        ],
        extra_preargs=['/EHsc']
    )

    try:
        c.link_executable(obj, 'pyscm/svc', libraries=['Secur32', 'Shlwapi', 'Advapi32'])
    except LinkError as err:
        pass


setup(
    name='pyscm',
    version=__import__('pyscm').__version__,
    description='Service Control Manager for Windows and Linux',
    author='Seliverstov Maksim',
    author_email='Maksim.V.Seliverstov@yandex.ru',
    packages=find_packages(),
    zip_safe=False,
    keywords=[
        'windows service', 'linux demon',
        'service', 'demon',
        'runit',
        'c/c++ service windows'
    ],
    package_dir={'pyscm': 'pyscm'},
    package_data={
        'pyscm': ['svc.exe'],
        'pyscm.include': ['../include/skeleton/*.h']
    }
)
