FROM spritsail/alpine-cmake:3.9
COPY . ./fbam
RUN ./fbam/docker_test.sh
