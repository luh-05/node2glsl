set -euxo pipefail

cmake --build build --preset debug --target doc_doxygen
sphinx-build -b html docs docs/_build/html
