FROM spritsail/alpine-cmake:3.9

ARG enabled
ARG command
ARG logfile
ARG path_pattern

ENV HF_VAR_FS_MONIT_ENABLED=$enabled
ENV HF_VAR_FS_MONIT_COMMAND=$command
ENV HF_VAR_FS_MONIT_LOGFILE=$logfile
ENV HF_VAR_FS_MONIT_PATH_PATTERN=$path_pattern
COPY . ./fbam
RUN ./fbam/docker_test.sh
