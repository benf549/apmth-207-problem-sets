#!/usr/bin/env bash
set -e

if [[ $# -ne 1 ]]; then
    cat <<EOF
USAGE: ${0} <relative/path/to/mount/dir>

The path argument (relative to the current directory) will be mounted inside the
docker image under '/scratch'.  All data written to this directory will still be
available on the host (inside the directory passed as an argument above, it must
be your exercise working directory for example) once the container has been shut
down.

The docker image can be obtained by

docker pull cselab/korali

from the docker.io registry.

Installing docker:

Mac:     https://docs.docker.com/desktop/mac/install/
Windows: https://docs.docker.com/desktop/windows/install/ (requires WSL 2, see System Requirements)
Linux:   Use your repository package manager.  E.g. for Ubuntu
         https://phoenixnap.com/kb/install-docker-on-ubuntu-20-04
         Alternatively, you can install podman which is a rootless alternative
         to docker (https://podman.io/)
EOF
    exit 1
fi

WD=/scratch
docker run \
    -v "$(pwd)/${1}":${WD} -w ${WD} -e HOME=${WD} \
    -it --rm cselab/korali:latest
exit 0
