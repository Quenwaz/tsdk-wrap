from setuptools import setup
from setuptools import find_packages

with open("README.md", "r", encoding="utf-8") as f:
  long_description = f.read()

setup(name='pytsdk',
      version='0.0.7',
      description='Python version of dji_thermal_sdk',
      long_description=long_description,
      author='quenwa zhang',
      author_email='404937333@qq.com',
      keywords="thermal,dji,tsdk",
      url='https://github.com/Quenwaz/tsdk-wrap',
      install_requires=[
        "enum34;python_version<'3.4'",
        "numpy"
      ],
      long_description_content_type='text/markdown',
      packages=find_packages(where="."),
    #   package_dir={"": "pytsdk"},
      package_data={
        "pytsdk.libs":["*.dll", "*.ini"]
      },
    #   include_package_data = True,
      platforms=["Windows"],
      license='MIT License',
      classifiers=[
        'Development Status :: 4 - Beta',
        'Intended Audience :: Developers',
        'License :: OSI Approved :: MIT License',
        'Operating System :: Microsoft :: Windows',
        'Natural Language :: Chinese (Simplified)',
        'Programming Language :: Python :: 3',
        'Topic :: Software Development :: Libraries :: Python Modules'
      ],
)
