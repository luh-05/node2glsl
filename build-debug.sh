set -euxo pipefail

cmake --preset debug
cmake --build --preset debug
